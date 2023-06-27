#include "parser/expression.h"
#include "util/source.h"
#include "util/strings.h"
#include "parser/tokenizer.h"
int isCallExpression(SourceFile file, Token token) {
  return checkToken(token, TT_Symbol, "(");
}
Expression readCallExpression(SourceFile file, cstring source) {
  Expression expr = Expression_create();
  expr->type = ET_Call;
  expr->bind = BT_Left;
  List_Option opt = {1, (Buffer_Free)Expression_dispose};
  expr->call.args = List_create(opt);
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
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
  token = readTokenSkipNewline(file, selector);
  if (!checkToken(token, TT_Symbol, ")")) {
    Token_dispose(token);
    ExpressionContext ectx = pushExpressionContext();
    setMaxOperatorLevel(12);
    Expression arg = readExpression(file, selector);
    popExpressionContext(ectx);
    if (!arg) {
      goto failed;
    }
    for (;;) {
      List_insert_tail(expr->call.args, arg);
      selector = arg->node->position.end;
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (checkToken(token, TT_Symbol, ",")) {
        selector = token->raw.end;
        Token_dispose(token);
      } else {
        break;
      }
      ectx = pushExpressionContext();
      setMaxOperatorLevel(12);
      arg = readExpression(file, selector);
      popExpressionContext(ectx);
      if (!arg) {
        goto failed;
      }
    }
  }
  if (!checkToken(token, TT_Symbol, ")")) {
    pushError("Unexcept token.missing token ')'",
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