#pragma once
#include "util/error.h"
#include "util/source.h"

typedef struct s_TokenContext *TokenContext;
typedef struct s_Token *Token;

typedef enum e_Token_Type {
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
} Token_Type;

struct s_Token {
  Token_Type type;
  strings raw;
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
int checkToken(Token token, Token_Type tt, cstring source);
Token pairToken(SourceFile file, cstring source, Token_Type tl, cstring left,
                Token_Type tr, cstring right);