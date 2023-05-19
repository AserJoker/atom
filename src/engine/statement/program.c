#include "ast.h"
#include <string.h>

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

ImportSpecifier readImportDefaultSpecifier(SourceFile file, cstring source) {
  ImportSpecifier import_spec = ImportSpecifier_create();
  import_spec->_local = readIdentifier(file, source);
  import_spec->_node->_type = NT_ImportDefaultSpecifier;
  import_spec->_node->_position.begin = source;
  import_spec->_node->_position.end = import_spec->_local->_raw->_raw.end;
  return import_spec;
}

ImportAttribute readImportAttribute(SourceFile file, cstring source) {
  ImportAttribute import_attr = ImportAttribute_create();
  cstring selector = source;
  Identifier key = readIdentifier(file, selector);
  if (!key) {
    goto failed;
  }
  import_attr->_key = key;
  selector = key->_node->_position.end;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!checkToken(token, TT_Symbol, ":")) {
    Token_dispose(token);
    Error error;
    error.error = "Unexcept Error";
    error.location = getLocation(file, selector);
    setAstError(error);
    goto failed;
  }
  selector = token->_raw.end;
  Token_dispose(token);
  Literal lit = readLiteral(file, selector);
  if (!lit) {
    Error error;
    error.error = "Unexcept Error";
    error.location = getLocation(file, selector);
    setAstError(error);
    goto failed;
  }
  import_attr->_value = lit;
  selector = lit->_node->_position.end;
  import_attr->_node->_position.begin = source;
  import_attr->_node->_position.end = selector;
  return import_attr;
failed:
  ImportAttribute_dispose(import_attr);
  return NULL;
}

ImportSpecifier readImportNamespaceSpecifier(SourceFile file, cstring source) {
  ImportSpecifier import_spec = ImportSpecifier_create();
  cstring selector = source;
  import_spec->_node->_type = NT_ImportNamespaceSpecifier;
  selector = skipToken(file, selector);
  if (!selector) {
    goto failed;
  }
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!checkToken(token, TT_Keyword, "as")) {
    Token_dispose(token);
    Error error;
    error.error = "Unexcept error,must be 'as'";
    error.location = getLocation(file, selector);
    setAstError(error);
    goto failed;
  }
  Token_dispose(token);
  selector = skipToken(file, selector);
  if (!selector) {
    goto failed;
  }
  Identifier identifier = readIdentifier(file, selector);
  if (!identifier) {
    Error error = getAstError();
    if (!error.error) {
      error.error = "Unexcept error";
      error.location = getLocation(file, selector);
      setAstError(error);
    }
    goto failed;
  }
  selector = identifier->_raw->_raw.end;
  import_spec->_local = identifier;
  import_spec->_node->_position.begin = source;
  import_spec->_node->_position.end = selector;
  return import_spec;
failed:
  ImportSpecifier_dispose(import_spec);
  return NULL;
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
  if (checkToken(token, TT_Keyword, "as")) {
    selector = token->_raw.end;
    Token_dispose(token);
    identify = readIdentifier(file, selector);
    if (!identify) {
      Error error;
      error.error = "Unexpected token";
      error.location = getLocation(file, selector);
      setAstError(error);
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
  ImportStatement import_s = ImportStatement_create();
  cstring selector = source;
  selector = skipToken(file, selector);
  if (!selector) {
    goto failed;
  }
  Token token = readTokenSkipNewline(file, selector);
  if (strings_is(token->_raw, "*")) {
    Token_dispose(token);
    ImportSpecifier import_spec = readImportNamespaceSpecifier(file, selector);
    if (!import_spec) {
      Error error;
      error.error = "Unexcept error";
      error.location = getLocation(file, selector);
      setAstError(error);
      Token_dispose(token);
      goto failed;
    }
    List_insert_tail(import_s->_specifiers, import_spec);
    selector = import_spec->_node->_position.end;
  } else {
    Token_dispose(token);
    for (;;) {
      Token token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (token->_type == TT_Identifier) {
        Token_dispose(token);
        ImportSpecifier import_spec =
            readImportDefaultSpecifier(file, selector);
        if (!import_spec) {
          Error error = getAstError();
          if (!error.error) {
            error.error = "Unexcept error";
            error.location = getLocation(file, source);
            setAstError(error);
          }
          goto failed;
        }
        List_insert_tail(import_s->_specifiers, import_spec);
        selector = import_spec->_node->_position.end;
      } else if (token->_type == TT_Symbol) {
        if (strings_is(token->_raw, "{")) {
          selector = token->_raw.end;
          Token_dispose(token);
          ImportSpecifier import_spec = readImportSpecifier(file, selector);
          while (import_spec) {
            List_insert_tail(import_s->_specifiers, import_spec);
            selector = import_spec->_node->_position.end;
            Token token = readTokenSkipNewline(file, selector);
            if (!token) {
              goto failed;
            }
            if (checkToken(token, TT_Symbol, ",")) {
              selector = token->_raw.end;
              Token_dispose(token);
              import_spec = readImportSpecifier(file, selector);
            } else {
              Token_dispose(token);
              break;
            }
          }
          token = readTokenSkipNewline(file, selector);
          if (!token) {
            goto failed;
          }
          if (!checkToken(token, TT_Symbol, "}")) {
            Error error;
            error.error = "Unexcept error";
            error.location = getLocation(file, selector);
            setAstError(error);
            Token_dispose(token);
            goto failed;
          } else {
            selector = token->_raw.end;
            Token_dispose(token);
          }
        } else {
          Error error;
          error.error = "Unexcept error";
          error.location = getLocation(file, selector);
          setAstError(error);
          Token_dispose(token);
          goto failed;
        }
      }
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (checkToken(token, TT_Symbol, ",")) {
        selector = token->_raw.end;
        Token_dispose(token);
      } else {
        Token_dispose(token);
        break;
      }
    }
  }
  if (List_size(import_s->_specifiers)) {
    Token token = readTokenSkipNewline(file, selector);
    if (!checkToken(token, TT_Keyword, "from")) {
      Error error;
      error.error = "Unexcept error";
      error.location = getLocation(file, selector);
      setAstError(error);
      Token_dispose(token);
      goto failed;
    }
    selector = token->_raw.end;
    Token_dispose(token);
  }
  Literal literal = readLiteral(file, selector);
  if (!literal) {
    Error error;
    error.error = "Unexcept error";
    error.location = getLocation(file, selector);
    setAstError(error);
    goto failed;
  }
  import_s->_source = literal;
  selector = literal->_raw->_raw.end;
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (checkToken(token, TT_Keyword, "assert")) {
    selector = token->_raw.end;
    Token_dispose(token);
    Token token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
    if (!checkToken(token, TT_Symbol, "{")) {
      Error error;
      error.error = "Unexcept error";
      error.location = getLocation(file, selector);
      setAstError(error);
      Token_dispose(token);
      goto failed;
    }
    selector = token->_raw.end;
    Token_dispose(token);
    ImportAttribute import_attr = readImportAttribute(file, selector);
    List_insert_tail(import_s->_assertions, import_attr);
    selector = import_attr->_node->_position.end;
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
    if (!checkToken(token, TT_Symbol, "}")) {
      Error error;
      error.error = "Unexcept error";
      error.location = getLocation(file, selector);
      setAstError(error);
      Token_dispose(token);
      goto failed;
    }
    selector = token->_raw.end;
    Token_dispose(token);
  } else {
    Token_dispose(token);
  }
  import_s->_node->_position.begin = source;
  import_s->_node->_position.end = selector;
  return import_s;
failed:
  if (import_s) {
    ImportStatement_dispose(import_s);
  }
  return NULL;
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
Interpreter readInterpreter(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipComment(file, selector);
  if (!token) {
    return NULL;
  }
  if (token->_type == TT_Interpreter) {
    Interpreter interpreter = Interpreter_create();
    interpreter->_interpreter = token->_raw;
    interpreter->_node->_position = token->_raw;
    selector = token->_raw.end;
    Token_dispose(token);
    Token newline = readTokenSkipComment(file, selector);
    if (!newline) {
      return NULL;
    }
    interpreter->_node->_position.end = newline->_raw.end;
    selector = newline->_raw.end;
    Token_dispose(newline);
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
  Interpreter interpreter = readInterpreter(file, selector);
  if (interpreter) {
    selector = interpreter->_node->_position.end;
    program->_interpreter = interpreter;
  }
  Error error = getAstError();
  if (error.error) {
    goto failed;
  }
  Directive directive = readDirective(file, selector);
  while (directive) {
    List_insert_tail(program->_directives, directive);
    selector = directive->_node->_position.end;
    directive = readDirective(file, selector);
  }
  if (error.error) {
    goto failed;
  }
  Statement statement = readStatement(file, selector);
  while (statement) {
    List_insert_tail(program->_body, statement);
    selector = statement->_node->_position.end;
    Token token = readTokenSkipComment(file, selector);
    if (!token) {
      goto failed;
    }
    if (checkToken(token, TT_Symbol, ";") || token->_type == TT_Newline) {
      selector = token->_raw.end;
      Token_dispose(token);
    } else if (token->_type == TT_Eof) {
      Token_dispose(token);
      break;
    } else {
      Error error;
      error.error = "Unexcept error";
      error.location = getLocation(file, selector);
      setAstError(error);
      Token_dispose(token);
      goto failed;
    }
    statement = readStatement(file, selector);
  }
  if (getAstError().error != NULL) {
    Program_dispose(program);
    return NULL;
  }
  program->_node->_position.begin = source;
  program->_node->_position.end = source + strlen(source);
  return program;
failed:
  Program_dispose(program);
  return NULL;
}