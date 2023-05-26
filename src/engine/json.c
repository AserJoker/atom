#include "json.h"
#include "ast.h"
JSON_Value JSON_fromBlockStatement(Statement statement) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("BlockStatement"));
  JSON_setField(
      obj, "body",
      JSON_fromList(statement->block.body, (ToJSON)JSON_fromStatement));
  return obj;
}
JSON_Value JSON_fromEmptyStatement(Statement statement) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("EmptyStatement"));
  return obj;
}
JSON_Value JSON_fromExpressionStatement(Statement statement) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("ExpressionStatement"));
  JSON_setField(obj, "expression",
                JSON_fromExpression(statement->expression.expression));
  return obj;
}
JSON_Value JSON_fromExpression(Expression expression) {
  if (expression->node->type == NT_BinaryExpression) {
    return JSON_fromBinaryExpression(expression);
  }
  if (expression->node->type == NT_BracketExpression) {
    return JSON_fromBracketExpression(expression);
  }
  if (expression->node->type == NT_IdentifierExpression) {
    return JSON_fromIdentifierExpression(expression);
  }
  if (expression->node->type == NT_LiteralExpression) {
    return JSON_fromLiteralExpression(expression);
  }
  return JSON_createNull();
}
JSON_Value JSON_fromBinaryExpression(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("BinaryExpression"));
  cstring s_operator = cstring_from(expression->binary.operator->raw);
  JSON_setField(obj, "operator", JSON_createString(s_operator));
  Buffer_free(s_operator);
  if (expression->binary.left) {
    JSON_setField(obj, "left", JSON_fromExpression(expression->binary.left));
  }
  if (expression->binary.right) {
    JSON_setField(obj, "right", JSON_fromExpression(expression->binary.right));
  }
  return obj;
}
JSON_Value JSON_fromBracketExpression(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("BracketExpression"));
  JSON_setField(obj, "expression",
                JSON_fromExpression(expression->bracket.expression));
  return obj;
}
JSON_Value JSON_fromIdentifierExpression(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("IdentifierExpression"));
  JSON_setField(obj, "identifier", JSON_fromIdentifier(expression->identifier));
  return obj;
}
JSON_Value JSON_fromLiteralExpression(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("LiteralExpression"));
  JSON_setField(obj, "literal", JSON_fromLiteral(expression->literal));
  return obj;
}
JSON_Value JSON_fromIdentifier(Identifier identifier) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Identifier"));
  cstring s_raw = cstring_from(identifier->raw->raw);
  JSON_setField(obj, "raw", JSON_createString(s_raw));
  Buffer_free(s_raw);
  return obj;
}
JSON_Value JSON_fromLiteral(Literal literal) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("literal"));
  cstring s_raw = cstring_from(literal->raw->raw);
  JSON_setField(obj, "raw", JSON_createString(s_raw));
  Buffer_free(s_raw);
  return obj;
}

JSON_Value JSON_fromStatement(Statement statement) {
  if (statement->node->type == NT_BlockStatement) {
    return JSON_fromBlockStatement(statement);
  }
  if (statement->node->type == NT_EmptyStatement) {
    return JSON_fromEmptyStatement(statement);
  }
  if (statement->node->type == NT_ExpressionStatement) {
    return JSON_fromExpressionStatement(statement);
  }
  return JSON_createNull();
}
JSON_Value JSON_fromProgram(Program program) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Program"));
  JSON_setField(obj, "body",
                JSON_fromList(program->body, (ToJSON)JSON_fromStatement));
  return obj;
}