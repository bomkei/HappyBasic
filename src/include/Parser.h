#pragma once

class ParserCore
{
public:
  std::vector<Object>& variables;
  std::vector<AST::Function*>& functions;
  std::vector<AST::Class*>& classes;

  std::vector<Token> tokens;
  size_t index;

  Token* csmtok;
  
  bool in_function;
  std::vector<AST::Expr*>* func_args;

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

  ParserCore(
    std::vector<Object>& variables,
    std::vector<AST::Function*>& functions,
    std::vector<AST::Class*>& classes
  );

  void Initialize(std::vector<Token>&& tokens);

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

  AST::If* If();
  AST::For* For();
  AST::While* While();
  AST::While* DoWhile();
  AST::Function* Define();
  AST::Class* Class();

  AST::Stmt* Statements();
  AST::Block* Parse();

};