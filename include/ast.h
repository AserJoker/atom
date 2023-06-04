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

struct s_Array;
typedef struct s_Array *Array;

struct s_ObjectProperty;
typedef struct s_ObjectProperty *ObjectProperty;

struct s_Object;
typedef struct s_Object *Object;

struct s_StaticBlock;
typedef struct s_StaticBlock *StaticBlock;

struct s_ClassProperty;
typedef struct s_ClassProperty *ClassProperty;

struct s_Class;
typedef struct s_Class *Class;

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
  NT_AwaitExpression,
  NT_NewExpression,
  NT_DeleteExpression,
  NT_ArrayPattern,
  NT_ObjectPattern,
  NT_ClassPattern,

  // Template
  NT_Template,

  // Identifier
  NT_Identifier,

  // Literal
  NT_Literal,

  // Lambda and function
  NT_Lambda,
  NT_Function,

  // Array
  NT_Array,

  // Object
  NT_ObjectProperty,
  NT_Getter,
  NT_Setter,
  NT_Object,

  // Class
  NT_Class,
  NT_ClassProperty,
  NT_StaticBlock,
  NT_Private,
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
    struct {
      Expression nil;
      Expression expression;
    } await, newexpr, deleteexpr;
    Identifier identifier;
    Literal literal;
    Lambda lambda;
    Function function;
    Template template;
    Array array;
    Object object;
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
  Expression name;
  Statement body;
  int async;
};

struct s_Template {
  AstNode node;
  Expression tag;
  List datas;
  List parts;
};

struct s_Array {
  AstNode node;
  List items;
};

struct s_ObjectProperty {
  AstNode node;
  Expression key;
  Expression value;
};
struct s_Object {
  AstNode node;
  List properties;
};

struct s_Class {
  AstNode node;
  List decorators;
  List properties;
};

struct s_ClassProperty {
  AstNode node;
  List decorators;
  union {
    struct {
      Expression key;
      Expression init;
    } field;
    List staticblock;
    Identifier privite;
    Function method;
    Class clazz;
  };
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
Expression readAsyncExpression(SourceFile file, cstring source);
Expression readAwaitExpression(SourceFile file, cstring source);
Expression readNewExpression(SourceFile file, cstring source);
Expression readArrayPattern(SourceFile file, cstring source);
Expression readObjectPattern(SourceFile file, cstring source);
Expression readDeleteExpression(SourceFile file, cstring source);
Expression readClassPattern(SourceFile file, cstring source);

Function readFunctionDefinition(Function func, SourceFile file, cstring source);

void BinaryExpression_dispose(Expression expression);
void LiteralExpression_dispose(Expression expression);
void IdentifierExpression_dispose(Expression expression);
void BracketExpression_dispose(Expression expression);
void ComputeExpression_dispose(Expression expr);
void LambdaExpression_dispose(Expression expr);
void FunctionExpression_dispose(Expression expr);
void CallExpression_dispose(Expression expression);
void TemplateExpression_dispose(Expression expression);
void AwaitExpression_dispose(Expression expression);
void ArrayPattern_dispose(Expression expression);
void ObjectPattern_dispose(Expression expression);
void NewExpression_dispose(Expression expr);
void DeleteExpression_dispose(Expression expr);
void ClassPattern_dispose(Expression expr);

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

Array Array_create();
Array readArray(SourceFile file, cstring source);
void Array_dispose(Array array);

Object Object_create();
Object readObject(SourceFile file, cstring source);
void Object_dispose(Object obj);

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
JSON_Value JSON_fromAwaitExpression(Expression expression);
JSON_Value JSON_fromNewExpression(Expression expression);
JSON_Value JSON_fromDeleteExpression(Expression expression);
JSON_Value JSON_fromArrayPattern(Expression expression);
JSON_Value JSON_fromObjectPattern(Expression expression);
JSON_Value JSON_fromClassPattern(Expression expression);

JSON_Value JSON_fromIdentifier(Identifier identifier);
JSON_Value JSON_fromLiteral(Literal literal);
JSON_Value JSON_fromLambda(Lambda lambda);
JSON_Value JSON_fromFunction(Function lambda);
JSON_Value JSON_fromTemplate(Template template);
JSON_Value JSON_fromArray(Array array);
JSON_Value JSON_fromObject(Object object);
JSON_Value JSON_fromObjectProperty(ObjectProperty object);
