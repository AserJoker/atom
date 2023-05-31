#include "ast.h"
Lambda Lambda_create() {
  Lambda lambda = (Lambda)Buffer_alloc(sizeof(struct s_Lambda));
  List_Option opt = {1, (Buffer_Free)Expression_dispose};
  lambda->args = List_create(opt);
  lambda->body = NULL;
  lambda->async = 0;
  lambda->node = AstNode_create();
  lambda->node->type = NT_Lambda;
  return lambda;
}
void Lambda_dispose(Lambda lambda) {
  List_dispose(lambda->args);
  if (lambda->body) {
    Statement_dispose(lambda->body);
  }
  AstNode_dispose(lambda->node);
  Buffer_free(lambda);
}

Lambda readLambda(SourceFile file, cstring source) {

  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  if (!Token_check(token, TT_Symbol, "(")) {
    ErrorStack_push(
        Error_init("Exception token.", getLocation(file, source), NULL));
    Token_dispose(token);
    return NULL;
  }
  cstring selector = token->raw.end;
  Token_dispose(token);
  Lambda lambda = Lambda_create();
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    Token_dispose(token);
    goto failed;
  }
  if (!Token_check(token, TT_Symbol, ")")) {
    Token_dispose(token);
    AstContext ctx = pushAstContext();
    enableCommaTail();
    Expression arg = readExpression(file, selector);
    popAstContext(ctx);
    for (;;) {
      List_insert_tail(lambda->args, arg);
      selector = arg->node->position.end;
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (Token_check(token, TT_Symbol, ",")) {
        selector = token->raw.end;
        Token_dispose(token);
        AstContext ctx = pushAstContext();
        enableCommaTail();
        arg = readExpression(file, selector);
        popAstContext(ctx);
        if (!arg) {
          goto failed;
        }
      } else if (Token_check(token, TT_Symbol, ")")) {
        break;
      } else {
        Token_dispose(token);
        ErrorStack_push(
            Error_init("Unexcept token.", getLocation(file, selector), NULL));
        goto failed;
      }
    }
  }
  selector = token->raw.end;
  Token_dispose(token);
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!Token_check(token, TT_Symbol, "=>")) {
    Token_dispose(token);
    ErrorStack_push(Error_init("Unexcept token,missing token '=>'",
                               getLocation(file, selector), NULL));
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  Statement body = readStatement(file, selector);
  if (!body) {
    goto failed;
  }
  selector = body->node->position.end;
  lambda->body = body;
  lambda->node->position.begin = source;
  lambda->node->position.end = selector;
  return lambda;
failed:
  Lambda_dispose(lambda);
  return NULL;
}
Expression LambdaExpression_create() {
  Expression expr = Expression_create();
  expr->level = -2;
  expr->node->type = NT_LambdaExpression;
  return expr;
}
void LambdaExpression_dispose(Expression expr) {
  if (expr->lambda) {
    Lambda_dispose(expr->lambda);
  }
  AstNode_dispose(expr->node);
  Buffer_free(expr);
}
Expression readLambdaExpression(SourceFile file, cstring source) {
  Lambda lambda = readLambda(file, source);
  if (!lambda) {
    return NULL;
  }
  Expression lambda_expr = LambdaExpression_create();
  lambda_expr->lambda = lambda;
  lambda_expr->node->position = lambda->node->position;
  return lambda_expr;
}