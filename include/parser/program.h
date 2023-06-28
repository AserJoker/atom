#pragma once
#include "ast.h"
typedef struct s_Program {
  AstNode node;
  List body;
} *Program;
Program readProgram(SourceFile file, cstring source);
void Program_dispose(Program program);