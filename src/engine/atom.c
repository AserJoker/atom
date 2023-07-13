#include "engine/atom.h"
#include <string.h>

static Bool JS_Atom_isAlone(JS_Atom atom) {
  List queue = List_create(False);
  List cache = List_create(False);
  List_insert_tail(queue, atom);
  Bool isAtomAlone = True;
  while (!List_empty(queue)) {
    List_Node it = List_head(queue);
    JS_Atom val = List_get(it);
    if (!List_contains(cache, val)) {
      if (!List_empty(val->groups)) {
        isAtomAlone = False; // find a path to root
        break;
      }
      LIST_LOOP(val->refs) { List_insert_tail(queue, List_get(it)); }
      List_insert_tail(cache, val);
    }
    List_remove(queue, it);
  }
  Buffer_dispose(cache);
  Buffer_dispose(queue);
  return isAtomAlone;
}

static void JS_Atom_autoRelease(JS_Atom atom) {
  List queue = List_create(False);
  List cache = List_create(False);
  List destroy_list = List_create(True);
  List_insert_tail(queue, atom);
  while (!List_empty(queue)) {
    List_Node it = List_head(queue);
    JS_Atom current = List_get(it);
    if (!List_contains(cache, current)) {
      List_insert_tail(cache, current);
      if (JS_Atom_isAlone(current)) {
        while (!List_empty(current->deRefs)) {
          List_Node it = List_head(current->deRefs);
          JS_Atom deRef = List_get(it);
          List_erase(deRef->refs, current);
          List_insert_tail(queue, deRef);
          List_remove(current->deRefs, it);
        }
        if (!List_contains(destroy_list, current)) {
          List_insert_tail(destroy_list, current);
        }
      }
    }
    List_remove(queue, it);
  }
  Buffer_dispose(queue);
  Buffer_dispose(cache);
  Buffer_dispose(destroy_list);
}

static void JS_AtomGroup_dispose(JS_AtomGroup group) {
  Buffer_dispose(group->children);
  if (group->parent) {
    LIST_LOOP(group->parent->children) {
      if (List_get(it) == group) {
        List_remove(group->parent->children, it);
        group->parent = NULL;
        break;
      }
    }
  }
  while (!List_empty(group->values)) {
    List_Node it = List_head(group->values);
    JS_Atom atom = (JS_Atom)List_get(it);
    List_erase(atom->groups, group);
    if (List_empty(atom->groups)) {
      JS_Atom_autoRelease(atom);
    }
    List_remove(group->values, it);
  }
  Buffer_dispose(group->values);
}

JS_AtomGroup JS_AtomGroup_create(JS_AtomGroup parent) {
  JS_AtomGroup group = (JS_AtomGroup)Buffer_alloc(sizeof(struct s_JS_AtomGroup),
                                                  JS_AtomGroup_dispose);
  group->children = List_create(True);
  group->values = List_create(False);
  group->parent = parent;
  if (parent) {
    List_insert_tail(parent->children, group);
  }
  return group;
}

void JS_AtomGroup_remove(JS_AtomGroup group, JS_Atom atom) {
  LIST_LOOP(atom->groups) {
    if (List_get(it) == group) {
      List_remove(atom->groups, it);
      break;
    }
  }
  LIST_LOOP(group->values) {
    if (List_get(it) == atom) {
      List_remove(atom->groups, it);
      break;
    }
  }
  if (List_empty(atom->groups)) {
    JS_Atom_autoRelease(atom);
  }
}

void JS_AtomGroup_add(JS_AtomGroup group, JS_Atom atom) {
  LIST_LOOP(atom->groups) {
    if (List_get(it) == group) {
      return;
    }
  }
  List_insert_tail(atom->groups, group);
  List_insert_tail(group->values, atom);
}

static void JS_Atom_dispose(JS_Atom atom) {
  Buffer_dispose(atom->data);
  Buffer_dispose(atom->groups);
  Buffer_dispose(atom->refs);
  Buffer_dispose(atom->deRefs);
}

JS_Atom JS_Atom_create(JS_AtomGroup group) {
  JS_Atom atom =
      (JS_Atom)Buffer_alloc(sizeof(struct s_JS_Atom), JS_Atom_dispose);
  atom->data = NULL;
  atom->refs = List_create(False);
  atom->groups = List_create(False);
  atom->deRefs = List_create(False);
  JS_AtomGroup_add(group, atom);
  return atom;
}

void JS_Atom_addRef(JS_Atom parent, JS_Atom obj) {
  List_insert_tail(obj->refs, parent);
  List_insert_tail(parent->deRefs, obj);
}
JS_Atom JS_Atom_removeRef(JS_Atom parent, JS_Atom obj) {
  LIST_LOOP(obj->refs) {
    if (List_get(it) == parent) {
      List_remove(obj->refs, it);
      List_erase(parent->deRefs, obj);
      if (List_empty(obj->groups)) {
        JS_Atom_autoRelease(obj);
      }
      break;
    }
  }
  return obj;
}