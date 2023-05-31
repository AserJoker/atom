#include "ast.h"

ObjectProperty ObjectProperty_create() {
  ObjectProperty property =
      (ObjectProperty)Buffer_alloc(sizeof(struct s_ObjectProperty));
  property->node = AstNode_create();
  property->node->type = NT_ObjectProperty;
  property->key = NULL;
  property->value = NULL;
  return property;
}
void ObjectProperty_dispose(ObjectProperty property) {
  if (property->key) {
    Expression_dispose(property->key);
  }
  if (property->value) {
    Expression_dispose(property->value);
  }
  AstNode_dispose(property->node);
  Buffer_free(property);
}

Object Object_create() {
  Object obj = (Object)Buffer_alloc(sizeof(struct s_Object));
  obj->node = AstNode_create();
  obj->node->type = NT_Object;
  List_Option opt = {1, (Buffer_Free)ObjectProperty_dispose};
  obj->properties = List_create(opt);
  return obj;
}
void Object_dispose(Object obj) {
  List_dispose(obj->properties);
  AstNode_dispose(obj->node);
  Buffer_free(obj);
}

Expression ObjectPattern_create() {
  Expression expression = Expression_create();
  expression->node->type = NT_ObjectPattern;
  expression->level = -2;
  return expression;
}
void ObjectPattern_dispose(Expression expression) {
  if (expression->object) {
    Object_dispose(expression->object);
  }
  AstNode_dispose(expression->node);
  Buffer_free(expression);
}
ObjectProperty readObjectPreperty(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  
}

Object readObject(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  if (!Token_check(token, TT_Symbol, "{")) {
    Token_dispose(token);
    ErrorStack_push(Error_init("Unexcept token.missing token '{'",
                               getLocation(file, source), NULL));
    return NULL;
  }
  cstring selector = token->raw.end;
  Token_dispose(token);
  Object obj = Object_create();
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!Token_check(token, TT_Symbol, "}")) {
    Token_dispose(token);
    ObjectProperty property = readObjectPreperty(file, selector);
    if (!property) {
      goto failed;
    }
    for (;;) {
      List_insert_tail(obj->properties, property);
      selector = property->node->position.end;
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (Token_check(token, TT_Symbol, ",")) {
        selector = token->raw.end;
        Token_dispose(token);
      } else if (Token_check(token, TT_Symbol, "}")) {
        break;
      } else {
        Token_dispose(token);
        ErrorStack_push(Error_init("Unexception token.missing token '}'",
                                   getLocation(file, selector), NULL));
        goto failed;
      }
    }
  }
  selector = token->raw.end;
  Token_dispose(token);
  obj->node->position.begin = source;
  obj->node->position.end = selector;
  return obj;
failed:
  Object_dispose(obj);
  return NULL;
}
Expression readObjectPattern(SourceFile file, cstring source) {
  Object obj = readObject(file, source);
  if (!obj) {
    return NULL;
  }
  Expression obj_expr = ObjectPattern_create();
  obj_expr->object = obj;
  obj_expr->node->position = obj->node->position;
  return obj_expr;
}