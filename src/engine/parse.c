#include "ast.h"
#include <string.h>

typedef struct s_Context {
  Error error;
} Context;

static Context ctx;

struct s_AstNode {
  NodeType _type;
  strings _position;
};

struct s_Interpreter {
  AstNode _node;
  strings _interpreter;
};

struct s_Directive {
  AstNode _node;
  strings _directive;
};

struct s_Statement {
  AstNode _node;
};

struct s_Program {
  AstNode _node;
  Interpreter _interpreter;
  List _directives; // Directive[]
  List _body;       // Statement[]
};

Token readTokenSkipComment(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readToken(file, selector);
  while (token->_type == TT_Comment) {
    selector = token->_raw.end;
    Token_dispose(token);
    Token token = readToken(file, selector);
  }
  return token;
}

void AstNode_dispose(AstNode node);
void AstNode_dispose(AstNode node) { Buffer_free(node); }

AstNode AstNode_create() {
  AstNode node = (AstNode)Buffer_alloc(sizeof(struct s_AstNode));
  return node;
}

Statement Statement_create() {
  Statement statement = (Statement)Buffer_alloc(sizeof(struct s_Statement));
  statement->_node = AstNode_create();
  return statement;
}
void Statement_dispose(Statement statement) {
  AstNode_dispose(statement->_node);
  Buffer_free(statement);
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
}

Statement readStatement(SourceFile file, cstring source) {
  Token token = readTokenSkipComment(file, source);
  if (token->_type == TT_Keyword) {
    
  }
  return NULL;
}

Interpreter readInterpreter(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipComment(file, selector);
  if (token->_type == TT_Interpreter) {
    Token newline = readTokenSkipComment(file, token->_raw.end);
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
  if (literal->_type == TT_String) {
    selector = literal->_raw.end;
    Token newline = readTokenSkipComment(file, selector);
    if (newline->_type == TT_Symbol && *newline->_raw.begin == ';') {
      selector = newline->_raw.end;
      Token_dispose(newline);
      newline = readTokenSkipComment(file, selector);
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
  if (program->_interpreter) {
    selector = program->_interpreter->_node->_position.end;
  }
  Directive directive = readDirective(file, selector);
  while (directive) {
    List_insert_tail(program->_directives, directive);
    selector = directive->_node->_position.end;
    directive = readDirective(file, selector);
  }
  program->_node->_position.begin = source;
  program->_node->_position.end = source + strlen(source);
  return program;
}

const cstring Token_getTypeString(Token token) {
  switch (token->_type) {
  case TT_BigInt:
    return "TT_BigInt";
  case TT_Interpreter:
    return "TT_Interpreter";
  case TT_Newline:
    return "TT_Newline";
  case TT_Number:
    return "TT_Number";
  case TT_String:
    return "TT_String";
  case TT_Keyword:
    return "TT_Keyword";
  case TT_Symbol:
    return "TT_Symbol";
  case TT_Identifier:
    return "TT_Identifier";
  case TT_Regex:
    return "TT_Regex";
  case TT_Comment:
    return "TT_Comment";
  case TT_Eof:
    return "TT_Eof";
  case TT_Init:
    return "TT_Init";
  case TT_Template:
    return "TT_Template";
  case TT_TemplateStart:
    return "TT_TemplateStart";
  case TT_TemplatePart:
    return "TT_TemplatePart";
  case TT_TemplateEnd:
    return "TT_TemplateEnd";
  }
}

cstring Token_toString(Token token) {
  size_t len = token->_raw.end - token->_raw.begin;
  cstring buf = (cstring)Buffer_alloc(len + 1);
  if (token->_raw.begin[0] == '\n') {
    cstring buf = (cstring)Buffer_alloc(3);
    memcpy(buf, "\\n", 3);
    return buf;
  } else {
    cstring buf = (cstring)Buffer_alloc(len + 1);
    memcpy(buf, token->_raw.begin, len);
    buf[len] = '\0';
    return buf;
  }
}

cstring printInterpreter(Interpreter interpreter) {
  size_t len =
      interpreter->_interpreter.end - interpreter->_interpreter.begin + 2;
  cstring result = (cstring)Buffer_alloc(len);
  result[len] = 0;
  *result = '\"';
  cstring selector = result + 1;
  cstring sselector = interpreter->_interpreter.begin;
  while (sselector != interpreter->_interpreter.end) {
    *selector++ = *sselector++;
  }
  *selector = '\"';
  return result;
}

cstring printDirective(Directive directive) {
  size_t len = directive->_directive.end - directive->_directive.begin;
  cstring result = (cstring)Buffer_alloc(len);
  result[len] = 0;
  cstring selector = result;
  cstring sselector = directive->_directive.begin;
  while (sselector != directive->_directive.end) {
    *selector++ = *sselector++;
  }
  return result;
}

cstring printProgram(Program program) {
  List_Option opt = {1, Buffer_free};
  List parts = List_create(opt);
  size_t len = 0;
  List_insert_tail(parts, Buffer_fromString("{"));
  len++;
  List_insert_tail(parts, Buffer_fromString("\"type\":\"NT_Program\""));
  len += strlen("\"type\":\"NT_Program\"");
  if (program->_interpreter) {
    List_insert_tail(parts, Buffer_fromString(",\"interpreter\":"));
    len += strlen(",\"interpreter\":");
    cstring s_interpreter = printInterpreter(program->_interpreter);
    List_insert_tail(parts, s_interpreter);
    len += strlen(s_interpreter);
  }
  if (List_size(program->_directives) != 0) {
    List_insert_tail(parts, Buffer_fromString(",\"directives\":["));
    len += strlen(",\"directives\":[");
    for (List_Node node = List_head(program->_directives);
         node != List_tail(program->_directives); node = List_next(node)) {
      Directive directive = (Directive)List_get(node);
      cstring s_directive = printDirective(directive);
      if (node != List_head(program->_directives)) {
        List_insert_tail(parts, Buffer_fromString(","));
        len += 1;
      }
      List_insert_tail(parts, s_directive);
      len += strlen(s_directive);
    }
    List_insert_tail(parts, Buffer_fromString("]"));
    len++;
  }
  List_insert_tail(parts, Buffer_fromString("}"));
  len++;
  cstring result = (cstring)Buffer_alloc(len + 1);
  result[len] = 0;
  cstring selector = result;
  for (List_Node node = List_head(parts); node != List_tail(parts);
       node = List_next(node)) {
    cstring data = (cstring)List_get(node);
    while (*data) {
      *selector++ = *data++;
    }
  }
  List_dispose(parts);
  return result;
}

AstNode parse(SourceFile file) {
  cstring source = file->_source;
  initTokenizerContext();
  ctx.error.error = NULL;
  Program program = readProgram(file, source);
  cstring result = printProgram(program);
  printf("%s", result);
  Buffer_free(result);
  Program_dispose(program);
  Error error = getTokenizerError();
  if (error.error) {
    fprintf(stderr, "%s at\n  %s:%lld:%lld", error.error,
            error.location._filename, error.location._position._line + 1,
            error.location._position._column + 1);
  }
  uninitTokenizerContext();
  if (ctx.error.error) {
    fprintf(stderr, "%s at\n  %s:%lld:%lld", ctx.error.error,
            ctx.error.location._filename,
            ctx.error.location._position._line + 1,
            ctx.error.location._position._column + 1);
  }
  return NULL;
}