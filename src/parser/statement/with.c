#include "parser/statement.h"
int isWithStatement(SourceFile file, Token token) {
  return checkToken(token, TT_Keyword, "with");
}
Statement readWithStatement(SourceFile file, cstring source) {
  cstring selector = source;
  Statement statement = Statement_create();
  statement->type = ST_With;
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
  statement->with.obj = readExpression(file, selector);
  popExpressionContext(ectx);
  if (!statement->with.obj) {
    goto failed;
  }
  selector = statement->with.obj->node->position.end;
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
  statement->with.body = readBlockStatement(file, selector);
  if (!statement->with.body) {
    goto failed;
  }
  selector = statement->with.body->node->position.end;
  statement->node->position.begin = source;
  statement->node->position.end = selector;
  return statement;
failed:
  Statement_dispose(statement);
  return NULL;
}