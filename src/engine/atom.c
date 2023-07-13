#include "engine/atom.h"
#include <string.h>

static Bool JS_Atom_tryDispose(JS_Atom atom) {
  List queue = List_create(False);
  List cache = List_create(False);
  List_insert_tail(queue, atom);
  Bool isValueLived = False;
  while (!List_empty(queue)) {
    List_Node it = List_head(queue);
    JS_Atom val = List_get(it);
    if (!List_contains(cache, val)) {
      if (!List_empty(val->groups)) {
        isValueLived = True;
        break;
      }
      LIST_LOOP(val->refs) {
        JS_Atom val = List_get(it);
        List_insert_tail(queue, val);
      }
      List_insert_tail(cache, val);
    }
    List_remove(queue, it);
  }
  Buffer_dispose(cache);
  Buffer_dispose(queue);
  if (!isValueLived) {
    Buffer_dispose(atom);
    return True;
  }
  return False;
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
  List gc_list = List_create(False);
  LIST_LOOP(group->values) {
    JS_Atom atom = (JS_Atom)List_get(it);
    LIST_LOOP(atom->groups) {
      if (List_get(it) == group) {
        List_remove(atom->groups, it);
        break;
      }
    }
    if (List_empty(atom->groups)) {
      List_insert_tail(gc_list, atom);
    }
  }
  List cache = List_create(False);
  while (!List_empty(gc_list)) {
    List_Node it = List_head(gc_list);
    JS_Atom atom = (JS_Atom)List_get(it);
    LIST_LOOP(atom->refs) {
      JS_Atom ref = (JS_Atom)List_get(it);
      if (List_contains(cache, ref)) {
        List_Node tmp = it;
        it = List_next(it);
        List_remove(atom->refs, tmp);
        if (List_empty(atom->refs)) {
          break;
        }
        it = List_last(it);
      }
    }
    if (JS_Atom_tryDispose(atom)) {
      List_insert_tail(cache, atom);
    }
    List_remove(gc_list, it);
  }
  Buffer_dispose(cache);
  Buffer_dispose(gc_list);
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
    JS_Atom_tryDispose(atom);
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
}

JS_Atom JS_Atom_create(JS_AtomGroup group) {
  JS_Atom atom =
      (JS_Atom)Buffer_alloc(sizeof(struct s_JS_Atom), JS_Atom_dispose);
  atom->data = NULL;
  atom->refs = List_create(False);
  atom->groups = List_create(False);
  JS_AtomGroup_add(group, atom);
  return atom;
}

void JS_Atom_addRef(JS_Atom parent, JS_Atom obj) {
  List_insert_tail(obj->refs, parent);
}
JS_Atom JS_Atom_removeRef(JS_Atom parent, JS_Atom obj) {
  LIST_LOOP(obj->refs) {
    if (List_get(it) == parent) {
      List_remove(obj->refs, it);
      if (List_empty(obj->groups)) {
        JS_Atom_tryDispose(obj);
      }
      break;
    }
  }
  return obj;
}