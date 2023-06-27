#include "parser/statement.h"

int isIfStatement(SourceFile file, Token token) {
  return checkToken(token, TT_Keyword, "if");
}
Statement readIfStatement(SourceFile file, cstring source) {
  cstring selector = source;
  Statement statement = Statement_create();
  statement->type = ST_If;
  statement->ifStatement.alternate = NULL;
  statement->ifStatement.condition = NULL;
  statement->ifStatement.consequent = NULL;
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
    pushError("Unexcept token.missing token '('",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  ExpressionContext ectx = pushExpressionContext();
  statement->ifStatement.condition = readExpression(file, selector);
  popExpressionContext(ectx);
  if (!statement->ifStatement.condition) {
    goto failed;
  }
  selector = statement->ifStatement.condition->node->position.end;
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!checkToken(token, TT_Symbol, ")")) {
    pushError("Unexcept token.missing token ')'",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  statement->ifStatement.consequent = readStatement(file, selector);
  if (!statement->ifStatement.consequent) {
    goto failed;
  }
  selector = statement->ifStatement.consequent->node->position.end;
  token = readTokenSkipNewline(file, selector);
  if (checkToken(token, TT_Keyword, "else")) {
    selector = token->raw.end;
    statement->ifStatement.alternate = readStatement(file, selector);
    if (!statement->ifStatement.alternate) {
      goto failed;
    }
    selector = statement->ifStatement.alternate->node->position.end;
  }
  Token_dispose(token);
  statement->node->position.begin = source;
  statement->node->position.end = selector;
  return statement;
failed:
  Statement_dispose(statement);
  return NULL;
}