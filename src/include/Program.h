#pragma once

class Program
{
  static Program* instance;

  std::vector<Object> variables;
  std::vector<AST::Function*> functions;
  std::vector<AST::Class*> classes;

  std::string source;

  Tokenizer* tokenizer;
  ParserWrap* parser;

  AST::Stmt* prs_result;
  AST::Function* cur_func;

  i64 find_func(std::string const&) const;

  friend class AST_Runner;

public:
  Program();

  void OpenFile();

  void Tokenize();
  void Parse();

  Object Run();

  [[noreturn]]
  static void Error(Token const& tok, std::string const& msg);
};
