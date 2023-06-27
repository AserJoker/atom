#include "statement.h"
void ImportAttribute_dispose(ImportAttribute attribute) {
  Token_dispose(attribute->key);
  Token_dispose(attribute->value);
  Buffer_free(attribute);
}
ImportAttribute readImportAttribute(SourceFile file, cstring source) {
  cstring selector = source;
  ImportAttribute attr =
      (ImportAttribute)Buffer_alloc(sizeof(struct s_ImportAttribute));
  attr->key = NULL;
  attr->value = NULL;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (token->type != TT_Identifier) {
    pushError("Unexcept token", getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  attr->key = token;
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!checkToken(token, TT_Symbol, ":")) {
    pushError("Unexcept token.missing token ':'",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (token->type != TT_String) {
    pushError("Unexcept token", getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  attr->value = token;
  return attr;
failed:
  ImportAttribute_dispose(attr);
  return NULL;
}
void ImportSpecifier_dispose(ImportSpecifier specifier) {
  if (specifier->imported) {
    Token_dispose(specifier->imported);
  }
  if (specifier->local) {
    Token_dispose(specifier->local);
  }
  Buffer_free(specifier);
}

ImportSpecifier readNamespaceSpecifier(SourceFile file, cstring source) {
  cstring selector = source;
  ImportSpecifier spec =
      (ImportSpecifier)Buffer_alloc(sizeof(struct s_ImportSpecifier));
  spec->imported = NULL;
  spec->importType = IST_Namespace;
  spec->local = NULL;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  token = readTokenSkipNewline(file, selector);
  if (!checkToken(token, TT_Keyword, "as")) {
    pushError("Unexcept token.missing token 'as'",
              getLocation(file, token->raw.end));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (token->type != TT_Identifier) {
    pushError("Unexcept token", getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  spec->imported = token;
  spec->position.begin = source;
  spec->position.end = selector;
  return spec;
failed:
  ImportSpecifier_dispose(spec);
  return NULL;
}
ImportSpecifier readDefaultSpecifier(SourceFile file, cstring source) {
  cstring selector = source;
  ImportSpecifier spec =
      (ImportSpecifier)Buffer_alloc(sizeof(struct s_ImportSpecifier));
  spec->imported = NULL;
  spec->importType = IST_Default;
  spec->local = NULL;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  spec->imported = token;
  spec->position = token->raw;
  return spec;
failed:
  ImportSpecifier_dispose(spec);
  return NULL;
}
ImportSpecifier readEntitySpecifier(SourceFile file, cstring source) {
  cstring selector = source;
  ImportSpecifier spec =
      (ImportSpecifier)Buffer_alloc(sizeof(struct s_ImportSpecifier));
  spec->imported = NULL;
  spec->importType = IST_Entity;
  spec->local = NULL;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (token->type != TT_Identifier) {
    pushError("Unexcept token", getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  spec->local = token;
  selector = token->raw.end;
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (checkToken(token, TT_Keyword, "as")) {
    selector = token->raw.end;
    Token_dispose(token);
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
    if (token->type != TT_Identifier) {
      pushError("Unexcept token", getLocation(file, token->raw.begin));
      Token_dispose(token);
      goto failed;
    }
    spec->imported = token;
    selector = token->raw.end;
  } else {
    spec->imported = (Token)Buffer_alloc(sizeof(struct s_Token));
    spec->imported->type = spec->local->type;
    spec->imported->raw = spec->local->raw;
    Token_dispose(token);
  }
  spec->position.begin = source;
  spec->position.end = selector;
  return spec;
failed:
  ImportSpecifier_dispose(spec);
  return NULL;
}

int isImportStatement(SourceFile file, Token token) {
  return checkToken(token, TT_Keyword, "import");
}
Statement readImportStatement(SourceFile file, cstring source) {
  cstring selector = source;
  Statement statement = Statement_create();
  statement->type = ST_Import;
  statement->import.source = NULL;
  List_Option spec_opt = {1, (Buffer_Free)ImportSpecifier_dispose};
  statement->import.specifiers = List_create(spec_opt);
  List_Option attr_opt = {1, (Buffer_Free)ImportAttribute_dispose};
  statement->import.asserts = List_create(attr_opt);
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);

  token = readTokenSkipNewline(file, selector);
  if (checkToken(token, TT_Symbol, "*")) {
    Token_dispose(token);
    ImportSpecifier ns = readNamespaceSpecifier(file, selector);
    if (!ns) {
      goto failed;
    }
    List_insert_tail(statement->import.specifiers, ns);
    selector = ns->position.end;
    ;
  } else {
    if (token->type == TT_Identifier) {
      Token_dispose(token);
      ImportSpecifier df = readDefaultSpecifier(file, selector);
      if (!df) {
        goto failed;
      }
      List_insert_tail(statement->import.specifiers, df);
      selector = df->position.end;
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (checkToken(token, TT_Symbol, ",")) {
        selector = token->raw.end;
        Token_dispose(token);
        token = readTokenSkipNewline(file, selector);
        if (!token) {
          goto failed;
        }
      }
    }
    if (checkToken(token, TT_Symbol, "{")) {
      selector = token->raw.end;
      Token_dispose(token);
      for (;;) {
        ImportSpecifier spec = readEntitySpecifier(file, selector);
        if (!spec) {
          goto failed;
        }
        List_insert_tail(statement->import.specifiers, spec);
        selector = spec->position.end;
        token = readTokenSkipNewline(file, selector);
        if (!token) {
          goto failed;
        }
        if (checkToken(token, TT_Symbol, ",")) {
          selector = token->raw.end;
          Token_dispose(token);
        } else if (checkToken(token, TT_Symbol, "}")) {
          selector = token->raw.end;
          break;
        } else {
          pushError("Unexcept token.", getLocation(file, token->raw.begin));
          Token_dispose(token);
          goto failed;
        }
      }
    }
    Token_dispose(token);
  }
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!checkToken(token, TT_Keyword, "from")) {
    pushError("Unexcept token.missing token 'from'",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (token->type != TT_String) {
    pushError("Unexcept token.", getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  statement->import.source = token;
  selector = token->raw.end;

  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (checkToken(token, TT_Keyword, "assert")) {
    selector = token->raw.end;
    Token_dispose(token);
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
    if (!checkToken(token, TT_Symbol, "{")) {
      pushError("Unexcept token.missing token '{'",
                getLocation(file, token->raw.begin));
      Token_dispose(token);
      goto failed;
    }
    selector = token->raw.end;
    Token_dispose(token);
    token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
    if (!checkToken(token, TT_Symbol, "}")) {
      Token_dispose(token);
      for (;;) {
        ImportAttribute attr = readImportAttribute(file, selector);
        if (!attr) {
          goto failed;
        }
        List_insert_tail(statement->import.asserts, attr);
        selector = attr->value->raw.end;
        token = readTokenSkipNewline(file, selector);
        if (!token) {
          goto failed;
        }
        if (checkToken(token, TT_Symbol, ",")) {
          selector = token->raw.end;
          Token_dispose(token);
        } else {
          break;
        }
      }
    }
    if (!checkToken(token, TT_Symbol, "}")) {
      pushError("Unexcept token.missing token '}'",
                getLocation(file, token->raw.begin));
      Token_dispose(token);
      goto failed;
    }
    selector = token->raw.end;
    Token_dispose(token);
  } else {
    Token_dispose(token);
  }
  statement->node->position.begin = source;
  statement->node->position.end = selector;
  return statement;
failed:
  Statement_dispose(statement);
  return NULL;
}