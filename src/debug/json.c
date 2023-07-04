#include "debug/debug.h"

static JSON_Value JSON_fromAstList(AstNode iterator, ToJSON toJSON) {
  JSON_Value arr = JSON_createArray();
  AstNode it = iterator;
  int index = 0;
  if (it) {
    while (it->type == ANT_Reserved) {
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
                JSON_fromList(node->s_program.body, (ToJSON)JSON_fromAstNode));
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
  JSON_setField(obj, "raw", JSON_fromToken(node->e_identifier));
  return obj;
}
static JSON_Value JSON_fromLiteral(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Literal"));
  JSON_setField(obj, "raw", JSON_fromToken(node->e_literal));
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
  JSON_setField(
      obj, "body",
      JSON_fromList(node->s_block.statements, (ToJSON)JSON_fromAstNode));
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
  if (node->e_function.name) {
    JSON_setField(obj, "name", JSON_createString(node->e_function.name));
  }
  JSON_setField(obj, "args",
                JSON_fromList(node->e_function.args, (ToJSON)JSON_fromAstNode));
  JSON_setField(obj, "body", JSON_fromAstNode(node->e_function.body));
  JSON_setField(obj, "async", JSON_createBoolean(node->e_function.async));
  JSON_setField(obj, "generator",
                JSON_createBoolean(node->e_function.generator));
  return obj;
}
JSON_Value JSON_fromLambda(AstNode node) {

  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Lambda"));
  JSON_setField(obj, "args",
                JSON_fromList(node->e_lambda.args, (ToJSON)JSON_fromAstNode));
  JSON_setField(obj, "body", JSON_fromAstNode(node->e_lambda.body));
  JSON_setField(obj, "async", JSON_createBoolean(node->e_lambda.async));
  return obj;
}

JSON_Value JSON_fromTemplate(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Template"));
  if (node->e_template.tag) {
    JSON_setField(obj, "tag", JSON_fromToken(node->e_template.tag));
  }
  JSON_setField(obj, "args",
                JSON_fromList(node->e_template.args, (ToJSON)JSON_fromAstNode));
  JSON_setField(obj, "parts",
                JSON_fromList(node->e_template.parts, (ToJSON)JSON_fromToken));
  return obj;
}

JSON_Value JSON_fromObjectProperty(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "key", JSON_fromAstNode(node->e_oprop.key));
  if (node->e_oprop.type == OPT_Field) {
    JSON_setField(obj, "type", JSON_createString("Field"));
    JSON_setField(obj, "value", JSON_fromAstNode(node->e_oprop.field));
  } else {
    if (node->e_oprop.type == OPT_Method) {
      JSON_setField(obj, "type", JSON_createString("Method"));
      JSON_setField(obj, "async",
                    JSON_createBoolean(node->e_oprop.method.async));
      JSON_setField(obj, "generator",
                    JSON_createBoolean(node->e_oprop.method.generator));
    } else if (node->e_oprop.type == OPT_Getter) {
      JSON_setField(obj, "type", JSON_createString("Getter"));
    } else {
      JSON_setField(obj, "type", JSON_createString("Setter"));
    }
    JSON_setField(
        obj, "args",
        JSON_fromList(node->e_oprop.method.args, (ToJSON)JSON_fromAstNode));
    JSON_setField(obj, "body", JSON_fromAstNode(node->e_oprop.method.body));
  }
  return obj;
}

JSON_Value JSON_fromObject(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Object"));
  JSON_setField(
      obj, "properties",
      JSON_fromList(node->e_object.properties, (ToJSON)JSON_fromAstNode));
  return obj;
}

JSON_Value JSON_fromArray(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Array"));
  JSON_setField(obj, "items",
                JSON_fromList(node->e_array.items, (ToJSON)JSON_fromAstNode));
  return obj;
}

JSON_Value JSON_fromClassProperty(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "static", JSON_createBoolean(node->e_cprop.isStatic));
  JSON_setField(obj, "private", JSON_createBoolean(node->e_cprop.isPrivate));
  JSON_setField(obj, "key", JSON_fromAstNode(node->e_cprop.key));
  JSON_setField(
      obj, "decorators",
      JSON_fromList(node->e_class.decorators, (ToJSON)JSON_fromAstNode));
  if (node->e_cprop.type == CPT_Field) {
    JSON_setField(obj, "type", JSON_createString("Field"));
  } else {
    if (node->e_cprop.type == CPT_Method) {
      JSON_setField(obj, "type", JSON_createString("Method"));
    } else if (node->e_cprop.type == CPT_Getter) {
      JSON_setField(obj, "type", JSON_createString("Getter"));
    } else {
      JSON_setField(obj, "type", JSON_createString("Setter"));
    }
    JSON_setField(obj, "async", JSON_createBoolean(node->e_cprop.method.async));
    JSON_setField(obj, "generator",
                  JSON_createBoolean(node->e_cprop.method.generator));
    JSON_setField(
        obj, "args",
        JSON_fromList(node->e_cprop.method.args, (ToJSON)JSON_fromAstNode));
    JSON_setField(obj, "body",
                  JSON_fromBlockStatement(node->e_cprop.method.body));
  }
  return obj;
}

JSON_Value JSON_fromClass(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Class"));
  JSON_setField(
      obj, "decorators",
      JSON_fromList(node->e_class.decorators, (ToJSON)JSON_fromAstNode));
  if (node->e_class.name)
    JSON_setField(obj, "name", JSON_fromToken(node->e_class.name));
  if (node->e_class.extends)
    JSON_setField(obj, "extends", JSON_fromAstNode(node->e_class.extends));
  JSON_setField(
      obj, "properties",
      JSON_fromList(node->e_class.properties, (ToJSON)JSON_fromClassProperty));
  JSON_setField(obj, "staticBlocks",
                JSON_fromList(node->e_class.staticBlocks,
                              (ToJSON)JSON_fromBlockStatement));

  return obj;
}

JSON_Value JSON_fromReturn(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("return"));
  if (node->s_return) {
    JSON_setField(obj, "value", JSON_fromAstNode(node->s_return));
  }
  return obj;
}

JSON_Value JSON_fromYield(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("yield"));
  if (node->s_return) {
    JSON_setField(obj, "value", JSON_fromAstNode(node->s_return));
  }
  return obj;
}

JSON_Value JSON_fromLabel(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("label"));
  JSON_setField(obj, "label", JSON_fromToken(node->s_label.label));
  JSON_setField(obj, "body", JSON_fromAstNode(node->s_label.body));
  return obj;
}

JSON_Value JSON_fromBreak(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("break"));
  if (node->s_break.label) {
    JSON_setField(obj, "label", JSON_fromToken(node->s_break.label));
  }
  return obj;
}

JSON_Value JSON_fromContinue(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("break"));
  if (node->s_break.label) {
    JSON_setField(obj, "label", JSON_fromToken(node->s_break.label));
  }
  return obj;
}
JSON_Value JSON_fromAssigment(AstNode node) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("assigment"));
  if (node->s_assigment.type == AT_Const) {
    JSON_setField(obj, "assigmentType", JSON_createString("const"));
  } else if (node->s_assigment.type == AT_Let) {
    JSON_setField(obj, "assigmentType", JSON_createString("let"));
  } else {
    JSON_setField(obj, "assigmentType", JSON_createString("var"));
  }
  JSON_setField(obj, "body", JSON_fromAstNode(node->s_assigment.body));
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
  case ANT_Array:
    return JSON_fromArray(node);
  case ANT_Class:
    return JSON_fromClass(node);
  case ANT_ReturnStatement:
    return JSON_fromReturn(node);
  case ANT_YieldStatement:
    return JSON_fromYield(node);
  case ANT_LabelStatement:
    return JSON_fromLabel(node);
  case ANT_BreakStatement:
    return JSON_fromBreak(node);
  case ANT_ContinueStatement:
    return JSON_fromContinue(node);
  case ANT_AssigmentStatement:
    return JSON_fromAssigment(node);
  default:
    return JSON_fromCalculate(node);
  }
}