#include "ast.h"
ArrayPattern ArrayPattern_create() {
  ArrayPattern pattern =
      (ArrayPattern)Buffer_alloc(sizeof(struct s_ArrayPattern));
  List_Option opt = {1, (Buffer_Free)Expression_dispose};
  pattern->_items = List_create(opt);
  return pattern;
}
void ArrayPattern_dispose(ArrayPattern pattern) {
  List_dispose(pattern->_items);
  Buffer_free(pattern);
}
Expression readArrayExpression(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  if (!checkToken(token, TT_Symbol, "[")) {
    Token_dispose(token);
    return NULL;
  }
  cstring selector = token->_raw.end;
  ArrayPattern pattern = ArrayPattern_create();
  Token_dispose(token);
  Context *context = pushContext();
  enableCommaTail();
  Expression item = readExpression(file, selector);
  while (item) {
    List_insert_tail(pattern->_items, item);
    selector = item->_node->_position.end;
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      ArrayPattern_dispose(pattern);
      popContext(context);
      return NULL;
    }
    if (!checkToken(token, TT_Symbol, ",")) {
      Token_dispose(token);
      break;
    }
    selector = token->_raw.end;
    Token_dispose(token);
    item = readExpression(file, selector);
  }
  disableCommaTail();
  popContext(context);
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    ArrayPattern_dispose(pattern);
    return NULL;
  }
  if (!checkToken(token, TT_Symbol, "]")) {
    Token_dispose(token);
    ArrayPattern_dispose(pattern);
    Error error = {"Unexcept token: missing token ']'",
                   getLocation(file, selector)};
    return NULL;
  }
  selector = token->_raw.end;
  Token_dispose(token);
  Expression array_expr = Expression_create();
  array_expr->_pattern._array = pattern;
  array_expr->_node->_type = NT_ArrayPattern;
  array_expr->_node->_position.begin = source;
  array_expr->_node->_position.end = selector;
  array_expr->_level = -2;
  return array_expr;
}