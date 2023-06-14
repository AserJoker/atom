#include "expression.h"
#include "statement.h"
int isFunctionExpression(SourceFile file, Token token) {
  if (checkToken(token, TT_Keyword, "async")) {
    Token next = readTokenSkipNewline(file, token->raw.end);
    if (!next) {
      return 0;
    }
    if (checkToken(next, TT_Keyword, "function")) {
      Token_dispose(next);
      return 1;
    }
    Token_dispose(next);
    return 0;
  }
  if (checkToken(token, TT_Keyword, "function")) {
    return 1;
  }
  return 0;
}
Expression readFunctionExpression(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  Expression expr = Expression_create();
  expr->function = (Function)Buffer_alloc(sizeof(struct s_Function));
  expr->function->name = NULL;
  expr->function->body = NULL;
  expr->function->async = 0;
  expr->function->generator = 0;
  expr->type = ET_Function;
  List_Option opt = {1, (Buffer_Free)Expression_dispose};
  expr->function->args = List_create(opt);
  cstring selector = source;
  if (checkToken(token, TT_Keyword, "async")) {
    expr->function->async = 1;
    selector = token->raw.end;
    Token_dispose(token);
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (!checkToken(token, TT_Keyword, "function")) {
    pushError("Unexcept token.missing token 'function'",
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
  if (checkToken(token, TT_Symbol, "*")) {
    expr->function->generator = 1;
    selector = token->raw.end;
    Token_dispose(token);
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (token->type == TT_Identifier) {
    expr->function->name = token;
    selector = token->raw.end;
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
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
      List_insert_tail(expr->function->args, arg);
      selector = arg->node->position.end;
      token = readTokenSkipNewline(file, selector);
      if (checkToken(token, TT_Symbol, ",")) {
        selector = token->raw.end;
        Token_dispose(token);
      } else {
        break;
      }
      ExpressionContext ectx = pushExpressionContext();
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
  expr->function->body = readBlockStatement(file, selector);
  if (!expr->function->body) {
    goto failed;
  }
  selector = expr->function->body->node->position.end;
  expr->node->position.begin = source;
  expr->node->position.end = selector;
  return expr;
failed:
  Expression_dispose(expr);
  return NULL;
}