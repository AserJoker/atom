#include "ast.h"
Expression readAsyncExpression(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  if (!Token_check(token, TT_Keyword, "async")) {
    Token_dispose(token);
    ErrorStack_push(Error_init("Unexcept token.missing token 'async'",
                               getLocation(file, source), NULL));
    return NULL;
  }
  cstring selector = token->raw.end;
  Token_dispose(token);
  Token next = readTokenSkipNewline(file, selector);
  if (!next) {
    return NULL;
  }
  Expression current_expr = NULL;
  if (Token_check(next, TT_Symbol, "(")) {
    current_expr = readLambdaExpression(file, selector);
  } else if (Token_check(next, TT_Keyword, "function")) {
    current_expr = readFunctionExpression(file, selector);
  } else {
    Token_dispose(next);
    ErrorStack_push(
        Error_init("keyword 'async' must before lambda or function expression",
                   getLocation(file, selector), NULL));
    return NULL;
  }
  Token_dispose(next);
  return current_expr;
}