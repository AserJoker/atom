#include "ast.h"
Expression readIdentifierExpression(SourceFile file, cstring source) {
  Identifier identifier = readIdentifier(file, source);
  if (!identifier) {
    Error error = {"Unexcept error", getLocation(file, source)};
    setAstError(error);
    return NULL;
  }
  Expression identifier_expr = Expression_create();
  identifier_expr->_identifier = identifier;
  identifier_expr->_node->_type = NT_IdentifierExpression;
  identifier_expr->_node->_position.begin = source;
  identifier_expr->_node->_position.end = identifier->_node->_position.end;
  return identifier_expr;
}