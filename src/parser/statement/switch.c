#include "parser/statement.h"

static void SwitchPattern_dispose(SwitchPattern pattern) {
  if (pattern->condition) {
    Expression_dispose(pattern->condition);
  }
  List_dispose(pattern->body);
  Buffer_free(pattern);
}
static SwitchPattern readSwitchPattern(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  SwitchPattern pattern =
      (SwitchPattern)Buffer_alloc(sizeof(struct s_SwitchPattern));
  pattern->condition = NULL;
  List_Option opt = {1, (Buffer_Free)Statement_dispose};
  pattern->body = List_create(opt);
  if (checkToken(token, TT_Keyword, "case")) {
    selector = token->raw.end;
    Token_dispose(token);
    ExpressionContext ectx = pushExpressionContext();
    pattern->condition = readExpression(file, selector);
    popExpressionContext(ectx);
    if (!pattern->condition) {
      goto failed;
    }
    selector = pattern->condition->node->position.end;
  } else if (checkToken(token, TT_Keyword, "default")) {
    selector = token->raw.end;
    Token_dispose(token);
  } else {
    pushError("Unexcept token.", getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }

  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!checkToken(token, TT_Symbol, ":")) {
    pushError("Unexcept token.missing token ':'",
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
  if (!checkToken(token, TT_Keyword, "case") &&
      !checkToken(token, TT_Keyword, "default")) {
    Token_dispose(token);
    Statement statement = readStatement(file, selector);
    if (!statement) {
      goto failed;
    }
    for (;;) {
      List_insert_tail(pattern->body, statement);
      selector = statement->node->position.end;
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (checkToken(token, TT_Keyword, "case") ||
          checkToken(token, TT_Keyword, "default") ||
          checkToken(token, TT_Symbol, "}")) {
        Token_dispose(token);
        break;
      } else {
        Token_dispose(token);
      }
      statement = readStatement(file, selector);
      if (!statement) {
        goto failed;
      }
    }
  } else {
    Token_dispose(token);
  }
  pattern->position.begin = source;
  pattern->position.end = selector;
  return pattern;
failed:
  SwitchPattern_dispose(pattern);
  return NULL;
}

int isSwitchStatement(SourceFile file, Token token) {
  return checkToken(token, TT_Keyword, "switch");
}
Statement readSwitchStatement(SourceFile file, cstring source) {
  Statement statement = Statement_create();
  statement->type = ST_Switch;
  statement->switchStatement.condition = NULL;
  List_Option opt = {1, (Buffer_Free)SwitchPattern_dispose};
  statement->switchStatement.patterns = List_create(opt);
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
  if (!checkToken(token, TT_Symbol, "(")) {
    pushError("Unexcept token. missing token '('",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  ExpressionContext ectx = pushExpressionContext();
  statement->switchStatement.condition = readExpression(file, selector);
  popExpressionContext(ectx);
  if (!statement->switchStatement.condition) {
    goto failed;
  }
  selector = statement->switchStatement.condition->node->position.end;
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!checkToken(token, TT_Symbol, ")")) {
    pushError("Unexcept token. missing token ')'",
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
  if (!checkToken(token, TT_Symbol, "{")) {
    pushError("Unexcept token. missing token '{'",
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
    SwitchPattern pattern = readSwitchPattern(file, selector);
    if (!pattern) {
      goto failed;
    }
    for (;;) {
      List_insert_tail(statement->switchStatement.patterns, pattern);
      selector = pattern->position.end;
      token = readTokenSkipNewline(file, selector);
      if (!checkToken(token, TT_Keyword, "case") &&
          !checkToken(token, TT_Keyword, "default")) {
        break;
      }
      Token_dispose(token);
      pattern = readSwitchPattern(file, selector);
      if (!pattern) {
        goto failed;
      }
    }
  }

  if (!checkToken(token, TT_Symbol, "}")) {
    pushError("Unexcept token. missing token '}'",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  statement->node->position.begin = source;
  statement->node->position.end = selector;
  return statement;
failed:
  Statement_dispose(statement);
  return NULL;
}