#pragma once
#include "ast.h"
#include "expression.h"
typedef struct s_Statement *Statement;
typedef enum e_StatementType {
  ST_Unknown,
  ST_Block,
  ST_Empty,
  ST_Expression,
} StatementType;

struct s_Statement {
  AstNode node;
  StatementType type;
  union {
    struct {
      List body;
    } block;
    Expression expression;
  };
};
Statement Statement_create();
Statement readStatement(SourceFile file, cstring source);
void Statement_dispose(Statement statement);

int isEmptyStatement(SourceFile file, Token token);
Statement readEmptyStatement(SourceFile file, cstring source);

int isBlockStatement(SourceFile file, Token token);
Statement readBlockStatement(SourceFile file, cstring source);
Statement readExpressionStatement(SourceFile file, cstring source);