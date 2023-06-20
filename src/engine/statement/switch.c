#include "statement.h"

static SwitchPattern readSwitchPattern(SourceFile file, cstring source) {}

static void SwitchPattern_dispose(SwitchPattern pattern) {
  if (pattern->condition) {
    Expression_dispose(pattern->condition);
  }
  List_dispose(pattern->body);
  Buffer_free(pattern);
}

int isSwitchStatement(SourceFile file, Token token) {
  return checkToken(token, TT_Keyword, "switch");
}
Statement readSwitchStatement(SourceFile file, cstring source) {
  Statement statement = Statement_create();
  statement->type = ST_Switch;
  statement->switchStatement.condition = NULL;
  List_Option opt = {1, (Buffer_Free)SwitchPattern_dispose};
  statement->switchStatement.patterns = List_create(opt);
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!checkToken(token, TT_Symbol, "(")) {
    pushError("Unexcept token. missing token '('",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  ExpressionContext ectx = pushExpressionContext();
  statement->switchStatement.condition = readExpression(file, selector);
  popExpressionContext(ectx);
  if (!statement->switchStatement.condition) {
    goto failed;
  }
  selector = statement->switchStatement.condition->node->position.end;
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!checkToken(token, TT_Symbol, ")")) {
    pushError("Unexcept token. missing token ')'",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!checkToken(token, TT_Symbol, "{")) {
    pushError("Unexcept token. missing token '{'",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!checkToken(token, TT_Symbol, "}")) {
    Token_dispose(token);
    
  }

  if (!checkToken(token, TT_Symbol, "}")) {
    pushError("Unexcept token. missing token '}'",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  statement->node->position.begin = source;
  statement->node->position.end = selector;
  return statement;
failed:
  Statement_dispose(statement);
  return NULL;
}