#include "compiler/astnode.h"
#include <stdarg.h>
#include <string.h>

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
  } else if (node->type == ANT_ForStatement) {
    Buffer_dispose(node->s_for.init);
    Buffer_dispose(node->s_for.condition);
    Buffer_dispose(node->s_for.update);
    Buffer_dispose(node->s_for.body);
  } else if (node->type == ANT_ForInStatement ||
             node->type == ANT_ForOfStatement) {
    Buffer_dispose(node->s_forIn.iterator);
    Buffer_dispose(node->s_forIn.object);
    Buffer_dispose(node->s_for.body);
  } else if (node->type == ANT_ObjectDestruct) {
    Buffer_dispose(node->e_objectPattern.properties);
  } else if (node->type == ANT_ObjectDestructProperty) {
    Buffer_dispose(node->e_oPatternProp.field);
    Buffer_dispose(node->e_oPatternProp.alias);
    Buffer_dispose(node->e_oPatternProp.init);
  } else if (node->type == ANT_ArrayDestruct) {
    Buffer_dispose(node->e_arrayPattern.items);
  } else if (node->type == ANT_ArrayDestructProperty) {
    Buffer_dispose(node->e_arrPatternProp.name);
    Buffer_dispose(node->e_arrPatternProp.init);
  } else {
    Buffer_dispose(node->e_binary.left);
    Buffer_dispose(node->e_binary.right);
    if (node->e_binary.flag) {
      Buffer_dispose(node->e_binary.flag);
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
  node->e_binary.left = 0;
  node->e_binary.right = 0;
  node->e_binary.flag = 0;
  return node;
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
      g_ectx->current->e_binary.right = expr;
    } else {
      if (g_ectx->root->level <= expr->level) {
        expr->e_binary.left = g_ectx->root;
        g_ectx->root = expr;
      } else {
        AstNode iterator = g_ectx->root;
        while (iterator->e_binary.right->level > expr->level) {
          iterator = iterator->e_binary.right;
        }
        expr->e_binary.left = iterator->e_binary.right;
        iterator->e_binary.right = expr;
      }
    }
  }

  if (expr->bind == BT_Right || expr->bind == BT_Both) {
    g_ectx->current = expr;
  } else {
    g_ectx->current = NULL;
  }
}

static AstNode JS_Compile_readExpression(SourceFile file, cstring source);
static AstNode JS_Compile_readStatement(SourceFile file, cstring source);
static AstNode JS_Compile_readBlockStatement(SourceFile file, cstring source);
CHECKER(JS_Compile_isLiteral) {
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
READER(JS_Compile_readLiteral) {
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
CHECKER(JS_Compile_isIdentifier) { return token->type == TT_Identifier; }

READER(JS_Compile_readIdentifier) {
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

CHECKER(JS_Compile_isCalculate) {
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

READER(JS_Compile_readCalculate) {
  Token token = Token_readSkipNewline(file, source);
  AstNode expr = AstNode_create();
  expr->e_binary.flag = cstring_from(token->raw);
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

CHECKER(JS_Compile_isUnary) {
  return Strings_contains(token->raw, unaryOperators);
}

READER(JS_Compile_readUnary) {
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
  node->e_binary.flag = cstring_from(token->raw);
  Buffer_dispose(token);
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

CHECKER(JS_Compile_isUpdate) {
  return Strings_contains(token->raw, updateOperators);
}

READER(JS_Compile_readUpdate) {
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
  node->e_binary.flag = cstring_from(token->raw);
  Buffer_dispose(token);
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

CHECKER(JS_Compile_isBracket) { return Token_check(token, TT_Symbol, "("); }
READER(JS_Compile_readBracket) {
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
  AstNode bracket = JS_Compile_readExpression(file, selector);
  ExpressionContext_pop(ectx);
  if (!bracket) {
    goto failed;
  }
  node->e_binary.left = bracket;
  selector = bracket->position.end;
  node->e_binary.flag = (cstring)cstring_toBuffer("()");
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

CHECKER(JS_Compile_isCall) { return Token_check(token, TT_Symbol, "("); }
READER(JS_Compile_readCall) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_Call;
  node->level = -2;
  node->bind = BT_Left;
  node->e_call.callee = NULL;
  node->e_call.args = List_create(True);
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
      AstNode arg = JS_Compile_readExpression(file, selector);
      ExpressionContext_pop(ectx);
      if (!arg) {
        goto failed;
      }
      List_insert_tail(node->e_call.args, arg);
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
CHECKER(JS_Compile_isMember) { return Token_check(token, TT_Symbol, "."); }
READER(JS_Compile_readMember) {
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
  node->e_binary.flag = cstring_from(token->raw);
  Buffer_dispose(token);
  node->position.begin = source;
  node->position.end = selector;
  return node;

failed:
  Buffer_dispose(node);
  return NULL;
}

CHECKER(JS_Compile_isCompute) { return Token_check(token, TT_Symbol, "["); }
READER(JS_Compile_readCompute) {
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
  AstNode key = JS_Compile_readExpression(file, selector);
  ExpressionContext_pop(ectx);
  if (!key) {
    goto failed;
  }
  selector = key->position.end;
  node->e_binary.right = key;
  node->e_binary.flag = cstring_toBuffer("compute");
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
CHECKER(JS_Compile_isOptional) { return Token_check(token, TT_Symbol, "?."); }
READER(JS_Compile_readOptional) {
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
    AstNode node = JS_Compile_readCall(file, selector);
    if (!node) {
      return NULL;
    }
    node->type = ANT_OptionalCall;
    return node;
  } else if (Token_check(token, TT_Symbol, "[")) {
    Buffer_dispose(token);
    AstNode node = JS_Compile_readCompute(file, selector);
    if (!node) {
      return NULL;
    }
    Buffer_dispose(node->e_binary.flag);
    node->e_binary.flag = cstring_toBuffer("optional-compute");
    return node;
  } else {
    Buffer_dispose(token);
    AstNode node = AstNode_create();
    node->type = ANT_Binary;
    node->level = -2;
    node->bind = BT_Both;
    node->e_binary.flag = cstring_toBuffer("?.");
    node->position.begin = source;
    node->position.end = selector;
    return node;
  }
  return NULL;
}
CHECKER(JS_Compile_isPrefix) {
  return Token_check(token, TT_Keyword, "await") ||
         Token_check(token, TT_Keyword, "new") ||
         Token_check(token, TT_Keyword, "delete");
}
READER(JS_Compile_readPrefix) {
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
  node->e_binary.flag = cstring_from(token->raw);
  Buffer_dispose(token);
  node->position.begin = source;
  node->position.end = selector;
  return node;

failed:
  Buffer_dispose(node);
  return NULL;
}
CHECKER(JS_Compile_isAssigment) {
  return Token_check(token, TT_Keyword, "const") ||
         Token_check(token, TT_Keyword, "let") ||
         Token_check(token, TT_Keyword, "var");
}
READER(JS_Compile_readAssigment) {
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
  node->e_binary.flag = cstring_from(token->raw);
  Buffer_dispose(token);
  node->position.begin = source;
  node->position.end = selector;
  return node;

failed:
  Buffer_dispose(node);
  return NULL;
}

CHECKER(JS_Compile_isFunction) {
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
READER(JS_Compile_readFunction) {
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
      AstNode arg = JS_Compile_readExpression(file, selector);
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
  node->e_function.body = JS_Compile_readBlockStatement(file, selector);
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

CHECKER(JS_Compile_isLambda) {
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

READER(JS_Compile_readLambda) {
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
    AstNode arg = JS_Compile_readIdentifier(file, selector);
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
        AstNode arg = JS_Compile_readExpression(file, selector);
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
    node->e_lambda.body = JS_Compile_readStatement(file, selector);
  } else {
    ExpressionContext ectx = ExpressionContext_push();
    g_ectx->maxLevel = 12;
    node->e_lambda.body = JS_Compile_readExpression(file, selector);
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

CHECKER(JS_Compile_isArray) { return Token_check(token, TT_Symbol, "["); }
READER(JS_Compile_readArray) {
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
      AstNode item = JS_Compile_readExpression(file, selector);
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
    node->e_oprop.key = JS_Compile_readCompute(file, selector);
  } else {
    Buffer_dispose(token);
    node->e_oprop.key = JS_Compile_readIdentifier(file, selector);
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
    node->e_oprop.field = JS_Compile_readExpression(file, selector);
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
        AstNode arg = JS_Compile_readExpression(file, selector);
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
    node->e_oprop.method.body = JS_Compile_readBlockStatement(file, selector);
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

CHECKER(JS_Compile_isObject) { return Token_check(token, TT_Symbol, "{"); }
READER(JS_Compile_readObject) {
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

READER(JS_Compile_readClassProperty) {
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
      AstNode dec = JS_Compile_readExpression(file, selector);
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
    node->e_cprop.key = JS_Compile_readIdentifier(file, selector);
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
    node->e_cprop.key = JS_Compile_readExpression(file, selector);
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
    node->e_cprop.field = JS_Compile_readExpression(file, selector);
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
        AstNode arg = JS_Compile_readExpression(file, selector);
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
    node->e_cprop.method.body = JS_Compile_readBlockStatement(file, selector);
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
CHECKER(JS_Compile_isClass) {
  return Token_check(token, TT_Symbol, "@") ||
         Token_check(token, TT_Keyword, "class");
}
READER(JS_Compile_readClass) {
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
      AstNode dec = JS_Compile_readExpression(file, selector);
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
    node->e_class.extends = JS_Compile_readExpression(file, selector);
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
        AstNode block = JS_Compile_readBlockStatement(file, selector);
        if (!block) {
          goto failed;
        }
        List_insert_tail(node->e_class.staticBlocks, block);
        selector = block->position.end;
      } else {
        Buffer_dispose(token);
        Buffer_dispose(next);
        AstNode property = JS_Compile_readClassProperty(file, selector);
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

CHECKER(JS_Compile_isTemplate) {
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
READER(JS_Compile_readTemplate) {
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
      AstNode arg = JS_Compile_readExpression(file, selector);
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

CHECKER(JS_Compile_isObjectDestruct) {
  if (Token_check(token, TT_Symbol, "{")) {
    Token pair =
        Token_pair(file, token->raw.begin, TT_Symbol, "{", TT_Symbol, "}");
    if (!pair) {
      return False;
    }
    cstring selector = pair->raw.end;
    Buffer_dispose(pair);
    Token next = Token_readSkipNewline(file, selector);
    if (!next) {
      return False;
    }
    if (Token_check(next, TT_Keyword, "in") ||
        Token_check(next, TT_Keyword, "of") ||
        Strings_contains(next->raw, operator_11)) {
      Buffer_dispose(next);
      return True;
    }
    Buffer_dispose(next);
  }
  return False;
}

READER(JS_Compile_readObjectDestructProp) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_ObjectDestructProperty;
  node->e_oPatternProp.init = NULL;
  node->e_oPatternProp.field = NULL;
  node->e_oPatternProp.alias = NULL;
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (token->type == TT_Identifier) {
    Buffer_dispose(token);
    ExpressionContext ectx = ExpressionContext_push();
    AstNode field = JS_Compile_readIdentifier(file, selector);
    ExpressionContext_pop(ectx);
    if (!node) {
      goto failed;
    }
    node->e_oPatternProp.field = field;
  } else if (Token_check(token, TT_Symbol, "[")) {
    Buffer_dispose(token);
    ExpressionContext ectx = ExpressionContext_push();
    AstNode field = JS_Compile_readCompute(file, selector);
    ExpressionContext_pop(ectx);
    if (!node) {
      goto failed;
    }
    node->e_oPatternProp.field = field;
  } else {
    Error_push("object properties destructuring pattern expected.",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  selector = node->e_oPatternProp.field->position.end;
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (Token_check(token, TT_Symbol, ":")) {
    selector = token->raw.end;
    Buffer_dispose(token);
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
    if (token->type != TT_Identifier) {
      Error_push("Identifier expected.",
                 SourceFile_getLocation(file, token->raw.begin));
      Buffer_dispose(token);
      goto failed;
    }
    node->e_oPatternProp.alias = token;
    selector = token->raw.end;
  } else {
    Buffer_dispose(token);
  }
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (Token_check(token, TT_Symbol, "=")) {
    selector = token->raw.end;
    Buffer_dispose(token);
    ExpressionContext ectx = ExpressionContext_push();
    g_ectx->maxLevel = 12;
    AstNode init = JS_Compile_readExpression(file, selector);
    ExpressionContext_pop(ectx);
    if (!init) {
      goto failed;
    }
    node->e_oPatternProp.init = init;
    selector = init->position.end;
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

READER(JS_Compile_readObjectDestruct) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_ObjectDestruct;
  node->e_objectPattern.properties = List_create(True);
  node->bind = BT_None;
  node->level = -2;
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
      if (Token_check(token, TT_Symbol, "}")) {
        break;
      }
      Buffer_dispose(token);
      AstNode prop = JS_Compile_readObjectDestructProp(file, selector);
      if (!prop) {
        goto failed;
      }
      List_insert_tail(node->e_objectPattern.properties, prop);
      selector = prop->position.end;
      token = Token_readSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (Token_check(token, TT_Symbol, ",")) {
        selector = token->raw.end;
      } else if (!Token_check(token, TT_Symbol, "}")) {
        Error_push("Unexcept token.missing token ','",
                   SourceFile_getLocation(file, token->raw.begin));
        Buffer_dispose(token);
        goto failed;
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

CHECKER(JS_Compile_isArrayDestruct) {
  if (Token_check(token, TT_Symbol, "[")) {
    Token pair =
        Token_pair(file, token->raw.begin, TT_Symbol, "[", TT_Symbol, "]");
    if (!pair) {
      return False;
    }
    cstring selector = pair->raw.end;
    Buffer_dispose(pair);
    Token next = Token_readSkipNewline(file, selector);
    if (!next) {
      return False;
    }
    if (Token_check(next, TT_Keyword, "in") ||
        Token_check(next, TT_Keyword, "of") ||
        Strings_contains(next->raw, operator_11)) {
      Buffer_dispose(next);
      return True;
    }
    Buffer_dispose(next);
  }
  return False;
}

READER(JS_Compile_readArrayDestructProperty) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_ArrayDestructProperty;
  node->e_arrPatternProp.init = NULL;
  node->e_arrPatternProp.name = NULL;
  Token token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (token->type != TT_Identifier) {
    Error_push("Array element destructuring pattern expected.",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  node->e_arrPatternProp.name = token;
  selector = token->raw.end;
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (Token_check(token, TT_Symbol, "=")) {
    selector = token->raw.end;
    Buffer_dispose(token);
    ExpressionContext ectx = ExpressionContext_push();
    g_ectx->maxLevel = 12;
    AstNode init = JS_Compile_readExpression(file, selector);
    ExpressionContext_pop(ectx);
    if (!init) {
      goto failed;
    }
    node->e_arrPatternProp.init = init;
    selector = init->position.end;
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

READER(JS_Compile_readArrayDestruct) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_ArrayDestruct;
  node->bind = BT_None;
  node->level = -2;
  node->e_arrayPattern.items = List_create(True);
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
      token = Token_readSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (Token_check(token, TT_Symbol, "]") || token->type == TT_Eof) {
        break;
      }
      Buffer_dispose(token);
      AstNode prop = JS_Compile_readArrayDestructProperty(file, selector);
      if (!prop) {
        goto failed;
      }
      List_insert_tail(node->e_arrayPattern.items, prop);
      selector = prop->position.end;
      token = Token_readSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (Token_check(token, TT_Symbol, ",")) {
        selector = token->raw.end;
      } else if (!Token_check(token, TT_Symbol, "]") && token->type != TT_Eof) {
        Error_push("Unexcept token.missing token ','",
                   SourceFile_getLocation(file, token->raw.begin));
        Buffer_dispose(token);
        goto failed;
      }
      Buffer_dispose(token);
    }
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

static ProcessHandle atom_expression_handlers[] = {
    {JS_Compile_isObjectDestruct, JS_Compile_readObjectDestruct},
    {JS_Compile_isArrayDestruct, JS_Compile_readArrayDestruct},
    {JS_Compile_isArray, JS_Compile_readArray},
    {JS_Compile_isObject, JS_Compile_readObject},
    {JS_Compile_isClass, JS_Compile_readClass},
    {JS_Compile_isTemplate, JS_Compile_readTemplate},
    {JS_Compile_isLambda, JS_Compile_readLambda},
    {JS_Compile_isFunction, JS_Compile_readFunction},
    {JS_Compile_isAssigment, JS_Compile_readAssigment},
    {JS_Compile_isBracket, JS_Compile_readBracket},
    {JS_Compile_isIdentifier, JS_Compile_readIdentifier},
    {JS_Compile_isLiteral, JS_Compile_readLiteral},
    {JS_Compile_isUnary, JS_Compile_readUnary},
    {JS_Compile_isPrefix, JS_Compile_readPrefix},
    {0, 0}};
static ProcessHandle operator_expression_handlers[] = {
    {JS_Compile_isUpdate, JS_Compile_readUpdate},
    {JS_Compile_isCalculate, JS_Compile_readCalculate},
    {JS_Compile_isCall, JS_Compile_readCall},
    {JS_Compile_isMember, JS_Compile_readMember},
    {JS_Compile_isCompute, JS_Compile_readCompute},
    {JS_Compile_isOptional, JS_Compile_readOptional},
    {0, 0}};

READER(JS_Compile_readExpression) {
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
CHECKER(JS_Compile_isExpressionStatement) { return True; }

READER(JS_Compile_readExpressionStatement) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_ExpressionStatement;
  ExpressionContext ectx = ExpressionContext_push();
  AstNode expr = JS_Compile_readExpression(file, source);
  ExpressionContext_pop(ectx);
  if (!expr) {
    goto failed;
  }
  selector = expr->position.end;
  node->e_binary.left = expr;
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

CHECKER(JS_Compile_isBlockStatement) {
  return Token_check(token, TT_Symbol, "{");
}

READER(JS_Compile_readBlockStatement) {
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
      AstNode statement = JS_Compile_readStatement(file, selector);
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

CHECKER(JS_Compile_isReturnStatement) {
  return Token_check(token, TT_Keyword, "return") ||
         Token_check(token, TT_Keyword, "yield");
}
READER(JS_Compile_readReturnStatement) {
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
    node->s_return = JS_Compile_readExpression(file, selector);
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
CHECKER(JS_Compile_isLabelStatement) {
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
READER(JS_Compile_readLabelStatement) {
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
  node->s_label.body = JS_Compile_readStatement(file, selector);
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
CHECKER(JS_Compile_isBreakStatement) {
  return Token_check(token, TT_Keyword, "break") ||
         Token_check(token, TT_Keyword, "continue");
}
READER(JS_Compile_readBreakStatement) {
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

CHECKER(JS_Compile_isAssigmentStatement) {
  return Token_check(token, TT_Keyword, "const") ||
         Token_check(token, TT_Keyword, "let") ||
         Token_check(token, TT_Keyword, "var");
}
READER(JS_Compile_readAssigement) {
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
  node->s_assigment.body = JS_Compile_readExpression(file, selector);
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

CHECKER(JS_Compile_isIf) { return Token_check(token, TT_Keyword, "if"); }
READER(JS_Compile_readIf) {
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
  node->s_if.condition = JS_Compile_readExpression(file, selector);
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
  node->s_if.consequent = JS_Compile_readStatement(file, selector);
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
    node->s_if.alternate = JS_Compile_readStatement(file, selector);
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

CHECKER(JS_Compile_isWith) { return Token_check(token, TT_Keyword, "with"); }
READER(JS_Compile_readWith) {
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
  node->s_with.obj = JS_Compile_readExpression(file, selector);
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
  node->s_with.body = JS_Compile_readBlockStatement(file, selector);
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

CHECKER(JS_Compile_isWhile) { return Token_check(token, TT_Keyword, "while"); }
READER(JS_Compile_readWhile) {
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
  node->s_while.condition = JS_Compile_readExpression(file, selector);
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

  node->s_while.body = JS_Compile_readBlockStatement(file, selector);
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

CHECKER(JS_Compile_isDoWhile) { return Token_check(token, TT_Keyword, "do"); }
READER(JS_Compile_readDoWhile) {
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
  node->s_doWhile.body = JS_Compile_readBlockStatement(file, selector);
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
  node->s_doWhile.condition = JS_Compile_readExpression(file, selector);
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

CHECKER(JS_Compile_isExport) {
  return Token_check(token, TT_Keyword, "export");
}

READER(JS_Compile_readExport) {
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
      AstNode item = JS_Compile_readIdentifier(file, selector);
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
    AstNode item = JS_Compile_readExpression(file, selector);
    ExpressionContext_pop(ectx);
    if (!item) {
      goto failed;
    }
    AstNode defaultNode = AstNode_create();
    defaultNode->type = ANT_DefaultExport;
    defaultNode->s_defualtExport.item = item;
    List_insert_tail(node->s_export.exports, defaultNode);
    selector = item->position.end;
  } else if (JS_Compile_isAssigment(file, token)) {
    Buffer_dispose(token);
    AstNode item = JS_Compile_readAssigment(file, selector);
    if (!item) {
      goto failed;
    }
    List_insert_tail(node->s_export.exports, item);
    selector = item->position.end;
  } else if (JS_Compile_isClass(file, token)) {
    Buffer_dispose(token);
    AstNode item = JS_Compile_readClass(file, selector);
    if (!item) {
      goto failed;
    }
    List_insert_tail(node->s_export.exports, item);
    selector = item->position.end;
  } else if (JS_Compile_isFunction(file, token)) {
    Buffer_dispose(token);
    AstNode item = JS_Compile_readFunction(file, selector);
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

READER(JS_Compile_readSwitchPattern) {
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
    node->s_switchPattern.condition = JS_Compile_readExpression(file, selector);
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
    AstNode item = JS_Compile_readStatement(file, selector);
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

CHECKER(JS_Compile_isSwitch) {
  return Token_check(token, TT_Keyword, "switch");
}
READER(JS_Compile_readSwitch) {
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
  node->s_switch.condition = JS_Compile_readExpression(file, selector);
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
    AstNode pattern = JS_Compile_readSwitchPattern(file, selector);
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

READER(JS_Compile_readNamespaceImport) {
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
READER(JS_Compile_readDefaultImport) {
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
READER(JS_Compile_readEntityImport) {
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
READER(JS_Compile_readImportAttribute) {
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
CHECKER(JS_Compile_isImport) {
  return Token_check(token, TT_Keyword, "import");
}
READER(JS_Compile_readImport) {
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
    AstNode ns = JS_Compile_readNamespaceImport(file, selector);
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
      AstNode def = JS_Compile_readDefaultImport(file, selector);
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
        AstNode e = JS_Compile_readEntityImport(file, selector);
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
      AstNode attr = JS_Compile_readImportAttribute(file, selector);
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
      Error_push("Unexcept token.missing token '}'",
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

CHECKER(JS_Compile_isFor) { return Token_check(token, TT_Keyword, "for"); }
READER(JS_Compile_readFor) {
  cstring selector = source;
  AstNode node = AstNode_create();
  node->type = ANT_ForStatement;
  node->s_for.init = NULL;
  node->s_for.condition = NULL;
  node->s_for.update = NULL;
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
  token = Token_readSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!Token_check(token, TT_Symbol, ";")) {
    Buffer_dispose(token);
    AstNode init = JS_Compile_readStatement(file, selector);
    if (!init) {
      goto failed;
    }
    selector = init->position.end;
    if (init->type == ANT_AssigmentStatement) {
      AstNode body = init->s_assigment.body;
      if (body->type == ANT_Binary && strcmp(body->e_binary.flag, "in") == 0) {
        node->s_forIn.type = init->s_assigment.type;
        node->s_forIn.iterator = body->e_binary.left;
        node->s_forIn.object = body->e_binary.right;
        body->e_binary.left = NULL;
        body->e_binary.right = NULL;
        Buffer_dispose(init);
        node->type = ANT_ForInStatement;
      } else if (body->type == ANT_Binary &&
                 strcmp(body->e_binary.flag, "of") == 0) {
        node->s_forIn.type = init->s_assigment.type;
        node->s_forIn.iterator = body->e_binary.left;
        node->s_forIn.object = body->e_binary.right;
        body->e_binary.left = NULL;
        body->e_binary.right = NULL;
        Buffer_dispose(init);
        node->type = ANT_ForOfStatement;
      }
    } else if (init->type == ANT_Binary &&
               strcmp(init->e_binary.flag, "in") == 0) {
      node->s_forIn.type = init->s_assigment.type;
      node->s_forIn.iterator = init->e_binary.left;
      node->s_forIn.object = init->e_binary.right;
      init->e_binary.left = NULL;
      init->e_binary.right = NULL;
      Buffer_dispose(init);
      node->type = ANT_ForInStatement;
    } else if (init->type == ANT_Binary &&
               strcmp(init->e_binary.flag, "of") == 0) {
      node->s_forIn.type = init->s_assigment.type;
      node->s_forIn.iterator = init->e_binary.left;
      node->s_forIn.object = init->e_binary.right;
      init->e_binary.left = NULL;
      init->e_binary.right = NULL;
      Buffer_dispose(init);
      node->type = ANT_ForOfStatement;
    }
    if (node->type == ANT_ForStatement) {
      node->s_for.init = init;
    }
    token = Token_readSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
  }
  if (node->type == ANT_ForStatement) {
    if (!Token_check(token, TT_Symbol, ";")) {
      Error_push("Unexcept token.missing token ';'",
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
    if (!Token_check(token, TT_Symbol, ";")) {
      Buffer_dispose(token);
      AstNode condition = JS_Compile_readExpression(file, selector);
      if (!condition) {
        goto failed;
      }
      node->s_for.condition = condition;
      selector = condition->position.end;
      token = Token_readSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
    }
    if (!Token_check(token, TT_Symbol, ";")) {
      Error_push("Unexcept token.missing token ';'",
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
      AstNode update = JS_Compile_readExpression(file, selector);
      if (!update) {
        goto failed;
      }
      node->s_for.update = update;
      selector = update->position.end;
      token = Token_readSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
    }
  }
  if (!Token_check(token, TT_Symbol, ")")) {
    Error_push("Unexcept token.missing token ';'",
               SourceFile_getLocation(file, token->raw.begin));
    Buffer_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Buffer_dispose(token);
  node->s_for.body = JS_Compile_readStatement(file, selector);
  if (!node->s_for.body) {
    goto failed;
  }
  selector = node->s_for.body->position.end;
  node->position.begin = source;
  node->position.end = selector;
  return node;
failed:
  Buffer_dispose(node);
  return NULL;
}

static ProcessHandle statement_handlers[] = {
    {JS_Compile_isFor, JS_Compile_readFor},
    {JS_Compile_isImport, JS_Compile_readImport},
    {JS_Compile_isSwitch, JS_Compile_readSwitch},
    {JS_Compile_isExport, JS_Compile_readExport},
    {JS_Compile_isDoWhile, JS_Compile_readDoWhile},
    {JS_Compile_isWhile, JS_Compile_readWhile},
    {JS_Compile_isWith, JS_Compile_readWith},
    {JS_Compile_isIf, JS_Compile_readIf},
    {JS_Compile_isAssigment, JS_Compile_readAssigement},
    {JS_Compile_isBreakStatement, JS_Compile_readBreakStatement},
    {JS_Compile_isLabelStatement, JS_Compile_readLabelStatement},
    {JS_Compile_isReturnStatement, JS_Compile_readReturnStatement},
    {JS_Compile_isBlockStatement, JS_Compile_readBlockStatement},
    {JS_Compile_isExpressionStatement, JS_Compile_readExpressionStatement},
    {0, 0}};

READER(JS_Compile_readStatement) {
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

READER(JS_Compile_readProgram) {
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
      AstNode statement = JS_Compile_readStatement(file, selector);
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

AstNode JS_Compile_read(SourceFile file, cstring source) {
  TokenContext tctx = Token_pushTokenContext();
  ExpressionContext ectx = ExpressionContext_push();
  AstNode node = JS_Compile_readProgram(file, source);
  ExpressionContext_pop(ectx);
  Token_popTokenContext(tctx);
  return node;
}