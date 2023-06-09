#include "ast.h"
Statement DefinitionStatement_create() {
  Statement definition = Statement_create();
  definition->node->type = NT_DefinitionSatement;
  definition->definition.type = DT_VAR;
  definition->definition.expression = NULL;
  return definition;
}
void DefinitionStatement_dispose(Statement statement) {
  if (statement->definition.expression) {
    Expression_dispose(statement->definition.expression);
  }
  AstNode_dispose(statement->node);
  Buffer_free(statement);
}
Statement readDefinitionStatement(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  Statement definition = DefinitionStatement_create();
  if (Token_check(token, TT_Keyword, "const")) {
    definition->definition.type = DT_CONST;
    selector = token->raw.end;
    Token_dispose(token);
  } else if (Token_check(token, TT_Keyword, "let")) {
    definition->definition.type = DT_LET;
    selector = token->raw.end;
    Token_dispose(token);
  } else if (Token_check(token, TT_Keyword, "var")) {
    definition->definition.type = DT_VAR;
    selector = token->raw.end;
    Token_dispose(token);
  } else {
    ErrorStack_push(
        Error_init("Unexcept token.", getLocation(file, selector), NULL));
    Token_dispose(token);
    goto failed;
  }
  Expression expression = readExpression(file, selector);
  if (!expression) {
    goto failed;
  }
  selector = expression->node->position.end;
  token = readTokenSkipComment(file, selector);
  if (!token) {
    goto failed;
  }
  if (Token_check(token, TT_Symbol, ";")) {
    selector = token->raw.end;
  }
  Token_dispose(token);
  definition->definition.expression = expression;
  definition->node->position.begin = source;
  definition->node->position.end = selector;
  return definition;
failed:
  DefinitionStatement_dispose(definition);
  return NULL;
}