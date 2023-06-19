#include "expression.h"
int isArrayExpression(SourceFile file, Token token) {
  return checkToken(token, TT_Symbol, "[");
}
Expression readArrayExpression(SourceFile file, cstring source) {
  cstring selector = source;
  Expression expr = Expression_create();
  expr->type = ET_Array;
  expr->array = (Array)Buffer_alloc(sizeof(struct s_Array));
  List_Option opt = {1, (Buffer_Free)Expression_dispose};
  expr->array->items = List_create(opt);
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  token = readTokenSkipNewline(file, selector);
  if (!checkToken(token, TT_Symbol, "]")) {
    Token_dispose(token);
    ExpressionContext ectx = pushExpressionContext();
    setMaxOperatorLevel(12);
    Expression item = readExpression(file, selector);
    popExpressionContext(ectx);
    if (!item) {
      goto failed;
    }
    for (;;) {
      List_insert_tail(expr->array->items, item);
      selector = item->node->position.end;
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (checkToken(token, TT_Symbol, ",")) {
        selector = token->raw.end;
      }
      Token_dispose(token);
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (checkToken(token, TT_Symbol, "]")) {
        break;
      }
      Token_dispose(token);
      ExpressionContext ectx = pushExpressionContext();
      setMaxOperatorLevel(12);
      item = readExpression(file, selector);
      popExpressionContext(ectx);
      if (!item) {
        goto failed;
      }
    }
  }
  if (!checkToken(token, TT_Symbol, "]")) {
    pushError("Unexcept token.missing token ']'",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  expr->node->position.begin = source;
  expr->node->position.end = selector;
  return expr;
failed:
  Expression_dispose(expr);
  return NULL;
}