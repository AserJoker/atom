#include "ast.h"
Statement YieldStatement_create() {
  Statement statement = Statement_create();
  statement->node->type = NT_YieldStatement;
  statement->return_.expression = NULL;
  return statement;
}
Statement readYieldStatement(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  if (!Token_check(token, TT_Keyword, "yield")) {
    Token_dispose(token);
    ErrorStack_push(
        Error_init("Unexcept token.", getLocation(file, selector), NULL));
    return NULL;
  }
  selector = token->raw.end;
  Token_dispose(token);
  Statement ret_state = YieldStatement_create();
  token = readTokenSkipComment(file, selector);
  if (!token) {
    goto failed;
  }
  if (token->type == TT_Newline || token->type == TT_MultiLineComment) {
    selector = token->raw.end;
  } else {
    Expression body = readExpression(file, selector);
    if (!body) {
      goto failed;
    }
    ret_state->yield_.expression = body;
    selector = body->node->position.end;
  }
  Token_dispose(token);
  token = readTokenSkipComment(file, selector);
  if (!token) {
    goto failed;
  }
  if (Token_check(token, TT_Symbol, ";")) {
    selector = token->raw.end;
  }
  Token_dispose(token);
  ret_state->node->position.begin = source;
  ret_state->node->position.end = selector;
  return ret_state;
failed:
  Statement_dispose(ret_state);
  return NULL;
}
void YieldStatement_dispose(Statement statement) {
  if (statement->return_.expression) {
    Expression_dispose(statement->yield_.expression);
  }
  AstNode_dispose(statement->node);
  Buffer_free(statement);
}