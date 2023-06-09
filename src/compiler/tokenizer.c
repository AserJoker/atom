#include "compiler/tokenizer.h"
#include "util/error.h"

struct s_TokenContext {
  int isRegexEnable;
  int isTemplateEnable;
};

static TokenContext g_context = NULL;

static cstring g_keywords[] = {
    "assert",     "as",      "await",   "async",  "break",
    "case",       "catch",   "class", "const",  "continue",
    "debugger",   "default", "delete",  "do",     "else",
    "export",     "extends", "finally", "for",    "function",
    "from",       "if",      "get",     "import", "in",
    "instanceof", "let",     "new",     "of",     "return",
    "static",     "super",   "switch",  "set",    "this",
    "throw",      "try",     "typeof",  "var",    "void",
    "while",      "with",    "yield",   0};

static cstring g_symbols[] = {
    "===", "!==", "...", "==", "!=", "<=", ">=", "++", "--", "+=", "-=", "|=",
    "&=",  "=>",  "*=",  "/=", "%=", "&&", "||", "?.", "+",  "-",  "*",  "/",
    "%",   "!",   "(",   ")",  "[",  "]",  "{",  "}",  "|",  "&",  ";",  ":",
    "<",   ">",   ",",   ".",  "?",  "@",  "#",  "=",  0};

TokenContext Token_pushTokenContext() {
  TokenContext result = g_context;
  g_context = (TokenContext)Buffer_alloc(sizeof(struct s_TokenContext), NULL);
  g_context->isRegexEnable = 0;
  g_context->isTemplateEnable = 0;
  return result;
}
void Token_popTokenContext(TokenContext tctx) {
  if (g_context) {
    Buffer_dispose(g_context);
  }
  g_context = tctx;
}

void Token_dispose(Token token) { Buffer_dispose(token); }
Token Token_create() {
  Token token = (Token)Buffer_alloc(sizeof(struct s_Token), NULL);
  token->raw.begin = 0;
  token->raw.end = 0;
  token->type = TT_Eof;
  return token;
}

static Token readEofToken(SourceFile file, cstring source) {
  if (!*source) {
    Token token = Token_create();
    token->raw.begin = source;
    token->raw.end = source + 1;
    token->type = TT_Eof;
    return token;
  }
  return NULL;
}

static Token readHexNumberToken(SourceFile file, cstring source) {
  if (*source == '0' && (*(source + 1) == 'x' || *(source + 1) == 'X')) {
    cstring selector = source + 2;
    while (*selector) {
      if ((*selector >= '0' && *selector <= '9') ||
          (*selector >= 'a' && *selector <= 'f') ||
          (*selector >= 'A' && *selector <= 'F')) {
        selector++;
      } else {
        break;
      }
    }
    Token token = Token_create();
    token->raw.begin = source;
    token->raw.end = selector;
    token->type = TT_Number;
    return token;
  }
  return NULL;
}

static Token readDecNumberToken(SourceFile file, cstring source) {
  if (*source >= '0' && *source <= '9') {
    cstring selector = source;
    int float_flag = 0;
    while (*selector) {
      if (*selector >= '0' && *selector <= '9') {
        selector++;
      } else if (*selector == '.') {
        if (!float_flag) {
          float_flag = 1;
        } else {
          break;
        }
      } else {
        break;
      }
    }
    Token token = Token_create();
    token->raw.begin = source;
    if (!float_flag && *selector == 'n') {
      selector++;
      token->raw.end = selector;
      token->type = TT_BigInt;
    } else {
      token->raw.end = selector;
      token->type = TT_Number;
    }
    return token;
  }
  return NULL;
}

static Token readNumberToken(SourceFile file, cstring source) {
  if (*source >= '0' && *source <= '9') {
    if (*source == '0' && (*(source + 1) == 'x' || *(source + 1) == 'X')) {
      return readHexNumberToken(file, source);
    } else {
      return readDecNumberToken(file, source);
    }
  }
  return NULL;
}

static Token readStringToken(SourceFile file, cstring source) {
  if (*source == '\"' || *source == '\'') {
    cstring selector = source + 1;
    while (*selector && *selector != '\r' && *selector != '\n') {
      if (*selector == *source && *(selector - 1) != '\\') {
        selector++;
        break;
      }
      selector++;
    }
    if (*(selector - 1) == *source) {
      Token token = Token_create();
      token->raw.begin = source;
      token->raw.end = selector;
      token->type = TT_String;
      return token;
    } else {
      Error_set(Error_create("Unterminated string e_literal.",
                             SourceFile_getLocation(file, selector), NULL));
    }
  }
  return NULL;
}

static Token readIdentifierToken(SourceFile file, cstring source) {
  if ((*source >= 'a' && *source <= 'z') ||
      (*source >= 'A' && *source <= 'Z') || *source == '$' || *source == '_') {
    cstring selector = source;
    while (*selector) {
      if ((*selector >= 'a' && *selector <= 'z') ||
          (*selector >= 'A' && *selector <= 'Z') ||
          (*selector >= '0' && *selector <= '9') || *selector == '$' ||
          *selector == '_') {
        selector++;
      } else {
        break;
      }
    }
    Token token = Token_create();
    token->type = TT_Identifier;
    token->raw.begin = source;
    token->raw.end = selector;
    for (int index = 0; g_keywords[index] != 0; index++) {
      if (Strings_is(token->raw, g_keywords[index])) {
        token->type = TT_Keyword;
        break;
      }
    }
    return token;
  }
  return NULL;
}

static Token readCommentToken(SourceFile file, cstring source) {
  if (*source == '/' && *(source + 1) == '/') {
    cstring selector = source;
    while (*selector) {
      if (*selector == '\r' || *selector == '\n') {
        break;
      }
    }
    Token token = Token_create();
    token->raw.begin = source;
    token->raw.end = selector;
    token->type = TT_Comment;
    return token;
  }
  if (*source == '/' && *(source + 1) == '*') {
    cstring selector = source + 2;
    int newline_flag = 0;
    for (;;) {
      if (!*selector) {
        Error_set(Error_create("'*/' expected.",
                               SourceFile_getLocation(file, selector), NULL));
        return NULL;
      } else if (*selector == '/' && *(selector - 1) == '*') {
        selector++;
        break;
      } else if (*selector == '\n' || *selector == '\r') {
        selector++;
        newline_flag = 1;
      } else {
        selector++;
      }
    }
    Token token = Token_create();
    token->raw.begin = source;
    token->raw.end = selector;
    if (newline_flag) {
      token->type = TT_MultiLineComment;
    } else {
      token->type = TT_Comment;
    }
    return token;
  }
  return NULL;
}

static Token readRegexToken(SourceFile file, cstring source) {
  if (*source == '/') {
    cstring selector = source + 1;
    while (*selector) {
      if (*selector == '/' && *(selector - 1) != '\\') {
        selector++;
        break;
      } else {
        selector++;
      }
    }
    while (*selector == 'g' || *selector == 'i' || *selector == 'm' ||
           *selector == 'y') {
      selector++;
    }
    Token token = Token_create();
    token->raw.begin = source;
    token->raw.end = selector;
    token->type = TT_Regex;
    return token;
  }
  return NULL;
}

static Token readSymbolToken(SourceFile file, cstring source) {
  int index = 0;
  cstring selector = source;
  for (; g_symbols[index] != 0; index++) {
    cstring s = g_symbols[index];
    selector = source;
    while (*s) {
      if (*selector != *s) {
        break;
      }
      selector++;
      s++;
    }
    if (!*s) {
      break;
    }
  }
  if (g_symbols[index]) {
    Token token = Token_create();
    token->raw.begin = source;
    token->raw.end = selector;
    token->type = TT_Symbol;
    return token;
  }
  return NULL;
}

static Token readNewlineToken(SourceFile file, cstring source) {
  if (*source == '\r' || *source == '\n') {
    cstring selector = source;
    while (*selector == '\r' || *selector == '\n') {
      selector++;
    }
    Token token = Token_create();
    token->raw.begin = source;
    token->raw.end = selector;
    token->type = TT_Newline;
    return token;
  }
  return NULL;
}

static Token readTemplateToken(SourceFile file, cstring source) {
  if (*source == '`') {
    cstring selector = source + 1;
    for (;;) {
      if (*selector == '`' && *(selector - 1) != '\\') {
        selector++;
        break;
      }
      if (*selector == '{' && *(selector - 1) == '$' &&
          *(selector - 2) != '\\') {
        selector++;
        break;
      }
      if (!*selector) {
        Error_set(Error_create("Unterminated e_template e_literal.",
                               SourceFile_getLocation(file, selector), NULL));
        return NULL;
      }
      selector++;
    }
    Token token = Token_create();
    token->raw.begin = source;
    token->raw.end = selector;
    if (*(selector - 1) == '`') {
      token->type = TT_Template;
    } else {
      token->type = TT_TemplateStart;
    }
    return token;
  }
  return NULL;
}

static Token readTemplatePartOrEndToken(SourceFile file, cstring source) {
  if (*source == '}') {
    cstring selector = source;
    for (;;) {
      if (*selector == '`' && *(selector - 1) != '\\' ||
          (*selector == '{' && *(selector - 1) == '$' &&
           *(selector - 2) != '\\')) {
        selector++;
        break;
      } else if (!*selector) {
        Error_set(Error_create("Unterminated e_template e_literal.",
                               SourceFile_getLocation(file, selector), NULL));
        return NULL;
      }
      selector++;
    }
    Token token = Token_create();
    token->raw.begin = source;
    token->raw.end = selector;
    if (*(selector - 1) == '`') {
      token->type = TT_TemplateEnd;
    } else {
      token->type = TT_TemplatePart;
    }
    return token;
  }
  return NULL;
}

void Token_enableReadRegex() { g_context->isRegexEnable = 1; }
void Token_disableReadRegex() { g_context->isRegexEnable = 0; }
void Token_enableReadTemplate() { g_context->isTemplateEnable = 1; };
void Token_disableReadTemplate() { g_context->isTemplateEnable = 0; };

Token Token_read(SourceFile file, cstring source) {
  while (*source == ' ' || *source == '\t') {
    source++;
  }
  if (*source == '\0') {
    return readEofToken(file, source);
  } else if (*source == '\r' || *source == '\n') {
    return readNewlineToken(file, source);
  } else if (*source >= '0' && *source <= '9') {
    return readNumberToken(file, source);
  } else if (*source == '\"' || *source == '\'') {
    return readStringToken(file, source);
  } else if ((*source >= 'a' && *source <= 'z') ||
             (*source >= 'A' && *source <= 'Z') || *source == '$' ||
             *source == '_') {
    return readIdentifierToken(file, source);
  } else if (*source == '/' && (*(source + 1) == '/' || *(source + 1) == '*')) {
    return readCommentToken(file, source);
  } else if (*source == '/' && g_context->isRegexEnable) {
    return readRegexToken(file, source);
  } else if (*source == '`') {
    return readTemplateToken(file, source);
  } else if (*source == '}' && g_context->isTemplateEnable) {
    return readTemplatePartOrEndToken(file, source);
  } else {
    return readSymbolToken(file, source);
  }
}
Token Token_readSkipComment(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = Token_read(file, selector);
  while (token && token->type == TT_Comment) {
    selector = token->raw.end;
    Token_dispose(token);
    token = Token_read(file, selector);
  }
  return token;
}
Token Token_readSkipNewline(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = Token_readSkipComment(file, selector);
  while (token &&
         (token->type == TT_MultiLineComment || token->type == TT_Newline)) {
    selector = token->raw.end;
    Token_dispose(token);
    token = Token_readSkipComment(file, selector);
  }
  return token;
}
Bool Token_check(Token token, Token_Type tt, cstring source) {
  return token->type == tt && Strings_is(token->raw, source);
}
Token Token_pair(SourceFile file, cstring source, Token_Type tl, cstring left,
                 Token_Type tr, cstring right) {
  cstring selector = source;
  int level = 0;
  Token token = Token_read(file, selector);
  if (!token) {
    return NULL;
  }
  if (!Token_check(token, tl, left)) {
    Token_dispose(token);
    return NULL;
  }
  selector = token->raw.end;
  level++;
  Token_dispose(token);
  token = Token_read(file, selector);
  for (;;) {
    if (Token_check(token, tr, right)) {
      level--;
      if (!level) {
        break;
      }
    } else if (Token_check(token, tl, left)) {
      level++;
    } else if (token->type == TT_Eof) {
      Token_dispose(token);
      return NULL;
    }
    selector = token->raw.end;
    Token_dispose(token);
    token = Token_read(file, selector);
    if (!token) {
      return NULL;
    }
  }
  return token;
}