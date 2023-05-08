#include "ast.h"
#include "dlist.h"
#include "strings.h"
#include <stdio.h>
#include <string.h>
struct s_Position {
  size_t _line;
  size_t _column;
};
struct s_Location {
  cstring _filename;
  Position _position;
};
struct s_Token {
  Location _location;
  strings _raw;
  TokenType _type;
};
struct s_AstNode {
  NodeType _type;
  union {
  } _data;
};

typedef struct s_SourceLine {
  strings _raw;
  size_t _begin;
  size_t _end;
} *SourceLine;

typedef struct s_SourceFile {
  DList _lines;
  cstring _source;
  cstring _filename;
} *SourceFile;

typedef struct s_BlockFrame {
  cstring _start;
  cstring _end;
  struct s_BlockFrame *_last;
} *BlockFrame;

typedef struct s_Context {
  BlockFrame root;
  BlockFrame current;
} Context;

static Context ctx = {NULL};

static const cstring symbols[] = {
    "===", "!==", "==", ">=", "<=", "!=", "&&", "||", "++", "--", "+=", "-=",
    "/=",  "*=",  "&=", "|=", "%=", "<<", ">>", "::", "+",  "-",  "*",  "/",
    "%",   "&",   "|",  "!",  "~",  ";",  ",",  ".",  ">",  "<",  "=",  "{",
    "}",   "[",   "]",  "(",  ")",  "@",  "?",  ":",  "\n", NULL};

static BlockFrame BlockFrame_create() {
  BlockFrame bf = (BlockFrame)Buffer_alloc(sizeof(struct s_BlockFrame));
  bf->_start = NULL;
  bf->_end = NULL;
  return bf;
}
static void BlockFrame_dispose(BlockFrame bf) { Buffer_free(bf); }

static Token Token_create() {
  Token token = (Token)Buffer_alloc(sizeof(struct s_Token));
  token->_location._position._column = 0;
  token->_location._position._line = 0;
  token->_raw.begin = 0;
  token->_raw.end = 0;
  token->_location._filename = NULL;
  token->_type = TT_Init;
  return token;
}

SourceLine SourceLine_create() {
  SourceLine sl = (SourceLine)Buffer_alloc(sizeof(struct s_SourceLine));
  sl->_begin = 0;
  sl->_end = 0;
  sl->_raw.begin = 0;
  sl->_raw.end = 0;
  return sl;
}

void SourceLine_dispose(SourceLine sl) { Buffer_free(sl); }

SourceFile SourceFile_read(cstring filename) {
  SourceFile sf = (SourceFile)Buffer_alloc(sizeof(struct s_SourceFile));
  FILE *fp = NULL;
  cstring source = NULL;
  fopen_s(&fp, filename, "rb");
  fseek(fp, 0, SEEK_END);
  size_t len = ftell(fp);
  source = (cstring)Buffer_alloc(len + 1);
  source[len] = 0;
  fseek(fp, 0, SEEK_SET);
  fread(source, 1, len, fp);
  fclose(fp);
  sf->_source = source;
  sf->_filename = filename;
  DList_Option opt = {1, (Buffer_Free)SourceLine_dispose};
  sf->_lines = DList_create(opt);
  cstring linestart = source;
  while (*source) {
    if (*source == '\n') {
      source++;
      if (*source == '\r') {
        source++;
      }
      SourceLine sl = (SourceLine)Buffer_alloc(sizeof(struct s_SourceLine));
      sl->_raw.begin = linestart;
      sl->_raw.end = source;
      linestart = source;
      sl->_begin = sl->_raw.begin - sf->_source;
      sl->_end = sl->_raw.end - sf->_source;
      DList_insert_tail(sf->_lines, sl);
    } else {
      source++;
    }
  }
  return sf;
}

void SourceFile_dispose(SourceFile sf) {
  DList_dispose(sf->_lines);
  Buffer_free(sf->_source);
  Buffer_free(sf);
}

static void Token_dispose(Token token) { Buffer_free(token); }

Location getLocation(SourceFile file, cstring source) {
  Location loc;
  loc._filename = file->_filename;
  size_t index = 0;
  for (DList_Node it = DList_head(file->_lines); it != DList_tail(file->_lines);
       it = DList_next(it)) {
    SourceLine line = DList_get(it);
    if (source >= line->_raw.begin && source < line->_raw.end) {
      loc._position._line = index;
      loc._position._column = source - line->_raw.begin;
      break;
    }
    index++;
  }
  return loc;
}

cstring skipSpace(cstring source) {
  while (*source) {
    if (*source == ' ' || *source == '\t') {
      source++;
    } else {
      break;
    }
  }
  return source;
}

Token readHex(SourceFile file, cstring source) {
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

Token readNumber(SourceFile file, cstring source) {
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
        // TODO: invalid number
      }
    } else {
      break;
    }
  }
  token->_location = getLocation(file, source);
  token->_raw.begin = source;
  token->_raw.end = selector;
  token->_type = TT_Number;
  return token;
}
Token readIdentifier(SourceFile file, cstring source) {
  cstring selector = source;
  while (*selector) {
    if ((*selector >= 'a' && *selector <= 'z') ||
        (*selector >= 'A' && *selector <= 'Z') ||
        (*selector >= '0' && *selector <= '9') || *selector == '_' ||
        *selector == '$' || *selector == '#') {
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
  return token;
}
Token readSymbol(SourceFile file, cstring source) {
  cstring selector = source;
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
  // TODO: syntax error: known symbol
  return NULL;
}
Token readRegex(SourceFile file, cstring source) {
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
Token readComment(SourceFile file, cstring source) {
  Token token = Token_create();
  token->_type = TT_Comment;
  cstring selector = source;
  if (*(selector + 1) == '/') {
    while (*selector != '\0') {
      if (*selector == '\n') {
        break;
      } else {
        selector++;
      }
    }
    if (*selector) {
      selector++;
      if (*selector == '\r') {
        selector++;
      }
    }
  } else {
    while (*selector != '\0') {
      if (*selector == '*' && *(selector + 1) == '/') {
        selector += 2;
        break;
      } else {
        break;
      }
    }
    if (!*selector) {
      Buffer_free(token);
      return NULL;
      // TODO: not find block comment end
    }
  }
  token->_raw.end = selector;
  token->_location = getLocation(file, source);
  token->_raw.begin = source;
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
  while (*selector) {
    if (*selector == *source && *(selector - 1) != '\\') {
      break;
    } else {
      selector++;
    }
  }
  if (*selector) {
    // TODO: Syntax Error: string end pair
  }
  Token token = Token_create();
  token->_location = getLocation(file, source);
  token->_type = TT_String;
  token->_raw.begin = source;
  token->_raw.end = selector + 1;
  return token;
}

Token readToken(SourceFile file, cstring source) {
  source = skipSpace(source);
  if (*source == 0) {
    return readEof(file, source);
  }
  if (*source >= '0' && *source <= '9') {
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
  } else {
    return readSymbol(file, source);
  }
}
const cstring Token_getTypeString(Token token) {
  switch (token->_type) {

  case TT_Number:
    return "TT_Number";
  case TT_String:
    return "TT_String";
  case TT_Keyword:
    return "TT_Keyword";
  case TT_Symbol:
    return "TT_Symbol";
  case TT_Identifier:
    return "TT_Identifier";
  case TT_Regex:
    return "TT_Regex";
  case TT_Comment:
    return "TT_Comment";
  case TT_Eof:
    return "TT_Eof";
  case TT_Init:
    return "TT_Init";
  case TT_Template:
    return "TT_Template";
  case TT_TemplateStart:
    return "TT_TemplateStart";
  case TT_TemplatePart:
    return "TT_TemplatePart";
  case TT_TemplateEnd:
    return "TT_TemplateEnd";
  }
}

cstring Token_toString(Token token) {
  size_t len = token->_raw.end - token->_raw.begin;
  cstring buf = (cstring)Buffer_alloc(len + 1);
  if (token->_raw.begin[0] == '\n') {
    cstring buf = (cstring)Buffer_alloc(3);
    memcpy(buf, "\\n", 3);
    return buf;
  } else {
    cstring buf = (cstring)Buffer_alloc(len + 1);
    memcpy(buf, token->_raw.begin, len);
    buf[len] = '\0';
    return buf;
  }
}

AstNode parse(SourceFile file) {
  cstring source = file->_source;
  DList_Option opt = {0, NULL};
  while (*source) {
    Token token = readToken(file, source);
    TokenType tt = token->_type;
    printf("(%s)`%s`,%s:%lld\n", Token_getTypeString(token),
           Token_toString(token), token->_location._filename,
           token->_location._position._line + 1);
    source = token->_raw.end;
    Token_dispose(token);
    if (tt == TT_Eof) {
      break;
    }
  }
  BlockFrame_dispose(ctx.current);
  return NULL;
}