#include "ast.h"
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
  if (expression->node->type == NT_ComputeExpression ||
      expression->node->type == NT_OptionalComputeExpression) {
    return ComputeExpression_dispose(expression);
  }
  if (expression->node->type == NT_LambdaExpression) {
    return LambdaExpression_dispose(expression);
  }
  if (expression->node->type == NT_FunctionExpression) {
    return FunctionExpression_dispose(expression);
  }
  if (expression->node->type == NT_CallExpression ||
      expression->node->type == NT_OptionalCallExpression) {
    return CallExpression_dispose(expression);
  }
  if (expression->node->type == NT_TemplateExpression) {
    return TemplateExpression_dispose(expression);
  }
  if (expression->node->type == NT_AwaitExpression) {
    return AwaitExpression_dispose(expression);
  }
  if (expression->node->type == NT_ArrayPattern) {
    return ArrayPattern_dispose(expression);
  }
  if (expression->node->type == NT_ObjectPattern) {
    return ObjectPattern_dispose(expression);
  }
  if (expression->node->type == NT_NewExpression) {
    return NewExpression_dispose(expression);
  }
  if (expression->node->type == NT_DeleteExpression) {
    return DeleteExpression_dispose(expression);
  }
  if (expression->node->type == NT_ClassPattern) {
    return ClassPattern_dispose(expression);
  }
  if (expression->node->type == NT_ThisExpression ||
      expression->node->type == NT_SuperExpression) {
    AstNode_dispose(expression->node);
    Buffer_free(expression);
  }
  if (expression->node->type == NT_Private) {
    BinaryExpression_dispose(expression);
  }
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

Token findPair(SourceFile file, cstring start, cstring end, cstring source) {
  int index = 1;
  cstring selector = source;
  while (*selector) {
    Token token = readTokenSkipNewline(file, selector);
    if (!token) {
      return NULL;
    }
    if (strings_is(token->raw, end)) {
      index--;
      if (!index) {
        return token;
      }
    }
    if (strings_is(token->raw, start)) {
      index++;
    }
    selector = token->raw.end;
    Token_dispose(token);
  }
  if (!*selector) {
    ErrorStack_push(Error_init("Except ')' after expression",
                               getLocation(file, selector), NULL));
  }
  return NULL;
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
            Token_dispose(token); // a+b => a+b;
            break;                // ++a => a++;
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
      if (Token_check(token, TT_Symbol, "#")) {
        Expression expression = Expression_create();
        expression->level = -2;
        expression->node->type = NT_Private;
        if (!expr) {
          expr = expression;
        } else {
          current->binary.right = expression;
        }
        current = expression;
        selector = token->raw.end;
        Token_dispose(token);
      } else if (token->type == TT_Identifier) {
        Token next = readTokenSkipNewline(file, token->raw.end);
        if (!next) {
          Token_dispose(token);
          goto failed;
        }
        Token_dispose(token);
        Expression value_expr = NULL;
        if (Token_check(next, TT_Symbol, "=>")) {
          Token_dispose(next);
          value_expr = readLambdaExpression(file, selector);
          if (!value_expr) {
            goto failed;
          }
        } else {
          Token_dispose(next);
          value_expr = readIdentifierExpression(file, selector);
          if (!value_expr) {
            goto failed;
          }
        }
        selector = value_expr->identifier->node->position.end;
        if (!expr) {
          expr = value_expr;
        } else {
          current->binary.right = value_expr;
          current = NULL;
        }
      } else if (Token_check(token, TT_Keyword, "this")) {
        selector = token->raw.end;
        Token_dispose(token);
        Expression this_expr = Expression_create();
        this_expr->node->type = NT_ThisExpression;
        this_expr->node->position.begin = source;
        this_expr->node->position.end = selector;
        if (!expr) {
          expr = this_expr;
        } else {
          current->binary.right = this_expr;
          current = NULL;
        }
      } else if (Token_check(token, TT_Keyword, "super")) {
        selector = token->raw.end;
        Token_dispose(token);
        Expression super_expr = Expression_create();
        super_expr->node->type = NT_SuperExpression;
        super_expr->node->position.begin = source;
        super_expr->node->position.end = selector;
        if (!expr) {
          expr = super_expr;
        } else {
          current->binary.right = super_expr;
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
          Token pair = findPair(file, "(", ")", token->raw.end);
          Token_dispose(token);
          if (!pair) {
            goto failed;
          }
          cstring lambda_selector = pair->raw.end;
          Token_dispose(pair);
          token = readTokenSkipNewline(file, lambda_selector);
          if (!token) {
            goto failed;
          }
          if (Token_check(token, TT_Symbol, "=>")) {
            Token_dispose(token);
            Expression lambda_expr = readLambdaExpression(file, selector);
            if (!lambda_expr) {
              goto failed;
            }
            selector = lambda_expr->node->position.end;
            if (!expr) {
              expr = lambda_expr;
            } else {
              current->binary.right = lambda_expr;
              current = NULL;
            }
          } else {
            Token_dispose(token);
            Expression bracket_expr = readBracketExpression(file, selector);
            if (!bracket_expr) {
              goto failed;
            }
            selector = bracket_expr->node->position.end;
            if (!expr) {
              expr = bracket_expr;
            } else {
              current->binary.right = bracket_expr;
              current = NULL;
            }
          }
        } else if (Token_check(token, TT_Symbol, "[")) {
          Token_dispose(token);
          Expression array_expr = readArrayPattern(file, selector);
          if (!array_expr) {
            goto failed;
          }
          selector = array_expr->node->position.end;
          if (!expr) {
            expr = array_expr;
          } else {
            current->binary.right = array_expr;
            current = NULL;
          }
        } else if (Token_check(token, TT_Symbol, "{")) {
          Token_dispose(token);
          Expression object_expr = readObjectPattern(file, selector);
          if (!object_expr) {
            goto failed;
          }
          selector = object_expr->node->position.end;
          if (!expr) {
            expr = object_expr;
          } else {
            current->binary.right = object_expr;
            current = NULL;
          }
        } else if (Token_check(token, TT_Symbol, "@")) {
          Token_dispose(token);
          Expression class_expr = readClassPattern(file, selector);
          if (!class_expr) {
            goto failed;
          }
          selector = class_expr->node->position.end;
          if (!expr) {
            expr = class_expr;
          } else {
            current->binary.right = class_expr;
            current = NULL;
          }
        } else if (strings_contains(token->raw, unaryOperators)) {
          Expression unary_expr = Expression_create();
          unary_expr->node->type = NT_BinaryExpression;
          unary_expr->level = -1;
          unary_expr->binary.operator= token;
          selector = token->raw.end;
          if (!expr) {
            expr = unary_expr;
          } else {
            current->binary.right = unary_expr;
          }
          current = unary_expr;
        } else if (token->type == TT_Template ||
                   token->type == TT_TemplateStart) {
          Token_dispose(token);
          Expression template_expr = readTemplateExpression(file, selector);
          if (!template_expr) {
            goto failed;
          }
          if (!expr) {
            expr = template_expr;
          } else {
            current->binary.right = template_expr;
          }
          current = NULL;
        } else {
          Token_dispose(token);
          ErrorStack_push(
              Error_init("Unexcept token.", getLocation(file, selector), NULL));
          goto failed;
        }
      } else if (token->type == TT_Keyword) {
        if (Token_check(token, TT_Keyword, "function")) {
          Token_dispose(token);
          Expression function_expr = readFunctionExpression(file, selector);
          if (!function_expr) {
            goto failed;
          }
          selector = function_expr->node->position.end;
          if (!expr) {
            expr = function_expr;
          } else {
            current->binary.right = function_expr;
            current = NULL;
          }
        } else if (Token_check(token, TT_Keyword, "async")) {
          Expression current_expr = readAsyncExpression(file, selector);
          if (!current_expr) {
            goto failed;
          }
          selector = current_expr->node->position.end;
          if (!expr) {
            expr = current_expr;
          } else {
            current->binary.right = current_expr;
            current = NULL;
          }
        } else if (Token_check(token, TT_Keyword, "await")) {
          Expression current_expr = readAwaitExpression(file, selector);
          if (!current_expr) {
            goto failed;
          }
          selector = token->raw.end;
          Token_dispose(token);
          if (!expr) {
            expr = current_expr;
          } else {
            current->binary.right = current_expr;
          }
          current = current_expr;
        } else if (Token_check(token, TT_Keyword, "new")) {
          Expression current_expr = readNewExpression(file, selector);
          if (!current_expr) {
            goto failed;
          }
          selector = token->raw.end;
          Token_dispose(token);
          if (!expr) {
            expr = current_expr;
          } else {
            current->binary.right = current_expr;
          }
          current = current_expr;
        } else if (Token_check(token, TT_Keyword, "delete")) {
          Expression current_expr = readDeleteExpression(file, selector);
          if (!current_expr) {
            goto failed;
          }
          selector = token->raw.end;
          Token_dispose(token);
          if (!expr) {
            expr = current_expr;
          } else {
            current->binary.right = current_expr;
          }
          current = current_expr;
        } else if (Token_check(token, TT_Keyword, "class")) {
          Token_dispose(token);
          Expression class_expr = readClassPattern(file, selector);
          if (!class_expr) {
            goto failed;
          }
          selector = class_expr->node->position.end;
          if (!expr) {
            expr = class_expr;
          } else {
            current->binary.right = class_expr;
            current = NULL;
          }
        } else {
          Token_dispose(token);
          ErrorStack_push(
              Error_init("Unexcept token.", getLocation(file, selector), NULL));
          goto failed;
        }
      } else if (token->type == TT_Template ||
                 token->type == TT_TemplateStart) {
        Token_dispose(token);
        Expression template_expr = readTemplateExpression(file, selector);
        if (!template_expr) {
          goto failed;
        }
        selector = template_expr->node->position.end;
        if (!expr) {
          expr = template_expr;
        } else {
          current->binary.right = template_expr;
          current = NULL;
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
          current = NULL;
          selector = token->raw.end;
        } else if (strings_is(token->raw, ".") ||
                   strings_is(token->raw, "?.")) {
          Token next = readTokenSkipNewline(file, token->raw.end);
          if (!next) {
            Token_dispose(token);
            goto failed;
          }
          if (strings_is(token->raw, "?.")) {
            if (Token_check(next, TT_Symbol, "(")) {
              selector = token->raw.end;
              Token_dispose(token);
              Token_dispose(next);
              Expression call_expr = readCallExpression(file, selector);
              if (!call_expr) {
                goto failed;
              }
              call_expr->node->type = NT_OptionalCallExpression;
              insertBinaryExpression(&expr, call_expr);
              current = NULL;
              selector = call_expr->node->position.end;
            } else if (Token_check(next, TT_Symbol, "[")) {
              selector = token->raw.end;
              Token_dispose(token);
              Token_dispose(next);
              Expression compute_expr = readComputeExpression(file, selector);
              if (!compute_expr) {
                goto failed;
              }
              compute_expr->node->type = NT_OptionalComputeExpression;
              insertBinaryExpression(&expr, compute_expr);
              current = NULL;
              selector = compute_expr->node->position.end;
            }
          } else {
            Token_dispose(next);
            Expression binary_expr = Expression_create();
            binary_expr->node->type = NT_BinaryExpression;
            binary_expr->level = -2;
            binary_expr->binary.operator= token;
            insertBinaryExpression(&expr, binary_expr);
            current = binary_expr;
            selector = token->raw.end;
          }
        } else if (strings_is(token->raw, "(")) {
          Token_dispose(token);
          Expression call_expr = readCallExpression(file, selector);
          if (!call_expr) {
            goto failed;
          }
          insertBinaryExpression(&expr, call_expr);
          current = NULL;
          selector = call_expr->node->position.end;
        } else if (strings_is(token->raw, "[")) {
          Token_dispose(token);
          Expression compute_expr = readComputeExpression(file, selector);
          if (!compute_expr) {
            goto failed;
          }
          insertBinaryExpression(&expr, compute_expr);
          current = NULL;
          selector = compute_expr->node->position.end;
        } else if (Token_check(token, TT_Symbol, "?")) {
          Expression triple_expr = Expression_create();
          triple_expr->binary.operator= token;
          selector = token->raw.end;
          triple_expr->binary.right = Expression_create();
          triple_expr->binary.right->node->type = NT_BinaryExpression;
          Expression truth = readExpression(file, selector);
          if (!truth) {
            Expression_dispose(triple_expr);
            goto failed;
          }
          triple_expr->binary.right->binary.left = truth;
          selector = truth->node->position.end;
          token = readTokenSkipNewline(file, selector);
          if (!token) {
            Expression_dispose(triple_expr);
            goto failed;
          }
          if (!Token_check(token, TT_Symbol, ":")) {
            Expression_dispose(triple_expr);
            ErrorStack_push(Error_init("Unexcept token.missing token ':'",
                                       getLocation(file, selector), NULL));
            goto failed;
          }
          selector = token->raw.end;
          triple_expr->binary.right->binary.operator= token;
          Expression falsely = readExpression(file, selector);
          if (!falsely) {
            Expression_dispose(triple_expr);
            goto failed;
          }
          triple_expr->binary.right->binary.right = falsely;
          selector = falsely->node->position.end;
          insertBinaryExpression(&expr, triple_expr);
          current = NULL;
          triple_expr->node->position.begin = source;
          triple_expr->node->position.end = selector;
          triple_expr->level = -2;
          triple_expr->node->type = NT_BinaryExpression;
        } else {
          int level = 0;
          for (; opts[level] != 0; level++) {
            if (strings_contains(token->raw, opts[level]->operators)) {
              break;
            }
          }
          if (opts[level]) {
            if (getMaxOperatorLevel() != -1) {
              if (level > getMaxOperatorLevel()) {
                Token_dispose(token);
                break;
              }
            }
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
      } else if (token->type == TT_Template ||
                 token->type == TT_TemplateStart) {
        Token_dispose(token);
        Expression template_expr = readTemplateExpression(file, selector);
        if (!template_expr) {
          goto failed;
        }
        selector = template_expr->node->position.end;
        if (template_expr->level >= expr->level) {
          template_expr->template->tag = expr;
          expr = template_expr;

        } else {
          Expression iterator = expr;
          while (iterator->binary.right->level > template_expr->level) {
            iterator = iterator->binary.right;
            if (!iterator->binary.right) {
              break;
            }
          }
          template_expr->template->tag = iterator->binary.right;
          iterator->binary.right = template_expr;
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
