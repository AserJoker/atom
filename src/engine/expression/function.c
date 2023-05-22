#include "ast.h"
FunctionPattern FunctionPattern_create() {
  FunctionPattern pattern =
      (FunctionPattern)Buffer_alloc(sizeof(struct s_FunctionPattern));
  List_Option opt = {1, (Buffer_Free)Expression_dispose};
  pattern->_args = List_create(opt);
  pattern->_async = 0;
  pattern->_body = NULL;
  pattern->_name = NULL;
  return pattern;
}
void FunctionPattern_dispose(FunctionPattern pattern) {
  if (pattern->_name) {
    Expression_dispose(pattern->_name);
  }
  if (pattern->_args) {
    List_dispose(pattern->_args);
  }
  if (pattern->_body) {
    BlockStatement_dispose(pattern->_body);
  }
  Buffer_free(pattern);
}
Expression readFunctionPattern(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  if (!checkToken(token, TT_Keyword, "function")) {
    Token_dispose(token);
    return NULL;
  }
  cstring selector = token->_raw.end;
  Token_dispose(token);
  FunctionPattern pattern = FunctionPattern_create();
  token = readTokenSkipNewline(file, selector);
  if (token->_type == TT_Identifier) {
    Token_dispose(token);
    Identifier name = readIdentifier(file, selector);
    if (!name) {
      Token_dispose(token);
      FunctionPattern_dispose(pattern);
      Error error = {"Unexcept function name", getLocation(file, selector)};
      setAstError(error);
      return NULL;
    }
    selector = name->_node->_position.end;
    Expression name_expr = Expression_create();
    name_expr->_node->_position = name->_node->_position;
    name_expr->_node->_type = NT_IdentifierExpression;
    name_expr->_identifier = name;
    pattern->_name = name_expr;
    selector = name->_node->_position.end;
  } else if (checkToken(token, TT_Symbol, "[")) {
    selector = token->_raw.end;
    Token_dispose(token);
    Expression name = readExpression(file, source);
    if (!name) {
      Token_dispose(token);
      FunctionPattern_dispose(pattern);
      Error error = {"Unexcept function name", getLocation(file, selector)};
      setAstError(error);
      return NULL;
    }
    pattern->_name = name;
    selector = name->_node->_position.end;
  } else {
    Token_dispose(token);
  }
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    FunctionPattern_dispose(pattern);
    return NULL;
  }
  if (!checkToken(token, TT_Symbol, "(")) {
    Token_dispose(token);
    FunctionPattern_dispose(pattern);
    Error error = {"Unexcept token: missing token '('",
                   getLocation(file, selector)};
    setAstError(error);
    return NULL;
  }
  selector = token->_raw.end;
  Token_dispose(token);
  Context *current = pushContext();
  enableCommaTail();
  Expression arg = readExpression(file, selector);
  while (arg) {
    List_insert_tail(pattern->_args, arg);
    selector = arg->_node->_position.end;
    Token token = readTokenSkipComment(file, selector);
    if (!token) {
      FunctionPattern_dispose(pattern);
      popContext(current);
      return NULL;
    }
    if (!checkToken(token, TT_Symbol, ",")) {
      Token_dispose(token);
      break;
    }
    selector = token->_raw.end;
    Token_dispose(token);
    arg = readExpression(file, selector);
  }
  disableCommaTail();
  popContext(current);
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    FunctionPattern_dispose(pattern);
    return NULL;
  }
  if (!checkToken(token, TT_Symbol, ")")) {
    Token_dispose(token);
    FunctionPattern_dispose(pattern);
    Error error = {"Unexcept token: missing token ')'",
                   getLocation(file, selector)};
    setAstError(error);
    return NULL;
  }
  selector = token->_raw.end;
  Token_dispose(token);
  BlockStatement body = readBlockStatement(file, selector);
  if (!body) {
    FunctionPattern_dispose(pattern);
    Error error = {"Unexcept token", getLocation(file, selector)};
    setAstError(error);
    return NULL;
  }
  selector = body->_node->_position.end;
  pattern->_body = body;
  Expression func_expr = Expression_create();
  func_expr->_node->_type = NT_FunctionPattern;
  func_expr->_node->_position.begin = source;
  func_expr->_node->_position.end = selector;
  func_expr->_pattern._function = pattern;
  func_expr->_level = -2;
  return func_expr;
}