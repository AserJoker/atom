#pragma once
#include "tokenizer.h"
#include "util/bool.h"
#include "util/list.h"
#include "util/source.h"
#include "util/strings.h"
struct s_AstNode;
typedef struct s_AstNode *AstNode;
typedef enum { AT_Const, AT_Let, AT_Var } AssigmentType;
struct s_AstNode {
  int type;
  Strings position;
  int bind;
  int level;
  union {
    struct {
      AstNode left;
      AstNode right;
      cstring flag;
    } binary;
    struct {
      cstring name;
      Bool async;
      Bool generator;
      List args;
      AstNode body;
    } e_function;
    struct {
      Bool async;
      List args;
      AstNode body;
    } e_lambda;
    struct {
      Token tag;
      List args;
      List parts;
    } e_template;

    struct {
      AstNode key;
      enum { OPT_Field, OPT_Method, OPT_Getter, OPT_Setter } type;
      union {
        AstNode field;
        struct {
          Bool async;
          Bool generator;
          List args;
          AstNode body;
        } method, getter, setter;
      };
    } e_oprop;
    struct {
      List properties;
    } e_object;
    struct {
      List items;
    } e_array;
    struct {
      List statements;
    } s_block;

    AstNode s_return, s_yield;

    struct {
      Token label;
      AstNode body;
    } s_label;
    struct {
      Token label;
    } s_break, s_continue;
    struct {
      AssigmentType type;
      AstNode body;
    } s_assigment;

    struct {
      AstNode condition;
      AstNode consequent;
      AstNode alternate;
    } s_if;

    struct {
      AstNode obj;
      AstNode body;
    } s_with;
    struct {
      AstNode condition;
      AstNode body;
    } s_while, s_doWhile;
    struct {
      AstNode condition;
      List body;
    } s_switch;
    struct {
      AstNode condition;
      List body;
    } s_switchPattern;
    struct {
      List exports;
    } s_export;

    struct {
      List imports;
      List attributes;
      Token source;
    } s_import;

    struct {
      enum { IPT_Entity, IPT_Default, IPT_Namespace } type;
      Token local;
      Token imported;
    } s_importPattern;
    struct {
      Token key;
      Token value;
    } s_importAttribute;

    struct {
      AstNode item;
    } s_defualtExport;

    struct {
      AstNode body;
      AstNode init;
      AstNode condition;
      AstNode update;
    } s_for;

    struct {
      AstNode body;
      AstNode iterator;
      AstNode object;
      AssigmentType type;
    } s_forOf, s_forIn;

    struct {
      List body;
    } s_program;

    struct {
      List decorators;
      AstNode key;
      Bool isStatic;
      Bool isPrivate;
      enum { CPT_Field, CPT_Method, CPT_Getter, CPT_Setter } type;
      union {
        AstNode field;
        struct {
          Bool async;
          Bool generator;
          List args;
          AstNode body;
        } method, getter, setter;
      };
    } e_cprop;
    struct {
      List decorators;
      List properties;
      List staticBlocks;
      Token name;
      AstNode extends;
    } e_class;

    Token e_identifier, e_literal;
  };
};
AstNode AstNode_read(SourceFile file, cstring source);

enum {
  ANT_Reserved = 0,

  ANT_Program,

  ANT_EmptyStatement,
  ANT_ExpressionStatement,
  ANT_BlockStatement,
  ANT_WithStatement,
  ANT_IfStatement,
  ANT_WhileStatement,
  ANT_DoWhileStatement,

  ANT_ForStatement,
  ANT_ForInStatement,
  ANT_ForOfStatement,

  ANT_AssigmentStatement,
  ANT_SwitchStatement,
  ANT_SwitchPattern,
  ANT_DefaultExport,
  ANT_ExportStatement,

  ANT_ImportStatement,
  ANT_ImportPattern,
  ANT_ImportAttribute,

  ANT_ReturnStatement,
  ANT_YieldStatement,
  ANT_BreakStatement,
  ANT_ContinueStatement,
  ANT_LabelStatement,
  ANT_Identifier,
  ANT_Literal,
  ANT_Function,
  ANT_Lambda,
  ANT_Template,
  ANT_Object,
  ANT_Array,
  ANT_ObjectProperty,
  ANT_Class,
  ANT_ClassProperty,
  ANT_OptionalCall,
  ANT_Call,
  ANT_Binary,
  ANT_ObjectPattern,
  ANT_ArrayPattern,
};
enum { BT_None, BT_Left, BT_Right, BT_Both };

#define CHECKER(name) static Bool name(SourceFile file, Token token)
#define READER(name) static AstNode name(SourceFile file, cstring source)