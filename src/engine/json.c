#include "json.h"
#include "expression.h"
#include "parser.h"
static JSON_Value JSON_fromToken(Token token) {
  cstring s = cstring_from(token->raw);
  JSON_Value res = JSON_createString(s);
  Buffer_free(s);
  return res;
}
static JSON_Value JSON_fromBlockStatement(Statement statement) {

  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Block"));
  JSON_setField(
      obj, "body",
      JSON_fromList(statement->block.body, (ToJSON)JSON_fromStatement));
  return obj;
}
static JSON_Value JSON_fromEmptyStatement(Statement statement) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Empty"));
  return obj;
}
static JSON_Value JSON_fromExpressionStatement(Statement statement) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Expression"));
  JSON_setField(obj, "expression", JSON_fromExpression(statement->expression));
  return obj;
}
static JSON_Value JSON_fromAssignmentStatement(Statement statement) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Assignment"));
  cstring assignment_type = "unknown";
  if (statement->assignement.type == AT_Const) {
    assignment_type = "const";
  } else if (statement->assignement.type == AT_Let) {
    assignment_type = "let";
  } else if (statement->assignement.type == AT_Var) {
    assignment_type = "var";
  }
  JSON_setField(obj, "assignment_type", JSON_createString(assignment_type));
  JSON_setField(obj, "expression",
                JSON_fromExpression(statement->assignement.expression));
  return obj;
}
static JSON_Value JSON_fromReturnStatement(Statement statement) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Return"));
  JSON_setField(obj, "expression",
                statement->expression
                    ? JSON_fromExpression(statement->expression)
                    : JSON_createNull());
  return obj;
}
static JSON_Value JSON_fromYieldStatement(Statement statement) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Yield"));
  JSON_setField(obj, "expression",
                statement->expression
                    ? JSON_fromExpression(statement->expression)
                    : JSON_createNull());
  return obj;
}

static JSON_Value JSON_fromBreakStatement(Statement statement) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Break"));
  JSON_setField(obj, "label",
                statement->label ? JSON_fromToken(statement->label)
                                 : JSON_createNull());
  return obj;
}

static JSON_Value JSON_fromContinueStatement(Statement statement) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Continue"));
  JSON_setField(obj, "label",
                statement->label ? JSON_fromToken(statement->label)
                                 : JSON_createNull());
  return obj;
}
static JSON_Value JSON_fromWhileStatement(Statement statement) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("While"));
  JSON_setField(obj, "condition",
                JSON_fromExpression(statement->whileStatement.condition));
  JSON_setField(obj, "body",
                JSON_fromStatement(statement->whileStatement.body));
  return obj;
}

static JSON_Value JSON_fromLabelStatement(Statement statement) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Label"));
  JSON_setField(obj, "label", JSON_fromToken(statement->labelStatement.label));
  JSON_setField(obj, "body",
                JSON_fromStatement(statement->labelStatement.body));
  return obj;
}

static JSON_Value JSON_fromIfStatement(Statement statement) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("If"));
  JSON_setField(obj, "condition",
                JSON_fromExpression(statement->ifStatement.condition));
  JSON_setField(obj, "consequent",
                JSON_fromStatement(statement->ifStatement.consequent));
  if (statement->ifStatement.alternate) {
    JSON_setField(obj, "alternate",
                  JSON_fromStatement(statement->ifStatement.alternate));
  }
  return obj;
}
static JSON_Value JSON_fromSwitchPattern(SwitchPattern pattern) {
  JSON_Value obj = JSON_createObject();
  if (pattern->condition) {
    JSON_setField(obj, "condition", JSON_fromExpression(pattern->condition));
  }
  JSON_setField(obj, "body",
                JSON_fromList(pattern->body, (ToJSON)JSON_fromStatement));
  return obj;
}
static JSON_Value JSON_fromSwitchStatement(Statement statement) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Switch"));
  JSON_setField(obj, "condition",
                JSON_fromExpression(statement->switchStatement.condition));
  JSON_setField(obj, "patterns",
                JSON_fromList(statement->switchStatement.patterns,
                              (ToJSON)JSON_fromSwitchPattern));
  return obj;
}
JSON_Value JSON_fromStatement(Statement statement) {
  switch (statement->type) {
  case ST_Block:
    return JSON_fromBlockStatement(statement);
  case ST_Empty:
    return JSON_fromEmptyStatement(statement);
  case ST_Expression:
    return JSON_fromExpressionStatement(statement);
  case ST_Assignment:
    return JSON_fromAssignmentStatement(statement);
  case ST_Return:
    return JSON_fromReturnStatement(statement);
  case ST_Yield:
    return JSON_fromYieldStatement(statement);
  case ST_Break:
    return JSON_fromBreakStatement(statement);
  case ST_Continue:
    return JSON_fromContinueStatement(statement);
  case ST_While:
    return JSON_fromWhileStatement(statement);
  case ST_Label:
    return JSON_fromLabelStatement(statement);
  case ST_If:
    return JSON_fromIfStatement(statement);
  case ST_Switch:
    return JSON_fromSwitchStatement(statement);
  default:
    return JSON_createNull();
  }
}
static JSON_Value JSON_fromBracket(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Bracket"));
  if (expression->bracket) {
    JSON_setField(obj, "subExpression",
                  JSON_fromExpression(expression->bracket));
  }
  return obj;
}
static JSON_Value JSON_fromCalculate(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Calculate"));
  if (expression->binary.left) {
    JSON_setField(obj, "left", JSON_fromExpression(expression->binary.left));
  }
  if (expression->binary.right) {
    JSON_setField(obj, "right", JSON_fromExpression(expression->binary.right));
  }
  if (expression->binary.operator) {
    JSON_setField(obj, "operator", JSON_fromToken(expression->binary.operator));
  }
  return obj;
}

static JSON_Value JSON_fromLiteral(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Literal"));
  JSON_setField(obj, "raw", JSON_fromToken(expression->literal));
  return obj;
}
static JSON_Value JSON_fromIdentifier(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Identifier"));
  JSON_setField(obj, "raw", JSON_fromToken(expression->Identifier));
  return obj;
}

static JSON_Value JSON_fromLambda(Expression expression) {

  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Lambda"));
  JSON_setField(
      obj, "args",
      JSON_fromList(expression->lambda->args, (ToJSON)JSON_fromExpression));
  JSON_setField(obj, "body", JSON_fromStatement(expression->lambda->body));
  JSON_setField(obj, "async", JSON_createBoolean(expression->lambda->async));
  return obj;
}

static JSON_Value JSON_fromFunction(Expression expression) {

  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Function"));
  if (expression->function->name) {
    JSON_setField(obj, "name", JSON_fromToken(expression->function->name));
  } else {
    JSON_setField(obj, "name", JSON_createNull());
  }
  JSON_setField(
      obj, "args",
      JSON_fromList(expression->function->args, (ToJSON)JSON_fromExpression));
  JSON_setField(obj, "body", JSON_fromStatement(expression->function->body));
  JSON_setField(obj, "async", JSON_createBoolean(expression->function->async));
  JSON_setField(obj, "generator",
                JSON_createBoolean(expression->function->generator));
  JSON_setField(obj, "getter",
                JSON_createBoolean(expression->function->getter));
  JSON_setField(obj, "setter",
                JSON_createBoolean(expression->function->setter));
  return obj;
}

static JSON_Value JSON_fromCompute(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Compute"));
  if (expression->compute.host) {
    JSON_setField(obj, "host", JSON_fromExpression(expression->compute.host));
  }
  JSON_setField(obj, "key", JSON_fromExpression(expression->compute.key));
  return obj;
}

static JSON_Value JSON_fromCall(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Call"));
  JSON_setField(obj, "callee", JSON_fromExpression(expression->call.callee));
  JSON_setField(
      obj, "args",
      JSON_fromList(expression->call.args, (ToJSON)JSON_fromExpression));
  return obj;
}

static JSON_Value JSON_fromOptionalCall(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("OptionalCompute"));
  JSON_setField(obj, "host", JSON_fromExpression(expression->compute.host));
  JSON_setField(obj, "key", JSON_fromExpression(expression->compute.key));
  return obj;
}

static JSON_Value JSON_fromOptionalCompute(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("OptionalCall"));
  JSON_setField(obj, "callee", JSON_fromExpression(expression->call.callee));
  JSON_setField(
      obj, "args",
      JSON_fromList(expression->call.args, (ToJSON)JSON_fromExpression));
  return obj;
}

static JSON_Value JSON_fromArray(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Array"));
  JSON_setField(
      obj, "items",
      JSON_fromList(expression->array->items, (ToJSON)JSON_fromExpression));
  return obj;
}
static JSON_Value JSON_fromObjectProperty(ObjectProperty property) {
  JSON_Value obj = JSON_createObject();
  if (property->key) {
    JSON_setField(obj, "key", JSON_fromExpression(property->key));
  }
  if (property->value) {
    JSON_setField(obj, "value", JSON_fromExpression(property->value));
  }
  return obj;
}
static JSON_Value JSON_fromObject(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Object"));
  JSON_setField(obj, "properties",
                JSON_fromList(expression->object->properties,
                              (ToJSON)JSON_fromObjectProperty));
  return obj;
}
static JSON_Value JSON_fromClassProperty(ClassProperty property) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(
      obj, "decorators",
      JSON_fromList(property->decorators, (ToJSON)JSON_fromExpression));
  if (property->key) {
    JSON_setField(obj, "key", JSON_fromExpression(property->key));
  }
  if (property->value) {
    JSON_setField(obj, "value", JSON_fromExpression(property->value));
  }
  JSON_setField(obj, "private", JSON_createBoolean(property->isPrivate));
  JSON_setField(obj, "static", JSON_createBoolean(property->isStatic));
  return obj;
}
static JSON_Value JSON_fromClass(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Class"));
  if (expression->clazz->name) {
    JSON_setField(obj, "name", JSON_fromToken(expression->clazz->name));
  }
  if (expression->clazz->extends) {
    JSON_setField(obj, "extends",
                  JSON_fromExpression(expression->clazz->extends));
  }
  JSON_setField(obj, "decorators",
                JSON_fromList(expression->clazz->decorators,
                              (ToJSON)JSON_fromExpression));
  JSON_setField(obj, "properties",
                JSON_fromList(expression->clazz->properties,
                              (ToJSON)JSON_fromClassProperty));
  JSON_setField(obj, "staticBlocks",
                JSON_fromList(expression->clazz->staticBlocks,
                              (ToJSON)JSON_fromStatement));
  return obj;
}

static JSON_Value JSON_fromNew(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("New"));
  JSON_setField(obj, "newExpression", JSON_fromExpression(expression->newExpr));
  return obj;
}

static JSON_Value JSON_fromDelete(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Delete"));
  JSON_setField(obj, "newExpression", JSON_fromExpression(expression->newExpr));
  return obj;
}

static JSON_Value JSON_fromAwait(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Await"));
  JSON_setField(obj, "newExpression", JSON_fromExpression(expression->newExpr));
  return obj;
}

static JSON_Value JSON_fromThis(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("This"));
  return obj;
}

static JSON_Value JSON_fromSuper(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Super"));
  return obj;
}

static JSON_Value JSON_fromCondition(Expression expression) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Condition"));
  JSON_setField(obj, "condition",
                JSON_fromExpression(expression->condition.condition));
  JSON_setField(obj, "consequent",
                JSON_fromExpression(expression->condition.consequent));
  JSON_setField(obj, "alternate",
                JSON_fromExpression(expression->condition.alternate));
  return obj;
}

JSON_Value JSON_fromExpression(Expression expression) {
  JSON_Value obj = NULL;
  switch (expression->type) {
  case ET_Bracket:
    obj = JSON_fromBracket(expression);
    break;
  case ET_Calculate:
    obj = JSON_fromCalculate(expression);
    break;
  case ET_Literal:
    obj = JSON_fromLiteral(expression);
    break;
  case ET_Identifier:
    obj = JSON_fromIdentifier(expression);
    break;
  case ET_Lambda:
    obj = JSON_fromLambda(expression);
    break;
  case ET_Function:
    obj = JSON_fromFunction(expression);
    break;
  case ET_Compute:
    obj = JSON_fromCompute(expression);
    break;
  case ET_Call:
    obj = JSON_fromCall(expression);
    break;
  case ET_OptionalCompute:
    obj = JSON_fromOptionalCall(expression);
    break;
  case ET_OptionalCall:
    obj = JSON_fromOptionalCompute(expression);
    break;
  case ET_Array:
    obj = JSON_fromArray(expression);
    break;
  case ET_Object:
    obj = JSON_fromObject(expression);
    break;
  case ET_Class:
    obj = JSON_fromClass(expression);
    break;
  case ET_New:
    obj = JSON_fromNew(expression);
    break;
  case ET_Delete:
    obj = JSON_fromDelete(expression);
    break;
  case ET_Await:
    obj = JSON_fromAwait(expression);
    break;
  case ET_This:
    obj = JSON_fromThis(expression);
    break;
  case ET_Super:
    obj = JSON_fromSuper(expression);
    break;
  case ET_Condition:
    obj = JSON_fromCondition(expression);
    break;
  default:
    break;
  }
  JSON_setField(obj, "level", JSON_createDouble(expression->level));
  return obj;
}