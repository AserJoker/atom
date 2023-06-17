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
void setMaxOperatorLevel(int level);
int getMaxOperatorLevel();

Location getExpressionLocation(SourceFile file, cstring source);

Expression Expression_create();
Expression readExpression(SourceFile file, cstring source);
void Expression_dispose(Expression expression);

int isLiteralExpression(SourceFile file, Token token);
Expression readLiteralExpression(SourceFile file, cstring source);

int isIdentifierExpression(SourceFile file, Token token);
Expression readIdentifierExpression(SourceFile file, cstring source);

int isBracketExpression(SourceFile file, Token token);
Expression readBracketExpression(SourceFile file, cstring source);

int isLambdaExpression(SourceFile file, Token token);
Expression readLambdaExpression(SourceFile file, cstring source);

int isFunctionExpression(SourceFile file, Token token);
Expression readFunctionExpression(SourceFile file, cstring source);

int isCalculateOperator(SourceFile file, Token token);
Expression readCalculateExpression(SourceFile file, cstring source);

int isUpdateOperator(SourceFile file, Token token);
Expression readUpdateExpression(SourceFile file, cstring source);

int isMemberOperator(SourceFile file, Token token);
Expression readMemberExpression(SourceFile file, cstring source);

int isUnaryOperator(SourceFile file, Token token);
Expression readUnaryExpression(SourceFile file, cstring source);

int isComputeExpression(SourceFile file, Token token);
Expression readComputeExpression(SourceFile file, cstring source);

typedef enum e_BindType { BT_Unknown, BT_Left, BT_Right, BT_Both } BindType;

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
  ET_Bracket,
  ET_Lambda,
  ET_Function,
  ET_Compute,
  ET_Call,
  ET_OptionalCompute,
  ET_OptionalCall,
  ET_Array,
  EF_Object,
  ET_Class,
} ExpressionType;

typedef struct s_Statement *Statement;

typedef struct s_Lambda {
  List args;
  Statement body;
  int async;
} *Lambda;

typedef struct s_Function {
  Token name;
  List args;
  Statement body;
  int async;
  int generator;
} *Function;

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
    Expression bracket;
    struct {
      Expression host;
      Expression key;
    } compute;
    Lambda lambda;
    Function function;
    Token literal;
    Token Identifier;
  };
};