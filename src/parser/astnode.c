#include "parser/astnode.h"
#include <stdarg.h>

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

static void AstNode_dispose(AstNode node) {
  if (node->type == ANT_Identifier) {
    Buffer_dispose(node->e_identifier);
  } else if (node->type == ANT_Literal) {
    Buffer_dispose(node->e_literal);
  } else if (node->type == ANT_Function) {
    Buffer_dispose(node->e_function.args);
    Buffer_dispose(node->e_function.body);
    Buffer_dispose(node->e_function.name);
  } else if (node->type == ANT_Lambda) {
    Buffer_dispose(node->e_lambda.args);
    Buffer_dispose(node->e_lambda.body);
  } else if (node->type == ANT_Template) {
    Buffer_dispose(node->e_template.tag);
    Buffer_dispose(node->e_template.args);
    Buffer_dispose(node->e_template.parts);
  } else if (node->type == ANT_ObjectProperty) {
    Buffer_dispose(node->e_oprop.key);
    if (node->e_oprop.type == OPT_Field) {
      Buffer_dispose(node->e_oprop.field);
    } else {
      Buffer_dispose(node->e_oprop.method.args);
      Buffer_dispose(node->e_oprop.method.body);
    }
  } else if (node->type == ANT_Array) {
    Buffer_dispose(node->e_array.items);
  } else if (node->type == ANT_BlockStatement) {
    Buffer_dispose(node->s_block.statements);
  } else if (node->type == ANT_Program) {
    Buffer_dispose(node->s_program.body);
  } else if (node->type == ANT_Class) {
    Buffer_dispose(node->e_class.decorators);
    Buffer_dispose(node->e_class.extends);
    Buffer_dispose(node->e_class.name);
    Buffer_dispose(node->e_class.properties);
  } else if (node->type == ANT_ClassProperty) {
    Buffer_dispose(node->e_cprop.decorators);
    Buffer_dispose(node->e_cprop.key);
    if (node->e_cprop.type == CPT_Field) {
      Buffer_dispose(node->e_cprop.field);
    } else if (node->e_cprop.type == CPT_StaticBlock) {
      Buffer_dispose(node->e_cprop.staticBlock);
    } else {
      Buffer_dispose(node->e_cprop.method.args);
      Buffer_dispose(node->e_cprop.method.body);
    }
  } else {
    Buffer_dispose(node->binary.left);
    Buffer_dispose(node->binary.right);
    if (node->binary.flag) {
      Buffer_dispose(node->binary.flag);
    }
  }
}
static AstNode AstNode_create() {
  AstNode node =
      (AstNode)Buffer_alloc(sizeof(struct s_AstNode), AstNode_dispose);
  node->bind = BT_None;
  node->level = 0;
  node->position.begin = 0;
  node->position.end = 0;
  node->type = ANT_Fake;
  node->binary.left = 0;
  node->binary.right = 0;
  node->binary.flag = 0;
  return node;
}

static AstNode AstNode_insertToList(AstNode iterator, AstNode node) {
  if (iterator->binary.right == NULL) {
    iterator->binary.right = node;
    return iterator;
  } else {
    AstNode it = AstNode_create();
    it->binary.left = iterator->binary.right;
    it->binary.right = node;
    iterator->binary.right = it;
    return it;
  }
}

static void AstNode_toList(AstNode iterator, AstNode node, ...) {
  AstNode it = iterator;
  va_list list;
  va_start(list, node);
  AstNode next = va_arg(list, AstNode);
  for (;;) {
    it = AstNode_insertToList(it, node);
    if (!next) {
      break;
    }
    node = next;
    next = va_arg(list, AstNode);
  }
  va_end(list);
}

typedef struct s_ProcessHandle {
  Bool (*checker)(SourceFile file, Token token);
  AstNode (*handler)(SourceFile file, cstring source);
} ProcessHandle;

typedef struct s_ExpressionContext {
  TokenContext tctx;
  int maxLevel;
  AstNode root;
  AstNode current;
} *ExpressionContext;

static ExpressionContext g_ectx = NULL;

static ExpressionContext ExpressionContext_push() {
  ExpressionContext result = g_ectx;
  g_ectx =
      (ExpressionContext)Buffer_alloc(sizeof(struct s_ExpressionContext), NULL);
  g_ectx->current = NULL;
  g_ectx->root = NULL;
  g_ectx->maxLevel = -1;
  g_ectx->tctx = Token_pushTokenContext();
  return result;
}
static void ExpressionContext_pop(ExpressionContext ectx) {
  Token_popTokenContext(g_ectx->tctx);
  Buffer_dispose(g_ectx);
  g_ectx = ectx;
}

static Bool Expression_isComplete() { return g_ectx->root && !g_ectx->current; }

static void Expression_insert(AstNode expr) {
  if (!g_ectx->root) {
    g_ectx->root = expr;
  } else {
    if (g_ectx->current) {
      g_ectx->current->binary.right = expr;
    } else {
      if (g_ectx->root->level <= expr->level) {
        expr->binary.left = g_ectx->root;
        g_ectx->root = expr;
      } else {
        AstNode iterator = g_ectx->root;
        while (iterator->binary.right->level > expr->level) {
          iterator = iterator->binary.right;
        }
        expr->binary.left = iterator->binary.right;
        iterator->binary.right = expr;
      }
    }
  }

  if (expr->bind == BT_Right || expr->bind == BT_Both) {
    g_ectx->current = expr;
  } else {
    g_ectx->current = NULL;
  }
}

static AstNode AstNode_readExpression(SourceFile file, cstring source);
static AstNode AstNode_readStatement(SourceFile file, cstring source);
static AstNode AstNode_readBlockStatement(SourceFile file, cstring source);
CHECKER(AstNode_isLiteral) {
  if (!token) {
    return False;
  }
  if (token->type == TT_Number || token->type == TT_String ||
      token->type == TT_Regex) {
    return True;
  }
  if (Token_check(token, TT_Keyword, "true") ||
      Token_check(token, TT_Keyword, "false") ||
      Token_check(token, TT_Keyword, "undefined") ||
      Token_check(token, TT_Keyword, "null") ||
      Token_check(token, TT_Keyword, "this") ||
      Token_check(token, TT_Keyword, "super")) {
    return True;
  }
  return False;
}
READER(AstNode_readLiteral) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_Literal;
  node->level = -2;
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  selector = token->raw.end;
  node->e_literal = token;
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}
CHECKER(AstNode_isIdentifier) { return token->type == TT_Identifier; }

READER(AstNode_readIdentifier) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->level = -2;
  node->bind = BT_None;
  node->type = ANT_Identifier;
  Token token = Token_readSkipNewline(file, source);
  if (!token) {
    goto failed;
  }
  selector = token->raw.end;
  node->e_identifier = token;
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

CHECKER(AstNode_isCalculate) {
  if (Token_check(token, TT_Keyword, "instanceof") ||
      Token_check(token, TT_Keyword, "in") ||
      Token_check(token, TT_Keyword, "of")) {
    return True;
  }
  if (token->type != TT_Symbol) {
    return False;
  }
  int level = 0;
  for (;;) {
    if (g_ectx->maxLevel != -1 && level >= g_ectx->maxLevel) {
      return False;
    }
    s_OperatorSet *set = opts[level];
    if (!set) {
      return False;
    }
    if (Strings_contains(token->raw, set->operators)) {
      return True;
    }
    level++;
  }
}

READER(AstNode_readCalculate) {
  Token token = Token_readSkipNewline(file, source);
  AstNode expr = AstNode_create();
  expr->binary.flag = cstring_from(token->raw);
  if (Token_check(token, TT_Keyword, "instanceof") ||
      Token_check(token, TT_Keyword, "in") ||
      Token_check(token, TT_Keyword, "of")) {
    expr->level = 4;
  } else {
    int level = 0;
    for (;;) {
      s_OperatorSet *set = opts[level];
      if (Strings_contains(token->raw, set->operators)) {
        break;
      }
      level++;
    }
    expr->level = level;
  }
  expr->bind = BT_Both;
  expr->type = ANT_Binary;
  expr->position = token->raw;
  Buffer_dispose(token);
  return expr;
}

CHECKER(AstNode_isUnary) {
  return Strings_contains(token->raw, unaryOperators);
}

READER(AstNode_readUnary) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_Binary;
  node->level = -1;
  node->bind = BT_Right;
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  selector = token->raw.end;
  node->binary.flag = cstring_from(token->raw);
  Buffer_dispose(token);
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

CHECKER(AstNode_isUpdate) {
  return Strings_contains(token->raw, updateOperators);
}

READER(AstNode_readUpdate) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_Binary;
  node->level = -1;
  node->bind = BT_Left;
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  selector = token->raw.end;
  node->binary.flag = cstring_from(token->raw);
  Buffer_dispose(token);
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

CHECKER(AstNode_isBracket) { return Token_check(token, TT_Symbol, "("); }
READER(AstNode_readBracket) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_Binary;
  node->level = -2;
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  ExpressionContext ectx = ExpressionContext_push();
  AstNode bracket = AstNode_readExpression(file, selector);
  ExpressionContext_pop(ectx);
  if (!bracket) {
    goto failed;
  }
  node->binary.left = bracket;
  selector = bracket->position.end;
  node->binary.flag = (cstring)cstring_toBuffer("()");
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!Token_check(token, TT_Symbol, ")")) {
    Error_push("Unexcept token.missing token ')'",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  AstNode_dispose(node);
  return NULL;
}

CHECKER(AstNode_isCall) { return Token_check(token, TT_Symbol, "("); }
READER(AstNode_readCall) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_Call;
  node->level = -2;
  node->bind = BT_Left;
  AstNode iterator = node;
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!Token_check(token, TT_Symbol, ")")) {
    Buffer_dispose(token);
    for (;;) {
      ExpressionContext ectx = ExpressionContext_push();
      g_ectx->maxLevel = 12;
      AstNode arg = AstNode_readExpression(file, selector);
      ExpressionContext_pop(ectx);
      if (!arg) {
        goto failed;
      }
      iterator = AstNode_insertToList(iterator, arg);
      selector = arg->position.end;
      token = Token_readSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (Token_check(token, TT_Symbol, ",")) {
        selector = token->raw.end;
        Buffer_dispose(token);
      } else if (Token_check(token, TT_Symbol, ")")) {
        break;
      } else {
        Error_push("Unexcept token",
                   SourceFile_getLocation(file, token->raw.begin));
        Buffer_dispose(token);
        goto failed;
      }
    }
  }
  if (!Token_check(token, TT_Symbol, ")")) {
    Error_push("Unexcept token.",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}
CHECKER(AstNode_isMember) { return Token_check(token, TT_Symbol, "."); }
READER(AstNode_readMember) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_Binary;
  node->bind = BT_Both;
  node->level = -2;
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  selector = token->raw.end;
  node->binary.flag = cstring_from(token->raw);
  Buffer_dispose(token);
  node->position.begin = source;
  node->position.end = selector;
  return node;

failed:
  Buffer_dispose(node);
  return NULL;
}

CHECKER(AstNode_isCompute) { return Token_check(token, TT_Symbol, "["); }
READER(AstNode_readCompute) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_Binary;
  node->bind = BT_Left;
  node->level = -2;
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  ExpressionContext ectx = ExpressionContext_push();
  AstNode key = AstNode_readExpression(file, selector);
  ExpressionContext_pop(ectx);
  if (!key) {
    goto failed;
  }
  selector = key->position.end;
  node->binary.right = key;
  node->binary.flag = cstring_toBuffer("compute");
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!Token_check(token, TT_Symbol, "]")) {
    Error_push("Unexcept token.missing token ']'",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  node->position.begin = source;
  node->position.end = selector;
  return node;

failed:
  Buffer_dispose(node);
  return NULL;
}
CHECKER(AstNode_isOptional) { return Token_check(token, TT_Symbol, "?."); }
READER(AstNode_readOptional) {
  cstring selector = source;
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  if (Token_check(token, TT_Symbol, "(")) {
    Buffer_dispose(token);
    AstNode node = AstNode_readCall(file, selector);
    if (!node) {
      return NULL;
    }
    node->type = ANT_OptionalCall;
    return node;
  } else if (Token_check(token, TT_Symbol, "[")) {
    Buffer_dispose(token);
    AstNode node = AstNode_readCompute(file, selector);
    if (!node) {
      return NULL;
    }
    Buffer_dispose(node->binary.flag);
    node->binary.flag = cstring_toBuffer("optional-compute");
    return node;
  } else {
    Buffer_dispose(token);
    AstNode node = AstNode_create();
    node->type = ANT_Binary;
    node->level = -2;
    node->bind = BT_Both;
    node->binary.flag = cstring_toBuffer("?.");
    node->position.begin = source;
    node->position.end = selector;
    return node;
  }
  return NULL;
}
CHECKER(AstNode_isPrefix) {
  return Token_check(token, TT_Keyword, "await") ||
         Token_check(token, TT_Keyword, "new") ||
         Token_check(token, TT_Keyword, "delete");
}
READER(AstNode_readPrefix) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_Binary;
  node->bind = BT_Right;
  node->level = -1;
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  selector = token->raw.end;
  node->binary.flag = cstring_from(token->raw);
  Buffer_dispose(token);
  node->position.begin = source;
  node->position.end = selector;
  return node;

failed:
  Buffer_dispose(node);
  return NULL;
}
CHECKER(AstNode_isAssigment) {
  return Token_check(token, TT_Keyword, "const") ||
         Token_check(token, TT_Keyword, "let") ||
         Token_check(token, TT_Keyword, "var");
}
READER(AstNode_readAssigment) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_Binary;
  node->bind = BT_Right;
  node->level = 12;
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  selector = token->raw.end;
  node->binary.flag = cstring_from(token->raw);
  Buffer_dispose(token);
  node->position.begin = source;
  node->position.end = selector;
  return node;

failed:
  Buffer_dispose(node);
  return NULL;
}

CHECKER(AstNode_isFunction) {
  if (Token_check(token, TT_Keyword, "e_function")) {
    return True;
  } else if (Token_check(token, TT_Keyword, "async")) {
    token = Token_readSkipNewline(file, token->raw.end);
    if (token && Token_check(token, TT_Keyword, "e_function")) {
      Buffer_dispose(token);
      return True;
    }
    Buffer_dispose(token);
  }
  return False;
}
READER(AstNode_readFunction) {
  Bool async = False;
  Bool generator = False;
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_Function;
  node->level = -2;
  node->e_function.args = List_create(True);
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (Token_check(token, TT_Keyword, "async")) {
    node->e_function.async = True;
    selector = token->raw.end;
    Buffer_dispose(token);
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (Token_check(token, TT_Keyword, "e_function")) {
    selector = token->raw.end;
    Buffer_dispose(token);
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (Token_check(token, TT_Symbol, "*")) {
    node->e_function.generator = True;
    selector = token->raw.end;
    Buffer_dispose(token);
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (token->type == TT_Identifier) {
    node->e_function.name = cstring_from(token->raw);
    selector = token->raw.end;
    Buffer_dispose(token);
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (!Token_check(token, TT_Symbol, "(")) {
    Error_push("Unexcept token.missing token '('",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!Token_check(token, TT_Symbol, ")")) {
    Buffer_dispose(token);
    for (;;) {
      ExpressionContext ectx = ExpressionContext_push();
      g_ectx->maxLevel = 12;
      AstNode arg = AstNode_readExpression(file, selector);
      ExpressionContext_pop(ectx);
      if (!arg) {
        goto failed;
      }
      selector = arg->position.end;
      List_insert_tail(node->e_function.args, arg);
      token = Token_readSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (Token_check(token, TT_Symbol, ",")) {
        selector = token->raw.end;
        Buffer_dispose(token);
      } else if (Token_check(token, TT_Symbol, ")")) {
        break;
      }
    }
  }
  if (!Token_check(token, TT_Symbol, ")")) {
    Error_push("Unexcept token.missing token ')'",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  node->e_function.body = AstNode_readBlockStatement(file, selector);
  if (!node->e_function.body) {
    goto failed;
  }
  selector = node->e_function.body->position.end;
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

CHECKER(AstNode_isLambda) {
  Token it = token;
  cstring selector = token->raw.begin;
  if (Token_check(it, TT_Keyword, "async")) {
    selector = it->raw.end;
    it = Token_readSkipNewline(file, selector);
    if (!it) {
      return False;
    }
    if (Token_check(it, TT_Symbol, "(")) {
      Buffer_dispose(it);
      return True;
    }
    Buffer_dispose(it);
    return False;
  }

  if (Token_check(it, TT_Symbol, "(")) {
    Token next = Token_pair(file, selector, TT_Symbol, "(", TT_Symbol, ")");
    if (!next) {
      return False;
    }
    selector = next->raw.end;
    Buffer_dispose(next);
    next = Token_readSkipNewline(file, selector);
    if (!next) {
      return False;
    }
    if (Token_check(next, TT_Symbol, "=>")) {
      Buffer_dispose(next);
      return True;
    }
    Buffer_dispose(next);
    return False;
  }
  if (it->type == TT_Identifier) {
    it = Token_readSkipNewline(file, it->raw.end);
    if (!it) {
      return False;
    }
    if (Token_check(it, TT_Symbol, "=>")) {
      Buffer_dispose(it);
      return True;
    }
    Buffer_dispose(it);
    return False;
  }
  return False;
}

READER(AstNode_readLambda) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_Lambda;
  node->level = -2;
  node->e_lambda.args = List_create(True);
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (Token_check(token, TT_Keyword, "async")) {
    node->e_lambda.async = True;
    selector = token->raw.end;
    Buffer_dispose(token);
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (token->type == TT_Identifier) {
    Buffer_dispose(token);
    ExpressionContext ectx = ExpressionContext_push();
    AstNode arg = AstNode_readIdentifier(file, selector);
    ExpressionContext_pop(ectx);
    if (!arg) {
      goto failed;
    }
    List_insert_tail(node->e_lambda.args, arg);
    selector = arg->position.end;
  } else {
    if (!Token_check(token, TT_Symbol, "(")) {
      Error_push("Unexcept token.missing token '('",
                 SourceFile_getLocation(file, token->raw.begin));
      Buffer_dispose(token);
      goto failed;
    }
    selector = token->raw.end;
    Buffer_dispose(token);
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
    if (!Token_check(token, TT_Symbol, ")")) {
      Buffer_dispose(token);
      for (;;) {
        ExpressionContext ectx = ExpressionContext_push();
        g_ectx->maxLevel = 12;
        AstNode arg = AstNode_readExpression(file, selector);
        ExpressionContext_pop(ectx);
        if (!arg) {
          goto failed;
        }
        List_insert_tail(node->e_lambda.args, arg);
        selector = arg->position.end;
        token = Token_read(file, selector);
        if (!token) {
          goto failed;
        }
        if (Token_check(token, TT_Symbol, ",")) {
          selector = token->raw.end;
          Buffer_dispose(token);
        } else {
          break;
        }
      }
    }
    if (!Token_check(token, TT_Symbol, ")")) {
      Error_push("Unexcept token.missing token ')'",
                 SourceFile_getLocation(file, token->raw.begin));
      Buffer_dispose(token);
      goto failed;
    }
    selector = token->raw.end;
    Buffer_dispose(token);
  }
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!Token_check(token, TT_Symbol, "=>")) {
    Error_push("Unexcept token.missing token '=>'",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (Token_check(token, TT_Symbol, "{")) {
    node->e_lambda.body = AstNode_readStatement(file, selector);
  } else {
    ExpressionContext ectx = ExpressionContext_push();
    g_ectx->maxLevel = 12;
    node->e_lambda.body = AstNode_readExpression(file, selector);
    ExpressionContext_pop(ectx);
  }
  Buffer_dispose(token);
  if (!node->e_lambda.body) {
    goto failed;
  }
  selector = node->e_lambda.body->position.end;
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

CHECKER(AstNode_isArray) { return Token_check(token, TT_Symbol, "["); }
READER(AstNode_readArray) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_Array;
  node->level = -2;
  node->e_array.items = List_create(True);
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!Token_check(token, TT_Symbol, "]")) {
    Buffer_dispose(token);
    for (;;) {
      ExpressionContext ectx = ExpressionContext_push();
      g_ectx->maxLevel = 12;
      AstNode item = AstNode_readExpression(file, selector);
      ExpressionContext_pop(ectx);
      if (!item) {
        goto failed;
      }
      List_insert_tail(node->e_array.items, item);
      selector = item->position.end;
      token = Token_readSkipNewline(file, selector);
      Bool isSplit = False;
      if (!token) {
        goto failed;
      }
      if (Token_check(token, TT_Symbol, ",")) {
        selector = token->raw.end;
        Buffer_dispose(token);
        isSplit = True;
        token = Token_readSkipNewline(file, selector);
        if (!token) {
          goto failed;
        }
      }
      if (Token_check(token, TT_Symbol, "]") || token->type == TT_Eof) {
        break;
      } else {
        if (!isSplit) {
          Error_push("Unexcept token.",
                     SourceFile_getLocation(file, token->raw.begin));
          Buffer_dispose(token);
          goto failed;
        } else {
          Buffer_dispose(token);
        }
      }
    }
  }
  if (!Token_check(token, TT_Symbol, "]")) {
    Error_push("Unexcept token.",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

static AstNode ObjectProperty_read(SourceFile file, cstring source) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_ObjectProperty;
  node->e_oprop.key = NULL;
  node->e_oprop.type = OPT_Field;
  node->e_oprop.field = NULL;
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (Token_check(token, TT_Keyword, "async")) {
    if (node->e_oprop.type != OPT_Field) {
      Error_push("Unexcept token 'async.'",
                 SourceFile_getLocation(file, token->raw.begin));
      Buffer_dispose(token);
      goto failed;
    }
    node->e_oprop.type = OPT_Method;
    node->e_oprop.method.async = True;
    selector = token->raw.end;
    Buffer_dispose(token);
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (Token_check(token, TT_Keyword, "get")) {
    if (node->e_oprop.type != OPT_Field) {
      Error_push("Unexcept token 'get.'",
                 SourceFile_getLocation(file, token->raw.begin));
      Buffer_dispose(token);
      goto failed;
    }
    node->e_oprop.type = OPT_Getter;
    selector = token->raw.end;
    Buffer_dispose(token);
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }

  if (Token_check(token, TT_Keyword, "set")) {
    if (node->e_oprop.type != OPT_Field) {
      Error_push("Unexcept token 'set.'",
                 SourceFile_getLocation(file, token->raw.begin));
      Buffer_dispose(token);
      goto failed;
    }
    node->e_oprop.type = OPT_Setter;
    selector = token->raw.end;
    Buffer_dispose(token);
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (Token_check(token, TT_Symbol, "*")) {
    if (node->e_oprop.type != OPT_Field && node->e_oprop.type != OPT_Method) {
      Error_push("Unexcept token '*'.",
                 SourceFile_getLocation(file, token->raw.begin));
      Buffer_dispose(token);
      goto failed;
    }
    node->e_oprop.method.generator = True;
    node->e_oprop.type = OPT_Method;
    selector = token->raw.end;
    Buffer_dispose(token);
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (node->e_oprop.type != OPT_Field) {
    node->e_oprop.method.args = List_create(True);
  }
  if (Token_check(token, TT_Symbol, "[")) {
    Buffer_dispose(token);
    node->e_oprop.key = AstNode_readCompute(file, selector);
  } else {
    Buffer_dispose(token);
    node->e_oprop.key = AstNode_readIdentifier(file, selector);
  }
  if (!node->e_oprop.key) {
    goto failed;
  }
  selector = node->e_oprop.key->position.end;
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (Token_check(token, TT_Symbol, ":")) {
    if (node->e_oprop.type != OPT_Field) {
      Error_push("Unexcept token ':'.",
                 SourceFile_getLocation(file, token->raw.begin));
      Buffer_dispose(token);
      goto failed;
    }
    selector = token->raw.end;
    Buffer_dispose(token);
    ExpressionContext ectx = ExpressionContext_push();
    g_ectx->maxLevel = 12;
    node->e_oprop.field = AstNode_readExpression(file, selector);
    ExpressionContext_pop(ectx);
    if (!node->e_oprop.field) {
      goto failed;
    }
    selector = node->e_oprop.field->position.end;
  } else if (Token_check(token, TT_Symbol, "(")) {
    if (node->e_oprop.type == OPT_Field) {
      node->e_oprop.type = OPT_Method;
      node->e_oprop.method.args = List_create(True);
    }
    selector = token->raw.end;
    Buffer_dispose(token);
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
    if (!Token_check(token, TT_Symbol, ")")) {
      Buffer_dispose(token);
      for (;;) {
        ExpressionContext ectx = ExpressionContext_push();
        g_ectx->maxLevel = 12;
        AstNode arg = AstNode_readExpression(file, selector);
        ExpressionContext_pop(ectx);
        if (!arg) {
          goto failed;
        }
        List_insert_tail(node->e_oprop.method.args, arg);
        selector = arg->position.end;
        token = Token_readSkipNewline(file, selector);
        if (!token) {
          goto failed;
        }
        if (Token_check(token, TT_Symbol, ",")) {
          selector = token->raw.end;
          Buffer_dispose(token);
        } else {
          break;
        }
      }
    }

    if (!Token_check(token, TT_Symbol, ")")) {
      Error_push("Unexcept token.missing token ')'",
                 SourceFile_getLocation(file, token->raw.begin));
      Buffer_dispose(token);
      goto failed;
    }
    selector = token->raw.end;
    Buffer_dispose(token);
    node->e_oprop.method.body = AstNode_readBlockStatement(file, selector);
    if (!node->e_oprop.method.body) {
      goto failed;
    }
    selector = node->e_oprop.method.body->position.end;
  }
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

CHECKER(AstNode_isObject) { return Token_check(token, TT_Symbol, "{"); }
READER(AstNode_readObject) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_Object;
  node->level = -2;
  node->e_object.properties = List_create(True);
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!Token_check(token, TT_Symbol, "}")) {
    Buffer_dispose(token);
    for (;;) {
      AstNode prop = ObjectProperty_read(file, selector);
      if (!prop) {
        goto failed;
      }
      selector = prop->position.end;
      List_insert_tail(node->e_object.properties, prop);
      token = Token_readSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      Bool isSplit = False;
      if (Token_check(token, TT_Symbol, ",")) {
        selector = token->raw.end;
        Buffer_dispose(token);
        token = Token_readSkipNewline(file, selector);
        isSplit = True;
      }
      if (Token_check(token, TT_Symbol, "}") || token->type == TT_Eof) {
        break;
      } else if (isSplit) {
        Buffer_dispose(token);
      } else {
        Error_push("Unexcept token.",
                   SourceFile_getLocation(file, token->raw.begin));
        Buffer_dispose(token);
        goto failed;
      }
    }
  }
  if (!Token_check(token, TT_Symbol, "}")) {
    Error_push("Unexcept token.missing token '}'",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

READER(AstNode_readClassProperty) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_ClassProperty;
  node->e_cprop.decorators = List_create(True);
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}
CHECKER(AstNode_isClass) {
  return Token_check(token, TT_Symbol, "@") ||
         Token_check(token, TT_Keyword, "class");
}
READER(AstNode_readClass) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_Class;
  node->e_class.decorators = List_create(True);
  node->e_class.extends = NULL;
  node->e_class.name = NULL;
  node->e_class.properties = List_create(True);
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (Token_check(token, TT_Symbol, "@")) {
    Buffer_dispose(token);
    for (;;) {
      token = Token_readSkipNewline(file, selector);
      if (Token_check(token, TT_Symbol, "@")) {
        selector = token->raw.end;
        Buffer_dispose(token);
      } else {
        break;
      }
      ExpressionContext ectx = ExpressionContext_push();
      AstNode dec = AstNode_readExpression(file, selector);
      ExpressionContext_pop(ectx);
      if (!dec) {
        goto failed;
      }
      if (dec->type != ANT_Identifier && dec->type != ANT_Call) {
        Error_push("Decorators are not valid here.",
                   SourceFile_getLocation(file, dec->position.begin));
        Buffer_dispose(dec);
        goto failed;
      }
      List_insert_tail(node->e_class.decorators, dec);
      selector = dec->position.end;
    }
  }
  if (!Token_check(token, TT_Keyword, "class")) {
    Error_push("Unexcept token.missing token 'class'",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (token->type == TT_Identifier) {
    node->e_class.name = token;
    selector = token->raw.end;
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (Token_check(token, TT_Keyword, "extends")) {
    selector = token->raw.end;
    Buffer_dispose(token);
    ExpressionContext ectx = ExpressionContext_push();
    node->e_class.extends = AstNode_readExpression(file, selector);
    ExpressionContext_pop(ectx);
    if (!node->e_class.extends) {
      goto failed;
    }
    selector = node->e_class.extends->position.end;
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (!Token_check(token, TT_Symbol, "{")) {
    Error_push("Unexcept token.missing token '{'",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!Token_check(token, TT_Symbol, "}")) {
    Buffer_dispose(token);
    for (;;) {
      AstNode property = AstNode_readClassProperty(file, selector);
      if (!property) {
        goto failed;
      }
      List_insert_tail(node->e_class.properties, property);
      selector = property->position.end;
      token = Token_readSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (Token_check(token, TT_Symbol, "}") || token->type == TT_Eof) {
        break;
      }
      if (Token_check(token, TT_Symbol, ";")) {
        selector = token->raw.end;
      }
      Buffer_dispose(token);
    }
  }

  if (!Token_check(token, TT_Symbol, "}")) {
    Error_push("Unexcept token.missing token '}'",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

CHECKER(AstNode_isTemplate) {
  if (token->type == TT_Identifier) {
    Token next = Token_readSkipNewline(file, token->raw.end);
    if (!next) {
      return False;
    }
    if (next->type == TT_Template || next->type == TT_TemplateStart) {
      Buffer_dispose(next);
      return True;
    }
    Buffer_dispose(next);
    return False;
  }
  return token->type == TT_Template || token->type == TT_TemplateStart;
}
READER(AstNode_readTemplate) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_Template;
  node->level = -2;
  node->e_template.args = List_create(True);
  node->e_template.parts = List_create(True);
  node->e_template.tag = NULL;
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (token->type == TT_Identifier) {
    node->e_template.tag = token;
    selector = token->raw.end;
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  List_insert_tail(node->e_template.parts, token);
  selector = token->raw.end;
  if (token->type != TT_Template) {
    for (;;) {
      ExpressionContext ectx = ExpressionContext_push();
      Token_enableReadTemplate();
      AstNode arg = AstNode_readExpression(file, selector);
      ExpressionContext_pop(ectx);
      if (!arg) {
        goto failed;
      }
      List_insert_tail(node->e_template.args, arg);
      selector = arg->position.end;

      Token_enableReadTemplate();
      token = Token_readSkipNewline(file, selector);
      Token_disableReadTemplate();
      if (!token) {
        goto failed;
      }
      if (token->type != TT_TemplateEnd && token->type != TT_TemplatePart) {
        Error_push("Unexcept token.",
                   SourceFile_getLocation(file, token->raw.begin));
        Buffer_dispose(token);
        goto failed;
      }
      List_insert_tail(node->e_template.parts, token);
      selector = token->raw.end;
      if (token->type == TT_TemplateEnd) {
        break;
      }
    }
  }
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

static ProcessHandle atom_expression_handlers[] = {
    {AstNode_isArray, AstNode_readArray},
    {AstNode_isObject, AstNode_readObject},
    {AstNode_isClass, AstNode_readClass},
    {AstNode_isTemplate, AstNode_readTemplate},
    {AstNode_isLambda, AstNode_readLambda},
    {AstNode_isFunction, AstNode_readFunction},
    {AstNode_isAssigment, AstNode_readAssigment},
    {AstNode_isBracket, AstNode_readBracket},
    {AstNode_isIdentifier, AstNode_readIdentifier},
    {AstNode_isLiteral, AstNode_readLiteral},
    {AstNode_isUnary, AstNode_readUnary},
    {AstNode_isPrefix, AstNode_readPrefix},
    {0, 0}};
static ProcessHandle operator_expression_handlers[] = {
    {AstNode_isUpdate, AstNode_readUpdate},
    {AstNode_isCalculate, AstNode_readCalculate},
    {AstNode_isCall, AstNode_readCall},
    {AstNode_isMember, AstNode_readMember},
    {AstNode_isCompute, AstNode_readCompute},
    {AstNode_isOptional, AstNode_readOptional},
    {0, 0}};

static AstNode AstNode_readExpression(SourceFile file, cstring source) {
  cstring selector = source;
  Token_enableReadRegex();
  for (;;) {
    Token token = Token_readSkipComment(file, selector);
    if (!token) {
      goto failed;
    }
    Bool isNewlineFlag = False;
    while (token->type == TT_Newline || token->type == TT_MultiLineComment) {
      selector = token->raw.end;
      Buffer_dispose(token);
      token = Token_readSkipNewline(file, selector);
      if (Expression_isComplete()) {
        if (Token_check(token, TT_Symbol, "++") ||
            Token_check(token, TT_Symbol, "--")) {
          Buffer_dispose(token);
          break;
        }
      }
      isNewlineFlag = True;
    }
    if (token->type == TT_Eof) {
      if (Expression_isComplete()) {
        Buffer_dispose(token);
        break;
      } else {
        Error_push("Unexcept token.",
                   SourceFile_getLocation(file, token->raw.begin));
        Buffer_dispose(token);
        goto failed;
      }
    }

    if (Token_check(token, TT_Symbol, ";")) {
      if (Expression_isComplete()) {
        selector = token->raw.end;
        Buffer_dispose(token);
        break;
      } else {
        Error_push("Unexcept token.",
                   SourceFile_getLocation(file, token->raw.begin));
        Buffer_dispose(token);
        goto failed;
      }
    }
    ProcessHandle *handlers = NULL;
    if (Expression_isComplete()) {
      handlers = operator_expression_handlers;
    } else {
      handlers = atom_expression_handlers;
    }
    int indexOfHandler = 0;
    for (;;) {
      ProcessHandle handler = handlers[indexOfHandler];
      if (!handler.checker) {
        if (Expression_isComplete() &&
            (isNewlineFlag || token->type == TT_Symbol ||
             token->type == TT_TemplatePart || token->type == TT_TemplateEnd)) {
          Buffer_dispose(token);
          goto final;
        } else {
          if (!token) {
            goto failed;
          }
          Error_push("Unexcept token.",
                     SourceFile_getLocation(file, token->raw.begin));
          Buffer_dispose(token);
          goto failed;
        }
      }
      if (handler.checker(file, token)) {
        Buffer_dispose(token);
        AstNode node = handler.handler(file, selector);
        if (!node) {
          goto failed;
        }
        Expression_insert(node);
        selector = node->position.end;
        break;
      }
      indexOfHandler++;
    }
    if (Expression_isComplete()) {
      Token_disableReadRegex();
    } else {
      Token_enableReadRegex();
    }
  }
final : {
  AstNode node = g_ectx->root;
  g_ectx->root = NULL;
  node->position.begin = source;
  node->position.end = selector;
  return node;
}
failed:
  if (g_ectx->root) {
    Buffer_dispose(g_ectx->root);
  }
  return NULL;
}
static Bool AstNode_isExpressionStatement(SourceFile file, Token token) {
  return True;
}
static AstNode AstNode_readExpressionStatement(SourceFile file,
                                               cstring source) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_ExpressionStatement;
  ExpressionContext ectx = ExpressionContext_push();
  AstNode expr = AstNode_readExpression(file, source);
  ExpressionContext_pop(ectx);
  if (!expr) {
    goto failed;
  }
  selector = expr->position.end;
  node->binary.left = expr;
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

static Bool AstNode_isBlockStatement(SourceFile file, Token token) {
  return Token_check(token, TT_Symbol, "{");
}
static AstNode AstNode_readBlockStatement(SourceFile file, cstring source) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_BlockStatement;
  node->s_block.statements = List_create(True);
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!Token_check(token, TT_Symbol, "}")) {
    Buffer_dispose(token);
    for (;;) {
      AstNode statement = AstNode_readStatement(file, selector);
      if (!statement) {
        goto failed;
      }
      List_insert_tail(node->s_block.statements, statement);
      selector = statement->position.end;
      Token token = Token_readSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (Token_check(token, TT_Symbol, "}") || token->type == TT_Eof) {
        break;
      } else {
        Buffer_dispose(token);
      }
    }
  }
  if (!Token_check(token, TT_Symbol, "}")) {
    Error_push("Unexcept token.missing token '}'",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

static Bool AstNode_isEmptyStatement(SourceFile file, cstring source) {
  Token token = Token_readSkipNewline(file, source);
  if (!token) {
    return False;
  }
  if (Token_check(token, TT_Symbol, ";")) {
    Buffer_dispose(token);
    return True;
  }
  Buffer_dispose(token);
  return False;
}

static AstNode AstNode_readEmptyStatement(SourceFile file, cstring source) {
  AstNode node = AstNode_create();
  return node;
}

static ProcessHandle statement_handlers[] = {
    {AstNode_isBlockStatement, AstNode_readBlockStatement},
    {AstNode_isExpressionStatement, AstNode_readExpressionStatement},
    {0, 0}};

static AstNode AstNode_readStatement(SourceFile file, cstring source) {
  int index = 0;
  Token token = Token_readSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  for (;;) {
    ProcessHandle handle = statement_handlers[index];
    if (!handle.checker) {
      Error_push("Unexcept token.",
                 SourceFile_getLocation(file, token->raw.begin));
      Buffer_dispose(token);
      return NULL;
    }
    if (handle.checker(file, token)) {
      Buffer_dispose(token);
      return handle.handler(file, source);
    }
    index++;
  }
}

static AstNode AstNode_readProgram(SourceFile file, cstring source) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_Program;
  node->s_program.body = List_create(True);

  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (token->type != TT_Eof) {
    Buffer_dispose(token);
    for (;;) {
      AstNode statement = AstNode_readStatement(file, selector);
      if (!statement) {
        goto failed;
      }
      List_insert_tail(node->s_program.body, statement);
      selector = statement->position.end;
      Token token = Token_readSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (token->type == TT_Eof) {
        Buffer_dispose(token);
        break;
      } else {
        Buffer_dispose(token);
      }
    }
  } else {
    Buffer_dispose(token);
  }
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_free(node);
  return NULL;
}

AstNode AstNode_read(SourceFile file, cstring source) {
  TokenContext tctx = Token_pushTokenContext();
  ExpressionContext ectx = ExpressionContext_push();
  AstNode node = AstNode_readProgram(file, source);
  ExpressionContext_pop(ectx);
  Token_popTokenContext(tctx);
  return node;
}