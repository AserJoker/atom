#include "parser/statement.h"
int isReturnStatement(SourceFile file, Token token) {
  return checkToken(token, TT_Keyword, "return");
}
Statement readReturnStatement(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  selector = token->raw.end;
  Token_dispose(token);
  token = readTokenSkipComment(file, selector);
  if (!token) {
    return NULL;
  }
  Statement statement = Statement_create();
  statement->type = ST_Return;
  if (token->type != TT_Newline && token->type != TT_MultiLineComment &&
      !checkToken(token, TT_Symbol, ";") && token->type != TT_Eof) {

    ExpressionContext ectx = pushExpressionContext();
    statement->expression = readExpression(file, selector);
    popExpressionContext(ectx);
    if (!statement->expression) {
      Buffer_free(statement);
      return NULL;
    }
    selector = statement->expression->node->position.end;
  }
  Token_dispose(token);
  statement->node->position.begin = source;
  statement->node->position.end = selector;
  return statement;
}