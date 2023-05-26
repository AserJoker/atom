#pragma once
#include "json.h"
#include "tokenizer.h"
struct s_AstContext;
typedef struct s_AstContext *AstContext;

struct s_AstContext {
  TokenContext tContext;
};

struct s_AstNode;
typedef struct s_AstNode *AstNode;

struct s_Program;
typedef struct s_Program *Program;

struct s_Statement;
typedef struct s_Statement *Statement;

struct s_Expression;
typedef struct s_Expression *Expression;

struct s_Identifier;
typedef struct s_Identifier *Identifier;

struct s_Literal;
typedef struct s_Literal *Literal;

typedef enum e_NodeType {
  NT_Program,
  // statement
  NT_BlockStatement,
  NT_EmptyStatement,
  NT_ExpressionStatement,

  // expression
  NT_BinaryExpression,
  NT_LiteralExpression,
  NT_IdentifierExpression,
  NT_BracketExpression,

  // literal
  NT_StringLiteral,
  NT_NumberLiteral,
  NT_BooleanLiteral,
  NT_NullLiteral,
  NT_UndefinedLiteral,

  // Identifier
  NT_Identifier,

  // Literal
  NT_Literal
} NodeType;

struct s_AstNode {
  strings position;
  NodeType type;
};

struct s_Statement {
  AstNode node;
  union {
    struct {
      List body;
    } block;
    struct {
      Expression expression;
    } expression;
  };
};

struct s_Expression {
  int level;
  AstNode node;
  union {
    struct {
      Expression left;
      Expression right;
      Token operator;
    } binary;
    struct {
      Expression expression;
    } bracket;
    Identifier identifier;
    Literal literal;
  };
};

struct s_Identifier {
  AstNode node;
  Token raw;
};

struct s_Literal {
  AstNode node;
  Token raw;
};

struct s_Program {
  AstNode node;
  List body;
};

AstContext pushAstContext();
void popAstContext(AstContext ctx);

AstNode AstNode_create();
void AstNode_dispose(AstNode node);

Program Program_create();
void Program_dispose(Program program);
Program readProgram(SourceFile file, cstring source);

Program parse(SourceFile file);

Statement Statement_create();
void Statement_dispose(Statement statement);
Statement readStatement(SourceFile file, cstring source);

Statement BlockStatement_create();
void BlockStatement_dispose(Statement statement);
Statement readBlockStatement(SourceFile file, cstring source);

Statement EmptyStatement_create();
void EmptyStatement_dispose(Statement statement);
Statement readEmptyStatement(SourceFile file, cstring source);

Statement ExpressionStatement_create();
void ExpressionStatement_dispose(Statement statement);
Statement readExpressionStatement(SourceFile file, cstring source);

Expression Expression_create();
void Expression_dispose(Expression expression);

Expression readExpression(SourceFile file, cstring source);

Expression readIdentifierExpression(SourceFile file, cstring source);
Expression readBracketExpression(SourceFile file, cstring source);

void BinaryExpression_dispose(Expression expression);
void LiteralExpression_dispose(Expression expression);
void IdentifierExpression_dispose(Expression expression);
void BracketExpression_dispose(Expression expression);

Identifier Identifier_create();
Identifier readIdentifier(SourceFile file, cstring source);
void Identifier_dispose(Identifier identifier);

Literal Literal_create();
Literal readLiteral(SourceFile file, cstring source);
void Literal_dispose(Literal identifier);

JSON_Value JSON_fromProgram(Program program);
JSON_Value JSON_fromStatement(Statement statement);
JSON_Value JSON_fromBlockStatement(Statement statement);
JSON_Value JSON_fromEmptyStatement(Statement statement);
JSON_Value JSON_fromExpressionStatement(Statement statement);
JSON_Value JSON_fromExpression(Expression expression);
JSON_Value JSON_fromBinaryExpression(Expression expression);
JSON_Value JSON_fromBracketExpression(Expression expression);
JSON_Value JSON_fromIdentifierExpression(Expression expression);
JSON_Value JSON_fromLiteralExpression(Expression expression);
JSON_Value JSON_fromIdentifier(Identifier identifier);
JSON_Value JSON_fromLiteral(Literal literal);