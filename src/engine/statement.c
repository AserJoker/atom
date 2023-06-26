#include "statement.h"

struct s_StatementHandle {
  int (*checker)(SourceFile file, Token token);
  Statement (*reader)(SourceFile file, cstring source);
};
static struct s_StatementHandle handles[] = {
    {isBlockStatement, readBlockStatement},
    {isEmptyStatement, readEmptyStatement},
    {isAssignmentStatement, readAssignmentStatement},
    {isReturnStatement, readReturnStatement},
    {isBreakStatement, readBreakStatement},
    {isContinueStatement, readContinueStatement},
    {isLabelStatement, readLabelStatement},
    {isWhileStatement, readWhileStatement},
    {isIfStatement, readIfStatement},
    {isSwitchStatement, readSwitchStatement},
    {isExportStatement, readExportStatement},
    {isWithStatement, readWithStatement},
    {isExpressionStatement, readExpressionStatement},
    {0, 0}};
Statement Statement_create() {
  Statement statement = (Statement)Buffer_alloc(sizeof(struct s_Statement));
  statement->node = AstNode_create();
  statement->type = ST_Unknown;
  statement->node->type = NT_Statement;
  statement->block.body = NULL;
  return statement;
}
Statement readStatement(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  Statement statement = NULL;
  int index = 0;
  for (;;) {
    struct s_StatementHandle handle = handles[index++];
    if (!handle.checker) {
      break;
    }
    if (handle.checker(file, token)) {
      Token_dispose(token);
      statement = handle.reader(file, selector);
      break;
    }
  }
  if (statement) {
    selector = statement->node->position.end;
    token = readTokenSkipNewline(file, selector);
    if (checkToken(token, TT_Symbol, ";")) {
      selector = token->raw.end;
    }
    Token_dispose(token);
    statement->node->position.end = selector;
  }
  return statement;
}
void Statement_dispose(Statement statement) {
  switch (statement->type) {

  case ST_Block:
    if (statement->block.body) {
      List_dispose(statement->block.body);
    }
    break;
  case ST_Return:
  case ST_Yield:
  case ST_Expression:
    if (statement->expression) {
      Expression_dispose(statement->expression);
    }
    break;
  case ST_Assignment:
    Expression_dispose(statement->assignement.expression);
    break;
  case ST_Continue:
  case ST_Break:
    if (statement->label) {
      Token_dispose(statement->label);
    }
    break;
  case ST_While:
    if (statement->labelStatement.label) {
      Expression_dispose(statement->whileStatement.condition);
    }
    if (statement->labelStatement.body) {
      Statement_dispose(statement->whileStatement.body);
    }
    break;
  case ST_Label:
    if (statement->labelStatement.label) {
      Token_dispose(statement->labelStatement.label);
    }
    if (statement->labelStatement.body) {
      Statement_dispose(statement->labelStatement.body);
    }
    break;
  case ST_If:
    if (statement->ifStatement.condition) {
      Expression_dispose(statement->ifStatement.condition);
    }
    if (statement->ifStatement.consequent) {
      Statement_dispose(statement->ifStatement.consequent);
    }
    if (statement->ifStatement.alternate) {
      Statement_dispose(statement->ifStatement.alternate);
    }
    break;
  case ST_Switch:
    if (statement->switchStatement.condition) {
      Expression_dispose(statement->switchStatement.condition);
    }
    if (statement->switchStatement.patterns) {
      List_dispose(statement->switchStatement.patterns);
    }
    break;
  case ST_Export:
    if (statement->export.exports) {
      List_dispose(statement->export.exports);
    }
    break;
  case ST_With:
    if (statement->with.obj) {
      Expression_dispose(statement->with.obj);
    }
    if (statement->with.body) {
      Statement_dispose(statement->with.body);
    }
    break;
  default:
    break;
  }
  AstNode_dispose(statement->node);
  Buffer_free(statement);
}