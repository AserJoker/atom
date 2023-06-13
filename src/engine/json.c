#include "parser.h"
static JSON_Value JSON_fromToken(Token token) {
  cstring s = cstring_from(token->raw);
  JSON_Value res = JSON_createString(s);
  Buffer_free(s);
  return res;
}
JSON_Value JSON_fromStatement(Statement statement) {
  JSON_Value obj = JSON_createObject();
  switch (statement->type) {
  case ST_Block:
    JSON_setField(obj, "type", JSON_createString("Block"));
    JSON_setField(
        obj, "body",
        JSON_fromList(statement->block.body, (ToJSON)JSON_fromStatement));
    break;
  case ST_Empty:
    JSON_setField(obj, "type", JSON_createString("Empty"));
    break;
  case ST_Expression:
    JSON_setField(obj, "type", JSON_createString("Expression"));
    JSON_setField(obj, "expression",
                  JSON_fromExpression(statement->expression));
    break;
  default:
    break;
  }
  return obj;
}
JSON_Value JSON_fromExpression(Expression expression) {
  JSON_Value obj = JSON_createObject();
  switch (expression->type) {

  case ET_Unknown:
    break;
  case ET_Bracket:
    JSON_setField(obj, "type", JSON_createString("Bracket"));
    if (expression->bracket.sub) {
      JSON_setField(obj, "subExpression",
                    JSON_fromExpression(expression->bracket.sub));
    }
    break;
  case ET_Calculate:
    JSON_setField(obj, "type", JSON_createString("Calculate"));
    if (expression->binary.left) {
      JSON_setField(obj, "left", JSON_fromExpression(expression->binary.left));
    }
    if (expression->binary.right) {
      JSON_setField(obj, "right",
                    JSON_fromExpression(expression->binary.right));
    }
    if (expression->binary.operator) {
      JSON_setField(obj, "operator",
                    JSON_fromToken(expression->binary.operator));
    }
    break;
  case ET_Literal:
    JSON_setField(obj, "type", JSON_createString("Literal"));
    JSON_setField(obj, "raw", JSON_fromToken(expression->literal));
    break;
  case ET_Identifier:
    JSON_setField(obj, "type", JSON_createString("Identifier"));
    JSON_setField(obj, "raw", JSON_fromToken(expression->Identifier));
    break;
  case ET_Lambda:
    JSON_setField(obj, "type", JSON_createString("Lambda"));
    JSON_setField(
        obj, "args",
        JSON_fromList(expression->lambda.args, (ToJSON)JSON_fromExpression));
    JSON_setField(obj, "body", JSON_fromStatement(expression->lambda.body));
    JSON_setField(obj, "async", JSON_createBoolean(expression->lambda.async));
    break;
  }
  return obj;
}