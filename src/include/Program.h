#pragma once

#include <memory>

class ProgramImpl;
class Program
{
  std::unique_ptr<ProgramImpl> _impl;
  static inline Program* _instance;

  friend void CheckExpr(AST::Expr*);

public:
  Program();
  ~Program();

  void OpenFile();

  void Tokenize();
  void Parse();
  void Check();

  void ViewNodes();

  Object* GetVariable(std::string const&) const;
  std::vector<Object> GetVariables() const;

  AST::Function* GetFunction(std::string const&) const;
  AST::Function*& GetCurrentFunction() const;

  Object Run();

  static Program* GetInstance();

  [[noreturn]]
  static void Error(Token const& tok, std::string const& msg);
};
