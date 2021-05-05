#pragma once

class ParserCore
{
  std::vector<Object>& variables;
  std::vector<AST::Function*>& functions;

  std::vector<Token> tokens;
  size_t index;

  Token* csmtok;
  bool in_function;

  Token& get_tok();

  bool check();
  void swaptoken(std::string const& str);
  bool match(std::string const& str);
  bool consume(std::string const& str);
  void expect(std::string const& str);
  void next();
  int find_var(std::string const& name);

public:

  ParserCore(std::vector<Object>& variables, std::vector<AST::Function*>& functions);

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

  AST::Stmt* Stmt();

  AST::Stmt* Parse();


};