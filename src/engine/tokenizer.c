#include "tokenizer.h"
static TokenContext g_context = NULL;

static cstring g_keywords[] = {
    "await",  "async",    "break",    "case",       "catch",  "class",
    "const",  "continue", "debugger", "default",    "delete", "do",
    "else",   "export",   "extends",  "finally",    "for",    "function",
    "if",     "import",   "in",       "instanceof", "let",    "new",
    "return", "static",   "super",    "switch",     "this",   "throw",
    "try",    "typeof",   "var",      "void",       "while",  "with",
    "yield",  0};

static cstring g_symbols[] = {
    "===", "!==", "...", "==", "!=", "<=", ">=", "++", "--", "+=", "-=", "|=",
    "&=",  "=>",  "*=",  "/=", "%=", "&&", "||", "?.", "+",  "-",  "*",  "/",
    "%",   "!",   "(",   ")",  "[",  "]",  "{",  "}",  "|",  "&",  ";",  ":",
    "<",   ">",   ",",   ".",  "?",  "@",  "#",  "=",  0};

TokenContext pushTokenContext() {
  TokenContext result = g_context;
  g_context = (TokenContext)Buffer_alloc(sizeof(struct s_TokenContext));
  g_context->isRegexEnable = 0;
  g_context->isTemplateEnable = 0;
  return result;
}
void popTokenContext(TokenContext tctx) {
  if (g_context) {
    Buffer_free(g_context);
  }
  g_context = tctx;
}

void Token_dispose(Token token) { Buffer_free(token); }
Token Token_create() {
  Token token = (Token)Buffer_alloc(sizeof(struct s_Token));
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
      ErrorStack_push(Error_init("Unterminated string literal.",
                                 getLocation(file, selector), NULL));
    }
  }
  return NULL;
}

Token readIdentifierToken(SourceFile file, cstring source) {
  if ((*source >= 'a' && *source <= 'z') ||
      (*source >= 'A' && *source <= 'Z') || *source == '$' || *source == '@') {
    cstring selector = source;
    while (*selector) {
      if ((*selector >= 'a' && *selector <= 'z') ||
          (*selector >= 'A' && *selector <= 'Z') || *selector == '$') {
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
      if (strings_is(token->raw, g_keywords[index])) {
        token->type = TT_Keyword;
        break;
      }
    }
    return token;
  }
  return NULL;
}

Token readCommentToken(SourceFile file, cstring source) {
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
        ErrorStack_push(
            Error_init("'*/' expected.", getLocation(file, selector), NULL));
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
  }
  return NULL;
}

Token readRegexToken(SourceFile file, cstring source) {
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

Token readSymbolToken(SourceFile file, cstring source) {
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

Token readNewlineToken(SourceFile file, cstring source) {
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

Token readTemplateToken(SourceFile file, cstring source) {
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
        ErrorStack_push(Error_init("Unterminated template literal.",
                                   getLocation(file, selector), NULL));
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

Token readTemplatePartOrEndToken(SourceFile file, cstring source) {
  if (*source == '}') {
    cstring selector = source;
    for (;;) {
      if (*selector == '`' && *(selector - 1) != '\\' ||
          (*selector == '{' && *(selector - 1) == '$' &&
           *(selector - 2) != '\\')) {
        selector++;
        break;
      } else if (!*selector) {
        ErrorStack_push(Error_init("Unterminated template literal.",
                                   getLocation(file, selector), NULL));
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

void enableReadRegex() { g_context->isRegexEnable = 1; }
void disableReadRegex() { g_context->isRegexEnable = 0; }
void enableReadTemplate() { g_context->isTemplateEnable = 1; };
void disableReadTemplate() { g_context->isTemplateEnable = 0; };

Token readToken(SourceFile file, cstring source) {
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
Token readTokenSkipComment(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readToken(file, selector);
  while (token && token->type == TT_Comment) {
    selector = token->raw.end;
    Token_dispose(token);
    token = readToken(file, selector);
  }
  return token;
}
Token readTokenSkipNewline(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipComment(file, selector);
  if (token &&
      (token->type == TT_MultiLineComment || token->type == TT_Newline)) {
    selector = token->raw.end;
    Token_dispose(token);
    token = readTokenSkipComment(file, selector);
  }
  return token;
}
int Token_check(Token token, ATOM_TokenType tt, cstring source) {
  return token->type == tt && strings_is(token->raw, source);
}
