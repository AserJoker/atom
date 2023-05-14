#include "ast.h"
#include "json.h"
#include "list.h"
#include "source.h"
#include "strings.h"
#include <string.h>

typedef struct s_Context {
  Error error;
} Context;

static Context ctx;

Token readTokenSkipComment(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readToken(file, selector);
  while (token && token->_type == TT_Comment) {
    selector = token->_raw.end;
    Token_dispose(token);
    Token token = readToken(file, selector);
  }
  if (!token) {
    ctx.error = getTokenizerError();
  }
  return token;
}

Token readTokenSkipNewline(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipComment(file, selector);
  while (token && token->_type == TT_Newline) {
    selector = token->_raw.end;
    Token_dispose(token);
    Token token = readTokenSkipComment(file, selector);
  }
  if (!token) {
    ctx.error = getTokenizerError();
  }
  return token;
}

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

ImportSpecifier ImportSpecifier_create() {
  ImportSpecifier import_spec =
      (ImportSpecifier)Buffer_alloc(sizeof(struct s_ImportSpecifier));
  import_spec->_node = AstNode_create();
  import_spec->_imported = NULL;
  import_spec->_local = NULL;
  import_spec->_node->_type = NT_ImportSpecifier;
  return import_spec;
}

void ImportSpecifier_dispose(ImportSpecifier import_spec) {
  AstNode_dispose(import_spec->_node);
  if (import_spec->_imported) {
    Identifier_dispose(import_spec->_imported);
  }
  if (import_spec->_local) {
    Identifier_dispose(import_spec->_local);
  }
  Buffer_free(import_spec);
}

ImportAttribute ImportAttribute_create() {
  ImportAttribute import_attr =
      (ImportAttribute)Buffer_alloc(sizeof(struct s_ImportAttribute));
  import_attr->_node = AstNode_create();
  import_attr->_key = NULL;
  import_attr->_value = NULL;
  import_attr->_node->_type = NT_ImportAttribute;
  return import_attr;
}

void ImportAttribute_dispose(ImportAttribute import_attr) {
  AstNode_dispose(import_attr->_node);
  if (import_attr->_value) {
    Literal_dispose(import_attr->_value);
  }
  if (import_attr->_key) {
    Identifier_dispose(import_attr->_key);
  }
  Buffer_free(import_attr);
}

ImportStatement ImportStatement_create() {
  List_Option assertions_opt = {1, (Buffer_Free)ImportAttribute_dispose};
  List_Option specifier_opt = {1, (Buffer_Free)ImportSpecifier_dispose};
  ImportStatement import_s =
      (ImportStatement)Buffer_alloc(sizeof(struct s_ImportStatement));
  import_s->_node = AstNode_create();
  import_s->_node->_type = NT_ImportStatement;
  import_s->_assertions = List_create(assertions_opt);
  import_s->_specifiers = List_create(specifier_opt);
  import_s->_source = NULL;
  return import_s;
}

void ImportStatement_dispose(ImportStatement import_s) {
  List_dispose(import_s->_assertions);
  List_dispose(import_s->_specifiers);
  AstNode_dispose(import_s->_node);
  if (import_s->_source) {
    Literal_dispose(import_s->_source);
  }
  Buffer_free(import_s);
}

void Statement_dispose(Statement statement) {
  if (statement->_node->_type == NT_ImportStatement) {
    ImportStatement_dispose((ImportStatement)statement);
  }
}

Directive Directive_create() {
  Directive directive = (Directive)Buffer_alloc(sizeof(struct s_Directive));
  directive->_node = AstNode_create();
  directive->_node->_type = NT_Directive;
  return directive;
}
void Directive_dispose(Directive directive) {
  AstNode_dispose(directive->_node);
  Buffer_free(directive);
}
Interpreter Interpreter_create() {
  Interpreter interpreter =
      (Interpreter)Buffer_alloc(sizeof(struct s_Interpreter));
  interpreter->_node = AstNode_create();
  interpreter->_node->_type = NT_Interpreter;
  return interpreter;
}

void Interpreter_dispose(Interpreter interpreter) {
  AstNode_dispose(interpreter->_node);
  Buffer_free(interpreter);
}

Program Program_create() {
  Program program = (Program)Buffer_alloc(sizeof(struct s_Program));
  List_Option bopt = {1, (Buffer_Free)(Statement_dispose)};
  program->_body = List_create(bopt);
  List_Option dopt = {1, (Buffer_Free)Directive_dispose};
  program->_directives = List_create(dopt);
  program->_interpreter = NULL;
  program->_node = AstNode_create();
  program->_node->_type = NT_Program;
  return program;
}

void Program_dispose(Program program) {
  AstNode_dispose(program->_node);
  List_dispose(program->_directives);
  List_dispose(program->_body);
  if (program->_interpreter) {
    Interpreter_dispose(program->_interpreter);
  }
  Buffer_free(program);
}

Literal readLiteral(SourceFile file, cstring source) {
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

ImportSpecifier readImportDefaultSpecifier(SourceFile file, cstring source) {
  ImportSpecifier import_spec = ImportSpecifier_create();
  import_spec->_local = readIdentifier(file, source);
  import_spec->_node->_type = NT_ImportDefaultSpecifier;
  import_spec->_node->_position.begin = source;
  import_spec->_node->_position.end = import_spec->_local->_raw->_raw.end;
  return import_spec;
}

ImportSpecifier readImportSpecifier(SourceFile file, cstring source) {
  cstring selector = source;
  Identifier identify = readIdentifier(file, selector);
  if (!identify) {
    return NULL;
  }
  ImportSpecifier import_spec = ImportSpecifier_create();
  import_spec->_local = identify;
  selector = identify->_raw->_raw.end;
  Token token = readTokenSkipComment(file, selector);
  if (!token) {
    ImportSpecifier_dispose(import_spec);
    return NULL;
  }
  if (token->_type == TT_Keyword && strings_is(token->_raw, "as")) {
    selector = token->_raw.end;
    Token_dispose(token);
    identify = readIdentifier(file, selector);
    if (!identify) {
      ctx.error.error = "Unexpected token";
      ctx.error.location = getLocation(file, selector);
      ImportSpecifier_dispose(import_spec);
      return NULL;
    }
    import_spec->_imported = import_spec->_local;
    import_spec->_imported = identify;
    selector = identify->_raw->_raw.end;
  } else {
    Token_dispose(token);
  }
  import_spec->_node->_position.begin = source;
  import_spec->_node->_position.end = selector;
  import_spec->_node->_type = NT_ImportSpecifier;
  return import_spec;
}

ImportStatement readImportStatement(SourceFile file, cstring source) {
  Token keyword = readTokenSkipNewline(file, source);
  if (!keyword) {
    return NULL;
  }
  cstring selector = keyword->_raw.end;
  Token_dispose(keyword);
  ImportStatement import_s = ImportStatement_create();
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (token->_type == TT_Identifier) {
    Token_dispose(token);
    ImportSpecifier specifier = readImportDefaultSpecifier(file, selector);
    if (!specifier) {
      goto failed;
    }
    selector = specifier->_node->_position.end;
    List_insert_tail(import_s->_specifiers, specifier);
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
    if (token->_type == TT_Symbol && strings_is(token->_raw, ",")) {
      selector = token->_raw.end;
      Token_dispose(token);
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (token->_type != TT_Symbol || !strings_is(token->_raw, "{")) {
        ctx.error.error = "Unexpected token";
        ctx.error.location = getLocation(file, selector);
      } else {
        selector = token->_raw.end;
        specifier = readImportSpecifier(file, selector);
        while (specifier) {
          List_insert_tail(import_s->_specifiers, specifier);
          selector = specifier->_node->_position.end;
          Token_dispose(token);
          token = readTokenSkipNewline(file, selector);
          if (!token) {
            goto failed;
          }
          if (token->_type != TT_Symbol || !strings_is(token->_raw, ",")) {
            break;
          }
          selector = token->_raw.end;
          specifier = readImportSpecifier(file, selector);
        }
        Token_dispose(token);
        if (ctx.error.error) {
          goto failed;
        }
        token = readTokenSkipNewline(file, selector);
        if (!token) {
          goto failed;
        }
        if (token->_type != TT_Symbol || !strings_is(token->_raw, "}")) {
          ctx.error.error = "Unexpected token";
          ctx.error.location = getLocation(file, selector);
        } else {
          selector = token->_raw.end;
        }
      }
    }
  }
  Token_dispose(token);
  if (ctx.error.error != NULL) {
    goto failed;
  }
  if (List_size(import_s->_specifiers)) {
    keyword = readTokenSkipNewline(file, selector);
    if (!keyword) {
      goto failed;
    }
    if (keyword->_type != TT_Keyword || !strings_is(keyword->_raw, "from")) {
      ImportStatement_dispose(import_s);
      Token_dispose(keyword);
      ctx.error.error = "Unexpected token, expected \"from\"";
      ctx.error.location = getLocation(file, selector);
      return NULL;
    }
    selector = keyword->_raw.end;
    Token_dispose(keyword);
  }
  import_s->_source = readLiteral(file, selector);
  if (ctx.error.error) {
    goto failed;
  }
  if (!import_s->_source || import_s->_source->_raw->_type != TT_String) {
    ImportStatement_dispose(import_s);
    ctx.error.error = "Unexpected token";
    ctx.error.location = getLocation(file, selector);
    return NULL;
  }
  selector = import_s->_source->_node->_position.end;
  keyword = readTokenSkipNewline(file, selector);
  if (!keyword) {
    goto failed;
  }
  if (keyword->_type == TT_Symbol && strings_is(keyword->_raw, ";")) {
    import_s->_node->_position.begin = source;
    import_s->_node->_position.end = keyword->_raw.end;
    Token_dispose(keyword);
    return import_s;
  }
  Token_dispose(keyword);
  import_s->_node->_position.begin = source;
  import_s->_node->_position.end = selector;
  return import_s;
failed:
  if (import_s) {
    ImportStatement_dispose(import_s);
  }
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
  }
  Token_dispose(token);
  return NULL;
}

Interpreter readInterpreter(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipComment(file, selector);
  if (!token) {
    return NULL;
  }
  if (token->_type == TT_Interpreter) {
    Token newline = readTokenSkipComment(file, token->_raw.end);
    if (!newline) {
      Token_dispose(token);
      return NULL;
    }
    Interpreter interpreter = Interpreter_create();
    interpreter->_interpreter = token->_raw;
    interpreter->_node->_position = token->_raw;
    interpreter->_node->_position.end = newline->_raw.end;
    selector = newline->_raw.end;
    Token_dispose(newline);
    Token_dispose(token);
    return interpreter;
  }
  Token_dispose(token);
  return NULL;
}
Directive readDirective(SourceFile file, cstring source) {
  cstring selector = source;
  Token literal = readTokenSkipComment(file, selector);
  if (!literal) {
    return NULL;
  }
  if (literal->_type == TT_String) {
    selector = literal->_raw.end;
    Token newline = readTokenSkipComment(file, selector);
    if (!newline) {
      Token_dispose(literal);
      return NULL;
    }
    if (newline->_type == TT_Symbol && *newline->_raw.begin == ';') {
      selector = newline->_raw.end;
      Token_dispose(newline);
      newline = readTokenSkipComment(file, selector);
      if (!newline) {
        Token_dispose(literal);
        return NULL;
      }
    }
    if (newline->_type == TT_Newline) {
      Directive directive = Directive_create();
      directive->_node->_position.begin = literal->_raw.begin;
      directive->_node->_position.end = newline->_raw.end;
      directive->_directive = literal->_raw;
      Token_dispose(newline);
      Token_dispose(literal);
      return directive;
    }
  }
  Token_dispose(literal);
  return NULL;
}

Program readProgram(SourceFile file, cstring source) {
  Program program = Program_create();
  cstring selector = source;
  program->_interpreter = readInterpreter(file, selector);
  if (ctx.error.error) {
    Program_dispose(program);
    return NULL;
  }
  if (program->_interpreter) {
    selector = program->_interpreter->_node->_position.end;
  }
  Directive directive = readDirective(file, selector);
  while (directive) {
    List_insert_tail(program->_directives, directive);
    selector = directive->_node->_position.end;
    directive = readDirective(file, selector);
  }
  if (ctx.error.error) {
    Program_dispose(program);
    return NULL;
  }
  Statement statement = readStatement(file, selector);
  while (statement) {
    List_insert_tail(program->_body, statement);
    selector = statement->_node->_position.end;
    statement = readStatement(file, selector);
  }
  if (ctx.error.error != NULL) {
    Program_dispose(program);
    return NULL;
  }
  program->_node->_position.begin = source;
  program->_node->_position.end = source + strlen(source);
  return program;
}

AstNode parse(SourceFile file) {
  cstring source = file->_source;
  initTokenizerContext();
  ctx.error.error = NULL;
  Program program = readProgram(file, source);
  if (program) {
    JSON_Value val = JSON_fromProgram(program);
    Program_dispose(program);
    cstring source = JSON_stringlify(val);
    JSON_dispose(val);
    printf("%s", source);
    Buffer_free(source);
  }
  uninitTokenizerContext();
  if (ctx.error.error) {
    fprintf(stderr, "%s at\n  %s:%u:%u", ctx.error.error,
            ctx.error.location._filename,
            ctx.error.location._position._line + 1,
            ctx.error.location._position._column + 1);
  }
  return NULL;
}