#include "expression.h"
#include "statement.h"
static void ObjectProperty_dispose(ObjectProperty property) {
  if (property->key) {
    Expression_dispose(property->key);
  }
  if (property->value) {
    Expression_dispose(property->value);
  }
  Buffer_free(property);
}
static ObjectProperty readObjectProperty(SourceFile file, cstring source) {
  ObjectProperty property =
      (ObjectProperty)Buffer_alloc(sizeof(struct s_ObjectProperty));
  property->key = NULL;
  property->value = NULL;
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  int async = 0;
  int getter = 0;
  int setter = 0;
  int generator = 0;
  if (checkToken(token, TT_Keyword, "async")) {
    async = 1;
    selector = token->raw.end;
    Token_dispose(token);
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (checkToken(token, TT_Keyword, "get")) {
    getter = 1;
    selector = token->raw.end;
    Token_dispose(token);
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (checkToken(token, TT_Keyword, "set")) {
    setter = 1;
    selector = token->raw.end;
    Token_dispose(token);
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (checkToken(token, TT_Symbol, "*")) {
    generator = 1;
    selector = token->raw.end;
    Token_dispose(token);
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (token->type == TT_Identifier) {
    Token_dispose(token);
    property->key = readIdentifierExpression(file, selector);
    selector = property->key->node->position.end;
  } else if (checkToken(token, TT_Symbol, "[")) {
    Token_dispose(token);
    property->key = readComputeExpression(file, selector);
    selector = property->key->node->position.end;
  } else if (checkToken(token, TT_Symbol, "...")) {
    Token_dispose(token);
    ExpressionContext ectx = pushExpressionContext();
    setMaxOperatorLevel(12);
    property->value = readExpression(file, selector);
    popExpressionContext(ectx);
    if (!property->value) {
      goto failed;
    }
    return property;
  } else {
    pushError("Unexcept token.", getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  token = readTokenSkipNewline(file, selector);
  if (checkToken(token, TT_Symbol, ":")) {
    selector = token->raw.end;
    Token_dispose(token);
    ExpressionContext ectx = pushExpressionContext();
    setMaxOperatorLevel(12);
    property->value = readExpression(file, selector);
    popExpressionContext(ectx);
    if (!property->value) {
      goto failed;
    }
  } else if (checkToken(token, TT_Symbol, "(")) {
    Function func = (Function)Buffer_alloc(sizeof(struct s_Function));
    List_Option opt = {1, (Buffer_Free)Expression_dispose};
    func->args = List_create(opt);
    func->async = async;
    func->generator = generator;
    func->getter = getter;
    func->setter = setter;
    func->name = NULL;
    func->body = NULL;
    property->value = Expression_create();
    property->value->type = ET_Function;
    property->value->function = func;
    selector = token->raw.end;
    property->value->node->position.begin = source;
    Token_dispose(token);
    token = readTokenSkipNewline(file, selector);
    if (!checkToken(token, TT_Symbol, ")")) {
      Token_dispose(token);
      ExpressionContext ectx = pushExpressionContext();
      setMaxOperatorLevel(12);
      Expression arg = readExpression(file, selector);
      popExpressionContext(ectx);
      if (!arg) {
        goto failed;
      }
      for (;;) {
        List_insert_tail(func->args, arg);
        selector = arg->node->position.end;
        token = readTokenSkipNewline(file, selector);
        if (!token) {
          goto failed;
        }
        if (checkToken(token, TT_Symbol, ",")) {
          selector = token->raw.end;
          Token_dispose(token);
        } else {
          break;
        }
        ectx = pushExpressionContext();
        setMaxOperatorLevel(12);
        arg = readExpression(file, selector);
        popExpressionContext(ectx);
        if (!arg) {
          goto failed;
        }
      }
    }
    if (!checkToken(token, TT_Symbol, ")")) {
      pushError("Unexcept token.missing token ')'",
                getLocation(file, token->raw.begin));
      Token_dispose(token);
      goto failed;
    }
    selector = token->raw.end;
    Token_dispose(token);
    func->body = readStatement(file, selector);
    if (!func->body) {
      goto failed;
    }
    selector = func->body->node->position.end;
    property->value->node->position.end = selector;
  } else {
    Token_dispose(token);
  }
  return property;
failed:
  ObjectProperty_dispose(property);
  return NULL;
}
int isObjectExpression(SourceFile file, Token token) {
  return checkToken(token, TT_Symbol, "{");
}
Expression readObjectExpression(SourceFile file, cstring source) {
  Expression expr = Expression_create();
  expr->type = ET_Object;
  expr->bind = BT_None;
  expr->object = (Object)Buffer_alloc(sizeof(struct s_Object));
  List_Option opt = {1, (Buffer_Free)ObjectProperty_dispose};
  expr->object->properties = List_create(opt);
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!checkToken(token, TT_Symbol, "}")) {
    Token_dispose(token);
    ObjectProperty property = readObjectProperty(file, selector);
    if (!property) {
      goto failed;
    }
    for (;;) {
      List_insert_tail(expr->object->properties, property);
      if (property->key) {
        selector = property->key->node->position.end;
      }
      if (property->value) {
        selector = property->value->node->position.end;
      }
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (checkToken(token, TT_Symbol, ",")) {
        selector = token->raw.end;
      }
      Token_dispose(token);
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (checkToken(token, TT_Symbol, "}")) {
        break;
      }
      Token_dispose(token);
      property = readObjectProperty(file, selector);
      if (!property) {
        goto failed;
      }
    }
  }
  if (!checkToken(token, TT_Symbol, "}")) {
    pushError("Unexcept token.missing token '}'",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  expr->node->position.begin = source;
  expr->node->position.end = selector;
  return expr;
failed:
  Expression_dispose(expr);
  return NULL;
}