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
  if (expression->node->type == NT_ComputeExpression) {
    return JSON_fromComputeExpression(expression);
  }
  if (expression->node->type == NT_LambdaExpression) {
    return JSON_fromLambdaExpression(expression);
  }
  if (expression->node->type == NT_FunctionExpression) {
    return JSON_fromFunctionExpression(expression);
  }
  if (expression->node->type == NT_OptionalCallExpression) {
    return JSON_fromOptionalCallExpression(expression);
  }
  if (expression->node->type == NT_CallExpression) {
    return JSON_fromCallExpression(expression);
  }
  if (expression->node->type == NT_OptionalComputeExpression) {
    return JSON_fromOptionalComputeExpression(expression);
  }
  if (expression->node->type == NT_TemplateExpression) {
    return JSON_fromTemplateExpression(expression);
  }
  if (expression->node->type == NT_AwaitExpression) {
    return JSON_fromAwaitExpression(expression);
  }
  if (expression->node->type == NT_ArrayPattern) {
    return JSON_fromArrayPattern(expression);
  }
  if (expression->node->type == NT_ObjectPattern) {
    return JSON_fromObjectPattern(expression);
  }
  if (expression->node->type == NT_NewExpression) {
    return JSON_fromNewExpression(expression);
  }
  if (expression->node->type == NT_DeleteExpression) {
    return JSON_fromDeleteExpression(expression);
  }
  if (expression->node->type == NT_ClassPattern) {
    return JSON_fromClassPattern(expression);
  }
  if (expression->node->type == NT_Private) {
    return JSON_fromPrivateExpression(expression);
  }
  if (expression->node->type == NT_ThisExpression) {
    return JSON_fromThisExpression(expression);
  }
  if (expression->node->type == NT_SuperExpression) {
    return JSON_fromSuperExpression(expression);
  }
  return JSON_createNull();
}
JSON_Value JSON_fromThisExpression(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("ThisExpression"));
  return obj;
}
JSON_Value JSON_fromSuperExpression(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("SuperExpression"));
  return obj;
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
JSON_Value JSON_fromPrivateExpression(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Private"));
  if (expression->binary.left) {
    JSON_setField(obj, "left", JSON_fromExpression(expression->binary.left));
  }
  if (expression->binary.right) {
    JSON_setField(obj, "right", JSON_fromExpression(expression->binary.right));
  }
  return obj;
}
JSON_Value JSON_fromComputeExpression(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("ComputeExpression"));
  if (expression->binary.left) {
    JSON_setField(obj, "left", JSON_fromExpression(expression->compute.left));
  }
  if (expression->binary.right) {
    JSON_setField(obj, "right", JSON_fromExpression(expression->compute.right));
  }
  return obj;
}
JSON_Value JSON_fromOptionalComputeExpression(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("OptionalComputeExpression"));
  if (expression->binary.left) {
    JSON_setField(obj, "left", JSON_fromExpression(expression->compute.left));
  }
  if (expression->binary.right) {
    JSON_setField(obj, "right", JSON_fromExpression(expression->compute.right));
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
JSON_Value JSON_fromLambdaExpression(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("LambdaExpression"));
  JSON_setField(obj, "lambda", JSON_fromLambda(expression->lambda));
  return obj;
}
JSON_Value JSON_fromFunctionExpression(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("FunctionExpression"));
  JSON_setField(obj, "function", JSON_fromFunction(expression->function));
  return obj;
}
JSON_Value JSON_fromCallExpression(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("CallExpression"));
  JSON_setField(obj, "callee", JSON_fromExpression(expression->call.callee));
  JSON_setField(
      obj, "args",
      JSON_fromList(expression->call.args, (ToJSON)JSON_fromExpression));
  return obj;
}
JSON_Value JSON_fromOptionalCallExpression(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("OptionalCallExpression"));
  JSON_setField(obj, "callee", JSON_fromExpression(expression->call.callee));
  JSON_setField(
      obj, "args",
      JSON_fromList(expression->call.args, (ToJSON)JSON_fromExpression));
  return obj;
}
JSON_Value JSON_fromAwaitExpression(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("AwaitExpression"));
  JSON_setField(obj, "expression",
                JSON_fromExpression(expression->await.expression));
  return obj;
}

JSON_Value JSON_fromNewExpression(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("NewExpression"));
  JSON_setField(obj, "expression",
                JSON_fromExpression(expression->newexpr.expression));
  return obj;
}
JSON_Value JSON_fromDeleteExpression(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("DeleteExpression"));
  JSON_setField(obj, "expression",
                JSON_fromExpression(expression->deleteexpr.expression));
  return obj;
}
JSON_Value JSON_fromFunction(Function function) {
  JSON_Value obj = JSON_createObject();
  if (function->node->type == NT_Function) {
    JSON_setField(obj, "type", JSON_createString("Function"));
  } else if (function->node->type == NT_Getter) {
    JSON_setField(obj, "type", JSON_createString("Getter"));
  } else if (function->node->type == NT_Setter) {
    JSON_setField(obj, "type", JSON_createString("Setter"));
  }
  if (function->name) {
    JSON_setField(obj, "name", JSON_fromExpression(function->name));
  }
  JSON_setField(obj, "args",
                JSON_fromList(function->args, (ToJSON)JSON_fromExpression));
  JSON_setField(obj, "body", JSON_fromStatement(function->body));
  JSON_setField(obj, "async", JSON_createBoolean(function->async));
  JSON_setField(obj, "generator", JSON_createBoolean(function->generator));
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
JSON_Value JSON_fromLambda(Lambda lambda) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Lambda"));
  JSON_setField(obj, "args",
                JSON_fromList(lambda->args, (ToJSON)JSON_fromExpression));
  JSON_setField(obj, "body", JSON_fromStatement(lambda->body));
  JSON_setField(obj, "async", JSON_createBoolean(lambda->async));
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
JSON_Value JSON_fromToken(Token token) {
  cstring c_string = cstring_from(token->raw);
  JSON_Value result = JSON_createString(c_string);
  Buffer_free(c_string);
  return result;
}
JSON_Value JSON_fromTemplate(Template template) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Template"));
  if (template->tag) {
    JSON_setField(obj, "tag", JSON_fromExpression(template->tag));
  }
  JSON_setField(obj, "datas",
                JSON_fromList(template->datas, (ToJSON)JSON_fromToken));
  JSON_setField(obj, "parts",
                JSON_fromList(template->parts, (ToJSON)JSON_fromExpression));
  return obj;
}
JSON_Value JSON_fromTemplateExpression(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("TemplateExpression"));
  JSON_setField(obj, "template", JSON_fromTemplate(expression->template));
  return obj;
}

JSON_Value JSON_fromArrayPattern(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("ArrayPattern"));
  JSON_setField(obj, "array", JSON_fromArray(expression->array));
  return obj;
}

JSON_Value JSON_fromArray(Array array) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Array"));
  JSON_setField(obj, "items",
                JSON_fromList(array->items, (ToJSON)JSON_fromExpression));
  return obj;
}

JSON_Value JSON_fromObjectPattern(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("ObjectPattern"));
  JSON_setField(obj, "object", JSON_fromObject(expression->object));
  return obj;
}

JSON_Value JSON_fromObject(Object object) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Object"));
  JSON_setField(
      obj, "properties",
      JSON_fromList(object->properties, (ToJSON)JSON_fromObjectProperty));
  return obj;
}

JSON_Value JSON_fromObjectProperty(ObjectProperty property) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("ObjectProperty"));
  if (property->key) {
    JSON_setField(obj, "key", JSON_fromExpression(property->key));
  }
  JSON_setField(obj, "value", JSON_fromExpression(property->value));
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
  if (statement->node->type == NT_ReturnStatement) {
    return JSON_fromReturnStatement(statement);
  }
  if (statement->node->type == NT_DefinitionSatement) {
    return JSON_fromDefinitionStatement(statement);
  }
  if (statement->node->type == NT_ExportStatement) {
    return JSON_fromExportStatement(statement);
  }
  if (statement->node->type == NT_YieldStatement) {
    return JSON_fromYieldStatement(statement);
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

JSON_Value JSON_fromClassPattern(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("ClassPattern"));
  JSON_setField(obj, "clazz", JSON_fromClass(expression->clazz));
  return obj;
}
JSON_Value JSON_fromClass(Class clazz) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Class"));
  if (clazz->name) {
    JSON_setField(obj, "name", JSON_fromIdentifier(clazz->name));
  }
  if (clazz->extends) {
    JSON_setField(obj, "extends", JSON_fromExpression(clazz->extends));
  }
  JSON_setField(obj, "decorators",
                JSON_fromList(clazz->decorators, (ToJSON)JSON_fromExpression));
  JSON_setField(
      obj, "properties",
      JSON_fromList(clazz->properties, (ToJSON)JSON_fromClassProperty));
  return obj;
}
JSON_Value JSON_fromClassProperty(ClassProperty property) {
  JSON_Value obj = JSON_createObject();
  if (property->node->type == NT_ClassProperty) {
    JSON_setField(obj, "type", JSON_createString("ClassProperty"));
    if (property->field.init) {
      JSON_setField(obj, "init", JSON_fromExpression(property->field.init));
    }
    if (property->field.key) {
      JSON_setField(obj, "key", JSON_fromExpression(property->field.key));
    }
    JSON_setField(
        obj, "decorators",
        JSON_fromList(property->decorators, (ToJSON)JSON_fromExpression));
    JSON_setField(obj, "private", JSON_createBoolean(property->field.private_));
    JSON_setField(obj, "static", JSON_createBoolean(property->field.static_));
  }
  return obj;
}
JSON_Value JSON_fromReturnStatement(Statement statement) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("ReturnStatement"));
  if (statement->return_.expression) {
    JSON_setField(obj, "value",
                  JSON_fromExpression(statement->return_.expression));
  } else {
    JSON_setField(obj, "value", JSON_createNull());
  }
  return obj;
}
JSON_Value JSON_fromDefinitionStatement(Statement statement) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("DefinitionStatement"));
  JSON_setField(obj, "define",
                JSON_fromExpression(statement->return_.expression));
  if (statement->definition.type == DT_VAR) {
    JSON_setField(obj, "define_type", JSON_createString("var"));
  } else if (statement->definition.type == DT_CONST) {
    JSON_setField(obj, "define_type", JSON_createString("const"));
  } else if (statement->definition.type == DT_LET) {
    JSON_setField(obj, "define_type", JSON_createString("let"));
  }
  return obj;
}
JSON_Value JSON_fromExportStatement(Statement statement) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("ExportStatement"));
  JSON_setField(obj, "value", JSON_fromStatement(statement->export_.statement));
  return obj;
}
JSON_Value JSON_fromYieldStatement(Statement statement) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("YieldStatement"));
  if (statement->yield_.expression) {
    JSON_setField(obj, "value",
                  JSON_fromExpression(statement->yield_.expression));
  } else {
    JSON_setField(obj, "value", JSON_createNull());
  }
  return obj;
}