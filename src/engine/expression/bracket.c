#include "ast.h"
Expression readBracketExpression(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  if (!checkToken(token, TT_Symbol, "(")) {
    Token_dispose(token);
    return NULL;
  }
  selector = token->_raw.end;
  Token_dispose(token);
  Context *current = pushContext();
  Expression sub_expr = readExpression(file, selector);
  popContext(current);
  if (sub_expr) {
    selector = sub_expr->_node->_position.end;
  }
  Token next = readTokenSkipNewline(file, selector);
  if (!next) {
    if(sub_expr){
      Expression_dispose(sub_expr);
    }
    return NULL;
  }
  if (!checkToken(next, TT_Symbol, ")")) {
    if(sub_expr){
      Expression_dispose(sub_expr);
    }
    Token_dispose(next);
    Error error = {"Unexcept token: missing token ')'",
                   getLocation(file, selector)};
    setAstError(error);
    return NULL;
  }
  selector = next->_raw.end;
  Token_dispose(next);
  Expression brackets = Expression_create();
  brackets->_node->_type = NT_BracketExpression;
  brackets->_left = sub_expr;
  brackets->_node->_position.begin = source;
  brackets->_node->_position.end = selector;
  return brackets;
}