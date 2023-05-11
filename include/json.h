#pragma once
#include "list.h"
#include "strings.h"
#include "tree.h"
typedef cstring (*ToString)(void *);
cstring JSON_fromList(List list, ToString toString);
cstring JSON_fromTree(Tree tree, ToString toString);