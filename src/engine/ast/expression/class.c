#include "ast.h"

ClassProperty ClassProperty_create() {
  ClassProperty property =
      (ClassProperty)Buffer_alloc(sizeof(struct s_ClassProperty));
  property->node = AstNode_create();
  List_Option opt_dec = {1, (Buffer_Free)Expression_dispose};
  property->decorators = List_create(opt_dec);
  property->field.init = NULL;
  property->field.key = NULL;
  property->field.private_ = 0;
  property->field.static_ = 0;
  property->node->type = NT_ClassProperty;
  return property;
}
void ClassProperty_dispose(ClassProperty property) {
  List_dispose(property->decorators);
  if (property->node->type == NT_StaticBlock) {
    BlockStatement_dispose(property->staticblock);
  } else {
    if (property->field.key) {
      Expression_dispose(property->field.key);
    }
    if (property->field.init) {
      Expression_dispose(property->field.init);
    }
  }
  AstNode_dispose(property->node);
  Buffer_free(property);
}
ClassProperty readClassProperty(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  ClassProperty property = ClassProperty_create();
  if (Token_check(token, TT_Symbol, "@")) {
    while (Token_check(token, TT_Symbol, "@")) {
      selector = token->raw.end;
      Token_dispose(token);
      Expression deco = readExpression(file, selector);
      if (!deco) {
        goto failed;
      }
      List_insert_tail(property->decorators, deco);
      selector = deco->node->position.end;
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
    }
  }
  if (Token_check(token, TT_Keyword, "static")) {
    selector = token->raw.end;
    Token_dispose(token);
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
    if (Token_check(token, TT_Symbol, "{")) {
      Token_dispose(token);
      property->node->type = NT_StaticBlock;
      Statement block = readBlockStatement(file, selector);
      if (!block) {
        goto failed;
      }
      property->staticblock = block;
      selector = block->node->position.end;
    } else {
      property->field.static_ = 1;
    }
  }
  if (Token_check(token, TT_Keyword, "async")) {
    selector = token->raw.end;
    Token_dispose(token);
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
    if (Token_check(token, TT_Symbol, "#")) {
      selector = token->raw.end;
      Token_dispose(token);
      property->field.private_ = 1;
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
    }
    Token_dispose(token);
    Function fn = readFunctionDefinition(NULL, file, selector);
    if (!fn) {
      goto failed;
    }
    fn->async = 1;
    Expression expr = Expression_create();
    expr->node->type = NT_FunctionExpression;
    expr->function = fn;
    expr->node->position = fn->node->position;
    property->field.init = expr;
    selector = fn->node->position.end;
  } else if (token->type == TT_Identifier ||
             Token_check(token, TT_Symbol, "[") ||
             Token_check(token, TT_Symbol, "#")) {
    if (Token_check(token, TT_Symbol, "#")) {
      property->field.private_ = 1;
      selector = token->raw.end;
      Token_dispose(token);
      token = readTokenSkipNewline(file, selector);
    }
    if (Token_check(token, TT_Symbol, "[")) {
      selector = token->raw.end;
      Token_dispose(token);
      Expression compute = readExpression(file, selector);
      if (!compute) {
        goto failed;
      }
      property->field.compute = compute;
      selector = compute->node->position.end;
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (!Token_check(token, TT_Symbol, "]")) {
        Token_dispose(token);
        ErrorStack_push(Error_init("Unexcept token.missing token ']'.",
                                   getLocation(file, selector), NULL));
        goto failed;
      }
      selector = token->raw.end;
      Token_dispose(token);
    } else {
      Token_dispose(token);
      Expression identifier = readIdentifierExpression(file, selector);
      if (!identifier) {
        goto failed;
      }
      property->field.key = identifier;
      selector = identifier->node->position.end;
    }
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
    if (Token_check(token, TT_Symbol, "(")) {
      Token_dispose(token);
      if (property->field.key) {
        selector = property->field.key->node->position.begin;
        Expression_dispose(property->field.key);
      }
      if (property->field.compute) {
        selector = property->field.compute->node->position.begin;
        Expression_dispose(property->field.compute);
      }
      property->field.key = NULL;
      Function fn = readFunctionDefinition(NULL, file, selector);
      if (!fn) {
        goto failed;
      }
      Expression expr = Expression_create();
      expr->node->type = NT_FunctionExpression;
      expr->node->position = fn->node->position;
      expr->function = fn;
      selector = expr->node->position.end;
      property->field.init = expr;
    } else if (Token_check(token, TT_Symbol, "=")) {
      selector = token->raw.end;
      Token_dispose(token);
      Expression init = readExpression(file, selector);
      if (!init) {
        goto failed;
      }
      property->field.init = init;
      selector = init->node->position.end;
    }
  } else {
    Token_dispose(token);
    ErrorStack_push(
        Error_init("Unexcept token.", getLocation(file, selector), NULL));
    goto failed;
  }
  property->node->position.begin = source;
  property->node->position.end = selector;
  return property;
failed:
  ClassProperty_dispose(property);
  return NULL;
}

Class Class_create() {
  Class clazz = (Class)Buffer_alloc(sizeof(struct s_Class));
  clazz->node = AstNode_create();
  clazz->node->type = NT_Class;
  List_Option opt_dec = {1, (Buffer_Free)Expression_dispose};
  clazz->decorators = List_create(opt_dec);
  List_Option opt_prop = {1, (Buffer_Free)ClassProperty_dispose};
  clazz->properties = List_create(opt_prop);
  clazz->name = NULL;
  return clazz;
}

Class readClass(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  Class clazz = Class_create();
  if (Token_check(token, TT_Symbol, "@")) {
    while (Token_check(token, TT_Symbol, "@")) {
      selector = token->raw.end;
      Token_dispose(token);
      Expression deco = readExpression(file, selector);
      if (!deco) {
        goto failed;
      }
      List_insert_tail(clazz->decorators, deco);
      selector = deco->node->position.end;
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
    }
  }
  if (Token_check(token, TT_Keyword, "class")) {
    selector = token->raw.end;
    Token_dispose(token);
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (token->type == TT_Identifier) {
    Token_dispose(token);
    clazz->name = readIdentifier(file, selector);
    if (!clazz->name) {
      goto failed;
    }
    selector = clazz->name->node->position.end;
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (Token_check(token, TT_Keyword, "extends")) {
    selector = token->raw.end;
    Token_dispose(token);
    Expression extends = readExpression(file, selector);
    if (!extends) {
      goto failed;
    }
    clazz->extends = extends;
    selector = extends->node->position.end;
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (!Token_check(token, TT_Symbol, "{")) {
    Token_dispose(token);
    ErrorStack_push(Error_init("Unexcept token.missing token '{'",
                               getLocation(file, selector), NULL));
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!Token_check(token, TT_Symbol, "}")) {
    Token_dispose(token);
    ClassProperty property = readClassProperty(file, selector);
    if (!property) {
      goto failed;
    }
    for (;;) {
      List_insert_tail(clazz->properties, property);
      selector = property->node->position.end;
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (Token_check(token, TT_Symbol, ";")) {
        selector = token->raw.end;
        Token_dispose(token);
      }
      if (Token_check(token, TT_Symbol, "}")) {
        break;
      }
      Token_dispose(token);
      property = readClassProperty(file, selector);
      if (!property) {
        goto failed;
      }
    }
  }
  if (!Token_check(token, TT_Symbol, "}")) {
    Token_dispose(token);
    ErrorStack_push(Error_init("Unexcept token.missing token '}'",
                               getLocation(file, selector), NULL));
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  clazz->node->position.begin = source;
  clazz->node->position.end = selector;
  return clazz;
failed:
  Class_dispose(clazz);
  return NULL;
}

void Class_dispose(Class clazz) {
  List_dispose(clazz->decorators);
  List_dispose(clazz->properties);
  AstNode_dispose(clazz->node);
  if (clazz->name) {
    Identifier_dispose(clazz->name);
  }
  if (clazz->extends) {
    Expression_dispose(clazz->extends);
  }
  Buffer_free(clazz);
}

void ClassPattern_dispose(Expression expression) {
  if (expression->clazz) {
    Class_dispose(expression->clazz);
  }
  AstNode_dispose(expression->node);
  Buffer_free(expression);
}
Expression ClassPattern_create() {
  Expression expression = Expression_create();
  expression->node->type = NT_ClassPattern;
  expression->level = -2;
  expression->clazz = NULL;
  return expression;
}
Expression readClassPattern(SourceFile file, cstring source) {
  Class clazz = readClass(file, source);
  if (!clazz) {
    return NULL;
  }
  Expression clazz_expr = ClassPattern_create();
  clazz_expr->clazz = clazz;
  clazz_expr->node->position = clazz->node->position;
  return clazz_expr;
}