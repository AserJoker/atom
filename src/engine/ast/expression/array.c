#include "ast.h"

Array Array_create() {
  Array array = (Array)Buffer_alloc(sizeof(struct s_Array));
  array->node = AstNode_create();
  array->node->type = NT_Array;
  List_Option opt = {1, (Buffer_Free)Expression_dispose};
  array->items = List_create(opt);
  return array;
}
void Array_dispose(Array array) {
  List_dispose(array->items);
  AstNode_dispose(array->node);
  Buffer_free(array);
}
Expression ArrayExpression_create() {
  Expression expr = Expression_create();
  expr->node->type = NT_ArrayPattern;
  expr->level = -2;
  return expr;
}
void ArrayPattern_dispose(Expression expression) {
  if (expression->array) {
    Array_dispose(expression->array);
  }
  AstNode_dispose(expression->node);
  Buffer_free(expression);
}
Array readArray(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  if (!Token_check(token, TT_Symbol, "[")) {
    Token_dispose(token);
    ErrorStack_push(Error_init("Unexcept token.mssing token '['",
                               getLocation(file, source), NULL));
    return NULL;
  }
  cstring selector = token->raw.end;
  Token_dispose(token);
  Array array = Array_create();
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!Token_check(token, TT_Symbol, "]")) {
    Token_dispose(token);
    AstContext current = pushAstContext();
    enableCommaTail();
    Expression item = readExpression(file, selector);
    disableCommaTail();
    popAstContext(current);
    if (!item) {
      goto failed;
    }
    for (;;) {
      List_insert_tail(array->items, item);
      selector = item->node->position.end;
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (Token_check(token, TT_Symbol, ",")) {
        selector = token->raw.end;
        Token_dispose(token);
        token = readTokenSkipNewline(file, selector);
        if (!token) {
          goto failed;
        }
        if (Token_check(token, TT_Symbol, "]")) {
          break;
        }
        Token_dispose(token);
      } else if (Token_check(token, TT_Symbol, "]")) {
        break;
      } else {
        Token_dispose(token);
        ErrorStack_push(Error_init("Unexcept token.missing token ']'",
                                   getLocation(file, selector), NULL));
        goto failed;
      }
      AstContext current = pushAstContext();
      enableCommaTail();
      item = readExpression(file, selector);
      disableCommaTail();
      popAstContext(current);
      if (!item) {
        goto failed;
      }
    }
  }
  selector = token->raw.end;
  Token_dispose(token);
  array->node->position.begin = source;
  array->node->position.end = selector;
  return array;
failed:
  Array_dispose(array);
  return NULL;
}
Expression readArrayPattern(SourceFile file, cstring source) {
  Array arr = readArray(file, source);
  if (!arr) {
    return NULL;
  }
  Expression expr = ArrayExpression_create();
  expr->array = arr;
  expr->node->position = arr->node->position;
  return expr;
}