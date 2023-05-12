#include "ast.h"
#include "list.h"
#include "source.h"
#include "strings.h"
#include <stdio.h>
#include <string.h>

typedef struct s_BlockFrame {
  cstring _start;
  cstring _end;
} *BlockFrame;

typedef struct s_Context {
  List frames;
  BlockFrame current;
  Error error;
} Context;

static Context ctx = {NULL};

static const char *keywords[] = {
    "abstract",     "arguments", "await",    "boolean",    "break",
    "byte",         "case",      "catch",    "char",       "class",
    "const",        "continue",  "debugger", "default",    "delete",
    "do",           "double",    "else",     "enum",       "eval",
    "export",       "extends",   "false",    "final",      "finally",
    "float",        "for",       "function", "goto",       "if",
    "implements",   "import",    "in",       "instanceof", "int",
    "interface",    "let",       "long",     "native",     "new",
    "null",         "package",   "private",  "protected",  "public",
    "return",       "short",     "static",   "super",      "switch",
    "synchronized", "this",      "throw",    "throws",     "transient",
    "true",         "try",       "typeof",   "var",        "void",
    "volatile",     "while",     "with",     "yield",      "from",
    "as",           "assert",    NULL};

static const cstring symbols[] = {
    "===", "!==", "==", "?.(", ">=", "<=", "!=", "&&", "||", "++", "--", "+=",
    "-=",  "**",  "/=", "*=",  "&=", "|=", "%=", "<<", ">>", "??", "?.", "+",
    "-",   "*",   "/",  "%",   "&",  "|",  "!",  "~",  ";",  ",",  ".",  ">",
    "<",   "=",   "{",  "}",   "[",  "]",  "(",  ")",  "@",  "?",  ":",  NULL};

static BlockFrame BlockFrame_create() {
  BlockFrame bf = (BlockFrame)Buffer_alloc(sizeof(struct s_BlockFrame));
  bf->_start = NULL;
  bf->_end = NULL;
  return bf;
}
static void BlockFrame_dispose(BlockFrame bf) { Buffer_free(bf); }

Token Token_create() {
  Token token = (Token)Buffer_alloc(sizeof(struct s_Token));
  token->_location._position._column = 0;
  token->_location._position._line = 0;
  token->_raw.begin = 0;
  token->_raw.end = 0;
  token->_location._filename = NULL;
  token->_type = TT_Init;
  return token;
}

void Token_dispose(Token token) { Buffer_free(token); }

static cstring skipSpace(cstring source) {
  while (*source) {
    if (*source == ' ' || *source == '\t') {
      source++;
    } else {
      break;
    }
  }
  return source;
}

static Token readHex(SourceFile file, cstring source) {
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
  token->_type = TT_Number;
  token->_location = getLocation(file, source);
  token->_raw.begin = source;
  token->_raw.end = selector;
  return token;
}

static Token readNumber(SourceFile file, cstring source) {
  if (*source == '0' && (*(source + 1) == 'x' || *(source + 1) == 'X')) {
    return readHex(file, source);
  }
  Token token = Token_create();
  cstring selector = source;
  int decFlag = 0;
  while (*selector) {
    if (*selector >= '0' && *selector <= '9') {
      selector++;
    } else if (*selector == '.') {
      if (!decFlag) {
        decFlag = 1;
        selector++;
      } else {
        break;
      }
    } else {
      break;
    }
  }
  token->_location = getLocation(file, source);
  token->_raw.begin = source;
  token->_raw.end = selector;
  token->_type = TT_Number;
  if (*selector == 'n' && !decFlag) {
    token->_type = TT_BigInt;
    token->_raw.end++;
  }
  return token;
}
static Token readIdentifier(SourceFile file, cstring source) {
  cstring selector = source;
  while (*selector) {
    if ((*selector >= 'a' && *selector <= 'z') ||
        (*selector >= 'A' && *selector <= 'Z') ||
        (*selector >= '0' && *selector <= '9') || *selector == '_' ||
        *selector == '$' || (*selector == '#' && selector == source)) {
      selector++;
    } else {
      break;
    }
  }
  Token token = Token_create();
  token->_type = TT_Identifier;
  token->_location = getLocation(file, source);
  token->_raw.begin = source;
  token->_raw.end = selector;
  int index = 0;
  for (; keywords[index] != 0; index++) {
    if (strings_is(token->_raw, (const cstring)keywords[index])) {
      token->_type = TT_Keyword;
      break;
    }
  }
  return token;
}
static Token readTemplatePartOrEnd(SourceFile file, cstring source) {
  cstring selector = source;
  while (*selector) {
    if (*selector == '{' && *(selector - 1) == '$') {
      break;
    } else if (*selector == '`') {
      List_remove(ctx.frames, List_head(ctx.frames));
      ctx.current = (BlockFrame)List_get(List_head(ctx.frames));
      break;
    } else {
      selector++;
    }
  }
  if (!*selector) {
    ctx.error.error = "Unterminated template literal.";
    ctx.error.location = getLocation(file, selector);
    return NULL;
  }
  Token token = Token_create();
  token->_location = getLocation(file, source);
  token->_raw.begin = source;
  token->_raw.end = selector + 1;
  if (*selector == '`') {
    token->_type = TT_TemplateEnd;
  } else {
    token->_type = TT_TemplatePart;
  }
  return token;
}
static Token readSymbol(SourceFile file, cstring source) {
  cstring selector = source;
  if (*selector == '}') {
    if (strcmp(ctx.current->_start, "${") == 0) {
      return readTemplatePartOrEnd(file, source);
    } else {
      List_remove(ctx.frames, List_head(ctx.frames));
      ctx.current = (BlockFrame)List_get(List_head(ctx.frames));
    }
  }
  if (*selector == '{') {
    BlockFrame bf = BlockFrame_create();
    bf->_start = "{";
    bf->_end = "}";
    List_insert_head(ctx.frames, bf);
  }
  cstring symbol = NULL;
  for (int index = 0; symbols[index] != NULL; index++) {
    symbol = symbols[index];
    selector = source;
    while (*selector != 0) {
      if (*symbol == '\0') {
        break;
      }
      if (*symbol != *selector) {
        break;
      } else {
        symbol++;
        selector++;
      }
    }
    if (!*symbol) {
      Token token = Token_create();
      token->_location = getLocation(file, source);
      token->_type = TT_Symbol;
      token->_raw.begin = source;
      token->_raw.end = selector;
      return token;
    }
  }
  ctx.error.error = "Invalid character.";
  ctx.error.location = getLocation(file, selector);
  return NULL;
}
static Token readRegex(SourceFile file, cstring source) {
  cstring selector = source + 1;
  int inString = 0;
  while (*selector) {
    if (*selector == '\"' && *(selector - 1) != '\\') {
      inString = !inString;
    }
    if (*selector == '/' && *(selector - 1) != '\\' && !inString) {
      selector++;
      while (*selector) {
        if (*selector == 'i' || *selector == 'g' || *selector == 'm' ||
            *selector == 's') {
          selector++;
        } else {
          break;
        }
      }
      break;
    }
    selector++;
  }
  Token token = Token_create();
  token->_type = TT_Regex;
  token->_location = getLocation(file, source);
  token->_raw.begin = source;
  token->_raw.end = selector;
  return token;
}
static Token readComment(SourceFile file, cstring source) {
  cstring selector = source;
  if (*(selector + 1) == '/') {
    while (*selector != '\0') {
      if (*selector == '\n') {
        break;
      } else {
        selector++;
      }
    }
  } else {
    while (*selector != '\0') {
      if (*selector == '/' && *(selector - 1) == '*') {
        break;
      } else {
        selector++;
      }
    }
    if (!*selector) {
      ctx.error.error = "'*/' expected.";
      ctx.error.location = getLocation(file, selector);
      return NULL;
    } else {
      selector++;
    }
  }
  Token token = Token_create();
  token->_type = TT_Comment;
  token->_location = getLocation(file, source);
  token->_raw.begin = source;
  token->_raw.end = selector;
  return token;
}
Token readEof(SourceFile file, cstring source) {
  Token token = Token_create();
  token->_type = TT_Eof;
  token->_raw.begin = source;
  token->_raw.end = source;
  token->_location = getLocation(file, source);
  return token;
}
Token readString(SourceFile file, cstring source) {
  cstring selector = source + 1;
  while (*selector && *selector != '\n') {
    if (*selector == *source && *(selector - 1) != '\\') {
      break;
    } else {
      selector++;
    }
  }
  if (!*selector || *selector == '\n') {
    ctx.error.error = "Unterminated string literal.";
    ctx.error.location = getLocation(file, selector);
    return NULL;
  }
  Token token = Token_create();
  token->_location = getLocation(file, source);
  token->_type = TT_String;
  token->_raw.begin = source;
  token->_raw.end = selector + 1;
  return token;
}
Token readTemplate(SourceFile file, cstring source) {
  cstring selector = source + 1;
  while (*selector) {
    if (*selector == '`' && *(selector - 1) != '\\') {
      break;
    } else if (*selector == '{' && *(selector - 1) == '$') {
      BlockFrame bf = BlockFrame_create();
      bf->_start = "${";
      bf->_end = "}";
      List_insert_head(ctx.frames, bf);
      break;
    } else {
      selector++;
    }
  }
  if (!*selector) {
    ctx.error.error = "Unterminated template literal.";
    ctx.error.location = getLocation(file, selector);
    return NULL;
  }
  Token token = Token_create();
  token->_raw.begin = source;
  token->_raw.end = selector + 1;
  token->_location = getLocation(file, source);
  if (*selector == '`') {
    token->_type = TT_Template;
  } else {
    token->_type = TT_TemplateStart;
  }
  return token;
}

Token readNewline(SourceFile file, cstring source) {
  cstring selector = source;
  while (*selector) {
    if (*selector != '\n' && *selector != '\r') {
      break;
    }
    selector++;
  }
  Token token = Token_create();
  token->_type = TT_Newline;
  token->_raw.begin = source;
  token->_raw.end = selector;
  token->_location = getLocation(file, source);
  return token;
}

Token readInterepter(SourceFile file, cstring source) {
  cstring selector = source;
  if (*source == '#' && *(source + 1) == '!' && source == file->_source) {
    while (*selector) {
      if (*selector == '\n') {
        break;
      }
      selector++;
    }
  }
  Token token = Token_create();
  token->_type = TT_Interpreter;
  token->_raw.begin = source;
  token->_raw.end = selector;
  token->_location = getLocation(file, source);
  return token;
}

Token readToken(SourceFile file, cstring source) {
  source = skipSpace(source);
  if (*source == 0) {
    return readEof(file, source);
  }
  if (*source == '#' && *(source + 1) == '!' && source == file->_source) {
    return readInterepter(file, source);
  } else if (*source >= '0' && *source <= '9') {
    return readNumber(file, source);
  } else if ((*source >= 'a' && *source <= 'z') ||
             (*source >= 'A' && *source <= 'Z') || *source == '_' ||
             *source == '$' || *source == '#') {
    return readIdentifier(file, source);
  } else if ((*source == '/' && source[1] == '/') ||
             (*source == '/' && source[1] == '*')) {
    return readComment(file, source);
  } else if (*source == '/') {
    return readRegex(file, source);
  } else if (*source == '\"' || *source == '\'') {
    return readString(file, source);
  } else if (*source == '`') {
    return readTemplate(file, source);
  } else if (*source == '\n' || *source == '\r') {
    return readNewline(file, source);
  } else {
    return readSymbol(file, source);
  }
}

void initTokenizerContext() {
  List_Option opt = {1, (Buffer_Free)BlockFrame_dispose};
  ctx.frames = List_create(opt);
  BlockFrame bf = BlockFrame_create();
  List_insert_head(ctx.frames, bf);
  ctx.current = bf;
  bf->_start = "";
  bf->_end = "";
  ctx.error.error = NULL;
}
void uninitTokenizerContext() { List_dispose(ctx.frames); }
Error getTokenizerError() { return ctx.error; }