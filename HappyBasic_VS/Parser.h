#pragma once

namespace ParserCore {

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

}

