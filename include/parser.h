#pragma once
#include "ast.h"
#include "expression.h"
#include "json.h"
Expression parse(SourceFile file);

JSON_Value JSON_fromExpression(Expression expression);