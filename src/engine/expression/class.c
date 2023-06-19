#include "expression.h"
#include "statement.h"
static void ClassProperty_dispose(ClassProperty property) {
  if (property->key) {
    Expression_dispose(property->key);
  }
  if (property->value) {
    Expression_dispose(property->value);
  }
  List_dispose(property->decorators);
  Buffer_free(property);
}
static ClassProperty readClassProperty(SourceFile file, cstring source) {
  ClassProperty property =
      (ClassProperty)Buffer_alloc(sizeof(struct s_ClassProperty));
  property->isPrivate = 0;
  property->key = NULL;
  property->value = NULL;
  List_Option opt = {1, (Buffer_Free)Expression_dispose};
  property->decorators = List_create(opt);
  int async = 0;
  int private = 0;
  int getter = 0;
  int setter = 0;
  int generator = 0;
  int isStatic = 0;
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (checkToken(token, TT_Symbol, "@")) {
    for (;;) {
      selector = token->raw.end;
      Token_dispose(token);
      ExpressionContext ectx = pushExpressionContext();
      Expression dec = readExpression(file, selector);
      popExpressionContext(ectx);
      if (!dec) {
        goto failed;
      }
      List_insert_tail(property->decorators, dec);
      selector = dec->node->position.end;
      token = readTokenSkipNewline(file, selector);
      if (checkToken(token, TT_Symbol, "@")) {
        continue;
      } else {
        break;
      }
    }
  }
  if (checkToken(token, TT_Keyword, "static")) {
    isStatic = 1;
    selector = token->raw.end;
    Token_dispose(token);
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
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
  if (checkToken(token, TT_Symbol, "#")) {
    private = 1;
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
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  } else if (checkToken(token, TT_Symbol, "[")) {
    Token_dispose(token);
    property->key = readComputeExpression(file, selector);
    selector = property->key->node->position.end;
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  } else {
    pushError("Unexcept token", getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  if (checkToken(token, TT_Symbol, "=")) {
    selector = token->raw.end;
    Token_dispose(token);
    ExpressionContext ectx = pushExpressionContext();
    property->value = readExpression(file, selector);
    popExpressionContext(ectx);
    if (!property->value) {
      goto failed;
    }
  } else if (checkToken(token, TT_Symbol, "(")) {
    selector = token->raw.end;
    Token_dispose(token);
    Function func = (Function)Buffer_alloc(sizeof(struct s_Function));
    List_Option opt = {1, (Buffer_Free)Expression_dispose};
    func->args = List_create(opt);
    func->async = async;
    func->body = NULL;
    func->generator = generator;
    func->getter = getter;
    func->name = NULL;
    func->setter = setter;
    property->value = Expression_create();
    property->value->type = ET_Function;
    property->value->function = func;
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
    if (!checkToken(token, TT_Symbol, ")")) {
      Token_dispose(token);
      ExpressionContext ectx = pushExpressionContext();
      setMaxOperatorLevel(12);
      Expression arg = readExpression(file, selector);
      popExpressionContext(ectx);
      for (;;) {
        List_insert_tail(property->value->function->args, arg);
        selector = arg->node->position.end;
        token = readTokenSkipNewline(file, selector);
        if (!token) {
          goto failed;
        }
        if (checkToken(token, TT_Symbol, ",")) {
          selector = token->raw.end;
        } else if (checkToken(token, TT_Symbol, ")")) {
          break;
        }
        Token_dispose(token);
        ExpressionContext ectx = pushExpressionContext();
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
    property->value->function->body = readBlockStatement(file, selector);
    if (!property->value->function->body) {
      goto failed;
    }
    selector = property->value->function->body->node->position.end;
    property->value->node->position.end = selector;
  } else {
    Token_dispose(token);
  }
  property->isPrivate = private;
  property->isStatic = isStatic;
  return property;
failed:
  ClassProperty_dispose(property);
  return NULL;
}
static Statement readStaticBlock(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  selector = token->raw.end;
  Token_dispose(token);
  return readBlockStatement(file, selector);
}
static int isStaticBlock(SourceFile file, Token token) {
  if (checkToken(token, TT_Keyword, "static")) {
    Token next = readTokenSkipNewline(file, token->raw.end);
    if (next) {
      if (checkToken(next, TT_Symbol, "{")) {
        Token_dispose(next);
        return 1;
      }
      Token_dispose(next);
    }
  }
  return 0;
}
int isClassExpression(SourceFile file, Token token) {
  return checkToken(token, TT_Symbol, "@") ||
         checkToken(token, TT_Keyword, "class");
}
Expression readClassExpression(SourceFile file, cstring source) {
  Expression expr = Expression_create();
  expr->type = ET_Class;
  expr->clazz = (Class)Buffer_alloc(sizeof(struct s_Class));
  expr->clazz->extends = NULL;
  List_Option dec_opt = {1, (Buffer_Free)Expression_dispose};
  expr->clazz->decorators = List_create(dec_opt);
  List_Option pro_opt = {1, (Buffer_Free)ClassProperty_dispose};
  expr->clazz->properties = List_create(pro_opt);
  List_Option block_opt = {1, (Buffer_Free)Statement_dispose};
  expr->clazz->staticBlocks = List_create(block_opt);
  expr->clazz->name = NULL;
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  expr->node->position.begin = token->raw.begin;
  if (!checkToken(token, TT_Keyword, "class")) {
    for (;;) {
      selector = token->raw.end;
      Token_dispose(token);
      ExpressionContext ectx = pushExpressionContext();
      Expression decorator = readExpression(file, selector);
      popExpressionContext(ectx);
      if (!decorator) {
        goto failed;
      }
      List_insert_tail(expr->clazz->decorators, decorator);
      selector = decorator->node->position.end;
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (checkToken(token, TT_Symbol, "@")) {
        continue;
      } else {
        break;
      }
    }
  }
  if (!checkToken(token, TT_Keyword, "class")) {
    pushError("Unexcept token.missing token 'class'",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (token->type == TT_Identifier) {
    expr->clazz->name = token;
    selector = token->raw.end;
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (checkToken(token, TT_Keyword, "extends")) {
    selector = token->raw.end;
    Token_dispose(token);
    ExpressionContext ectx = pushExpressionContext();
    Expression extends = readExpression(file, selector);
    popExpressionContext(ectx);
    expr->clazz->extends = extends;
    selector = extends->node->position.end;
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (!checkToken(token, TT_Symbol, "{")) {
    pushError("Unexcept token.missing token '{'",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
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
    for (;;) {
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (isStaticBlock(file, token)) {
        Token_dispose(token);
        Statement block = readStaticBlock(file, selector);
        if (!block) {
          goto failed;
        }
        List_insert_tail(expr->clazz->staticBlocks, block);
        selector = block->node->position.end;
      } else {
        Token_dispose(token);
        ClassProperty property = readClassProperty(file, selector);
        if (!property) {
          goto failed;
        }
        List_insert_tail(expr->clazz->properties, property);
        if (property->key) {
          selector = property->key->node->position.end;
        }
        if (property->value) {
          selector = property->value->node->position.end;
        }
      }
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (checkToken(token, TT_Symbol, ";")) {
        selector = token->raw.end;
        Token_dispose(token);
        token = readTokenSkipNewline(file, selector);
        if (!token) {
          goto failed;
        }
      }
      if (checkToken(token, TT_Symbol, "}")) {
        break;
      }
      Token_dispose(token);
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
  expr->node->position.end = selector;
  return expr;
failed:
  Expression_dispose(expr);
  return NULL;
}
