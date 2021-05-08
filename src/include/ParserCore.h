#pragma once

class ParserCore
{
public:
  std::vector<Object>& variables;
  std::vector<AST::Function*>& functions;
  std::vector<AST::Class*> classes;

  std::vector<Token> tokens;
  size_t index;

  Token* csmtok;
  bool in_function;
  std::vector<AST::Expr*>* func_args;

  Token& get_tok();

  bool check();
  void swaptoken(std::string const& str);
  bool match(std::string const& str);
  bool consume(std::string const& str);
  void expect(std::string const& str);
  void next();
  int find_var(std::string const& name);

  ParserCore(
    std::vector<Object>& variables,
    std::vector<AST::Function*>& functions,
    std::vector<AST::Class*>& classes
  );

  void Initialize(std::vector<Token>&& tokens);

  AST::Expr* Primary();
  AST::Expr* IndexRef();
  AST::Expr* Unary();
  AST::Expr* Mul();
  AST::Expr* Add();
  AST::Expr* Shift();
  AST::Expr* Compare();
  AST::Expr* Equal();
  AST::Expr* Expr();

  AST::Stmt* If();
  AST::Stmt* For();
  AST::Stmt* While();
  AST::Stmt* DoWhile();
  AST::Stmt* Define();
  AST::Stmt* Class();
  AST::Stmt* Statements();


  //AST::Stmt* Stmt();

  //AST::Stmt* Parse();


};