#include "ast.h"

Expression TemplateExpression_create() {
  Expression template_expr = Expression_create();
  template_expr->node->type = NT_TemplateExpression;
  template_expr->level = -2;
  return template_expr;
}
void TemplateExpression_dispose(Expression expression) {
  Template_dispose(expression->template);
  AstNode_dispose(expression->node);
  Buffer_free(expression);
}
Template Template_create() {
  Template template = (Template)Buffer_alloc(sizeof(struct s_Template));
  template->node = AstNode_create();
  template->node->type = NT_Template;
  List_Option data_opt = {1, (Buffer_Free)Token_dispose};
  template->datas = List_create(data_opt);
  List_Option part_opt = {1, (Buffer_Free)Expression_dispose};
  template->parts = List_create(part_opt);
  template->tag = NULL;
  return template;
}
void Template_dispose(Template template) {
  if (template->tag) {
    Expression_dispose(template->tag);
  }
  List_dispose(template->datas);
  List_dispose(template->parts);
  AstNode_dispose(template->node);
  Buffer_free(template);
}
Template readTemplate(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  if (token->type == TT_Template) {
    Template template = Template_create();
    List_insert_tail(template->datas, token);
    template->node->position.begin = source;
    template->node->position.end = token->raw.end;
    return template;
  } else if (token->type == TT_TemplateStart) {
    Template template = Template_create();
    cstring selector = source;
    AstContext ctx = pushAstContext();
    enableReadTemplate();
    for (;;) {
      if (token->type == TT_TemplateStart || token->type == TT_TemplatePart ||
          token->type == TT_TemplateEnd) {
        List_insert_tail(template->datas, token);
        selector = token->raw.end;
      } else {
        Token_dispose(token);
        popAstContext(ctx);
        ErrorStack_push(Error_init("Unexcpet token.missing template literal.",
                                   getLocation(file, selector), NULL));
        goto failed;
      }
      if (token->type == TT_TemplateEnd) {
        break;
      }
      AstContext current = pushAstContext();
      Expression part = readExpression(file, selector);
      popAstContext(current);
      if (!part) {
        popAstContext(ctx);
        goto failed;
      }
      List_insert_tail(template->parts, part);
      selector = part->node->position.end;
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        popAstContext(ctx);
        goto failed;
      }
    }
    popAstContext(ctx);
    template->node->position.begin = source;
    template->node->position.end = selector;
    return template;
  failed:
    Template_dispose(template);
    return NULL;
  }
  Token_dispose(token);
  return NULL;
}
Expression readTemplateExpression(SourceFile file, cstring source) {
  Template template = readTemplate(file, source);
  if (!template) {
    return NULL;
  }
  Expression template_expr = TemplateExpression_create();
  template_expr->template = template;
  template_expr->node->position = template->node->position;
  return template_expr;
}