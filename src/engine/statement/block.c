#include "ast.h"
BlockStatement BlockStatement_create() {
  BlockStatement blockStatement =
      (BlockStatement)Buffer_alloc(sizeof(struct s_BlockStatement));
  List_Option opt = {1, (Buffer_Free)Statement_dispose};
  blockStatement->_body = List_create(opt);
  blockStatement->_node = AstNode_create();
  blockStatement->_node->_type = NT_BlockStatement;
  return blockStatement;
}
void BlockStatement_dispose(BlockStatement blockStatement) {
  List_dispose(blockStatement->_body);
  AstNode_dispose(blockStatement->_node);
  Buffer_free(blockStatement);
}
BlockStatement readBlockStatement(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  if (!checkToken(token, TT_Symbol, "{")) {
    Token_dispose(token);
    return NULL;
  }
  Token_dispose(token);
  Context *current = pushContext();
  cstring selector = skipToken(file, source);
  if (!selector) {
    popContext(current);
    return NULL;
  }
  BlockStatement block_s = BlockStatement_create();
  Statement statement = readStatement(file, selector);
  while (statement) {
    List_insert_tail(block_s->_body, statement);
    selector = statement->_node->_position.end;
    Token token = readTokenSkipComment(file, selector);
    if (token->_type == TT_Newline || checkToken(token, TT_Symbol, ";")) {
      selector = token->_raw.end;
    }
    Token_dispose(token);
    statement = readStatement(file, selector);
  }
  if (getAstError().error) {
    BlockStatement_dispose(block_s);
    popContext(current);
    return NULL;
  }
  token = readTokenSkipNewline(file, selector);
  if (!checkToken(token, TT_Symbol, "}")) {
    Token_dispose(token);
    BlockStatement_dispose(block_s);
    Error error;
    error.error = "Unexcept token,must be '}'";
    error.location = getLocation(file, selector);
    setAstError(error);
    popContext(current);
    return NULL;
  }
  popContext(current);
  selector = token->_raw.end;
  Token_dispose(token);
  block_s->_node->_position.begin = source;
  block_s->_node->_position.end = selector;
  disableTailCheck();
  return block_s;
}