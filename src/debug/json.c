#include "debug/debug.h"

static JSON_Value JSON_fromAstList(AstNode iterator, ToJSON toJSON) {
  JSON_Value arr = JSON_createArray();
  AstNode it = iterator;
  int index = 0;
  if (it) {
    while (it->type == ANT_Fake) {
      JSON_setIndex(arr, index, toJSON(it->binary.left));
      it = it->binary.right;
      index++;
    }
    JSON_setIndex(arr, index, toJSON(it));
  }
  return arr;
}

static JSON_Value JSON_fromProgram(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Program"));
  JSON_setField(obj, "body",
                JSON_fromAstList(node->binary.right, (ToJSON)JSON_fromAstNode));
  return obj;
}
static JSON_Value JSON_fromToken(Token token) {
  cstring str = cstring_from(token->raw);
  JSON_Value result = JSON_createString(str);
  Buffer_dispose(str);
  return result;
}

static JSON_Value JSON_fromIdentifier(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Identifier"));
  JSON_setField(obj, "raw", JSON_fromToken(node->identifier));
  return obj;
}
static JSON_Value JSON_fromLiteral(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Literal"));
  JSON_setField(obj, "raw", JSON_fromToken(node->literal));
  return obj;
}
static JSON_Value JSON_fromExpressionStatement(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("ExpressionStatement"));
  JSON_setField(obj, "expression", JSON_fromAstNode(node->binary.left));
  return obj;
}
static JSON_Value JSON_fromCalculate(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Binary"));
  if (node->binary.flag) {
    JSON_setField(obj, "flag", JSON_createString(node->binary.flag));
  }
  if (node->binary.left) {
    JSON_setField(obj, "left", JSON_fromAstNode(node->binary.left));
  }
  if (node->binary.right) {
    JSON_setField(obj, "right", JSON_fromAstNode(node->binary.right));
  }
  JSON_setField(obj, "level", JSON_createDouble(node->level));
  return obj;
}
JSON_Value JSON_fromBlockStatement(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("BlockStatement"));
  JSON_setField(obj, "body",
                JSON_fromAstList(node->binary.right, (ToJSON)JSON_fromAstNode));
  return obj;
}

JSON_Value JSON_fromCallExpression(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Call"));
  JSON_setField(obj, "callee", JSON_fromAstNode(node->binary.left));
  JSON_setField(obj, "args",
                JSON_fromAstList(node->binary.right, (ToJSON)JSON_fromAstNode));

  return obj;
}

JSON_Value JSON_fromFunction(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Function"));
  if (node->function.name) {
    JSON_setField(obj, "name", JSON_createString(node->function.name));
  }
  JSON_setField(obj, "args",
                JSON_fromList(node->function.args, (ToJSON)JSON_fromAstNode));
  JSON_setField(obj, "body", JSON_fromAstNode(node->function.body));
  JSON_setField(obj, "async", JSON_createBoolean(node->function.async));
  JSON_setField(obj, "generator", JSON_createBoolean(node->function.generator));
  return obj;
}
JSON_Value JSON_fromLambda(AstNode node) {

  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Lambda"));
  JSON_setField(obj, "args",
                JSON_fromList(node->lambda.args, (ToJSON)JSON_fromAstNode));
  JSON_setField(obj, "body", JSON_fromAstNode(node->lambda.body));
  JSON_setField(obj, "async", JSON_createBoolean(node->lambda.async));
  return obj;
}

JSON_Value JSON_fromTemplate(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Template"));
  if (node->template.tag) {
    JSON_setField(obj, "tag", JSON_fromToken(node->template.tag));
  }
  JSON_setField(obj, "args",
                JSON_fromList(node->template.args, (ToJSON)JSON_fromAstNode));
  JSON_setField(obj, "parts",
                JSON_fromList(node->template.parts, (ToJSON)JSON_fromToken));
  return obj;
}

JSON_Value JSON_fromObjectProperty(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "key", JSON_fromAstNode(node->oprop.key));
  if (node->oprop.type == OPT_Field) {
    JSON_setField(obj, "type", JSON_createString("Field"));
    JSON_setField(obj, "value", JSON_fromAstNode(node->oprop.field));
  } else {
    if (node->oprop.type == OPT_Method) {
      JSON_setField(obj, "type", JSON_createString("Method"));
      JSON_setField(obj, "async", JSON_createBoolean(node->oprop.method.async));
      JSON_setField(obj, "generator",
                    JSON_createBoolean(node->oprop.method.generator));
    } else if (node->oprop.type == OPT_Getter) {
      JSON_setField(obj, "type", JSON_createString("Getter"));
    } else {
      JSON_setField(obj, "type", JSON_createString("Setter"));
    }
    JSON_setField(
        obj, "args",
        JSON_fromList(node->oprop.method.args, (ToJSON)JSON_fromAstNode));
    JSON_setField(obj, "body", JSON_fromAstNode(node->oprop.method.body));
  }
  return obj;
}

JSON_Value JSON_fromObject(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Object"));
  JSON_setField(
      obj, "properties",
      JSON_fromList(node->object.properties, (ToJSON)JSON_fromAstNode));
  return obj;
}

JSON_Value JSON_fromAstNode(AstNode node) {
  switch (node->type) {
  case ANT_Identifier:
    return JSON_fromIdentifier(node);
  case ANT_Literal:
    return JSON_fromLiteral(node);
  case ANT_Program:
    return JSON_fromProgram(node);
  case ANT_Call:
    return JSON_fromCallExpression(node);
  case ANT_ExpressionStatement:
    return JSON_fromExpressionStatement(node);
  case ANT_BlockStatement:
    return JSON_fromBlockStatement(node);
  case ANT_Function:
    return JSON_fromFunction(node);
  case ANT_Lambda:
    return JSON_fromLambda(node);
  case ANT_Template:
    return JSON_fromTemplate(node);
  case ANT_Object:
    return JSON_fromObject(node);
  case ANT_ObjectProperty:
    return JSON_fromObjectProperty(node);
  default:
    return JSON_fromCalculate(node);
  }
}