#include "engine/value.h"
#include <string.h>
typedef struct s_JS_NamedAtom {
  cstring name;
  JS_Atom atom;
} *JS_NamedAtom;

static void JS_Scope_dispose(JS_Scope scope) {
  Buffer_dispose(scope->children);
  Buffer_dispose(scope->values);
  if (!scope->group->parent) {
    Buffer_dispose(scope->group);
  }
}
JS_Scope JS_Scope_create(JS_Scope parent) {
  JS_Scope scope =
      (JS_Scope)Buffer_alloc(sizeof(struct s_JS_Scope), JS_Scope_dispose);
  scope->children = List_create(True);
  scope->values = List_create(True);
  scope->parent = parent;
  if (parent) {
    List_insert_tail(parent->children, scope);
    scope->group = JS_AtomGroup_create(parent->group);
  } else {
    scope->group = JS_AtomGroup_create(NULL);
  }
  return scope;
}

static void JS_NamedAtom_dispose(JS_NamedAtom natom) {
  Buffer_dispose(natom->name);
}

JS_Atom JS_Scope_define(JS_Scope scope, cstring name, JS_Value value) {
  LIST_LOOP(scope->values) {
    JS_NamedAtom natom = List_get(it);
    if (strcmp(natom->name, name) == 0) {
      return NULL;
    }
  }
  JS_Atom atom = JS_Atom_create(scope->group);
  atom->data = value;
  JS_NamedAtom natom = (JS_NamedAtom)Buffer_alloc(sizeof(struct s_JS_NamedAtom),
                                                  JS_NamedAtom_dispose);
  natom->name = cstring_toBuffer(name);
  natom->atom = atom;
  List_insert_tail(scope->values, natom);
  return atom;
}
Bool JS_Scope_add(JS_Scope scope, cstring name, JS_Atom atom) {
  LIST_LOOP(scope->values) {
    JS_NamedAtom natom = List_get(it);
    if (strcmp(natom->name, name) == 0) {
      return False;
    }
  }
  JS_NamedAtom natom = (JS_NamedAtom)Buffer_alloc(sizeof(struct s_JS_NamedAtom),
                                                  JS_NamedAtom_dispose);
  natom->name = cstring_toBuffer(name);
  natom->atom = atom;
  JS_AtomGroup_add(scope->group, atom);
  List_insert_tail(scope->values, natom);
  return True;
}
Bool JS_Scope_remove(JS_Scope scope, cstring name) {
  LIST_LOOP(scope->values) {
    JS_NamedAtom natom = List_get(it);
    if (strcmp(natom->name, name) == 0) {
      JS_AtomGroup_remove(scope->group, natom->atom);
      List_remove(scope->values, it);
      return True;
    }
  }
  return False;
}
JS_Atom JS_Scope_get(JS_Scope scope, cstring name) {
  LIST_LOOP(scope->values) {
    JS_NamedAtom natom = List_get(it);
    if (strcmp(natom->name, name) == 0) {
      return natom->atom;
    }
  }
  return NULL;
}
JS_Atom JS_Scope_checkout(JS_Scope scope, cstring name) {
  JS_Atom atom = JS_Scope_get(scope, name);
  JS_Scope it = scope;
  while (!atom && it->parent) {
    it = it->parent;
    atom = JS_Scope_get(it, name);
    if (atom) {
      JS_Scope_add(scope, name, atom);
      return atom;
    }
  }
  return NULL;
}
static void JS_String_dispose(JS_String value) { Buffer_dispose(value->data); }
JS_String JS_createString(cstring data) {
  JS_String value =
      (JS_String)Buffer_alloc(sizeof(struct s_JS_String), JS_String_dispose);
  value->super.type = JS_STRING;
  value->data = cstring_toBuffer(data);
  return value;
}
void JS_setString(JS_Atom atom, cstring data) {
  JS_String val = atom->data;
  if (val->super.type != JS_STRING) {
    Buffer_dispose(atom->data);
    val = JS_createString("");
    atom->data = val;
  }
  val->data = cstring_toBuffer(data);
}
Bool JS_getString(JS_Atom atom, cstring *source) {
  JS_String str = atom->data;
  if (str->super.type == JS_STRING) {
    *source = str->data;
    return True;
  }
  return False;
}
Bool JS_isString(JS_Atom atom) {
  JS_Value val = atom->data;
  return val->type == JS_STRING;
}