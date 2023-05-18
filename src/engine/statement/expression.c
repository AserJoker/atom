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

static cstring unaryOperators[] = {"...", "!", "+", "-", "~", 0};

static cstring updateOperators[] = {"++", "--", 0};
void Expression_dispose(Expression expression) {
  if (expression->_left) {
    Expression_dispose(expression->_left);
  }
  if (expression->_right) {
    Expression_dispose(expression->_right);
  }
  if (expression->_identifier) {
    Identifier_dispose(expression->_identifier);
  }
  if (expression->_literal) {
    Literal_dispose(expression->_literal);
  }
  if (expression->_operator) {
    Token_dispose(expression->_operator);
  }
  AstNode_dispose(expression->_node);
  Buffer_free(expression);
}

Expression Expression_create() {
  Expression expr = (Expression)Buffer_alloc(sizeof(struct s_Expression));
  expr->_identifier = NULL;
  expr->_left = NULL;
  expr->_literal = NULL;
  expr->_right = NULL;
  expr->_operator = NULL;
  expr->_level = -2;
  expr->_node = AstNode_create();
  return expr;
}

void bindLeft(Expression *root, Expression target) {
  if ((*root)->_level <= target->_level) {
    target->_left = *root;
    *root = target;
  } else {
    Expression iter = *root;
    while (target->_level < iter->_right->_level) {
      iter = iter->_right;
    }
    target->_left = iter->_right;
    iter->_right = target;
  }
}

Expression readExpression(SourceFile file, cstring source) {
  Expression expr = NULL;
  cstring selector = source;
  Token token = readTokenSkipComment(file, selector);
  if (!token) {
    goto failed;
  }
  Expression current = NULL;
  for (;;) {
    if (token->_type == TT_Newline) {
      Token next = readTokenSkipNewline(file, selector);
      if (!next) {
        Token_dispose(token);
        goto failed;
      }
      if (next->_type == TT_Identifier && !current && expr) {
        Token_dispose(token);
        Token_dispose(next);
        break;
      }
      Token_dispose(token);
      token = next;
      selector = token->_raw.begin;
      // TODO: ASI 插入分号校验
    }
    if (token->_type == TT_Identifier) {
      Token_dispose(token);
      if (!current && expr) {
        break;
      }
      // TODO: template string expression
      Expression identifier_expr = readIdentifierExpression(file, selector);
      if (!identifier_expr) {
        goto failed;
      }
      selector = identifier_expr->_node->_position.end;
      if (!identifier_expr) {
        goto failed;
      }
      if (!expr) {
        expr = identifier_expr;
      } else {
        current->_right = identifier_expr;
        current = NULL;
      }
      token = readTokenSkipComment(file, selector);
    } else if (token->_type == TT_Symbol) {
      if (strings_is(token->_raw, ";")) {
        Token_dispose(token);
        break;
      }
      if (!current && expr) {
        // 表达式已完整 例如a + b 可作为一个原子嵌套到更复杂的表达式中
        // TODO: update expression （左结合表达式）
        if (strings_is(token->_raw, "(")) {
          int index = -2;
          Token_dispose(token);
          Expression call_expr = readBracketExpression(file, selector);
          if (!call_expr) {
            goto failed;
          }
          selector = call_expr->_node->_position.end;
          token = readTokenSkipNewline(file, selector);
          if (checkToken(token, TT_Symbol, "=>")) {
            Token_dispose(token);
            // TODO: lambda pattern
          }
          call_expr->_right = call_expr->_left;
          call_expr->_left = NULL;
          call_expr->_node->_type = NT_CallExpression;
          call_expr->_level = index;
          bindLeft(&expr, call_expr);
        } else if (strings_is(token->_raw, ".") ||
                   strings_is(token->_raw, "?.")) {
          if (strings_is(token->_raw, "?.")) {
            Token next = readTokenSkipNewline(file, token->_raw.end);
            if (!next) {
              Token_dispose(token);
              goto failed;
            }
            if (checkToken(next, TT_Symbol, "(")) {
              int index = -2;
              selector = next->_raw.begin;
              Token_dispose(token);
              Token_dispose(next);
              Expression call_expr = readBracketExpression(file, selector);
              if (!call_expr) {
                goto failed;
              }
              call_expr->_right = call_expr->_left;
              call_expr->_left = NULL;
              call_expr->_node->_type = NT_OptionalCallExpression;
              call_expr->_level = index;
              selector = call_expr->_node->_position.end;
              bindLeft(&expr, call_expr);
              goto end;
            } else if (checkToken(next, TT_Symbol, "[")) {
              selector = next->_raw.begin;
              Token_dispose(token);
              Token_dispose(next);
              Expression compute_expr = readComputeExpression(file, selector);
              if (!compute_expr) {
                goto failed;
              }
              selector = compute_expr->_node->_position.end;
              compute_expr->_node->_type = NT_OptionalComputeExpression;
              bindLeft(&expr, compute_expr);
              goto end;
            } else {
              Token_dispose(next);
            }
          }
          int index = -2;
          Expression bin_expr = Expression_create();
          bin_expr->_operator = token;
          bin_expr->_level = index;
          bin_expr->_node->_type = NT_BinaryExpression;
          current = bin_expr;
          bindLeft(&expr, bin_expr);
          selector = bin_expr->_operator->_raw.end;
        } else if (strings_is(token->_raw, "[")) {
          Token_dispose(token);
          Expression compute_expr = readComputeExpression(file, selector);
          if (!compute_expr) {
            goto failed;
          }
          selector = compute_expr->_node->_position.end;
          compute_expr->_node->_type = NT_ComputeExpression;
          bindLeft(&expr, compute_expr);
        } else {
          int index = 0;
          while (opts[index]) {
            if (strings_contains(token->_raw, opts[index]->operators)) {
              break;
            }
            index++;
          }
          if (!opts[index]) {
            Token_dispose(token);
            break;
          }
          Expression bin_expr = Expression_create();
          bin_expr->_operator = token;
          bin_expr->_level = index;
          bin_expr->_node->_type = NT_BinaryExpression;
          current = bin_expr;
          bindLeft(&expr, bin_expr);
          selector = bin_expr->_operator->_raw.end;
        }
      } else {
        // 不完整表达式 例如 a+ 缺少右值表达式
        // TODO: update expression (右结合)
        // TODO: 右结合表达式
        if (strings_is(token->_raw, "(")) {
          Token_dispose(token);
          Expression brackets = readBracketExpression(file, selector);
          if (!brackets) {
            goto failed;
          }
          if (!expr) {
            expr = brackets;
          } else {
            current->_right = brackets;
            current = NULL;
          }
          selector = brackets->_node->_position.end;
        } else if (strings_contains(token->_raw, unaryOperators)) {
          Expression unary_expr = Expression_create();
          unary_expr->_operator = token;
          unary_expr->_node->_type = NT_BinaryExpression;
          unary_expr->_level = -1;
          selector = token->_raw.end;
          if (!expr) {
            expr = unary_expr;
          } else {
            current->_right = unary_expr;
          }
          current = unary_expr;
          selector = token->_raw.end;
        } else {
          Token_dispose(token);
          break;
        }
      }
    end:
      token = readTokenSkipComment(file, selector);
    } else {
      Token_dispose(token);
      break;
    }
  }
  if (!expr) {
    return NULL;
  }
  expr->_node->_position.begin = source;
  expr->_node->_position.end = selector;
  return expr;
failed:
  if (expr) {
    Expression_dispose(expr);
  }
  return NULL;
}

ExpressionStatement ExpressionStatement_create() {
  ExpressionStatement expression_s =
      (ExpressionStatement)Buffer_alloc(sizeof(struct s_ExpressionStatement));
  expression_s->_expression = NULL;
  expression_s->_node = AstNode_create();
  expression_s->_node->_type = NT_ExpressionStatement;
  return expression_s;
}

void ExpressionStatement_dispose(ExpressionStatement expression_s) {
  if (expression_s->_expression) {
    Expression_dispose(expression_s->_expression);
  }
  AstNode_dispose(expression_s->_node);
  Buffer_free(expression_s);
}
ExpressionStatement readExpressionStatement(SourceFile file, cstring source) {
  cstring selector = source;
  Expression expression = readExpression(file, selector);
  if (!expression) {
    return NULL;
  }
  selector = expression->_node->_position.end;
  ExpressionStatement expr = ExpressionStatement_create();
  expr->_expression = expression;
  expr->_node->_position.begin = source;
  expr->_node->_position.end = selector;
  return expr;
}