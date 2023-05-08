#pragma once
#include "dlist.h"
#include "strings.h"
#include "tree.h"
typedef cstring (*ToString)(void *);
cstring JSON_fromDList(DList list, ToString toString);
cstring JSON_fromTree(Tree tree, ToString toString);