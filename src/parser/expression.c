#include "parser/expression.h"
#include "parser/ast.h"
#include "parser/statement.h"
#include "parser/tokenizer.h"

static ExpressionContext ctx = NULL;

static ExpressionContext ExpressionContext_create() {
  ExpressionContext ctx =
      (ExpressionContext)Buffer_alloc(sizeof(struct s_ExpressionContext));
  ctx->current = NULL;
  ctx->root = NULL;
  ctx->maxlevel = -1;
  ctx->token_ctx = NULL;
  return ctx;
}

ExpressionContext pushExpressionContext() {
  ExpressionContext result = ctx;
  ctx = ExpressionContext_create();
  ctx->token_ctx = pushTokenContext();
  return result;
}

void popExpressionContext(ExpressionContext ectx) {
  popTokenContext(ctx->token_ctx);
  Buffer_free(ctx);
  ctx = ectx;
}

void setMaxOperatorLevel(int level) { ctx->maxlevel = level; }
int getMaxOperatorLevel() { return ctx->maxlevel; }

int isExpressionComplete() { return ctx->root && !ctx->current; }

void insertExpression(Expression expr) {
  if (!ctx->root) {
    ctx->root = expr;
  } else {
    if (ctx->current) {
      ctx->current->binary.right = expr;
    } else {
      if (ctx->root->level <= expr->level) {
        expr->binary.left = ctx->root;
        ctx->root = expr;
      } else {
        Expression iterator = ctx->root;
        while (iterator->binary.right->level > expr->level) {
          iterator = iterator->binary.right;
        }
        expr->binary.left = iterator->binary.right;
        iterator->binary.right = expr;
      }
    }
  }

  if (expr->bind == BT_Right || expr->bind == BT_Both) {
    ctx->current = expr;
  } else {
    ctx->current = NULL;
  }
}

Location getExpressionLocation(SourceFile file, cstring source) {
  Token token = readToken(file, source);
  if (!token) {
    Error_dispose(getError());
    return getLocation(file, source);
  }
  cstring selector = token->raw.begin;
  Token_dispose(token);
  return getLocation(file, selector);
}

Expression Expression_create() {
  Expression expression = (Expression)Buffer_alloc(sizeof(struct s_Expression));
  expression->node = AstNode_create();
  expression->node->type = NT_Expression;
  expression->type = ET_Unknown;
  expression->binary.left = NULL;
  expression->binary.right = NULL;
  expression->binary.operator= NULL;
  expression->level = -2;
  expression->bind = BT_Unknown;
  return expression;
}

typedef int (*ExpressionChecker)(SourceFile file, Token token);
typedef Expression (*ExpressionReader)(SourceFile file, cstring source);
struct ExpressionHandler {
  ExpressionChecker checker;
  ExpressionReader reader;
};

static struct ExpressionHandler atom_handlers[] = {
    {isAssignmentExpression, readAssignmentExpression},
    {isUnaryOperator, readUnaryExpression},
    {isLambdaExpression, readLambdaExpression},
    {isLiteralExpression, readLiteralExpression},
    {isFunctionExpression, readFunctionExpression},
    {isIdentifierExpression, readIdentifierExpression},
    {isBracketExpression, readBracketExpression},
    {isArrayExpression, readArrayExpression},
    {isObjectExpression, readObjectExpression},
    {isClassExpression, readClassExpression},
    {isNewExpression, readNewExpression},
    {isDeleteExpression, readDeleteExpression},
    {isAwaitExpression, readAwaitExpression},
    {isThisExpression, readThisExpression},
    {isSuperExpression, readSuperExpression},
    {0, 0}};

static struct ExpressionHandler operator_handlers[] = {
    {isCalculateOperator, readCalculateExpression},
    {isOptionalExpression, readOptionalExpression},
    {isMemberOperator, readMemberExpression},
    {isUpdateOperator, readUpdateExpression},
    {isComputeExpression, readComputeExpression},
    {isCallExpression, readCallExpression},
    {isConditionExpression, readConditionExpression},
    {0, 0}};

Expression readExpression(SourceFile file, cstring source) {
  cstring selector = source;
  enableReadRegex();
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  for (;;) {
    if (token->type == TT_Newline || token->type == TT_MultiLineComment) {
      selector = token->raw.end;
      Token_dispose(token);
      token = readTokenSkipNewline(file, selector);
      if (isExpressionComplete()) {
        if (checkToken(token, TT_Symbol, "++") ||
            checkToken(token, TT_Symbol, "--")) {
          Token_dispose(token);
          break;
        }
      }
    }
    if (token->type == TT_Eof) {
      Token_dispose(token);
      if (isExpressionComplete()) {
        break;
      } else {
        pushError("Unexcept token.", getExpressionLocation(file, selector));
        goto failed;
      }
    }
    if (checkToken(token, TT_Symbol, ";")) {
      Token_dispose(token);
      if (isExpressionComplete()) {
        break;
      } else {
        pushError("Unexcept token.", getExpressionLocation(file, selector));
        goto failed;
      }
    }
    if (isExpressionComplete()) {
      int indexOfHandler = 0;
      for (;;) {
        struct ExpressionHandler handler = operator_handlers[indexOfHandler];
        if (handler.checker == NULL) {
          Token_dispose(token);
          break;
        } else {
          if (handler.checker(file, token)) {
            Token_dispose(token);
            Expression expr = handler.reader(file, selector);
            if (!expr) {
              goto failed;
            }
            insertExpression(expr);
            selector = expr->node->position.end;
            break;
          }
          indexOfHandler++;
        }
      }
      if (!operator_handlers[indexOfHandler].checker) {
        break;
      }
    } else {
      int indexOfHandler = 0;
      for (;;) {
        struct ExpressionHandler handler = atom_handlers[indexOfHandler];
        if (handler.checker == NULL) {
          pushError("Unexcept token.", getLocation(file, token->raw.begin));
          Token_dispose(token);
          goto failed;
        } else {
          if (handler.checker(file, token)) {
            Token_dispose(token);
            Expression expr = handler.reader(file, selector);
            if (!expr) {
              goto failed;
            }
            insertExpression(expr);
            selector = expr->node->position.end;
            break;
          }
        }
        indexOfHandler++;
      }
    }
    if (isExpressionComplete()) {
      disableReadRegex();
    } else {
      enableReadRegex();
    }
    token = readTokenSkipComment(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (ctx->root) {
    ctx->root->node->position.begin = source;
    ctx->root->node->position.end = selector;
  }
  goto finaly;
failed:
  if (ctx->root) {
    Expression_dispose(ctx->root);
    ctx->root = NULL;
  }
finaly : {
  Expression result = ctx->root;
  ctx->root = NULL;
  ctx->current = NULL;
  return result;
}
}

void Expression_dispose(Expression expression) {
  switch (expression->type) {
  case ET_Literal:
    if (expression->literal) {
      Token_dispose(expression->literal);
    }
    break;
  case ET_Identifier:
    if (expression->Identifier) {
      Token_dispose(expression->Identifier);
    }
    break;
  case ET_Lambda:
    if (expression->lambda->args) {
      List_dispose(expression->lambda->args);
    }
    if (expression->lambda->body) {
      Statement_dispose(expression->lambda->body);
    }
    Buffer_free(expression->lambda);
    break;
  case ET_Function:
    if (expression->function->args) {
      List_dispose(expression->function->args);
    }
    if (expression->function->name) {
      Token_dispose(expression->function->name);
    }
    if (expression->function->body) {
      Statement_dispose(expression->function->body);
    }
    Buffer_free(expression->function);
    break;
  case ET_Call:
  case ET_OptionalCall:
    if (expression->call.args) {
      List_dispose(expression->call.args);
    }
    if (expression->call.callee) {
      Expression_dispose(expression->call.callee);
    }
    break;
  case ET_Array:
    if (expression->array->items) {
      List_dispose(expression->array->items);
    }
    Buffer_free(expression->array);
    break;
  case ET_Object:
    if (expression->object->properties) {
      List_dispose(expression->object->properties);
    }
    Buffer_free(expression->object);
    break;
  case ET_Class:
    if (expression->clazz->extends) {
      Expression_dispose(expression->clazz->extends);
    }
    if (expression->clazz->decorators) {
      List_dispose(expression->clazz->decorators);
    }
    if (expression->clazz->properties) {
      List_dispose(expression->clazz->properties);
    }
    if (expression->clazz->name) {
      Token_dispose(expression->clazz->name);
    }
    if (expression->clazz->staticBlocks) {
      List_dispose(expression->clazz->staticBlocks);
    }
    Buffer_free(expression->clazz);
    break;
  case ET_Condition:
    if (expression->condition.condition) {
      Expression_dispose(expression->condition.condition);
    }
    if (expression->condition.consequent) {
      Expression_dispose(expression->condition.consequent);
    }
    if (expression->condition.alternate) {
      Expression_dispose(expression->condition.alternate);
    }
    break;
  case ET_Assignment:
    if (expression->assignment.expression) {
      Expression_dispose(expression->assignment.expression);
    }
    if (expression->assignment.type) {
      Token_dispose(expression->assignment.type);
    }
    break;
  default:
    if (expression->binary.left) {
      Expression_dispose(expression->binary.left);
    }
    if (expression->binary.right) {
      Expression_dispose(expression->binary.right);
    }
    if (expression->binary.operator) {
      Token_dispose(expression->binary.operator);
    }
  }
  AstNode_dispose(expression->node);
  Buffer_free(expression);
}