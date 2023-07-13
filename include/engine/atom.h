#pragma once
#include "util/bool.h"
#include "util/list.h"
#include "util/strings.h"

typedef struct s_JS_Atom *JS_Atom;
typedef struct s_JS_AtomGroup *JS_AtomGroup;

typedef struct s_JS_AtomGroup {
  JS_AtomGroup parent;
  List children;
  List values;
} *JS_AtomGroup;

struct s_JS_Atom {
  List refs;
  List deRefs;
  List groups;
  Buffer data;
};

JS_AtomGroup JS_AtomGroup_create(JS_AtomGroup parent);
void JS_AtomGroup_remove(JS_AtomGroup group, JS_Atom atom);
void JS_AtomGroup_add(JS_AtomGroup group, JS_Atom atom);

JS_Atom JS_Atom_create(JS_AtomGroup group);
void JS_Atom_addRef(JS_Atom parent, JS_Atom obj);
JS_Atom JS_Atom_removeRef(JS_Atom parent, JS_Atom obj);