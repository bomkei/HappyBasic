#pragma once

class AST_Runner
{
public:
  static Object Expr(AST::Expr* ast);
  static Object Stmt(AST::Stmt* ast);

  static Object Function(AST::Callfunc* ast);
  static void Instruction(AST::Instruction* inst);

};