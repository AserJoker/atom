#include "parser.h"
static JSON_Value JSON_fromToken(Token token) {
  cstring s = cstring_from(token->raw);
  JSON_Value res = JSON_createString(s);
  Buffer_free(s);
  return res;
}
JSON_Value JSON_fromExpression(Expression expression) {
  JSON_Value obj = JSON_createObject();
  switch (expression->type) {

  case ET_Unknown:
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
  }
  return obj;
}