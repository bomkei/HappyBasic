#pragma once

class ParserCore
{
  std::vector<Object>& variables;
  std::vector<AST::Function*>& functions;
  std::vector<AST::Class*>& classes;

  //std::vector<Token>& tokens;
  //size_t index = 0;
  Token*& token;
  Token* csmtok = nullptr;
  
  bool in_function = false;
  std::vector<AST::Expr*>* func_args = nullptr;

  bool in_class = false;
  AST::Class* cur_class = nullptr;

  Token& get_tok();

  bool check();
  void swaptoken(std::string const& str);
  bool match(std::string const& str);
  bool consume(std::string const& str);
  void expect(std::string const& str);
  void next();
  int find_var(std::string const& name);

public:

  ParserCore(
    //std::vector<Token>& tokens,
    Token*& topToken,
    std::vector<Object>& variables,
    std::vector<AST::Function*>& functions,
    std::vector<AST::Class*>& classes
  );

  //void Initialize(std::vector<Token>&& tokens);

  AST::Expr* Primary();
  AST::Expr* IndexRef();
  AST::Expr* MemberAccess();
  AST::Expr* Unary();
  AST::Expr* Mul();
  AST::Expr* Add();
  AST::Expr* Shift();
  AST::Expr* Compare();
  AST::Expr* Equal();
  AST::Expr* LogAND();
  AST::Expr* LogOR();
  AST::Expr* Assign();
  AST::Expr* Expr();

  AST::Stmt* Statements();
  AST::Block* Parse();

};