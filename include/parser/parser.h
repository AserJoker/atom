#pragma once
#include "./ast.h"
#include "./expression.h"
#include "./program.h"
#include "./statement.h"
#include "util/json.h"

Program parse(SourceFile file);

JSON_Value JSON_fromExpression(Expression expression);
JSON_Value JSON_fromStatement(Statement statement);
JSON_Value JSON_fromProgram(Program program);