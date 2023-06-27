#include "parser/statement.h"
int isForStatement(SourceFile file, Token token) {
  return checkToken(token, TT_Keyword, "for");
}
Statement readForStatement(SourceFile file, cstring source) {
  cstring selector = source;
  Statement statement = Statement_create();
  statement->type = ST_For;
  statement->forStatement.after = NULL;
  statement->forStatement.condition = NULL;
  statement->forStatement.init = NULL;
  statement->forStatement.body = NULL;
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
    pushError("Unexcept token.missing token '('",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  token = readTokenSkipNewline(file, selector);
  if (!checkToken(token, TT_Symbol, ";")) {
    Token_dispose(token);
    ExpressionContext ectx = pushExpressionContext();
    Expression init = readExpression(file, selector);
    popExpressionContext(ectx);
    if (!init) {
      goto failed;
    }
    selector = init->node->position.end;
    if (init->type == ET_Calculate &&
        checkToken(init->binary.operator, TT_Keyword, "in")) {
      statement->type = ST_ForIn;
      statement->forIn.init = init;
    } else if (init->type == ET_Calculate &&
               checkToken(init->binary.operator, TT_Keyword, "of")) {
      statement->type = ST_ForOf;
      statement->forOf.init = init;
    } else {
      statement->forStatement.init = init;
    }
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
    if (statement->type != ST_For) {
      if (!checkToken(token, TT_Symbol, ")")) {
        pushError("Unexcept token.missing token ')'",
                  getLocation(file, token->raw.end));
        Token_dispose(token);
        goto failed;
      }
    } else {
      if (!checkToken(token, TT_Symbol, ";")) {
        pushError("Unexcept token.missing token ';'",
                  getLocation(file, token->raw.end));
        Token_dispose(token);
        goto failed;
      }
    }
  }
  selector = token->raw.end;
  Token_dispose(token);
  if (statement->type != ST_For) {
    statement->forIn.body = readStatement(file, selector);
    if (!statement->forIn.body) {
      goto failed;
    }
    selector = statement->forIn.body->node->position.end;
  } else {
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
    if (!checkToken(token, TT_Symbol, ";")) {
      Token_dispose(token);
      ExpressionContext ectx = pushExpressionContext();
      Expression condition = readExpression(file, selector);
      popExpressionContext(ectx);
      if (!condition) {
        goto failed;
      }
      statement->forStatement.condition = condition;
      selector = condition->node->position.end;
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }

      if (!checkToken(token, TT_Symbol, ";")) {
        pushError("Unexcept token.missing token ';'",
                  getLocation(file, token->raw.end));
        Token_dispose(token);
        goto failed;
      }
      selector = token->raw.end;
      Token_dispose(token);
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (!checkToken(token, TT_Symbol, ")")) {
        Token_dispose(token);
        ExpressionContext ectx = pushExpressionContext();
        Expression after = readExpression(file, selector);
        popExpressionContext(ectx);
        if (!after) {
          goto failed;
        }
        statement->forStatement.after = after;
        selector = after->node->position.end;
        token = readTokenSkipNewline(file, selector);
        if (!token) {
          goto failed;
        }
        if (!checkToken(token, TT_Symbol, ")")) {
          pushError("Unexcept token.missing token ')'",
                    getLocation(file, token->raw.end));
          Token_dispose(token);
          goto failed;
        }
        selector = token->raw.end;
        Token_dispose(token);
      } else {
        selector = token->raw.end;
        Token_dispose(token);
      }
      statement->forStatement.body = readStatement(file, selector);
      if (!statement->forStatement.body) {
        goto failed;
      }
      selector = statement->forStatement.body->node->position.begin;
    }
  }
  statement->node->position.begin = source;
  statement->node->position.end = selector;
  return statement;
failed:
  Statement_dispose(statement);
  return NULL;
}