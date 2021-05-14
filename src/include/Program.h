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

  void ViewNodes();

  AST::Function* GetFunction(std::string const&) const;
  Object* GetVariable(std::string const&) const;

  std::vector<Object> GetVariables() const;

 // std::vector<AST::Class*>& GetClasses() const;
  //AST::Class*& GetCurrentClass() const;
  AST::Function*& GetCurrentFunction() const;

  Object Run();

  static Program* GetInstance();

  [[noreturn]]
  static void Error(Token const& tok, std::string const& msg);
};
