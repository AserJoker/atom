#include "ast.h"
Function Function_create() {
  Function func = (Function)Buffer_alloc(sizeof(struct s_Function));
  func->node = AstNode_create();
  List_Option opt = {1, (Buffer_Free)Expression_dispose};
  func->args = List_create(opt);
  func->async = 0;
  func->body = NULL;
  func->name = NULL;
  func->node->type = NT_Function;
  return func;
}
void Function_dispose(Function function) {
  List_dispose(function->args);
  if (function->name) {
    Expression_dispose(function->name);
  }
  if (function->body) {
    Statement_dispose(function->body);
  }
  AstNode_dispose(function->node);
  Buffer_free(function);
}
Function readFunctionDefinition(Function func, SourceFile file,
                                cstring source) {
  Function fn = func;
  if (!fn) {
    fn = Function_create();
  }
  cstring selector = source;
  // try read name
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (token->type == TT_Identifier) {
    fn->name = readIdentifierExpression(file, selector);
    selector = token->raw.end;
  } else if (Token_check(token, TT_Symbol, "[")) {
    selector = token->raw.end;
    Token_dispose(token);
    AstContext ctx = pushAstContext();
    Expression name = readExpression(file, selector);
    popAstContext(ctx);
    if (!name) {
      goto failed;
    }
    Expression compute = Expression_create();
    compute->compute.right = name;
    compute->node->type = NT_ComputeExpression;
    compute->node->position = name->node->position;
    fn->name = compute;
    selector = name->node->position.end;
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
    if (!Token_check(token, TT_Symbol, "]")) {
      Token_dispose(token);
      goto failed;
    }
    selector = token->raw.end;
  }
  Token_dispose(token);

  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!Token_check(token, TT_Symbol, "(")) {
    Token_dispose(token);
    ErrorStack_push(Error_init("Unexcept token. missing token '('",
                               getLocation(file, selector), NULL));
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);

  token = readTokenSkipNewline(file, selector);
  if (!Token_check(token, TT_Symbol, ")")) {
    Token_dispose(token);
    AstContext current = pushAstContext();
    enableCommaTail();
    Expression arg = readExpression(file, selector);
    popAstContext(current);
    if (!arg) {
      goto failed;
    }
    for (;;) {
      List_insert_tail(fn->args, arg);
      selector = arg->node->position.end;
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (Token_check(token, TT_Symbol, ",")) {
        selector = token->raw.end;
        Token_dispose(token);
      } else if (Token_check(token, TT_Symbol, ")")) {
        break;
      } else {
        Token_dispose(token);
        ErrorStack_push(Error_init("Unexcept token. missing token ')'",
                                   getLocation(file, selector), NULL));
        goto failed;
      }
      AstContext current = pushAstContext();
      enableCommaTail();
      arg = readExpression(file, selector);
      popAstContext(current);
      if (!arg) {
        goto failed;
      }
    }
  }
  selector = token->raw.end;
  Token_dispose(token);
  Statement body = readBlockStatement(file, selector);
  if (!body) {
    goto failed;
  }
  selector = body->node->position.end;
  fn->body = body;
  fn->node->position.begin = source;
  fn->node->position.end = selector;
  return fn;
failed:
  Function_dispose(fn);
  return NULL;
}

Function readFunction(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }

  if (!Token_check(token, TT_Keyword, "function") &&
      !Token_check(token, TT_Keyword, "async")) {
    Token_dispose(token);
    ErrorStack_push(
        Error_init("Unexcept token.", getLocation(file, source), NULL));
    return NULL;
  }
  Function func = Function_create();
  cstring selector = source;
  if (Token_check(token, TT_Keyword, "async")) {
    func->async = 1;
    selector = token->raw.end;
  }
  Token_dispose(token);

  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!Token_check(token, TT_Keyword, "function")) {
    Token_dispose(token);
    ErrorStack_push(
        Error_init("Unexcept token.", getLocation(file, selector), NULL));
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (Token_check(token, TT_Symbol, "*")) {
    selector = token->raw.end;
    func->generator = 1;
  } else {
    func->generator = 0;
  }
  Token_dispose(token);
  return readFunctionDefinition(func, file, selector);
failed:
  Function_dispose(func);
  return NULL;
}
Expression FunctionExpression_create() {
  Expression expr = Expression_create();
  expr->node->type = NT_FunctionExpression;
  expr->level = -2;
  return expr;
}
void FunctionExpression_dispose(Expression expr) {
  if (expr->function) {
    Function_dispose(expr->function);
  }
  AstNode_dispose(expr->node);
  Buffer_free(expr);
}
Expression readFunctionExpression(SourceFile file, cstring source) {
  Function function = readFunction(file, source);
  if (!function) {
    return NULL;
  }
  Expression expr = FunctionExpression_create();
  expr->function = function;
  expr->node->position = function->node->position;
  return expr;
}