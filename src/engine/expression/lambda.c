#include "ast.h"
LambdaPattern LambdaPattern_create() {
  LambdaPattern pattern =
      (LambdaPattern)Buffer_alloc(sizeof(struct s_LambdaPattern));
  List_Option opt = {1, (Buffer_Free)Expression_dispose};
  pattern->_args = List_create(opt);
  pattern->_async = 0;
  pattern->_body = 0;
  return pattern;
}
void LambdaPattern_dispose(LambdaPattern pattern) {
  if (pattern->_args) {
    List_dispose(pattern->_args);
  }
  if (pattern->_body) {
    Statement_dispose(pattern->_body);
  }
  Buffer_free(pattern);
}
Expression readLambdaPattern(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  LambdaPattern lambda = LambdaPattern_create();
  if (!checkToken(token, TT_Symbol, "(")) {
    Token_dispose(token);
    LambdaPattern_dispose(lambda);
    return NULL;
  }
  selector = token->_raw.end;
  Token_dispose(token);
  Context *current = pushContext();
  enableCommaTail();
  Expression arg = readExpression(file, selector);
  while (arg) {
    List_insert_tail(lambda->_args, arg);
    selector = arg->_node->_position.end;
    Token token = readTokenSkipNewline(file, selector);
    if (!token) {
      LambdaPattern_dispose(lambda);
      return NULL;
    }
    if (!checkToken(token, TT_Symbol, ",")) {
      Token_dispose(token);
      break;
    } else {
      selector = token->_raw.end;
      Token_dispose(token);
    }
    arg = readExpression(file, selector);
  }
  disableCommaTail();
  popContext(current);
  if (getAstError().error) {
    LambdaPattern_dispose(lambda);
    return NULL;
  }
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    LambdaPattern_dispose(lambda);
    return NULL;
  }
  if (!checkToken(token, TT_Symbol, ")")) {
    Token_dispose(token);
    LambdaPattern_dispose(lambda);
    Error error = {"Unexcept token:missing token ')'",
                   getLocation(file, selector)};
    setAstError(error);
    return NULL;
  } else {
    selector = token->_raw.end;
    Token_dispose(token);
  }
  token = readTokenSkipComment(file, selector);
  if (!token) {
    LambdaPattern_dispose(lambda);
    return NULL;
  }
  if (!checkToken(token, TT_Symbol, "=>")) {
    Token_dispose(token);
    LambdaPattern_dispose(lambda);
    return NULL;
  } else {
    selector = token->_raw.end;
    Token_dispose(token);
  }
  lambda->_body = readStatement(file, selector);
  if (!lambda->_body) {
    LambdaPattern_dispose(lambda);
    if (!getAstError().error) {
      Error error = {"Unexcept token", getLocation(file, selector)};
      setAstError(error);
    }
    return NULL;
  }
  selector = lambda->_body->_node->_position.end;
  Expression lambda_expr = Expression_create();
  lambda_expr->_node->_type = NT_LambdaPattern;
  lambda_expr->_node->_position.begin = source;
  lambda_expr->_node->_position.end = selector;
  lambda_expr->_pattern._lambda = lambda;
  lambda_expr->_level = -2;
  return lambda_expr;
}