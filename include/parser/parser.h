#pragma once
#include "./ast.h"
#include "./expression.h"
#include "./statement.h"
#include "util/json.h"
Statement parse(SourceFile file);

JSON_Value JSON_fromExpression(Expression expression);
JSON_Value JSON_fromStatement(Statement statement);