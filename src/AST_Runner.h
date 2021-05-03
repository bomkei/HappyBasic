#pragma once

class AST_Runner
{
public:
  static Object Run_Expr(AST::Expr* ast);
  static Object Run_Stmt(AST::Stmt* ast);

};