#pragma once

class AST
{
public:
  AST* lhs = nullptr;
  AST* rhs = nullptr;
  Token* tok = nullptr;

  int varIndex = 0;

  AST(AST* lhs, AST* rhs, Token* tok = nullptr)
    :lhs(lhs), rhs(rhs), tok(tok)
  {

  }
};

class AST_IF : public AST
{
public:
  AST* cond;
  AST* if_true;
  AST* if_false;
};

class AST_WHILE : public AST
{
public:
  AST* condition;
  AST* stmt;
};

