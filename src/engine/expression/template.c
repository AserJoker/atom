#include "ast.h"
TemplatePattern TemplatePattern_create() {
  TemplatePattern pattern =
      (TemplatePattern)Buffer_alloc(sizeof(struct s_TemplatePattern));
  List_Option strings_opt = {1, (Buffer_Free)Token_dispose};
  List_Option parts_opt = {1, (Buffer_Free)Expression_dispose};
  pattern->_tag = NULL;
  pattern->_strings = List_create(strings_opt);
  pattern->_parts = List_create(parts_opt);
  return pattern;
}
void TemplatePattern_dispose(TemplatePattern pattern) {
  List_dispose(pattern->_strings);
  List_dispose(pattern->_parts);
  if (pattern->_tag) {
    Identifier_dispose(pattern->_tag);
  }
  Buffer_free(pattern);
}

Expression readTemplateExpression(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  TemplatePattern pattern = TemplatePattern_create();
  if (token->_type == TT_Identifier) {
    selector = token->_raw.end;
    Token_dispose(token);
    pattern->_tag = readIdentifier(file, source);
    if (!pattern->_tag) {
      TemplatePattern_dispose(pattern);
      return NULL;
    }
    token = readTokenSkipComment(file, selector);
  }
  if (!token) {
    TemplatePattern_dispose(pattern);
    return NULL;
  }
  if (token->_type == TT_Template) {
    Expression expr = Expression_create();
    List_insert_tail(pattern->_strings, token);
    expr->_pattern._template = pattern;
    expr->_level = -2;
    expr->_node->_type = NT_TemplatePattern;
    expr->_node->_position = token->_raw;
    return expr;
  } else if (token->_type != TT_TemplateStart) {
    Token_dispose(token);
    return NULL;
  }
  while (token) {
    List_insert_tail(pattern->_strings, token);
    selector = token->_raw.end;
    TokenContext tctx = pushTokenContext();
    Expression expr = readExpression(file, selector);
    popTokenContext(tctx);
    if (!expr) {
      TemplatePattern_dispose(pattern);
      return NULL;
    }
    List_insert_tail(pattern->_parts, expr);
    selector = expr->_node->_position.end;
    token = readTokenSkipNewline(file, selector);
    if (token->_type == TT_TemplateEnd) {
      List_insert_tail(pattern->_strings, token);
      selector = token->_raw.end;
      break;
    } else if (token->_type == TT_TemplatePart) {
      continue;
    } else {
      Token_dispose(token);
      TemplatePattern_dispose(pattern);
      return NULL;
    }
  }
  Expression expr = Expression_create();
  expr->_pattern._template = pattern;
  expr->_node->_type = NT_TemplatePattern;
  expr->_node->_position.begin = source;
  expr->_node->_position.end = selector;
  expr->_level = -2;
  return expr;
}