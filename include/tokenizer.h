#pragma once
#include "error.h"
#include "source.h"

struct s_TokenContext;
typedef struct s_TokenContext *TokenContext;

struct s_Token;
typedef struct s_Token *Token;

typedef enum e_TokenType {
  TT_Number,
  TT_BigInt,
  TT_String,
  TT_Keyword,
  TT_Identifier,
  TT_Regex,
  TT_Symbol,
  TT_Newline,
  TT_Comment,
  TT_MultiLineComment,
  TT_Template,
  TT_TemplateStart,
  TT_TemplatePart,
  TT_TemplateEnd,
  TT_Eof
} ATOM_TokenType;

struct s_Token {
  ATOM_TokenType type;
  strings raw;
};

struct s_TokenContext {
  int isRegexEnable;
  int isTemplateEnable;
};

TokenContext pushTokenContext();
void popTokenContext(TokenContext ctx);
void enableReadRegex();
void disableReadRegex();
void enableReadTemplate();
void disableReadTemplate();

Token readToken(SourceFile file, cstring source);
void Token_dispose(Token token);
Token readTokenSkipComment(SourceFile file, cstring source);
Token readTokenSkipNewline(SourceFile file, cstring source);
int Token_check(Token token, ATOM_TokenType tt, cstring source);