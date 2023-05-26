#include "ast.h"
typedef struct s_OperatorSet {
  cstring *operators;
} s_OperatorSet;

static cstring operator_0[] = {0};
static cstring operator_1[] = {"*", "/", "%", 0};
static cstring operator_2[] = {"+", "-", 0};
static cstring operator_3[] = {"<<", ">>", 0};
static cstring operator_4[] = {">", ">=", "<", "<=", 0};
static cstring operator_5[] = {"==", "!=", 0};
static cstring operator_6[] = {"&", 0};
static cstring operator_7[] = {"^", 0};
static cstring operator_8[] = {"|", 0};
static cstring operator_9[] = {"&&", 0};
static cstring operator_10[] = {"||", 0};
static cstring operator_11[] = {
    "=", "/=", "*=", "%=", "+=", "-=", "<<=", ">>=", "&=", "^=", "|=", 0};
static cstring operator_12[] = {",", 0};
static s_OperatorSet optset0 = {.operators = &operator_0[0]};
static s_OperatorSet optset1 = {.operators = &operator_1[0]};
static s_OperatorSet optset2 = {.operators = &operator_2[0]};
static s_OperatorSet optset3 = {.operators = &operator_3[0]};
static s_OperatorSet optset4 = {.operators = &operator_4[0]};
static s_OperatorSet optset5 = {.operators = &operator_5[0]};
static s_OperatorSet optset6 = {.operators = &operator_6[0]};
static s_OperatorSet optset7 = {.operators = &operator_7[0]};
static s_OperatorSet optset8 = {.operators = &operator_8[0]};
static s_OperatorSet optset9 = {.operators = &operator_9[0]};
static s_OperatorSet optset10 = {.operators = &operator_10[0]};
static s_OperatorSet optset11 = {.operators = &operator_11[0]};
static s_OperatorSet optset12 = {.operators = &operator_12[0]};

static s_OperatorSet *opts[] = {
    &optset0, &optset1, &optset2, &optset3,  &optset4,  &optset5,  &optset6,
    &optset7, &optset8, &optset9, &optset10, &optset11, &optset12, 0};

static cstring unaryOperators[] = {"++", "--", "...", "!", "+", "-", "~", 0};

static cstring updateOperators[] = {"++", "--", 0};

Expression Expression_create() {
  Expression expression = (Expression)Buffer_alloc(sizeof(struct s_Expression));
  expression->node = AstNode_create();
  expression->binary.left = NULL;
  expression->binary.right = NULL;
  expression->binary.operator= NULL;
  expression->level = -2;
  return expression;
}
void Expression_dispose(Expression expression) {
  if (expression->node->type == NT_BinaryExpression) {
    return BinaryExpression_dispose(expression);
  }
  if (expression->node->type == NT_LiteralExpression) {
    return LiteralExpression_dispose(expression);
  }
  if (expression->node->type == NT_IdentifierExpression) {
    return IdentifierExpression_dispose(expression);
  }
  if (expression->node->type == NT_BracketExpression) {
    return BracketExpression_dispose(expression);
  }
}
void BinaryExpression_dispose(Expression expression) {
  if (expression->binary.left) {
    Expression_dispose(expression->binary.left);
  }
  if (expression->binary.right) {
    Expression_dispose(expression->binary.right);
  }
  if (expression->binary.operator) {
    Token_dispose(expression->binary.operator);
  }
  AstNode_dispose(expression->node);
  Buffer_free(expression);
}
void LiteralExpression_dispose(Expression expression) {
  Literal_dispose(expression->literal);
  AstNode_dispose(expression->node);
  Buffer_free(expression);
}
void IdentifierExpression_dispose(Expression expression) {
  Identifier_dispose(expression->identifier);
  AstNode_dispose(expression->node);
  Buffer_free(expression);
}
void Literal_dispose(Literal literal) {
  Token_dispose(literal->raw);
  AstNode_dispose(literal->node);
  Buffer_free(literal);
}
Literal Literal_create() {
  Literal literal = (Literal)Buffer_alloc(sizeof(struct s_Literal));
  literal->node = AstNode_create();
  literal->node->type = NT_Literal;
  literal->node->position.begin = 0;
  literal->node->position.end = 0;
  literal->raw = NULL;
  return literal;
}
void Identifier_dispose(Identifier identifier) {
  Token_dispose(identifier->raw);
  AstNode_dispose(identifier->node);
  Buffer_free(identifier);
}
Identifier Identifier_create() {
  Identifier identifier = (Identifier)Buffer_alloc(sizeof(struct s_Identifier));
  identifier->raw = NULL;
  identifier->node = AstNode_create();
  identifier->node->type = NT_Identifier;
  return identifier;
}

Identifier readIdentifier(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  if (token->type != TT_Identifier) {
    Token_dispose(token);
    return NULL;
  }
  Identifier identifier = Identifier_create();
  identifier->raw = token;
  identifier->node->position = token->raw;
  return identifier;
}

int isLiteralToken(Token token) {
  if (token->type == TT_String || token->type == TT_Number ||
      token->type == TT_BigInt || token->type == TT_Regex) {
    return 1;
  }
  if (Token_check(token, TT_Keyword, "true") ||
      Token_check(token, TT_Keyword, "false") ||
      Token_check(token, TT_Keyword, "null") ||
      Token_check(token, TT_Keyword, "undefined")) {
    return 1;
  }
  return 0;
}
Literal readLiteral(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  if (!isLiteralToken(token)) {
    Token_dispose(token);
    return NULL;
  }
  Literal literal = Literal_create();
  literal->raw = token;
  literal->node->type = NT_Literal;
  literal->node->position = token->raw;
  return literal;
}

Expression readIdentifierExpression(SourceFile file, cstring source) {
  Identifier identifier = readIdentifier(file, source);
  if (!identifier) {
    return NULL;
  }
  Expression expr = Expression_create();
  expr->node->type = NT_IdentifierExpression;
  expr->node->position = identifier->node->position;
  expr->identifier = identifier;
  return expr;
}
void BracketExpression_dispose(Expression expression) {
  if (expression->bracket.expression) {
    Expression_dispose(expression->bracket.expression);
  }
  AstNode_dispose(expression->node);
  Buffer_free(expression);
}

Expression readBracketExpression(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  if (!Token_check(token, TT_Symbol, "(")) {
    Token_dispose(token);
    return NULL;
  }
  selector = token->raw.end;
  Token_dispose(token);
  AstContext current = pushAstContext();
  Expression expression = readExpression(file, selector);
  popAstContext(current);
  if (!expression) {
    return NULL;
  }
  selector = expression->node->position.end;
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    Expression_dispose(expression);
    return NULL;
  }
  if (!Token_check(token, TT_Symbol, ")")) {
    Token_dispose(token);
    Expression_dispose(expression);
    ErrorStack_push(Error_init("Except ')' after expression",
                               getLocation(file, selector), NULL));
    return NULL;
  }
  selector = token->raw.end;
  Token_dispose(token);
  Expression bracket_expr = Expression_create();
  bracket_expr->level = -2;
  bracket_expr->node->type = NT_BracketExpression;
  bracket_expr->node->position.begin = source;
  bracket_expr->node->position.end = selector;
  bracket_expr->bracket.expression = expression;
  return bracket_expr;
}

void insertBinaryExpression(Expression *root, Expression current) {
  if (current->level >= (*root)->level) {
    current->binary.left = *root;
    *root = current;

  } else {
    Expression iterator = *root;
    while (iterator->binary.right->level > current->level) {
      iterator = iterator->binary.right;
      if (!iterator->binary.right) {
        break;
      }
    }
    current->binary.left = iterator->binary.right;
    iterator->binary.right = current;
  }
}

Expression readLiteralExpression(SourceFile file, cstring source) {
  Literal literal = readLiteral(file, source);
  if (!literal) {
    return NULL;
  }
  Expression literal_expr = Expression_create();
  literal_expr->level = -2;
  literal_expr->literal = literal;
  literal_expr->node->type = NT_LiteralExpression;
  literal_expr->node->position = literal->node->position;
  return literal_expr;
}

Expression readExpression(SourceFile file, cstring source) {
  Expression expr = NULL;
  Expression current = NULL;
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  for (;;) {
    if (token->type == TT_Newline || token->type == TT_MultiLineComment) {
      selector = token->raw.end;
      Token_dispose(token);
      token = readTokenSkipNewline(file, selector);
      if (expr && !current) {
        if (token->type == TT_Symbol) {
          if (strings_contains(token->raw, updateOperators)) {
            Token_dispose(token);
            break;
          }
        }
      }
    }
    if (Token_check(token, TT_Symbol, ";")) {
      Token_dispose(token);
      break;
    }
    if (current || !expr) {
      // resolve value
      if (token->type == TT_Identifier) {
        // TODO: template string tag
        Token_dispose(token);
        Expression identifier_expr = readIdentifierExpression(file, selector);
        if (!identifier_expr) {
          goto failed;
        }
        selector = identifier_expr->identifier->node->position.end;
        if (!expr) {
          expr = identifier_expr;
        } else {
          current->binary.right = identifier_expr;
          current = NULL;
        }
      } else if (isLiteralToken(token)) {
        Token_dispose(token);
        Expression literal_expr = readLiteralExpression(file, selector);
        if (!literal_expr) {
          goto failed;
        }
        selector = literal_expr->literal->node->position.end;
        if (!expr) {
          expr = literal_expr;
        } else {
          current->binary.right = literal_expr;
          current = NULL;
        }
      } else if (token->type == TT_Symbol) {
        if (Token_check(token, TT_Symbol, "(")) {
          Token_dispose(token);
          Expression bracket_expr = readBracketExpression(file, selector);
          if (!bracket_expr) {
            return NULL;
          }
          selector = bracket_expr->node->position.end;
          if (!expr) {
            expr = bracket_expr;
          } else {
            current->binary.right = bracket_expr;
            current = NULL;
          }
        } else {
          Token_dispose(token);
          ErrorStack_push(
              Error_init("Unexcept token.", getLocation(file, selector), NULL));
          goto failed;
        }
      } else {
        Token_dispose(token);
        goto failed;
      }
    } else {
      // a+b 表达式已经完整
      // resolve operator
      if (token->type == TT_Symbol) {
        if (strings_contains(token->raw, updateOperators)) {
          // a+b++
          Expression binary_expr = Expression_create();
          binary_expr->node->type = NT_BinaryExpression;
          binary_expr->level = -1;
          binary_expr->binary.operator= token;
          insertBinaryExpression(&expr, binary_expr);
          selector = token->raw.end;
        } else if (strings_is(token->raw, ".")) {
          Expression binary_expr = Expression_create();
          binary_expr->node->type = NT_BinaryExpression;
          binary_expr->level = -1;
          binary_expr->binary.operator= token;
          insertBinaryExpression(&expr, binary_expr);
          current = binary_expr;
          selector = token->raw.end;
        } else if (strings_is(token->raw, "(")) {
          // TODO: function call
        } else if (strings_is(token->raw, "[")) {
          // TODO: compute member
        } else {
          int level = 0;
          for (; opts[level] != 0; level++) {
            if (strings_contains(token->raw, opts[level]->operators)) {
              break;
            }
          }
          if (opts[level]) {
            Expression binary_expr = Expression_create();
            binary_expr->node->type = NT_BinaryExpression;
            binary_expr->level = level;
            binary_expr->binary.operator= token;
            insertBinaryExpression(&expr, binary_expr);
            current = binary_expr;
            selector = token->raw.end;
          } else {
            Token_dispose(token);
            break;
          }
        }
      } else {
        Token_dispose(token);
        break;
      }
    }
    if (expr && !current) {
      disableReadRegex();
    } else {
      enableReadRegex();
    }
    token = readTokenSkipComment(file, selector);
    if (!token) {
      goto failed;
    }
  }
  expr->node->position.begin = source;
  expr->node->position.end = selector;
  return expr;
failed:
  if (expr) {
    Expression_dispose(expr);
  }
  return NULL;
}
