#include "ast.h"
Statement Statement_create() {
  Statement statement = (Statement)Buffer_alloc(sizeof(struct s_Statement));
  statement->node = AstNode_create();
  statement->node->position.begin = 0;
  statement->node->position.end = 0;

  statement->block.body = NULL;

  return statement;
}

void Statement_dispose(Statement statement) {
  if (statement->node->type == NT_BlockStatement) {
    return BlockStatement_dispose(statement);
  }
  if (statement->node->type == NT_EmptyStatement) {
    return EmptyStatement_dispose(statement);
  }
  if (statement->node->type == NT_ExpressionStatement) {
    return ExpressionStatement_dispose(statement);
  }
  if (statement->node->type == NT_ReturnStatement) {
    return ReturnStatement_dispose(statement);
  }
  if (statement->node->type == NT_DefinitionSatement) {
    return DefinitionStatement_dispose(statement);
  }
  if (statement->node->type == NT_ExportStatement) {
    return ExportStatement_dispose(statement);
  }
  if (statement->node->type == NT_YieldStatement) {
    return YieldStatement_dispose(statement);
  }
}

Statement readStatement(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  if (token->type == TT_Eof) {
    Token_dispose(token);
    return NULL;
  }
  if (Token_check(token, TT_Symbol, ";")) {
    Token_dispose(token);
    return readEmptyStatement(file, source);
  } else if (Token_check(token, TT_Symbol, "{")) {
    Token_dispose(token);
    return readBlockStatement(file, source);
  } else if (Token_check(token, TT_Keyword, "return")) {
    Token_dispose(token);
    return readReturnStatement(file, source);
  } else if (Token_check(token, TT_Keyword, "const") ||
             Token_check(token, TT_Keyword, "var") ||
             Token_check(token, TT_Keyword, "let")) {
    Token_dispose(token);
    return readDefinitionStatement(file, source);
  } else if (Token_check(token, TT_Keyword, "export")) {
    Token_dispose(token);
    return readExportStatement(file, source);
  } else if (Token_check(token, TT_Keyword, "yield")) {
    Token_dispose(token);
    return readYieldStatement(file, source);
  } else {
    Token_dispose(token);
    return readExpressionStatement(file, source);
  }
}