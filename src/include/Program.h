#pragma once

class Program
{
public:

  static Program* instance;

  //---------- global ----------------
  // variables
  std::vector<Object> variables;

  // functions
  std::vector<AST::Function*> functions;
  
  // classes
  std::vector<AST::Class*> classes;
  //----------------------------------

  std::string source;

  Tokenizer* tokenizer;
  ParserCore* parser;

  AST::Stmt* prs_result;
  AST::Function* cur_func;

  i64 find_func(std::string const&) const;

  Program();

  void OpenFile();

  void Tokenize();
  void Parse();

  Object Run();

  [[noreturn]]
  static void Error(Token const& tok, std::string const& msg);
};
