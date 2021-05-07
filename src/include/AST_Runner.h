#pragma once

class AST_Runner
{
public:
  static size_t CallCount;

  static bool* LoopBreaked;
  static bool* LoopContinued;
  static bool* FuncReturned;
  static Object* ReturnValue;

  static Object Expr(AST::Expr* ast);
  static Object Stmt(AST::Stmt* ast);

  static Object Function(AST::Callfunc* ast);
  static void Instruction(AST::Instruction* inst);

  static Object UserFunc(AST::Callfunc* fun);

};