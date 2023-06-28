#pragma once
#include "util/bool.h"
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
  Strings raw;
};

TokenContext Token_pushTokenContext();
void Token_popTokenContext(TokenContext ctx);
void Token_enableReadRegex();
void Token_disableReadRegex();
void Token_enableReadTemplate();
void Token_disableReadTemplate();

Token Token_read(SourceFile file, cstring source);
void Token_dispose(Token token);
Token Token_readSkipComment(SourceFile file, cstring source);
Token Token_readSkipNewline(SourceFile file, cstring source);
Bool Token_check(Token token, Token_Type tt, cstring source);
Token Token_pair(SourceFile file, cstring source, Token_Type tl, cstring left,
                 Token_Type tr, cstring right);