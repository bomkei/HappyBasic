#pragma once

namespace AST_Runner
{
  Object Expr(AST::Expr* ast);
  Object Stmt(AST::Stmt* ast);

  Object Function(AST::Callfunc* ast);
  Object UserFunc(AST::Callfunc* fun);

  Object BuiltInMember(AST::Expr*);
};