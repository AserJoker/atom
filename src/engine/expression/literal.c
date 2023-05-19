#include "ast.h"
Expression readLiteralExpression(SourceFile file, cstring source) {
  Literal literal = readLiteral(file, source);
  if (!literal) {
    return NULL;
  }
  Expression literal_expr = Expression_create();
  literal_expr->_literal = literal;
  literal_expr->_node->_position = literal->_node->_position;
  literal_expr->_node->_type = NT_LiteralExpression;
  return literal_expr;
}
int isLiteralToken(Token token) {
  if (token->_type == TT_Number || token->_type == TT_String ||
      token->_type == TT_BigInt || token->_type == TT_Regex) {
    return 1;
  }
  if (token->_type == TT_Keyword) {
    if (strings_is(token->_raw, "true") || strings_is(token->_raw, "false") ||
        strings_is(token->_raw, "null") ||
        strings_is(token->_raw, "undefined")) {
      return 1;
    }
  }
  return 0;
}