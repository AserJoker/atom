#pragma once
#include "ast.h"
#include "list.h"
#include "source.h"
#include "tokenizer.h"
typedef struct s_Expression *Expression;

Expression Expression_create();
Expression readExpression(SourceFile file, cstring source);
void Expression_dispose(Expression expression);

typedef enum e_ExpressionType {
  ET_Unknown,
  ET_Calculate,
  ET_Literal,
  ET_Identifier,
} ExpressionType;

struct s_Expression {
  AstNode node;
  ExpressionType type;
  union {
    struct {
      Expression left;
      Expression right;
      Token operator;
    } binary;
    Token literal;
    Token Identifier;
  };
};