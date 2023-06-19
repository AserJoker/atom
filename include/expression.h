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

int isCallExpression(SourceFile file, Token token);
Expression readCallExpression(SourceFile file, cstring source);

int isOptionalExpression(SourceFile file, Token token);
Expression readOptionalExpression(SourceFile file, cstring source);

int isArrayExpression(SourceFile file, Token token);
Expression readArrayExpression(SourceFile file, cstring source);

int isObjectExpression(SourceFile file, Token token);
Expression readObjectExpression(SourceFile file, cstring source);

int isClassExpression(SourceFile file, Token token);
Expression readClassExpression(SourceFile file, cstring source);

int isNewExpression(SourceFile file, Token token);
Expression readNewExpression(SourceFile file, cstring source);

int isDeleteExpression(SourceFile file, Token token);
Expression readDeleteExpression(SourceFile file, cstring source);

int isAwaitExpression(SourceFile file, Token token);
Expression readAwaitExpression(SourceFile file, cstring source);

int isThisExpression(SourceFile file, Token token);
Expression readThisExpression(SourceFile file, cstring source);

int isSuperExpression(SourceFile file, Token token);
Expression readSuperExpression(SourceFile file, cstring source);

int isConditionExpression(SourceFile file, Token token);
Expression readConditionExpression(SourceFile file, cstring source);

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
  ET_Object,
  ET_Class,
  ET_Await,
  ET_New,
  ET_Delete,
  ET_This,
  ET_Super,
  ET_Condition
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
  int getter;
  int setter;
} *Function;

typedef struct s_Array {
  List items;
} *Array;

typedef struct s_ObjectProperty {
  Expression key;
  Expression value;
} *ObjectProperty;

typedef struct s_ClassProperty {
  Expression key;
  Expression value;
  int isPrivate;
  int isStatic;
  List decorators;
} *ClassProperty;

typedef struct s_Class {
  List properties;
  List staticBlocks;
  List decorators;
  Expression extends;
  Token name;
} *Class;

typedef struct s_Object {
  List properties;
} *Object;

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
    struct {
      Expression condition;
      Expression consequent;
      Expression alternate;
    } condition;
    Expression bracket, await, deleteExpr, newExpr;
    struct {
      Expression host;
      Expression key;
    } compute;
    Lambda lambda;
    struct {
      Expression callee;
      List args;
    } call;
    Function function;
    Array array;
    Object object;
    Class clazz;
    Token literal;
    Token Identifier;
  };
};