#pragma once
#include "strings.h"
struct s_AstNode;
typedef struct s_AstNode *AstNode;
struct s_Token;
typedef struct s_Token *Token;
struct s_Position;
typedef struct s_Position Position;
struct s_Location;
typedef struct s_Location Location;
typedef enum e_NodeType {
  NT_Identifier,
  NT_PriviteName,
  NT_Literals,
  NT_Programs,
  NT_Functions,
  NT_Statements,
  NT_Declarations,
  NT_Misc,
  NT_Expressions,
  NT_TemplateLiterals,
  NT_Patterns,
  NT_Claaaes,
  NT_Moudles
} NodeType;
typedef enum e_LiteralType {
  LT_RegexLiteral,
  LT_NullLiteral,
  LT_StringLiteral,
  LT_BooelanLiteral,
  LT_BigIntLiteral,
  LT_DecimalLiteral
} LiteralType;
typedef enum e_StatementType {
  ST_ExpressionStatement,
  ST_BlockStatement,
  ST_EmptyStatement,
  ST_DebuggerStatement,
  ST_WithStatement,
  ST_Control_flow,
  ST_Choice,
  ST_Exceptions,
  ST_Loops,
} StatementType;
typedef enum e_DeclarationType {
  DT_FunctionDeclaration,
  DT_VariableDeclaration,
  DT_VD_VariableDeclarator
} DeclarationType;
typedef enum e_MiscType {
  MT_Decorator,
  MT_Directive,
  MT_DirectiveLiteral,
  MT_InterpreterDirective
} MiscType;
typedef enum e_ExpressionType {
  ET_Super,
  ET_Import,
  ET_ThisExpression,
  ET_ArrowFunctionExpression,
  ET_YieldExpression,
  ET_AwaitExpression,
  ET_ArrayExpression,
  ET_ObjectExpression,
  ET_OE_ObjectMember,
  ET_OE_ObjectProperty,
  ET_OE_ObjectMethod,
  ET_RecordExpression,
  ET_TupleExpression,
  ET_FunctionExpression,
  ET_UnaryOperations,
  ET_UO_UnaryExpression,
  ET_UO_UE_UnaryOperator,
  ET_UO_UpdateExpression,
  ET_UO_UE_UpdateOperator,
  ET_BinaryOperations,
  ET_BO_BinaryExpression,
  ET_BO_BE_BinaryOperator,
  ET_BO_AssignmentExpression,
  ET_BO_AE_AssignmentOperator,
  ET_BO_LogicalExpression,
  ET_BO_LE_LogicalOperator,
  ET_BO_SpreadElement,
  ET_BO_ArgumentPlaceholder,
  ET_BO_MemberExpression,
  ET_BO_OptionalMemberExpression,
  ET_BO_BindExpression,
  ET_ConditionalExpression,
  ET_CallExpression,
  ET_OptionalCallExpression,
  ET_NewExpression,
  ET_SequenceExpression,
  ET_ParenthesizedExpression,
  ET_DoExpression,
  ET_ModuleExpression
} ExpressionType;
typedef enum e_TemplateLiteralType {
  TLT_TemplateLiteral,
  TLT_TaggedTemplateExpression,
  TLT_TemplateElement
} TemplateLiteralType;
typedef enum e_PatternType {
  PT_ObjectPattern,
  PT_ArrayPattern,
  PT_RestElement,
  PT_AssignmentPattern
} PatternType;
typedef enum e_ClassType {
  CT_ClassBody,
  CT_ClassMethod,
  CT_ClassPrivateMethod,
  CT_ClassProperty,
  CT_ClassPrivateProperty,
  CT_StaticBlock,
  CT_ClassDeclaration,
  CT_ClassExpression,
  CT_MetaProperty
} ClassType;
typedef enum e_ModuleType {
  MT_ModuleSpecifier,
  MT_Imports,
  MT_I_ImportDeclaration,
  MT_I_ImportSpecifier,
  MT_I_ImportDefaultSpecifier,
  MT_I_ImportNamespaceSpecifier,
  MT_I_ImportAttribute,
  MT_Exports,
  MT_E_ExportDeclaration,
  MT_E_ExportNamedDeclaration,
  MT_E_ExportSpecifier,
  MT_E_ExportNamespaceSpecifier,
  MT_E_ExportDefaultDeclaration,
  MT_E_ExportAllDeclaration
} ModuleType;

typedef enum e_TokenType {
  TT_Number,
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
  TT_Init
} TokenType;

struct s_SourceFile;
typedef struct s_SourceFile *SourceFile;
SourceFile SourceFile_read(cstring filename);
void SourceFile_dispose(SourceFile sf);
AstNode parse(SourceFile file);