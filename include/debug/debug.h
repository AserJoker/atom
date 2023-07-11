#pragma once
#include "compiler/astnode.h"
#include "util/json.h"

void Debug_init();

JSON_Value JSON_fromAstNode(AstNode s_program);