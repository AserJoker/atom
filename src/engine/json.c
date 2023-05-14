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

JSON_Value JSON_fromLiteral(Literal literal) {
  switch (literal->_node->_type) {
  case NT_StringLiteral:
    return JSON_fromStringLiteral(literal);
  case NT_NumericLiteral:
  case NT_BooelanLiteral:
  case NT_NullLiteral:
  case NT_UndefinedLiteral:
  case NT_RegExLiteral:
  case NT_BigIntLiteral:
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
    }
    index++;
  }
  JSON_setField(obj, "specifiers", specifiers);
  JSON_setField(obj, "source", JSON_fromLiteral(import_s->_source));
  return obj;
}

JSON_Value JSON_fromStatement(Statement statement) {
  switch (statement->_node->_type) {
  case NT_ImportStatement:
    return JSON_fromImportStatement((ImportStatement)statement);
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
  JSON_setField(obj, "interpreter",
                JSON_fromInterpreter(program->_interpreter));
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