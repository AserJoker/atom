#include "json.h"
#include "ast.h"
#include "buffer.h"
#include "list.h"
#include "strings.h"
#include <stdint.h>
#include <string.h>

JSON_Value JSON_fromIdentifier(Identifier identifier) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Identifier"));
  cstring s_identifier = cstring_from(identifier->_raw->_raw);
  JSON_setField(obj, "identifier", JSON_createString(s_identifier));
  Buffer_free(s_identifier);
  return obj;
}

JSON_Value JSON_fromStringLiteral(Literal literal) {
  cstring s_literal = cstring_from(literal->_raw->_raw);
  JSON_Value val = JSON_createObject();
  JSON_setField(val, "type", JSON_createString("StringLiteral"));
  JSON_setField(val, "literal", JSON_createString(s_literal));
  Buffer_free(s_literal);
  return val;
}

JSON_Value JSON_fromNumberLiteral(Literal literal) {
  cstring s_literal = cstring_from(literal->_raw->_raw);
  JSON_Value val = JSON_createObject();
  JSON_setField(val, "type", JSON_createString("NumberLiteral"));
  JSON_setField(val, "literal", JSON_createString(s_literal));
  Buffer_free(s_literal);
  return val;
}

JSON_Value JSON_fromBooleanLiteral(Literal literal) {
  cstring s_literal = cstring_from(literal->_raw->_raw);
  JSON_Value val = JSON_createObject();
  JSON_setField(val, "type", JSON_createString("BooleanLiteral"));
  JSON_setField(val, "literal", JSON_createString(s_literal));
  Buffer_free(s_literal);
  return val;
}

JSON_Value JSON_fromNullLiteral(Literal literal) {
  cstring s_literal = cstring_from(literal->_raw->_raw);
  JSON_Value val = JSON_createObject();
  JSON_setField(val, "type", JSON_createString("NullLiteral"));
  JSON_setField(val, "literal", JSON_createString(s_literal));
  Buffer_free(s_literal);
  return val;
}

JSON_Value JSON_fromUndefinedLiteral(Literal literal) {
  cstring s_literal = cstring_from(literal->_raw->_raw);
  JSON_Value val = JSON_createObject();
  JSON_setField(val, "type", JSON_createString("UndefinedLiteral"));
  JSON_setField(val, "literal", JSON_createString(s_literal));
  Buffer_free(s_literal);
  return val;
}

JSON_Value JSON_fromRegexLiteral(Literal literal) {
  cstring s_literal = cstring_from(literal->_raw->_raw);
  JSON_Value val = JSON_createObject();
  JSON_setField(val, "type", JSON_createString("RegexLiteral"));
  JSON_setField(val, "literal", JSON_createString(s_literal));
  Buffer_free(s_literal);
  return val;
}

JSON_Value JSON_fromBigIntLiteral(Literal literal) {
  cstring s_literal = cstring_from(literal->_raw->_raw);
  JSON_Value val = JSON_createObject();
  JSON_setField(val, "type", JSON_createString("BigIntLiteral"));
  JSON_setField(val, "literal", JSON_createString(s_literal));
  Buffer_free(s_literal);
  return val;
}

JSON_Value JSON_fromLiteral(Literal literal) {
  switch (literal->_node->_type) {
  case NT_StringLiteral:
    return JSON_fromStringLiteral(literal);
  case NT_NumericLiteral:
    return JSON_fromNumberLiteral(literal);
  case NT_BooelanLiteral:
    return JSON_fromBooleanLiteral(literal);
  case NT_NullLiteral:
    return JSON_fromNullLiteral(literal);
  case NT_UndefinedLiteral:
    return JSON_fromUndefinedLiteral(literal);
  case NT_RegExLiteral:
    return JSON_fromRegexLiteral(literal);
  case NT_BigIntLiteral:
    return JSON_fromBigIntLiteral(literal);
  default:
    return NULL;
  }
}

JSON_Value JSON_fromImportSpecifier(ImportSpecifier import_spec) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("ImportSpecifier"));
  if (import_spec->_local) {
    JSON_setField(obj, "local", JSON_fromIdentifier(import_spec->_local));
  } else {
    JSON_setField(obj, "local", JSON_createNull());
  }
  if (import_spec->_imported) {
    JSON_setField(obj, "imported", JSON_fromIdentifier(import_spec->_imported));
  } else {
    JSON_setField(obj, "imported", JSON_createNull());
  }
  return obj;
}

JSON_Value JSON_fromImportDefaultSpecifier(ImportSpecifier import_spec) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("ImportDefaultSpecifier"));
  if (import_spec->_local) {
    JSON_setField(obj, "local", JSON_fromIdentifier(import_spec->_local));
  } else {
    JSON_setField(obj, "local", JSON_createNull());
  }
  return obj;
}

JSON_Value JSON_fromImportNamespaceSpecifier(ImportSpecifier import_spec) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("ImportNamespaceSpecifier"));
  if (import_spec->_local) {
    JSON_setField(obj, "local", JSON_fromIdentifier(import_spec->_local));
  } else {
    JSON_setField(obj, "local", JSON_createNull());
  }
  return obj;
}

JSON_Value JSON_fromImportAttribute(ImportAttribute import_attr) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("ImportAttribute"));
  JSON_setField(obj, "key", JSON_fromIdentifier(import_attr->_key));
  JSON_setField(obj, "value", JSON_fromLiteral(import_attr->_value));
  return obj;
}

JSON_Value JSON_fromImportStatement(ImportStatement import_s) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("ImportStatement"));
  JSON_Value specifiers = JSON_createArray();
  int32_t index = 0;
  for (List_Node node = List_head(import_s->_specifiers);
       node != List_tail(import_s->_specifiers); node = List_next(node)) {
    ImportSpecifier importSpecifier = (ImportSpecifier)List_get(node);
    if (importSpecifier->_node->_type == NT_ImportSpecifier) {
      JSON_setIndex(specifiers, index,
                    JSON_fromImportSpecifier(importSpecifier));
    } else if (importSpecifier->_node->_type == NT_ImportDefaultSpecifier) {
      JSON_setIndex(specifiers, index,
                    JSON_fromImportDefaultSpecifier(importSpecifier));
    } else if (importSpecifier->_node->_type == NT_ImportNamespaceSpecifier) {
      JSON_setIndex(specifiers, index,
                    JSON_fromImportNamespaceSpecifier(importSpecifier));
    }
    index++;
  }
  JSON_setField(obj, "specifiers", specifiers);
  JSON_Value accertions = JSON_createArray();
  index = 0;
  for (List_Node node = List_head(import_s->_assertions);
       node != List_tail(import_s->_assertions); node = List_next(node)) {
    ImportAttribute import_attr = (ImportAttribute)List_get(node);
    JSON_setIndex(accertions, index, JSON_fromImportAttribute(import_attr));
  }
  JSON_setField(obj, "accertions", accertions);
  JSON_setField(obj, "source", JSON_fromLiteral(import_s->_source));
  return obj;
}
JSON_Value JSON_fromStatement(Statement statement);
JSON_Value JSON_fromEmptyStatement(Statement empty_S) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("EmptyStatement"));
  return obj;
}
JSON_Value JSON_fromBlockStatement(BlockStatement block_s) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("BlockStatement"));
  JSON_Value body = JSON_createArray();
  int32_t index = 0;
  for (List_Node node = List_head(block_s->_body);
       node != List_tail(block_s->_body); node = List_next(node)) {
    Statement statement = (Statement)List_get(node);
    JSON_setIndex(body, index++, JSON_fromStatement(statement));
  }
  JSON_setField(obj, "body", body);
  return obj;
}

JSON_Value JSON_fromExpression(Expression expression) {
  JSON_Value obj = JSON_createObject();
  if (expression->_node->_type == NT_IdentifierExpression) {
    JSON_setField(obj, "type", JSON_createString("IdentifierExpression"));
    JSON_setField(obj, "identifier",
                  JSON_fromIdentifier(expression->_identifier));
  }
  if (expression->_node->_type == NT_LiteralExpression) {
    JSON_setField(obj, "type", JSON_createString("LiteralExpression"));
    JSON_setField(obj, "literal", JSON_fromLiteral(expression->_literal));
  }
  if (expression->_node->_type == NT_BracketExpression) {
    JSON_setField(obj, "type", JSON_createString("BracketExpression"));
    if (expression->_left) {
      JSON_setField(obj, "left", JSON_fromExpression(expression->_left));
    }
  }
  if (expression->_node->_type == NT_BinaryExpression) {
    JSON_setField(obj, "type", JSON_createString("BinaryExpression"));
    cstring s_opt = cstring_from(expression->_operator->_raw);
    JSON_setField(obj, "operator", JSON_createString(s_opt));
    Buffer_free(s_opt);
    if (expression->_left) {
      JSON_setField(obj, "left", JSON_fromExpression(expression->_left));
    }
    if (expression->_right) {
      JSON_setField(obj, "right", JSON_fromExpression(expression->_right));
    }
  }
  if (expression->_node->_type == NT_ComputeExpression) {
    JSON_setField(obj, "type", JSON_createString("ComputeExpression"));
    if (expression->_left) {
      JSON_setField(obj, "property", JSON_fromExpression(expression->_left));
    }
    if (expression->_right) {
      JSON_setField(obj, "field", JSON_fromExpression(expression->_right));
    }
  }
  if (expression->_node->_type == NT_OptionalComputeExpression) {
    JSON_setField(obj, "type", JSON_createString("OptionalComputeExpression"));
    if (expression->_left) {
      JSON_setField(obj, "property", JSON_fromExpression(expression->_left));
    }
    if (expression->_right) {
      JSON_setField(obj, "field", JSON_fromExpression(expression->_right));
    }
  }
  if (expression->_node->_type == NT_CallExpression) {
    JSON_setField(obj, "type", JSON_createString("CallExpression"));
    if (expression->_left) {
      JSON_setField(obj, "callee", JSON_fromExpression(expression->_left));
    }
    if (expression->_right) {
      JSON_setField(obj, "args", JSON_fromExpression(expression->_right));
    } else {
      JSON_setField(obj, "args", JSON_createNull());
    }
  }
  if (expression->_node->_type == NT_OptionalCallExpression) {
    JSON_setField(obj, "type", JSON_createString("OptionalCallExpression"));
    if (expression->_left) {
      JSON_setField(obj, "callee", JSON_fromExpression(expression->_left));
    }
    if (expression->_right) {
      JSON_setField(obj, "args", JSON_fromExpression(expression->_right));
    } else {
      JSON_setField(obj, "args", JSON_createNull());
    }
  }
  if (expression->_node->_type == NT_LambdaPattern) {
    JSON_setField(obj, "type", JSON_createString("LambdaPattern"));
    JSON_Value args = JSON_createArray();
    if (expression->_pattern._lambda->_args) {
      int index = 0;
      List list_args = expression->_pattern._lambda->_args;
      for (List_Node node = List_head(list_args); node != List_tail(list_args);
           node = List_next(node)) {
        Expression arg = (Expression)List_get(node);
        JSON_setIndex(args, index++, JSON_fromExpression(arg));
      }
    }
    JSON_setField(obj, "args", args);
    JSON_setField(obj, "body",
                  JSON_fromStatement(expression->_pattern._lambda->_body));
    JSON_setField(obj, "async",
                  JSON_createBoolean(expression->_pattern._lambda->_async));
  }

  if (expression->_node->_type == NT_FunctionPattern) {
    JSON_setField(obj, "type", JSON_createString("FunctionPattern"));
    JSON_Value args = JSON_createArray();
    if (expression->_pattern._function->_args) {
      int index = 0;
      List list_args = expression->_pattern._function->_args;
      for (List_Node node = List_head(list_args); node != List_tail(list_args);
           node = List_next(node)) {
        Expression arg = (Expression)List_get(node);
        JSON_setIndex(args, index++, JSON_fromExpression(arg));
      }
    }
    JSON_setField(obj, "args", args);
    JSON_setField(
        obj, "body",
        JSON_fromBlockStatement(expression->_pattern._function->_body));
    JSON_setField(obj, "name",
                  JSON_fromExpression(expression->_pattern._function->_name));
    JSON_setField(obj, "async",
                  JSON_createBoolean(expression->_pattern._function->_async));
    JSON_setField(
        obj, "generator",
        JSON_createBoolean(expression->_pattern._function->_generator));
    JSON_setField(obj, "setter",
                  JSON_createBoolean(expression->_pattern._function->_setter));
    JSON_setField(obj, "getter",
                  JSON_createBoolean(expression->_pattern._function->_getter));
  }
  if (expression->_node->_type == NT_ArrayPattern) {
    JSON_setField(obj, "type", JSON_createString("ArrayPattern"));
    JSON_Value items = JSON_createArray();
    int index = 0;
    for (List_Node node = List_head(expression->_pattern._array->_items);
         node != List_tail(expression->_pattern._array->_items);
         node = List_next(node)) {
      Expression item = (Expression)List_get(node);
      JSON_setIndex(items, index++, JSON_fromExpression(item));
    }
    JSON_setField(obj, "items", items);
  }
  if (expression->_node->_type == NT_TemplatePattern) {
    JSON_setField(obj, "type", JSON_createString("TemplatePattern"));
    if (expression->_pattern._template->_tag) {
      JSON_setField(obj, "tag",
                    JSON_fromIdentifier(expression->_pattern._template->_tag));
    }
    JSON_Value _strings = JSON_createArray();
    int index = 0;
    List list_strings = expression->_pattern._template->_strings;
    for (List_Node node = List_head(list_strings);
         node != List_tail(list_strings); node = List_next(node)) {
      Token str = (Token)List_get(node);
      cstring source = cstring_from(str->_raw);
      JSON_setIndex(_strings, index++, JSON_createString(source));
      Buffer_free(source);
    }
    JSON_setField(obj, "strings", _strings);
    index = 0;
    List list_parts = expression->_pattern._template->_parts;
    JSON_Value parts = JSON_createArray();
    for (List_Node node = List_head(list_parts); node != List_tail(list_parts);
         node = List_next(node)) {
      Expression item = (Expression)List_get(node);
      JSON_setIndex(parts, index++, JSON_fromExpression(item));
    }
    JSON_setField(obj, "parts", parts);
  }
  if (expression->_node->_type == NT_ObjectPattern) {
    JSON_setField(obj, "type", JSON_createString("ObjectPattern"));
    JSON_Value properties = JSON_createArray();
    int index = 0;
    for (List_Node node = List_head(expression->_pattern._array->_items);
         node != List_tail(expression->_pattern._array->_items);
         node = List_next(node)) {
      ObjectProperty item = (ObjectProperty)List_get(node);
      JSON_Value property = JSON_createObject();
      if (item->_key) {
        JSON_setField(property, "key", JSON_fromExpression(item->_key));
      }
      if (item->_value) {
        JSON_setField(property, "value", JSON_fromExpression(item->_value));
      }
      JSON_setIndex(properties, index++, property);
    }
    JSON_setField(obj, "properties", properties);
  }
  if (expression->_node->_type == NT_AwaitExpression) {
    JSON_setField(obj, "type", JSON_createString("AwaitExpression"));
    if (expression->_right) {
      JSON_setField(obj, "expr", JSON_fromExpression(expression->_right));
    }
  }
  return obj;
}

JSON_Value JSON_fromExpressionStatement(ExpressionStatement statement) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("ExpressionStatement"));
  if (statement->_expression) {
    JSON_setField(obj, "expression",
                  JSON_fromExpression(statement->_expression));
  }
  return obj;
}

JSON_Value JSON_fromStatement(Statement statement) {
  switch (statement->_node->_type) {
  case NT_ImportStatement:
    return JSON_fromImportStatement((ImportStatement)statement);
  case NT_EmptyStatement:
    return JSON_fromEmptyStatement(statement);
  case NT_BlockStatement:
    return JSON_fromBlockStatement((BlockStatement)statement);
  case NT_ExpressionStatement:
    return JSON_fromExpressionStatement((ExpressionStatement)statement);
  default:
    return NULL;
  }
}

JSON_Value JSON_fromInterpreter(Interpreter interpreter) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Interpreter"));
  cstring s_interpreter = cstring_from(interpreter->_interpreter);
  JSON_setField(obj, "interpreter", JSON_createString(s_interpreter));
  Buffer_free(s_interpreter);
  return obj;
}

JSON_Value JSON_fromDirective(Directive directive) {
  JSON_Value obj = JSON_createObject();
  JSON_setField(obj, "type", JSON_createString("Directive"));
  cstring s_directive = cstring_from(directive->_directive);
  JSON_setField(obj, "directive", JSON_createString(s_directive));
  Buffer_free(s_directive);
  return obj;
}

JSON_Value JSON_fromProgram(Program program) {
  JSON_Value obj = JSON_createObject();
  if (program->_interpreter) {
    JSON_setField(obj, "interpreter",
                  JSON_fromInterpreter(program->_interpreter));
  }
  JSON_setField(obj, "type", JSON_createString("Program"));
  JSON_Value directives = JSON_createArray();
  int32_t index = 0;
  for (List_Node node = List_head(program->_directives);
       node != List_tail(program->_directives); node = List_next(node)) {
    Directive directive = (Directive)List_get(node);
    JSON_setIndex(directives, index++, JSON_fromDirective(directive));
  }
  JSON_setField(obj, "directives", directives);
  JSON_Value statements = JSON_createArray();
  index = 0;
  for (List_Node node = List_head(program->_body);
       node != List_tail(program->_body); node = List_next(node)) {
    Statement statement = (Statement)List_get(node);
    JSON_setIndex(statements, index++, JSON_fromStatement(statement));
  }
  JSON_setField(obj, "statements", statements);
  return obj;
}