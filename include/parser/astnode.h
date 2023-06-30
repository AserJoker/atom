#pragma once
#include "tokenizer.h"
#include "util/bool.h"
#include "util/list.h"
#include "util/source.h"
#include "util/strings.h"
struct s_AstNode;
typedef struct s_AstNode *AstNode;
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
    } function;
    Token identifier, literal;
  };
};
AstNode AstNode_read(SourceFile file, cstring source);

enum {
  ANT_Fake = 0,
  ANT_Program,
  ANT_ExpressionStatement,
  ANT_BlockStatement,
  ANT_Identifier,
  ANT_Literal,
  ANT_Function,
  ANT_OptionalCall,
  ANT_Call,
  ANT_Binary,
};
enum { BT_None, BT_Left, BT_Right, BT_Both };

#define CHECKER(name) static Bool name(SourceFile file, Token token)
#define READER(name) static AstNode name(SourceFile file, cstring source)