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
    Buffer_dispose(node->e_class.staticBlocks);
  } else if (node->type == ANT_ClassProperty) {
    Buffer_dispose(node->e_cprop.decorators);
    Buffer_dispose(node->e_cprop.key);
    if (node->e_cprop.type == CPT_Field) {
      Buffer_dispose(node->e_cprop.field);
    } else {
      Buffer_dispose(node->e_cprop.method.args);
      Buffer_dispose(node->e_cprop.method.body);
    }
  } else if (node->type == ANT_ReturnStatement ||
             node->type == ANT_YieldStatement) {
    Buffer_dispose(node->s_return);
  } else if (node->type == ANT_LabelStatement) {
    Buffer_dispose(node->s_label.label);
    Buffer_dispose(node->s_label.body);
  } else if (node->type == ANT_BreakStatement ||
             node->type == ANT_ContinueStatement) {
    Buffer_dispose(node->s_break.label);
  } else if (node->type == ANT_AssigmentStatement) {
    Buffer_dispose(node->s_assigment.body);
  } else if (node->type == ANT_IfStatement) {
    Buffer_dispose(node->s_if.condition);
    Buffer_dispose(node->s_if.consequent);
    Buffer_dispose(node->s_if.alternate);
  } else if (node->type == ANT_WithStatement) {
    Buffer_dispose(node->s_with.obj);
    Buffer_dispose(node->s_with.body);
  } else if (node->type == ANT_WhileStatement) {
    Buffer_dispose(node->s_while.condition);
    Buffer_dispose(node->s_while.body);
  } else if (node->type == ANT_ExportStatement) {
    Buffer_dispose(node->s_export.exports);
  } else if (node->type == ANT_DefaultExport) {
    Buffer_dispose(node->s_defualtExport.item);
  } else if (node->type == ANT_SwitchStatement) {
    Buffer_dispose(node->s_switch.condition);
    Buffer_dispose(node->s_switch.body);
  } else if (node->type == ANT_SwitchPattern) {
    Buffer_dispose(node->s_switchPattern.condition);
    Buffer_dispose(node->s_switchPattern.body);
  } else if (node->type == ANT_ImportStatement) {
    Buffer_dispose(node->s_import.imports);
    Buffer_dispose(node->s_import.attributes);
    Buffer_dispose(node->s_import.source);
  } else if (node->type == ANT_ImportPattern) {
    Buffer_dispose(node->s_importPattern.imported);
    Buffer_dispose(node->s_importPattern.local);
  } else if (node->type == ANT_ImportAttribute) {
    Buffer_dispose(node->s_importAttribute.key);
    Buffer_dispose(node->s_importAttribute.value);
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
  node->type = ANT_Reserved;
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
  if (Token_check(token, TT_Keyword, "function")) {
    return True;
  } else if (Token_check(token, TT_Keyword, "async")) {
    token = Token_readSkipNewline(file, token->raw.end);
    if (token && Token_check(token, TT_Keyword, "function")) {
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
  if (Token_check(token, TT_Keyword, "function")) {
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
  node->e_cprop.type = CPT_Field;
  node->e_cprop.isPrivate = False;
  node->e_cprop.isStatic = False;
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
  if (Token_check(token, TT_Keyword, "static")) {
    node->e_cprop.isStatic = True;
    selector = token->raw.end;
    Buffer_dispose(token);
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (Token_check(token, TT_Keyword, "async")) {
    node->e_cprop.method.async = True;
    node->e_cprop.type = CPT_Method;
    selector = token->raw.end;
    Buffer_dispose(token);
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (Token_check(token, TT_Keyword, "get")) {
    node->e_cprop.type = CPT_Getter;
    selector = token->raw.end;
    Buffer_dispose(token);
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }

  if (Token_check(token, TT_Keyword, "set")) {
    node->e_cprop.type = CPT_Setter;
    selector = token->raw.end;
    Buffer_dispose(token);
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (Token_check(token, TT_Symbol, "*")) {
    node->e_cprop.type = CPT_Method;
    node->e_cprop.method.generator = True;
    selector = token->raw.end;
    Buffer_dispose(token);
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (Token_check(token, TT_Symbol, "#")) {
    node->e_cprop.isPrivate = True;
    selector = token->raw.end;
    Buffer_dispose(token);
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (token->type == TT_Identifier) {
    Buffer_dispose(token);
    node->e_cprop.key = AstNode_readIdentifier(file, selector);
    if (!node->e_cprop.key) {
      goto failed;
    }
    selector = node->e_cprop.key->position.end;
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  } else if (Token_check(token, TT_Symbol, "[")) {
    selector = token->raw.end;
    Buffer_dispose(token);
    node->e_cprop.key = AstNode_readExpression(file, selector);
    if (!node->e_cprop.key) {
      goto failed;
    }
    selector = node->e_cprop.key->position.end;
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
    if (!Token_check(token, TT_Symbol, "]")) {
      Error_push("Unecept token.",
                 SourceFile_getLocation(file, token->raw.end));
      Buffer_dispose(token);
      goto failed;
    }
    selector = token->raw.end;
    Buffer_dispose(token);
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  } else {
    Error_push("Unecept token.", SourceFile_getLocation(file, token->raw.end));
    Buffer_dispose(token);
    goto failed;
  }
  if (Token_check(token, TT_Symbol, "=")) {
    selector = token->raw.end;
    if (node->e_cprop.type != CPT_Field) {
      Error_push("Unecept token.",
                 SourceFile_getLocation(file, token->raw.end));
      Buffer_dispose(token);
      goto failed;
    }
    Buffer_dispose(token);
    ExpressionContext ectx = ExpressionContext_push();
    node->e_cprop.field = AstNode_readExpression(file, selector);
    ExpressionContext_pop(ectx);
    if (!node->e_cprop.field) {
      goto failed;
    }
    selector = node->e_cprop.field->position.end;
  } else if (Token_check(token, TT_Symbol, "(")) {
    if (node->e_cprop.type == CPT_Field) {
      node->e_cprop.type = CPT_Method;
    }
    node->e_cprop.method.args = List_create(True);
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
        List_insert_tail(node->e_cprop.method.args, arg);
        selector = arg->position.end;
        token = Token_readSkipNewline(file, selector);
        if (!token) {
          goto failed;
        }
        if (Token_check(token, TT_Symbol, ")") || token->type == TT_Eof) {
          break;
        }
        if (Token_check(token, TT_Symbol, ",")) {
          selector = token->raw.end;
          Buffer_dispose(token);
        } else {
          Error_push("Unecept token.",
                     SourceFile_getLocation(file, token->raw.end));
          Buffer_dispose(token);
          goto failed;
        }
      }
    }

    if (!Token_check(token, TT_Symbol, ")")) {
      Error_push("Unecept token.",
                 SourceFile_getLocation(file, token->raw.end));
      Buffer_dispose(token);
      goto failed;
    }
    selector = token->raw.end;
    Buffer_dispose(token);
    node->e_cprop.method.body = AstNode_readBlockStatement(file, selector);
    if (!node->e_cprop.method.body) {
      goto failed;
    }
    selector = node->e_cprop.method.body->position.end;
  }
  node->position.begin = source;
  node->position.end = selector;
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
  node->e_class.staticBlocks = List_create(True);
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
      token = Token_readSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      while (Token_check(token, TT_Symbol, ";")) {
        selector = token->raw.end;
        Buffer_dispose(token);
        token = Token_readSkipNewline(file, selector);
        if (!token) {
          goto failed;
        }
      }
      if (Token_check(token, TT_Symbol, "}") || token->type == TT_Eof) {
        break;
      }
      Token next = Token_readSkipNewline(file, token->raw.end);
      if (!next) {
        Buffer_dispose(token);
        goto failed;
      }
      if (Token_check(token, TT_Keyword, "static") &&
          Token_check(next, TT_Symbol, "{")) {
        Buffer_dispose(next);
        selector = token->raw.end;
        Buffer_dispose(token);
        AstNode block = AstNode_readBlockStatement(file, selector);
        if (!block) {
          goto failed;
        }
        List_insert_tail(node->e_class.staticBlocks, block);
        selector = block->position.end;
      } else {
        Buffer_dispose(token);
        Buffer_dispose(next);
        AstNode property = AstNode_readClassProperty(file, selector);
        if (!property) {
          goto failed;
        }
        List_insert_tail(node->e_class.properties, property);
        selector = property->position.end;
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

READER(AstNode_readExpression) {
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
CHECKER(AstNode_isExpressionStatement) { return True; }

READER(AstNode_readExpressionStatement) {
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

CHECKER(AstNode_isBlockStatement) { return Token_check(token, TT_Symbol, "{"); }

READER(AstNode_readBlockStatement) {
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
      token = Token_readSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      while (Token_check(token, TT_Symbol, ";")) {
        selector = token->raw.end;
        Buffer_dispose(token);
        token = Token_readSkipNewline(file, selector);
        if (!token) {
          goto failed;
        }
      }
      if (Token_check(token, TT_Symbol, "}") || token->type == TT_Eof) {
        break;
      }
      Buffer_dispose(token);
      AstNode statement = AstNode_readStatement(file, selector);
      if (!statement) {
        goto failed;
      }
      List_insert_tail(node->s_block.statements, statement);
      selector = statement->position.end;
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

CHECKER(AstNode_isReturnStatement) {
  return Token_check(token, TT_Keyword, "return") ||
         Token_check(token, TT_Keyword, "yield");
}
READER(AstNode_readReturnStatement) {
  cstring selector = source;
  AstNode node = AstNode_create();
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (Token_check(token, TT_Keyword, "return")) {
    node->type = ANT_ReturnStatement;
  } else {
    node->type = ANT_YieldStatement;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  token = Token_readSkipComment(file, selector);
  if (token->type == TT_Newline || token->type == TT_MultiLineComment) {
    Buffer_dispose(token);
  } else {
    Buffer_dispose(token);
    ExpressionContext ectx = ExpressionContext_push();
    node->s_return = AstNode_readExpression(file, selector);
    ExpressionContext_pop(ectx);
    if (!node->s_return) {
      goto failed;
    }
    selector = node->s_return->position.end;
  }
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(token);
  return NULL;
}
CHECKER(AstNode_isLabelStatement) {
  if (token->type == TT_Identifier) {
    Token next = Token_readSkipNewline(file, token->raw.end);
    if (!next) {
      return False;
    }
    if (Token_check(next, TT_Symbol, ":")) {
      Buffer_dispose(next);
      return True;
    }
    Buffer_dispose(next);
  }
  return False;
}
READER(AstNode_readLabelStatement) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_LabelStatement;
  node->s_label.label = NULL;
  node->s_label.body = NULL;
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  node->s_label.label = token;
  selector = token->raw.end;
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  node->s_label.body = AstNode_readStatement(file, selector);
  if (!node->s_label.body) {
    goto failed;
  }
  selector = node->s_label.body->position.end;
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}
CHECKER(AstNode_isBreakStatement) {
  return Token_check(token, TT_Keyword, "break") ||
         Token_check(token, TT_Keyword, "continue");
}
READER(AstNode_readBreakStatement) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_BreakStatement;
  node->s_break.label = NULL;
  Token token = Token_readSkipNewline(file, selector);
  selector = token->raw.end;
  if (Token_check(token, TT_Keyword, "continue")) {
    node->type = ANT_ContinueStatement;
  }
  Buffer_dispose(token);
  token = Token_readSkipComment(file, selector);
  if (token->type == TT_Newline || token->type == TT_MultiLineComment ||
      Token_check(token, TT_Symbol, ";")) {
    Buffer_dispose(token);
  } else if (token->type == TT_Identifier) {
    node->s_break.label = token;
    selector = token->raw.end;
  } else {
    Error_push("Unexcept token.",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

CHECKER(AstNode_isAssigmentStatement) {
  return Token_check(token, TT_Keyword, "const") ||
         Token_check(token, TT_Keyword, "let") ||
         Token_check(token, TT_Keyword, "var");
}
READER(AstNode_readAssigement) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_AssigmentStatement;
  node->s_assigment.type = AT_Var;
  node->s_assigment.body = NULL;
  Token token = Token_readSkipNewline(file, selector);
  if (Token_check(token, TT_Keyword, "const")) {
    node->s_assigment.type = AT_Const;
  } else if (Token_check(token, TT_Keyword, "let")) {
    node->s_assigment.type = AT_Let;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  node->s_assigment.body = AstNode_readExpression(file, selector);
  if (!node->s_assigment.body) {
    goto failed;
  }
  selector = node->s_assigment.body->position.end;
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

CHECKER(AstNode_isIf) { return Token_check(token, TT_Keyword, "if"); }
READER(AstNode_readIf) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_IfStatement;
  node->s_if.condition = NULL;
  node->s_if.consequent = NULL;
  node->s_if.alternate = NULL;
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
  if (!Token_check(token, TT_Symbol, "(")) {
    Error_push("Unexcept token.missing token '('",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  ExpressionContext ectx = ExpressionContext_push();
  node->s_if.condition = AstNode_readExpression(file, selector);
  ExpressionContext_pop(ectx);
  if (!node->s_if.condition) {
    goto failed;
  }
  selector = node->s_if.condition->position.end;
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
  node->s_if.consequent = AstNode_readStatement(file, selector);
  if (!node->s_if.consequent) {
    goto failed;
  }
  selector = node->s_if.consequent->position.end;
  if (*(selector - 1) != '}') {
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
    if (!Token_check(token, TT_Symbol, ";")) {
      Error_push("Unexcept token.missing token ';'",
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
  if (Token_check(token, TT_Keyword, "else")) {
    selector = token->raw.end;
    Buffer_dispose(token);
    node->s_if.alternate = AstNode_readStatement(file, selector);
    if (!node->s_if.alternate) {
      goto failed;
    }
  } else {
    Buffer_dispose(token);
  }
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

CHECKER(AstNode_isWith) { return Token_check(token, TT_Keyword, "with"); }
READER(AstNode_readWith) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_WithStatement;
  node->s_with.obj = NULL;
  node->s_with.body = NULL;
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
  if (!Token_check(token, TT_Symbol, "(")) {
    Error_push("Unexcept token.missing token '('",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  ExpressionContext ectx = ExpressionContext_push();
  node->s_with.obj = AstNode_readExpression(file, selector);
  ExpressionContext_pop(ectx);
  if (!node->s_with.obj) {
    goto failed;
  }
  selector = node->s_with.obj->position.end;

  Buffer_dispose(token);
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
  node->s_with.body = AstNode_readBlockStatement(file, selector);
  if (!node->s_with.body) {
    goto failed;
  }
  selector = node->s_with.body->position.end;
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

CHECKER(AstNode_isWhile) { return Token_check(token, TT_Keyword, "while"); }
READER(AstNode_readWhile) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_WhileStatement;
  node->s_while.condition = NULL;
  node->s_while.body = NULL;

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
  if (!Token_check(token, TT_Symbol, "(")) {
    Error_push("Unexcept token.missing token '('",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  ExpressionContext ectx = ExpressionContext_push();
  node->s_while.condition = AstNode_readExpression(file, selector);
  ExpressionContext_pop(ectx);
  if (!node->s_while.condition) {
    goto failed;
  }
  selector = node->s_while.condition->position.end;

  Buffer_dispose(token);
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

  node->s_while.body = AstNode_readBlockStatement(file, selector);
  if (!node->s_while.body) {
    goto failed;
  }
  selector = node->s_while.body->position.end;
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

CHECKER(AstNode_isDoWhile) { return Token_check(token, TT_Keyword, "do"); }
READER(AstNode_readDoWhile) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_DoWhileStatement;
  node->s_doWhile.condition = NULL;
  node->s_doWhile.body = NULL;
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  node->s_doWhile.body = AstNode_readBlockStatement(file, selector);
  if (!node->s_doWhile.body) {
    goto failed;
  }
  selector = node->s_doWhile.body->position.end;
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!Token_check(token, TT_Keyword, "while")) {
    Error_push("Unexcept token.missing token 'while'",
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
  if (!Token_check(token, TT_Symbol, "(")) {
    Error_push("Unexcept token.missing token '('",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  ExpressionContext ectx = ExpressionContext_push();
  node->s_doWhile.condition = AstNode_readExpression(file, selector);
  ExpressionContext_pop(ectx);
  if (!node->s_doWhile.condition) {
    goto failed;
  }
  selector = node->s_doWhile.condition->position.end;
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
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (Token_check(token, TT_Symbol, ";")) {
    selector = token->raw.end;
  }
  Buffer_dispose(token);
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

CHECKER(AstNode_isExport) { return Token_check(token, TT_Keyword, "export"); }

READER(AstNode_readExport) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_ExportStatement;
  node->s_export.exports = List_create(True);
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
  if (Token_check(token, TT_Symbol, "{")) {
    selector = token->raw.end;
    Buffer_dispose(token);
    for (;;) {
      AstNode item = AstNode_readIdentifier(file, selector);
      if (!item) {
        goto failed;
      }
      List_insert_tail(node->s_export.exports, item);
      selector = item->position.end;
      token = Token_readSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (Token_check(token, TT_Symbol, ",")) {
        selector = token->raw.end;
        Buffer_dispose(token);
      } else if (Token_check(token, TT_Symbol, "}")) {
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
  } else if (Token_check(token, TT_Keyword, "default")) {
    selector = token->raw.end;
    Buffer_dispose(token);
    ExpressionContext ectx = ExpressionContext_push();
    AstNode item = AstNode_readExpression(file, selector);
    ExpressionContext_pop(ectx);
    if (!item) {
      goto failed;
    }
    AstNode defaultNode = AstNode_create();
    defaultNode->type = ANT_DefaultExport;
    defaultNode->s_defualtExport.item = item;
    List_insert_tail(node->s_export.exports, defaultNode);
    selector = item->position.end;
  } else if (AstNode_isAssigment(file, token)) {
    Buffer_dispose(token);
    AstNode item = AstNode_readAssigment(file, selector);
    if (!item) {
      goto failed;
    }
    List_insert_tail(node->s_export.exports, item);
    selector = item->position.end;
  } else if (AstNode_isClass(file, token)) {
    Buffer_dispose(token);
    AstNode item = AstNode_readClass(file, selector);
    if (!item) {
      goto failed;
    }
    List_insert_tail(node->s_export.exports, item);
    selector = item->position.end;
  } else if (AstNode_isFunction(file, token)) {
    Buffer_dispose(token);
    AstNode item = AstNode_readFunction(file, selector);
    if (!item) {
      goto failed;
    }
    List_insert_tail(node->s_export.exports, item);
    selector = item->position.end;
  } else {
    Error_push("Unexcept token.",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

READER(AstNode_readSwitchPattern) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_SwitchPattern;
  node->s_switchPattern.condition = NULL;
  node->s_switchPattern.body = List_create(True);
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (Token_check(token, TT_Keyword, "case")) {
    selector = token->raw.end;
    Buffer_dispose(token);
    ExpressionContext ectx = ExpressionContext_push();
    node->s_switchPattern.condition = AstNode_readExpression(file, selector);
    ExpressionContext_pop(ectx);
    if (!node->s_switchPattern.condition) {
      goto failed;
    }
    selector = node->s_switchPattern.condition->position.end;
  } else if (!Token_check(token, TT_Keyword, "default")) {
    Error_push("Unexcept token.",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  } else {
    selector = token->raw.end;
    Buffer_dispose(token);
  }
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!Token_check(token, TT_Symbol, ":")) {
    Error_push("Unexcept token.missing token ':'",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  for (;;) {
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
    while (Token_check(token, TT_Symbol, ";")) {
      selector = token->raw.end;
      Buffer_dispose(token);
      token = Token_readSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
    }
    if (Token_check(token, TT_Keyword, "case") ||
        Token_check(token, TT_Keyword, "default") ||
        Token_check(token, TT_Symbol, "}")) {
      Buffer_dispose(token);
      break;
    } else {
      Buffer_dispose(token);
    }
    AstNode item = AstNode_readStatement(file, selector);
    if (!item) {
      goto failed;
    }
    List_insert_tail(node->s_switchPattern.body, item);
    selector = item->position.end;
  }
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

CHECKER(AstNode_isSwitch) { return Token_check(token, TT_Keyword, "switch"); }
READER(AstNode_readSwitch) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_SwitchStatement;
  node->s_switch.condition = NULL;
  node->s_switch.body = List_create(True);
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
  if (!Token_check(token, TT_Symbol, "(")) {
    Error_push("Unexcept token.missing token '('",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  ExpressionContext ectx = ExpressionContext_push();
  node->s_switch.condition = AstNode_readExpression(file, selector);
  ExpressionContext_pop(ectx);
  if (!node->s_switch.condition) {
    goto failed;
  }
  selector = node->s_switch.condition->position.end;
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
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!Token_check(token, TT_Symbol, "{")) {
    Error_push("Unexcept token.missing token '{'",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  for (;;) {
    AstNode pattern = AstNode_readSwitchPattern(file, selector);
    if (!pattern) {
      goto failed;
    }
    List_insert_tail(node->s_switch.body, pattern);
    selector = pattern->position.end;
    Token token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
    if (Token_check(token, TT_Symbol, "}")) {
      Buffer_dispose(token);
      break;
    }
    Buffer_dispose(token);
  }
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
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

READER(AstNode_readNamespaceImport) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_ImportPattern;
  node->s_importPattern.type = IPT_Namespace;
  node->s_importPattern.local = NULL;
  node->s_importPattern.imported = NULL;
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
  if (!Token_check(token, TT_Keyword, "as")) {
    Error_push("Unexcept token.missing token 'as'",
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
  if (token->type != TT_Identifier) {
    Error_push("Unexcept token.",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  node->s_importPattern.local = token;
  selector = token->raw.end;
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}
READER(AstNode_readDefaultImport) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_ImportPattern;
  node->s_importPattern.type = IPT_Default;
  node->s_importPattern.imported = NULL;
  node->s_importPattern.local = NULL;
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  node->s_importPattern.local = token;
  selector = token->raw.end;
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}
READER(AstNode_readEntityImport) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_ImportPattern;
  node->s_importPattern.type = IPT_Entity;
  node->s_importPattern.imported = NULL;
  node->s_importPattern.local = NULL;
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  node->s_importPattern.imported = token;
  selector = token->raw.end;
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (Token_check(token, TT_Keyword, "as")) {
    selector = token->raw.end;
    Buffer_dispose(token);
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
    if (token->type != TT_Identifier) {
      Error_push("Unexcept token.",
                 SourceFile_getLocation(file, token->raw.begin));
      Buffer_dispose(token);
      goto failed;
    }
    node->s_importPattern.local = token;
    selector = token->raw.end;
  } else {
    Buffer_dispose(token);
  }
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}
READER(AstNode_readImportAttribute) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_ImportAttribute;
  node->s_importAttribute.key = NULL;
  node->s_importAttribute.value = NULL;
  node->position.begin = source;
  node->position.end = selector;
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (token->type != TT_Identifier) {
    Error_push("Unexcept token.",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  node->s_importAttribute.key = token;
  selector = token->raw.end;
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!Token_check(token, TT_Symbol, ":")) {
    Error_push("Unexcept token.",
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
  if (token->type != TT_String) {
    Error_push("Unexcept token.",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  node->s_importAttribute.value = token;
  selector = token->raw.end;
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}
CHECKER(AstNode_isImport) { return Token_check(token, TT_Keyword, "import"); }
READER(AstNode_readImport) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_ImportStatement;
  node->s_import.source = NULL;
  node->s_import.imports = List_create(True);
  node->s_import.attributes = List_create(True);
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
  if (Token_check(token, TT_Symbol, "*")) {
    Buffer_dispose(token);
    AstNode ns = AstNode_readNamespaceImport(file, selector);
    if (!ns) {
      goto failed;
    }
    List_insert_tail(node->s_import.imports, ns);
    selector = ns->position.end;
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  } else {
    if (token->type == TT_Identifier) {
      Buffer_dispose(token);
      AstNode def = AstNode_readDefaultImport(file, selector);
      if (!def) {
        goto failed;
      }
      List_insert_tail(node->s_import.imports, def);
      selector = def->position.end;
      token = Token_readSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
    }
    if (Token_check(token, TT_Symbol, ",")) {
      selector = token->raw.end;
      Buffer_dispose(token);
      token = Token_readSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
    }
    if (Token_check(token, TT_Symbol, "{")) {
      selector = token->raw.end;
      Buffer_dispose(token);
      for (;;) {
        token = Token_readSkipNewline(file, selector);
        if (!token) {
          goto failed;
        }
        if (Token_check(token, TT_Symbol, "}") || token->type == TT_Eof) {
          break;
        }
        if (token->type != TT_Identifier) {
          Error_push("Unexcept token.",
                     SourceFile_getLocation(file, token->raw.begin));
          Buffer_dispose(token);
          goto failed;
        }
        Buffer_dispose(token);
        AstNode e = AstNode_readEntityImport(file, selector);
        if (!e) {
          goto failed;
        }
        selector = e->position.end;
        List_insert_tail(node->s_import.imports, e);
        token = Token_readSkipNewline(file, selector);
        if (!token) {
          goto failed;
        }
        if (Token_check(token, TT_Symbol, ",")) {
          selector = token->raw.end;
          Buffer_dispose(token);
          token = Token_readSkipNewline(file, selector);
          if (!token) {
            goto failed;
          }
        }
        Buffer_dispose(token);
      }
      if (!Token_check(token, TT_Symbol, "}")) {
        Error_push("Unexcept token.",
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
    }
  }

  if (!List_empty(node->s_import.imports)) {
    if (!Token_check(token, TT_Keyword, "from")) {
      Error_push("Unexcept token.",
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
  }
  if (token->type != TT_String) {
    Error_push("Unexcept token.",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  node->s_import.source = token;
  selector = token->raw.end;
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (Token_check(token, TT_Keyword, "assert") ||
      Token_check(token, TT_Keyword, "with")) {
    selector = token->raw.end;
    Buffer_dispose(token);

    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
    if (!Token_check(token, TT_Symbol, "{")) {
      Error_push("Unexcept token.",
                 SourceFile_getLocation(file, token->raw.begin));
      Buffer_dispose(token);
      goto failed;
    }
    selector = token->raw.end;
    Buffer_dispose(token);
    for (;;) {
      token = Token_readSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (Token_check(token, TT_Symbol, "}") || token->type == TT_Eof) {
        break;
      } else {
        Buffer_dispose(token);
      }
      AstNode attr = AstNode_readImportAttribute(file, selector);
      if (!attr) {
        goto failed;
      }
      selector = attr->position.end;
      List_insert_tail(node->s_import.attributes, attr);
      token = Token_readSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (Token_check(token, TT_Symbol, ",")) {
        selector = token->raw.end;
      }
      Buffer_dispose(token);
    }
    if (!Token_check(token, TT_Symbol, "}")) {
      Error_push("Unexcept token.",
                 SourceFile_getLocation(file, token->raw.begin));
      Buffer_dispose(token);
      goto failed;
    } else {
      selector = token->raw.end;
      Buffer_dispose(token);
    }
  } else {
    Buffer_dispose(token);
  }
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

static ProcessHandle statement_handlers[] = {
    {AstNode_isImport, AstNode_readImport},
    {AstNode_isSwitch, AstNode_readSwitch},
    {AstNode_isExport, AstNode_readExport},
    {AstNode_isDoWhile, AstNode_readDoWhile},
    {AstNode_isWhile, AstNode_readWhile},
    {AstNode_isWith, AstNode_readWith},
    {AstNode_isIf, AstNode_readIf},
    {AstNode_isAssigment, AstNode_readAssigement},
    {AstNode_isBreakStatement, AstNode_readBreakStatement},
    {AstNode_isLabelStatement, AstNode_readLabelStatement},
    {AstNode_isReturnStatement, AstNode_readReturnStatement},
    {AstNode_isBlockStatement, AstNode_readBlockStatement},
    {AstNode_isExpressionStatement, AstNode_readExpressionStatement},
    {0, 0}};

READER(AstNode_readStatement) {
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

READER(AstNode_readProgram) {
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
      token = Token_readSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      while (Token_check(token, TT_Symbol, ";")) {
        selector = token->raw.end;
        Buffer_dispose(token);
        token = Token_readSkipNewline(file, selector);
        if (!token) {
          goto failed;
        }
      }
      if (token->type == TT_Eof) {
        Buffer_dispose(token);
        break;
      }
      Buffer_dispose(token);
      AstNode statement = AstNode_readStatement(file, selector);
      if (!statement) {
        goto failed;
      }
      List_insert_tail(node->s_program.body, statement);
      selector = statement->position.end;
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