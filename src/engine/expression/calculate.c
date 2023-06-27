#include "expression.h"
#include "source.h"
#include "strings.h"
#include "tokenizer.h"
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

int isCalculateOperator(SourceFile file, Token token) {
  if (checkToken(token, TT_Keyword, "instanceof") ||
      checkToken(token, TT_Keyword, "in") ||
      checkToken(token, TT_Keyword, "of")) {
    return 1;
  }
  if (token->type != TT_Symbol) {
    return 0;
  }
  int level = 0;
  for (;;) {
    if (getMaxOperatorLevel() != -1 && level >= getMaxOperatorLevel()) {
      return 0;
    }
    s_OperatorSet *set = opts[level];
    if (!set) {
      return 0;
    }
    if (strings_contains(token->raw, set->operators)) {
      return 1;
    }
    level++;
  }
}
int getCalculateLevel(Token token) {
  if (token->type != TT_Symbol) {
    return -1;
  }
  int level = 0;
  for (;;) {
    s_OperatorSet *set = opts[level];
    if (!set) {
      return -1;
    }
    if (strings_contains(token->raw, set->operators)) {
      return level;
    }
    level++;
  }
}

int isUnaryOperator(SourceFile file, Token token) {
  if (token->type != TT_Symbol) {
    return 0;
  }
  return strings_contains(token->raw, unaryOperators);
}
Expression readUnaryExpression(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  Expression expr = Expression_create();
  expr->binary.operator= token;
  expr->bind = BT_Right;
  expr->type = ET_Calculate;
  expr->level = -1;
  expr->node->position = token->raw;
  return expr;
}

int isUpdateOperator(SourceFile file, Token token) {
  if (token->type != TT_Symbol) {
    return 0;
  }
  return strings_contains(token->raw, updateOperators);
}
Expression readCalculateExpression(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  Expression expr = Expression_create();
  expr->binary.operator= token;
  if (checkToken(token, TT_Keyword, "instanceof") ||
      checkToken(token, TT_Keyword, "in") ||
      checkToken(token, TT_Keyword, "of")) {
    expr->level = 4;
  } else {
    expr->level = getCalculateLevel(token);
  }
  expr->bind = BT_Both;
  expr->type = ET_Calculate;
  expr->node->position = token->raw;
  return expr;
}
Expression readUpdateExpression(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  Expression expr = Expression_create();
  expr->binary.operator= token;
  expr->bind = BT_Left;
  expr->type = ET_Calculate;
  expr->level = -1;
  expr->node->position = token->raw;
  return expr;
}