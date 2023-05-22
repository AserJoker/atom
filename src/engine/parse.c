#include "ast.h"
#include "json.h"
#include "list.h"
#include "source.h"
#include "strings.h"
#include <string.h>

typedef struct s_Context {
  Error error;
  int skipTailCheck;
  int commaTailCheck;
} Context;

static Context *ctx = NULL;

void AstNode_dispose(AstNode node) { Buffer_free(node); }

AstNode AstNode_create() {
  AstNode node = (AstNode)Buffer_alloc(sizeof(struct s_AstNode));
  return node;
}

Identifier Identifier_create() {
  Identifier identifier = (Identifier)Buffer_alloc(sizeof(struct s_Identifier));
  identifier->_node = AstNode_create();
  identifier->_raw = NULL;
  identifier->_node->_type = NT_Identifier;
  return identifier;
}

void Identifier_dispose(Identifier identifier) {
  AstNode_dispose(identifier->_node);
  if (identifier->_raw) {
    Token_dispose(identifier->_raw);
  }
  Buffer_free(identifier);
}

Literal Literal_create() {
  Literal literal = (Literal)Buffer_alloc(sizeof(struct s_Literal));
  literal->_node = AstNode_create();
  literal->_raw = NULL;
  return literal;
}

void Literal_dispose(Literal literal) {
  AstNode_dispose(literal->_node);
  if (literal->_raw) {
    Token_dispose(literal->_raw);
  }
  Buffer_free(literal);
}

void Statement_dispose(Statement statement) {
  if (statement->_node->_type == NT_ImportStatement) {
    ImportStatement_dispose((ImportStatement)statement);
  } else if (statement->_node->_type == NT_EmptyStatement) {
    EmptyStatement_dispose(statement);
  } else if (statement->_node->_type == NT_BlockStatement) {
    BlockStatement_dispose((BlockStatement)statement);
  } else if (statement->_node->_type == NT_ExpressionStatement) {
    ExpressionStatement_dispose((ExpressionStatement)statement);
  }
}

Literal readLiteral(SourceFile file, cstring source) {
  enableReadRegex();
  Token rawToken = readTokenSkipNewline(file, source);
  if (!rawToken) {
    return NULL;
  }
  Literal literal = Literal_create();
  switch (rawToken->_type) {
  case TT_Regex:
    literal->_node->_type = NT_RegExLiteral;
    literal->_raw = rawToken;
    break;
  case TT_String:
    literal->_node->_type = NT_StringLiteral;
    literal->_raw = rawToken;
    break;
  case TT_Number:
    literal->_node->_type = NT_NumericLiteral;
    literal->_raw = rawToken;
    break;
  case TT_BigInt:
    literal->_node->_type = NT_BigIntLiteral;
    literal->_raw = rawToken;
    break;
  case TT_Keyword:
    if (strings_is(rawToken->_raw, "true") ||
        strings_is(rawToken->_raw, "false")) {
      literal->_node->_type = NT_BooelanLiteral;
      literal->_raw = rawToken;
      break;
    } else if (strings_is(rawToken->_raw, "null")) {
      literal->_node->_type = NT_NullLiteral;
      literal->_raw = rawToken;
      break;
    } else if (strings_is(rawToken->_raw, "undefined")) {
      literal->_node->_type = NT_UndefinedLiteral;
      literal->_raw = rawToken;
      break;
    }
  default:
    Literal_dispose(literal);
    Token_dispose(rawToken);
    return NULL;
  }
  literal->_node->_position.begin = source;
  literal->_node->_position.end = literal->_raw->_raw.end;
  return literal;
}

Identifier readIdentifier(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  if (token->_type == TT_Identifier) {
    Identifier identifier = Identifier_create();
    identifier->_raw = token;
    identifier->_node->_position.begin = source;
    identifier->_node->_position.end = token->_raw.end;
    return identifier;
  }
  Token_dispose(token);
  return NULL;
}

Statement readStatement(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  if (token->_type == TT_Keyword) {
    if (strings_is(token->_raw, "import")) {
      Token_dispose(token);
      return (Statement)readImportStatement(file, source);
    }
  } else if (token->_type == TT_Symbol) {
    if (strings_is(token->_raw, ";")) {
      Token_dispose(token);
      return (Statement)readEmptyStatement(file, source);
    } else if (strings_is(token->_raw, "{")) {
      Token_dispose(token);
      return (Statement)readBlockStatement(file, source);
    }
  }
  Token_dispose(token);
  return (Statement)readExpressionStatement(file, source);
}

Context *pushContext() {
  Context *current = ctx;
  ctx = (Context *)Buffer_alloc(sizeof(Context));
  ctx->skipTailCheck = 0;
  ctx->commaTailCheck = 0;
  ctx->error.error = 0;
  return current;
}
void popContext(Context *current) {
  if (ctx) {
    if (current) {
      current->error = ctx->error;
    }
    Buffer_free(ctx);
  }
  ctx = current;
}

AstNode parse(SourceFile file) {
  cstring source = file->_source;
  initTokenizerContext();
  Context *current = pushContext();
  Program program = readProgram(file, source);
  if (program) {
    JSON_Value val = JSON_fromProgram(program);
    cstring source = JSON_stringlify(val);
    JSON_dispose(val);
    printf("%s", source);
    Buffer_free(source);
    Program_dispose(program);
  }
  uninitTokenizerContext();
  if (ctx->error.error) {
    fprintf(stderr, "%s at\n  %s:%u:%u", ctx->error.error,
            ctx->error.location._filename,
            ctx->error.location._position._line + 1,
            ctx->error.location._position._column + 1);
  }
  popContext(current);
  return NULL;
}

void setAstError(Error error) { ctx->error = error; }
Error getAstError() { return ctx->error; }
void enableTailCheck() { ctx->skipTailCheck = 0; }
void disableTailCheck() { ctx->skipTailCheck = 1; }
int isTailCheckEnable() { return !ctx->skipTailCheck; }
int isCommaTail() { return ctx->commaTailCheck; }
void enableCommaTail() { ctx->commaTailCheck = 1; }
void disableCommaTail() { ctx->commaTailCheck = 0; }