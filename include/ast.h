#pragma once
#include "source.h"
#include "strings.h"
struct s_Token;
typedef struct s_Token *Token;

typedef enum e_TokenType {
  TT_Number,
  TT_BigInt,
  TT_String,
  TT_Keyword,
  TT_Symbol,
  TT_Identifier,
  TT_Regex,
  TT_Comment,
  TT_Eof,
  TT_Template,
  TT_TemplateStart,
  TT_TemplatePart,
  TT_TemplateEnd,
  TT_Newline,
  TT_Interpreter,
  TT_Init
} TokenType;

struct s_Token {
  Location _location;
  strings _raw;
  TokenType _type;
};

typedef struct s_Error {
  cstring error;
  Location location;
} Error;
struct s_SourceFile;
typedef struct s_SourceFile *SourceFile;
SourceFile SourceFile_read(cstring filename);
void SourceFile_dispose(SourceFile sf);
Token readToken(SourceFile file, cstring source);
void Token_dispose(Token token);
void initTokenizerContext();
void uninitTokenizerContext();
Error getTokenizerError();

struct s_AstNode;
typedef struct s_AstNode *AstNode;

struct s_Interpreter;
typedef struct s_Interpreter *Interpreter;

struct s_Directive;
typedef struct s_Directive *Directive;

struct s_Statement;
typedef struct s_Statement *Statement;

struct s_Program;
typedef struct s_Program *Program;

typedef enum e_NodeType {
  NT_Interpreter,
  NT_Directive,
  NT_Program,
  NT_ImportStatement,
} NodeType;

AstNode parse(SourceFile file);