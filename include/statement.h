#pragma once
#include "ast.h"
#include "expression.h"
typedef struct s_Statement *Statement;
typedef enum e_StatementType {
  ST_Unknown,
  ST_Block,
  ST_Empty,
  ST_Expression,
  ST_Assignment,
  ST_Return,
  ST_Yield,
  ST_Label,
  ST_Break,
  ST_Continue,
  ST_While,
  ST_If,
  ST_Switch,
  ST_For,
  ST_ForIn,
  ST_ForOf,
  ST_Import,
  ST_Export,
  ST_With,
  ST_DoWhile,
} StatementType;

typedef struct s_SwitchPattern {
  Expression condition;
  List body;
  strings position;
} *SwitchPattern;
struct s_Statement {
  AstNode node;
  StatementType type;
  union {
    struct {
      List body;
    } block;
    Expression expression;
    Token label;
    struct {
      enum { AT_Unknown, AT_Const, AT_Let, AT_Var } type;
      Expression expression;
    } assignement;

    struct {
      Expression condition;
      Statement body;
    } whileStatement;
    struct {
      Token label;
      Statement body;
    } labelStatement;
    struct {
      Expression condition;
      Statement consequent;
      Statement alternate;
    } ifStatement;
    struct {
      Expression condition;
      List patterns;
    } switchStatement;

    struct {
      Expression obj;
      Statement body;
    } with;

    struct {
      List exports;
      int isDefault;
    } export;

    struct {
      Statement init;
      Expression condition;
      Statement after;
    } forStatement;
    struct {
      Statement define;
      Expression object;
    } forIn, forOf;
    struct {
      List patterns;
      Token source;
      List asserts;
    } import;
  };
};
Statement Statement_create();
Statement readStatement(SourceFile file, cstring source);
void Statement_dispose(Statement statement);

int isEmptyStatement(SourceFile file, Token token);
Statement readEmptyStatement(SourceFile file, cstring source);

int isBlockStatement(SourceFile file, Token token);
Statement readBlockStatement(SourceFile file, cstring source);

int isExpressionStatement(SourceFile file, Token token);
Statement readExpressionStatement(SourceFile file, cstring source);

int isAssignmentStatement(SourceFile file, Token token);
Statement readAssignmentStatement(SourceFile file, cstring source);

int isReturnStatement(SourceFile file, Token token);
Statement readReturnStatement(SourceFile file, cstring source);

int isYieldStatement(SourceFile file, Token token);
Statement readYieldStatement(SourceFile file, cstring source);

int isBreakStatement(SourceFile file, Token token);
Statement readBreakStatement(SourceFile file, cstring source);

int isContinueStatement(SourceFile file, Token token);
Statement readContinueStatement(SourceFile file, cstring source);

int isWhileStatement(SourceFile file, Token token);
Statement readWhileStatement(SourceFile file, cstring source);

int isLabelStatement(SourceFile file, Token token);
Statement readLabelStatement(SourceFile file, cstring source);

int isIfStatement(SourceFile file, Token token);
Statement readIfStatement(SourceFile file, cstring source);

int isSwitchStatement(SourceFile file, Token token);
Statement readSwitchStatement(SourceFile file, cstring source);

int isExportStatement(SourceFile file, Token token);
Statement readExportStatement(SourceFile file, cstring source);

int isWithStatement(SourceFile file, Token token);
Statement readWithStatement(SourceFile file, cstring source);