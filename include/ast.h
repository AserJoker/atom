#pragma once
#include "json.h"
#include "source.h"
#include "strings.h"

struct s_Token;
typedef struct s_Token *Token;

typedef enum e_TokenType {
  TT_Number,
  TT_BigInt,
  TT_String,
  TT_Keyword,
  TT_Symbol,
  TT_Identifier,
  TT_Regex,
  TT_Comment,
  TT_Eof,
  TT_Template,
  TT_TemplateStart,
  TT_TemplatePart,
  TT_TemplateEnd,
  TT_Newline,
  TT_Interpreter,
  TT_Init
} TokenType;

struct s_Token {
  Location _location;
  strings _raw;
  TokenType _type;
};

typedef struct s_Error {
  cstring error;
  Location location;
} Error;
struct s_SourceFile;
typedef struct s_SourceFile *SourceFile;
SourceFile SourceFile_read(cstring filename);
void SourceFile_dispose(SourceFile sf);
Token readToken(SourceFile file, cstring source);
void Token_dispose(Token token);
void initTokenizerContext();
void uninitTokenizerContext();
Error getTokenizerError();

struct s_Identifier;
typedef struct s_Identifier *Identifier;

struct s_Literal;
typedef struct s_Literal *Literal;

struct s_ExpressionStatement;
typedef struct s_ExpressionStatement *ExpressionStatement;

struct s_BlockStatement;
typedef struct s_BlockStatement *BlockStatement;

struct s_ImportAttribute;
typedef struct s_ImportAttribute *ImportAttribute;

struct s_ImportSpecifier;
typedef struct s_ImportSpecifier *ImportSpecifier;

struct s_ImportStatement;
typedef struct s_ImportStatement *ImportStatement;

struct s_ExpressionStatement;
typedef struct s_ExpressionStatement *ExpressionStatement;

struct s_Statement;
typedef struct s_Statement *Statement;

struct s_AstNode;
typedef struct s_AstNode *AstNode;

struct s_Interpreter;
typedef struct s_Interpreter *Interpreter;

struct s_Directive;
typedef struct s_Directive *Directive;

struct s_Program;
typedef struct s_Program *Program;

struct s_Expression;
typedef struct s_Expression *Expression;

typedef enum e_NodeType {
  // Program
  NT_Interpreter,
  NT_Directive,
  NT_Program,
  //--Program

  // Identifier
  NT_Identifier,
  // --Identifier

  // Literal
  NT_StringLiteral,
  NT_NumericLiteral,
  NT_BooelanLiteral,
  NT_NullLiteral,
  NT_UndefinedLiteral,
  NT_RegExLiteral,
  NT_BigIntLiteral,
  //--Literal

  // Import
  NT_ImportSpecifier,
  NT_ImportDefaultSpecifier,
  NT_ImportNamespaceSpecifier,
  NT_ImportAttribute,
  NT_ImportStatement,
  //--Import

  // Block
  NT_BlockStatement,
  // --Block

  // Empty
  NT_EmptyStatement,
  //--Empty

  // Expression
  NT_ExpressionStatement,

  NT_BinaryExpression,
  NT_BracketExpression,
  NT_LiteralExpression,
  NT_IdentifierExpression,

  NT_CallExpression,
  NT_TemplateException,
  //--Expression

} NodeType;

struct s_AstNode {
  NodeType _type;
  strings _position;
};

struct s_Identifier {
  AstNode _node;
  Token _raw;
};

struct s_Expression {
  AstNode _node;
  Expression _left;
  Expression _right;
  Token _operator;
  Identifier _identifier;
  Literal _literal;
  int _level;
};

struct s_ExpressionStatement {
  AstNode _node;
  Expression _expression;
};

struct s_BlockStatement {
  AstNode _node;
  List _body;
};

struct s_Literal {
  AstNode _node;
  Token _raw;
};

struct s_ImportAttribute {
  AstNode _node;
  Identifier _key;
  Literal _value;
};

struct s_ImportSpecifier {
  AstNode _node;
  Identifier _local;
  Identifier _imported;
};

struct s_ImportStatement {
  AstNode _node;
  Literal _source;
  List _specifiers; // ImportSpecifier[]
  List _assertions; // ImportAttribute[]
};

struct s_Statement {
  AstNode _node;
};
struct s_Interpreter {
  AstNode _node;
  strings _interpreter;
};

struct s_Directive {
  AstNode _node;
  strings _directive;
};

struct s_Program {
  AstNode _node;
  Interpreter _interpreter;
  List _directives; // Directive[]
  List _body;       // Statement[]
};

AstNode parse(SourceFile file);
JSON_Value JSON_fromProgram(Program program);