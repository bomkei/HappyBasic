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

class AST_Block : public AST
{
public:
  std::vector<AST_Block*> list;

  Object Run();
};

class AST_If : public AST
{
public:
  AST* cond;
  AST_Block* if_true;
  AST_Block* if_false;
};

class AST_While : public AST
{
public:
  AST* condition;
  AST_Block* stmt;
};

