#pragma once
#include "json.h"
#include "tokenizer.h"
struct s_AstContext;
typedef struct s_AstContext *AstContext;

struct s_AstContext {
  TokenContext tContext;
  int isCommaTail;
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

struct s_Lambda;
typedef struct s_Lambda *Lambda;

struct s_Function;
typedef struct s_Function *Function;

struct s_Template;
typedef struct s_Template *Template;

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
  NT_ComputeExpression,
  NT_LambdaExpression,
  NT_FunctionExpression,
  NT_CallExpression,
  NT_OptionalCallExpression,
  NT_OptionalComputeExpression,
  NT_TemplateExpression,

  // Template
  NT_Template,

  // Identifier
  NT_Identifier,

  // Literal
  NT_Literal,

  // Lambda and function
  NT_Lambda,
  NT_Function,
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
      Expression left;
      Expression right;
    } compute;

    struct {
      Expression callee;
      List args;
    } call;
    struct {
      Expression expression;
    } bracket;
    Identifier identifier;
    Literal literal;
    Lambda lambda;
    Function function;
    Template template;
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

struct s_Lambda {
  AstNode node;
  List args; // Identifier define
  Statement body;
  int async;
};

struct s_Function {
  AstNode node;
  List args;
  Identifier name;
  Statement body;
  int async;
};

struct s_Template {
  Expression tag;
  AstNode node;
  List datas;
  List parts;
};

AstContext pushAstContext();
void popAstContext(AstContext ctx);
int isCommaTail();
void enableCommaTail();
void disableCommaTail();

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
Expression readComputeExpression(SourceFile file, cstring source);
Expression readLiteralExpression(SourceFile file, cstring source);
Expression readLambdaExpression(SourceFile file, cstring source);
Expression readFunctionExpression(SourceFile file, cstring source);
Expression readCallExpression(SourceFile file, cstring source);
Expression readTemplateExpression(SourceFile file, cstring source);

void BinaryExpression_dispose(Expression expression);
void LiteralExpression_dispose(Expression expression);
void IdentifierExpression_dispose(Expression expression);
void BracketExpression_dispose(Expression expression);
void ComputeExpression_dispose(Expression expr);
void LambdaExpression_dispose(Expression expr);
void FunctionExpression_dispose(Expression expr);
void CallExpression_dispose(Expression expression);
void TemplateExpression_dispose(Expression expression);

Identifier Identifier_create();
Identifier readIdentifier(SourceFile file, cstring source);
void Identifier_dispose(Identifier identifier);

int isLiteralToken(Token token);
Literal Literal_create();
Literal readLiteral(SourceFile file, cstring source);
void Literal_dispose(Literal identifier);

Lambda Lambda_create();
Lambda readLambda(SourceFile file, cstring source);
void Lambda_dispose(Lambda lambda);

Function Function_create();
Function readFunction(SourceFile file, cstring source);
void Function_dispose(Function function);

Template Template_create();
Template readTemplate(SourceFile file, cstring source);
void Template_dispose(Template template);

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
JSON_Value JSON_fromComputeExpression(Expression expression);
JSON_Value JSON_fromLambdaExpression(Expression expression);
JSON_Value JSON_fromFunctionExpression(Expression expression);
JSON_Value JSON_fromCallExpression(Expression expression);
JSON_Value JSON_fromOptionalCallExpression(Expression expression);
JSON_Value JSON_fromOptionalComputeExpression(Expression expression);
JSON_Value JSON_fromTemplateExpression(Expression expression);

JSON_Value JSON_fromIdentifier(Identifier identifier);
JSON_Value JSON_fromLiteral(Literal literal);
JSON_Value JSON_fromLambda(Lambda lambda);
JSON_Value JSON_fromFunction(Function lambda);
JSON_Value JSON_fromTemplate(Template template);