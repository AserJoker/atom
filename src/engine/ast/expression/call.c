#include "ast.h"
Expression CallExpression_create() {
  Expression call_expr = Expression_create();
  call_expr->node->type = NT_CallExpression;
  call_expr->level = -2;
  List_Option opt = {1, (Buffer_Free)Expression_dispose};
  call_expr->call.args = List_create(opt);
  return call_expr;
}
void CallExpression_dispose(Expression expression) {
  List_dispose(expression->call.args);
  if (expression->call.callee) {
    Expression_dispose(expression->call.callee);
  }
  AstNode_dispose(expression->node);
  Buffer_free(expression);
}
Expression readCallExpression(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  if (!Token_check(token, TT_Symbol, "(")) {
    Token_dispose(token);
    ErrorStack_push(Error_init("Unexcept token.missing token '('",
                               getLocation(file, source), NULL));
    return NULL;
  }
  cstring selector = token->raw.end;
  Token_dispose(token);
  Expression call_expr = CallExpression_create();
  token = readTokenSkipNewline(file, selector);
  if (!Token_check(token, TT_Symbol, ")")) {
    Token_dispose(token);
    AstContext current = pushAstContext();
    enableCommaTail();
    Expression arg = readExpression(file, selector);
    popAstContext(current);
    if (!arg) {
      goto failed;
    }
    for (;;) {
      List_insert_tail(call_expr->call.args, arg);
      selector = arg->node->position.end;
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (Token_check(token, TT_Symbol, ",")) {
        selector = token->raw.end;
        Token_dispose(token);
        AstContext current = pushAstContext();
        enableCommaTail();
        arg = readExpression(file, selector);
        popAstContext(current);
        if (!arg) {
          goto failed;
        }
      } else if (Token_check(token, TT_Symbol, ")")) {
        break;
      } else {
        Token_dispose(token);
        ErrorStack_push(Error_init("Unexcept token.missing token ')'",
                                   getLocation(file, selector), NULL));
        goto failed;
      }
    }
  }
  selector = token->raw.end;
  Token_dispose(token);
  call_expr->node->position.begin = source;
  call_expr->node->position.end = selector;
  return call_expr;
failed:
  CallExpression_dispose(call_expr);
  return NULL;
}