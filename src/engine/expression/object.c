#include "ast.h"
ObjectProperty ObjectProperty_create() {
  ObjectProperty property =
      (ObjectProperty)Buffer_alloc(sizeof(struct s_ObjectProperty));
  property->_key = NULL;
  property->_value = NULL;
  return property;
}

void ObjectProperty_dispose(ObjectProperty property) {
  if (property->_key) {
    Expression_dispose(property->_key);
  }
  if (property->_value) {
    Expression_dispose(property->_value);
  }
  Buffer_free(property);
}

ObjectPattern ObjectPattern_create() {
  ObjectPattern pattern =
      (ObjectPattern)Buffer_alloc(sizeof(struct s_ObjectPattern));
  List_Option opt = {1, (Buffer_Free)ObjectProperty_dispose};
  pattern->_properties = List_create(opt);
  return pattern;
}
void ObjectPattern_dispose(ObjectPattern pattern) {
  List_dispose(pattern->_properties);
  Buffer_free(pattern);
}

Expression readPropertyFunctionDefinition(SourceFile file,
                                          cstring source) { // (xxxx){xxxx}
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  FunctionPattern pattern = FunctionPattern_create();
  if (!checkToken(token, TT_Symbol, "(")) {
    Token_dispose(token);
    goto failed;
  }
  selector = token->_raw.end;
  Token_dispose(token);
  Context *context = pushContext();
  enableCommaTail();
  Expression arg = readExpression(file, selector);
  while (arg) {
    List_insert_tail(pattern->_args, arg);
    selector = arg->_node->_position.end;
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      popContext(context);
      goto failed;
    }
    if (!checkToken(token, TT_Symbol, ",")) {
      Token_dispose(token);
      break;
    } else {
      selector = token->_raw.end;
      Token_dispose(token);
    }
    arg = readExpression(file, selector);
  }
  disableCommaTail();
  popContext(context);
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!checkToken(token, TT_Symbol, ")")) {
    Token_dispose(token);
    Error error = {"Unexcept token: missing token ')'",
                   getLocation(file, selector)};
    setAstError(error);
    goto failed;
  }
  selector = token->_raw.end;
  Token_dispose(token);
  BlockStatement body = readBlockStatement(file, selector);
  if (!body) {
    if (!getAstError().error) {
      Error error = {"Unexcept token: missing token '{'",
                     getLocation(file, selector)};
      setAstError(error);
    }
    goto failed;
  }
  selector = body->_node->_position.end;
  pattern->_body = body;
  Expression func_expr = Expression_create();
  func_expr->_pattern._function = pattern;
  func_expr->_node->_type = NT_FunctionPattern;
  func_expr->_node->_position.begin = source;
  func_expr->_node->_position.end = selector;
  return func_expr;
failed:
  FunctionPattern_dispose(pattern);
  return NULL;
}

Expression readComputeKey(SourceFile file, cstring source) { // [xxxx]
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  if (!checkToken(token, TT_Symbol, "[")) {
    Token_dispose(token);
    return NULL;
  }
  Context *current = pushContext();
  cstring selector = token->_raw.end;
  Token_dispose(token);
  Expression key = readExpression(file, selector);
  if (!key) {
    popContext(current);
    return NULL;
  }
  selector = key->_node->_position.end;
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    popContext(current);
    return NULL;
  }
  if (!checkToken(token, TT_Symbol, "]")) {
    Token_dispose(token);
    popContext(current);
    return NULL;
  }
  selector = token->_raw.end;
  Token_dispose(token);
  popContext(current);
  key->_node->_position.begin = source;
  key->_node->_position.end = selector;
  return key;
}

Expression readPropertyKey(SourceFile file, cstring source) { // xxx|[xxxx]
  Token token = readTokenSkipNewline(file, source);
  if (token->_type == TT_Identifier) {
    Token_dispose(token);
    return readIdentifierExpression(file, source);
  } else if (checkToken(token, TT_Symbol, "[")) {
    Token_dispose(token);
    return readComputeKey(file, source);
  }
  Token_dispose(token);
  return NULL;
}

Expression readPropertyFunction(SourceFile file, cstring source) {
  cstring selector = source;
  Expression name = readPropertyKey(file, selector);
  if (!name) {
    return NULL;
  }
  selector = name->_node->_position.end;
  Expression func = readPropertyFunctionDefinition(file, selector);
  if (!func) {
    Expression_dispose(name);
    return NULL;
  }
  func->_pattern._function->_name = name;
  return func;
}

ObjectProperty readProperty(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  ObjectProperty property = ObjectProperty_create();
  cstring selector = source;
  if (checkToken(token, TT_Symbol, "...")) {
    Token_dispose(token);
    property->_value = readExpression(file, selector);
  } else if (checkToken(token, TT_Symbol, "*")) {
    selector = token->_raw.end;
    Token_dispose(token);
    Expression func = readPropertyFunction(file, selector);
    if (!func) {
      if (!getAstError().error) {
        Error error = {"Unexcept token", getLocation(file, selector)};
        setAstError(error);
      }
      goto failed;
    }
    func->_pattern._function->_generator = 1;
    property->_value = func;
  } else if (checkToken(token, TT_Keyword, "async")) {
    selector = token->_raw.end;
    Token_dispose(token);
    int generator = 0;
    token = readTokenSkipNewline(file, selector);
    if (checkToken(token, TT_Symbol, "*")) {
      generator = 1;
      selector = token->_raw.end;
      Token_dispose(token);
    } else {
      Token_dispose(token);
    }
    Expression func = readPropertyFunction(file, selector);
    if (!func) {
      if (!getAstError().error) {
        Error error = {"Unexcept token", getLocation(file, selector)};
        setAstError(error);
      }
      goto failed;
    }
    func->_pattern._function->_generator = generator;
    func->_pattern._function->_async = 1;
    property->_value = func;
  } else if (checkToken(token, TT_Symbol, "[") ||
             token->_type == TT_Identifier) {
    Token_dispose(token);
    Expression name = readPropertyKey(file, selector);
    if (!name) {
      goto failed;
    }
    selector = name->_node->_position.end;
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      Expression_dispose(name);
      goto failed;
    }
    if (checkToken(token, TT_Symbol, "(")) {
      Token_dispose(token);
      Expression func = readPropertyFunctionDefinition(file, selector);
      if (!func) {
        Expression_dispose(name);
        if (!getAstError().error) {
          Error error = {"Unexcept token", getLocation(file, selector)};
          setAstError(error);
        }
        goto failed;
      }
      func->_pattern._function->_name = name;
      func->_node->_position.begin = name->_node->_position.begin;
      property->_value = func;
      selector = func->_node->_position.begin;
    } else if (checkToken(token, TT_Symbol, ":")) {
      selector = token->_raw.end;
      Token_dispose(token);
      Expression value = readExpression(file, selector);
      if (!value) {
        Expression_dispose(name);
        goto failed;
      }
      property->_key = name;
      property->_value = value;
      selector = property->_value->_node->_position.end;
    } else if (checkToken(token, TT_Symbol, ",") ||
               checkToken(token, TT_Symbol, "}")) {
      Token_dispose(token);
      property->_value = name;
    } else {
      Expression_dispose(name);
      if (!getAstError().error) {
        Error error = {"Unexcept token", getLocation(file, selector)};
        setAstError(error);
      }
      goto failed;
    }
  } else if (checkToken(token, TT_Keyword, "get")) {
    selector = token->_raw.end;
    Token_dispose(token);
    Expression getter = readPropertyFunction(file, selector);
    if (!getter) {
      if (!getAstError().error) {
        Error error = {"Unexcept token", getLocation(file, selector)};
        setAstError(error);
      }
      goto failed;
    }
    getter->_pattern._function->_getter = 1;
    selector = getter->_node->_position.end;
    property->_value = getter;
  } else if (checkToken(token, TT_Keyword, "set")) {
    selector = token->_raw.end;
    Token_dispose(token);
    Expression getter = readPropertyFunction(file, selector);
    if (!getter) {
      if (!getAstError().error) {
        Error error = {"Unexcept token", getLocation(file, selector)};
        setAstError(error);
      }
      goto failed;
    }
    getter->_pattern._function->_setter = 1;
    selector = getter->_node->_position.end;
    property->_value = getter;
  } else {
    Token_dispose(token);
    goto failed;
  }
  return property;
failed:
  ObjectProperty_dispose(property);
  return NULL;
}

Expression readObjectExpression(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  if (!checkToken(token, TT_Symbol, "{")) {
    Token_dispose(token);
    return NULL;
  }
  cstring selector = token->_raw.end;
  Token_dispose(token);
  ObjectPattern pattern = ObjectPattern_create();
  Context *context = pushContext();
  enableCommaTail();
  ObjectProperty property = readProperty(file, selector);
  while (property) {
    List_insert_tail(pattern->_properties, property);
    selector = property->_value->_node->_position.end;
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      ObjectPattern_dispose(pattern);
      popContext(context);
      return NULL;
    }
    if (!checkToken(token, TT_Symbol, ",")) {
      Token_dispose(token);
      break;
    }
    selector = token->_raw.end;
    Token_dispose(token);
    property = readProperty(file, selector);
  }
  disableCommaTail();
  popContext(context);
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    ObjectPattern_dispose(pattern);
    return NULL;
  }
  if (!checkToken(token, TT_Symbol, "}")) {
    Token_dispose(token);
    ObjectPattern_dispose(pattern);
    if (!getAstError().error) {
      Error error = {"Unexcept token: missing token '}'",
                     getLocation(file, selector)};
      setAstError(error);
    }
    return NULL;
  }
  selector = token->_raw.end;
  Token_dispose(token);
  Expression object_pattern = Expression_create();
  object_pattern->_pattern._object = pattern;
  object_pattern->_node->_type = NT_ObjectPattern;
  object_pattern->_node->_position.begin = source;
  object_pattern->_node->_position.end = selector;
  object_pattern->_level = -2;
  return object_pattern;
}