#pragma once

namespace Compiler {
  enum CodeType {
    MoveRegister,
    MoveImmidiate,
    
    Add,
    Sub,
    Mul,
    Div,

    AddI,
    SubI,
    MulI,
    DivI,

    Push,
    Pop,

    Jump,
    Call,
    Return,


  };

  std::vector<u8> const& GetCodes();

  void CompileExpr(AST::Expr*);
  void CompileStmt(AST::Stmt*);
}