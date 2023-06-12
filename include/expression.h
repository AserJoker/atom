#pragma once
#include "ast.h"
#include "list.h"
#include "source.h"
#include "tokenizer.h"

typedef struct s_Expression *Expression;
typedef struct s_ExpressionContext *ExpressionContext;

ExpressionContext pushExpressionContext();
void popExpressionContext(ExpressionContext ectx);
int isExpressionComplete();
void insertExpression(Expression expr);

Location getExpressionLocation(SourceFile file, cstring source);

Expression Expression_create();
Expression readExpression(SourceFile file, cstring source);
void Expression_dispose(Expression expression);

int isLiteralToken(Token token);
Expression readLiteralExpression(SourceFile file, cstring source);

int isIdentifierToken(Token token);
Expression readIdentifierExpression(SourceFile file,cstring source);

typedef enum e_BindType { BT_Unknown, BT_Left, BT_Right, BT_Both } BindType;

int isCalculateOperator(Token token);
int isUnaryOperator(Token token);
int isUpdateOperator(Token token);
int getCalculateLevel(Token token);

struct s_ExpressionContext {
  TokenContext token_ctx;
  Expression root;
  Expression current;
  int maxlevel;
};

typedef enum e_ExpressionType {
  ET_Unknown,
  ET_Calculate,
  ET_Literal,
  ET_Identifier,
} ExpressionType;

struct s_Expression {
  AstNode node;
  ExpressionType type;
  int level;
  union {
    struct {
      Expression left;
      Expression right;
      Token operator;
      BindType bind;
    } binary;
    Token literal;
    Token Identifier;
  };
};