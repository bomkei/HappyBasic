#pragma once

class ParserCore {
  std::vector<Object>& variables;
  std::vector<AST::Function*>& functions;
  std::vector<AST::Struct*>& structs;

  Token*& token;
  Token* csmtok = nullptr;
  
  bool in_function = false;
  std::vector<AST::Expr*>* func_args = nullptr;

  bool in_struct = false;
  AST::Struct* cur_struct = nullptr;

  bool DontPlace_v = false;

  Token& get_tok();

  bool check();
  bool match(std::string const& str);
  bool consume(std::string const& str);
  void expect(std::string const& str);
  void next();
  int find_var(std::string const& name);

public:

  ParserCore(
    Token*& topToken,
    std::vector<Object>& variables,
    std::vector<AST::Function*>& functions,
    std::vector<AST::Struct*>& structs
  );

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