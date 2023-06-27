#include "parser/expression.h"
#include "parser/statement.h"
int isLambdaExpression(SourceFile file, Token token) {
  cstring selector = token->raw.begin;
  if (checkToken(token, TT_Keyword, "async")) {
    selector = token->raw.end;
  }
  token = readTokenSkipNewline(file, selector);
  if (token->type == TT_Identifier) {
    selector = token->raw.end;
    Token_dispose(token);
  } else if (checkToken(token, TT_Symbol, "(")) {
    Token_dispose(token);
    Token pair = pairToken(file, selector, TT_Symbol, "(", TT_Symbol, ")");
    if (!pair) {
      return 0;
    }
    selector = pair->raw.end;
    Token_dispose(pair);
  } else {
    Token_dispose(token);
    return 0;
  }
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    return 0;
  }
  if (checkToken(token, TT_Symbol, "=>")) {
    Token_dispose(token);
    return 1;
  }
  Token_dispose(token);
  return 0;
}
Expression readLambdaExpression(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  Expression expr = Expression_create();
  expr->type = ET_Lambda;
  expr->bind = BT_None;
  List_Option opt = {1, (Buffer_Free)Expression_dispose};
  expr->lambda = (Lambda)Buffer_alloc(sizeof(struct s_Lambda));
  expr->lambda->args = List_create(opt);
  if (checkToken(token, TT_Keyword, "async")) {
    expr->lambda->async = 1;
    selector = token->raw.end;
  }
  Token_dispose(token);
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (token->type == TT_Identifier) {
    Token_dispose(token);
    ExpressionContext ectx = pushExpressionContext();
    Expression arg = readIdentifierExpression(file, selector);
    popExpressionContext(ectx);
    if (!arg) {
      goto failed;
    }
    List_insert_tail(expr->lambda->args, arg);
    selector = arg->node->position.end;
  } else if (checkToken(token, TT_Symbol, "(")) {
    selector = token->raw.end;
    Token_dispose(token);
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
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
        List_insert_tail(expr->lambda->args, arg);
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
      Token_dispose(token);
      goto failed;
    } else {
      selector = token->raw.end;
      Token_dispose(token);
    }
  }
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!checkToken(token, TT_Symbol, "=>")) {
    pushError("Unexcept token.missing token '=>'",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  expr->lambda->body = readStatement(file, selector);
  if (!expr->lambda->body) {
    goto failed;
  }
  selector = expr->lambda->body->node->position.end;
  expr->node->position.begin = source;
  expr->node->position.end = selector;
  return expr;
failed:
  if (expr) {
    Expression_dispose(expr);
  }
  return NULL;
}