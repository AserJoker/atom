#include "ast.h"
Expression readComputeExpression(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  if (!checkToken(token, TT_Symbol, "[")) {
    Token_dispose(token);
    Error error = {"Unexcept token", getLocation(file, selector)};
    setAstError(error);
    return NULL;
  }
  selector = token->_raw.end;
  Token_dispose(token);
  Expression arg = readExpression(file, selector);
  if (!arg) {
    return NULL;
  }
  selector = arg->_node->_position.end;
  Token next = readTokenSkipNewline(file, selector);
  if (!next) {
    Expression_dispose(arg);
    return NULL;
  }
  if (!checkToken(next, TT_Symbol, "]")) {
    Expression_dispose(arg);
    Token_dispose(next);
    Error error = {"Unexcept token: missing token ']'",
                   getLocation(file, selector)};
    setAstError(error);
    return NULL;
  }
  selector = next->_raw.end;
  Token_dispose(next);
  Expression compute_expr = Expression_create();
  compute_expr->_right = arg;
  compute_expr->_node->_type = NT_ComputeExpression;
  compute_expr->_node->_position.begin = source;
  compute_expr->_node->_position.end = selector;
  return compute_expr;
}