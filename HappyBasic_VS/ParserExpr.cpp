#include <iostream>
#include <string>
#include <vector>

#include "Object.h"
#include "Token.h"
#include "Parser.h"

#include "AST.h"

namespace {
  Token*& token;
  Token* csmtok = nullptr;

  bool in_function = false;
  std::vector<AST::Expr*>* func_args = nullptr;

  bool in_struct = false;
  AST::Struct* cur_struct = nullptr;

  bool DontPlace_v = false;
}

Token& get_tok()
{
  return *token;
}

bool check()
{
  return token->type != Token::End;
}

bool match(std::string const& str)
{
  return token->str == str;
}

bool consume(std::string const& str)
{
  if( token->str == str )
  {
    csmtok = token;
    next();
    return true;
  }

  return false;
}

void expect(std::string const& str)
{
  if( !consume(str) )
    PrgCtx::Error(*token, "expect '" + str + "'");
}

void next()
{
  token = token->next;
}

int find_var(std::string const& name)
{
  for( int i = 0; i < variables.size(); i++ )
    if( variables[i].name == name )
      return i;

  return -1;
}

AST::Expr* Primary()
{
  if( consume("(") )
  {
    // ����
    if( get_tok().str == "{" ) {
      auto x = new AST::Expr;
      x->stmt = Statements();
      x->type = AST::Expr::Statements;
      expect(")");
      return x;
    }

    auto x = Expr();
    expect(")");
    return x;
  }

  //
  // �z��
  if( consume("[") )
  {
    auto x = new AST::Array;
    x->token = csmtok;

    if( !consume("]") )
    {
      do
      {
        x->elems.emplace_back(Expr());
      } while( consume(",") );

      expect("]");
    }

    return x;
  }

  auto tok = &get_tok();

  switch( tok->type )
  {
    // ���l
    case Token::Char:
    case Token::String:
    case Token::Number:
    {
      auto ast = new AST::Expr;

      ast->type = AST::Expr::Immidiate;
      ast->token = tok;
      next();

      return ast;
    }

    // ���ʎq
    case Token::Ident:
    {
      next();

      //
      // �X�R�[�v�������Z�q
      if( consume("::") ) {

      }

      // �֐��Ăяo��
      if( consume("(") ) {
        auto ast = new AST::Callfunc;
        ast->token = tok;

        // �s���S�ł���ꍇ�A�g�p�s�\
        if( cur_struct && cur_struct->name == tok->str ) {
          PrgCtx::Error(*tok, "cannot use incomplete struct");
        }

        if( !consume(")") ) {
          do {
            ast->args.emplace_back(Expr());
          } while( consume(",") );
          expect(")");
        }

        return ast;
      }

      // �֐��̒��ɂ���ꍇ�A������T��
      if( in_function ) {
        for( auto&& i : *func_args )
          if( i->token->str == tok->str ) {
            return i;
          }
      }

      // �\���̂̒��ɂ���ꍇ�A�����o�[����T��
      if( in_struct ) {
        for( auto&& i : cur_struct->member_list ) {
          if( i->type == AST::Stmt::Var &&
            i->expr->left->token->str == tok->str ) {
            
            alart;
            std::cout
              << "var '" + tok->str + "' is exists in struct '" + cur_struct->name + "'\n"
              << "will return this from Primary()\n";

            return i->expr->left;
          }
        }
      }

      auto ast = new AST::Expr;
      ast->type = AST::Expr::Variable;
      ast->token = tok;

      if( !DontPlace_v ) {
        if( (ast->varIndex = find_var(tok->str)) == -1 ) {
          ast->varIndex = variables.size();
          variables.emplace_back(tok->obj);
        }
      }

      return ast;
    }

  }

  PrgCtx::Error(*tok, "syntax error");
}

AST::Expr* IndexRef()
{
  auto x = Primary();

  while( check() )
  {
    if( consume("[") )
    {
      x = new AST::Expr(AST::Expr::IndexRef, x, Expr(), csmtok);
      expect("]");
    }
    else
      break;
  }

  return x;
}

AST::Expr* MemberAccess() {
  auto x = IndexRef();

  DontPlace_v = true;

  while( check() ) {
    if( consume(".") ) x = new AST::Expr(AST::Expr::MemberAccess, x, IndexRef(), csmtok);
    else
      break;
  }

  DontPlace_v = false;

  return x;
}

AST::Expr* Unary()
{
  if( consume("-") )
    return new AST::Expr(AST::Expr::Sub, AST::Expr::FromInt(0), MemberAccess(), csmtok);

  if( consume("new") ) {
    DontPlace_v = true;
    
    auto x = new AST::Expr(AST::Expr::New, MemberAccess(), nullptr, csmtok);
    DontPlace_v = false;
    
    return x;
  }

  return MemberAccess();
}

AST::Expr* Mul()
{
  auto x = Unary();

  while( check() )
  {
    if( consume("*") )
      x = new AST::Expr(AST::Expr::Mul, x, Unary(), csmtok);
    else if( consume("%") )
      x = new AST::Expr(AST::Expr::Mod, x, Unary(), csmtok);
    else if( consume("/") )
      x = new AST::Expr(AST::Expr::Div, x, Unary(), csmtok);
    else
      break;
  }

  return x;
}

AST::Expr* Add()
{
  auto x = Mul();

  while( check() )
  {
    if( consume("+") )
      x = new AST::Expr(AST::Expr::Add, x, Mul(), csmtok);
    else if( consume("-") )
      x = new AST::Expr(AST::Expr::Sub, x, Mul(), csmtok);
    else
      break;
  }

  return x;
}

AST::Expr* Shift()
{
  auto x = Add();

  while( check() )
  {
    if( consume("<<") )
      x = new AST::Expr(AST::Expr::Shift, x, Add(), csmtok);
    else if( consume(">>") )
      x = new AST::Expr(AST::Expr::Shift, Add(), x, csmtok);
    else
      break;
  }
  
  return x;
}

AST::Expr* Compare() {
  auto x = Shift();

  while( check() ) {
    if( consume(">") )
      x = new AST::Expr(AST::Expr::Bigger, x, Shift(), csmtok);
    else if( consume("<") )
      x = new AST::Expr(AST::Expr::Bigger, Shift(), x, csmtok);
    else if( consume(">=") )
      x = new AST::Expr(AST::Expr::BiggerOrEqual, x, Shift(), csmtok);
    else if( consume("<=") )
      x = new AST::Expr(AST::Expr::BiggerOrEqual, Shift(), x, csmtok);
    else
      break;
  }

  return x;
}

AST::Expr* Equal() {
  auto x = Compare();

  while( check() ) {
    if( consume("==") )
      x = new AST::Expr(AST::Expr::Equal, x, Compare(), csmtok);
    else if( consume("!=") )
      x = new AST::Expr(AST::Expr::NotEqual, x, Compare(), csmtok);
    else break;
  }

  return x;
}

AST::Expr* LogAND() {
  auto x = Equal();

  while( check() ) {
    if( consume("and") )
      x = new AST::Expr(AST::Expr::LogAND, x, Equal(), csmtok);
    else break;
  }

  return x;
}

AST::Expr* LogOR() {
  auto x = LogAND();

  while( check() ) {
    if( consume("or") )
      x = new AST::Expr(AST::Expr::LogOR, x, LogAND(), csmtok);
    else break;
  }

  return x;
}

AST::Expr* Assign() {
  auto x = LogOR();
  auto tk = &get_tok();

  if( consume("=") )
    x = new AST::Expr(AST::Expr::Assign, x, Assign(), tk);

  else if( consume("+=") )
    x = new AST::Expr(AST::Expr::Assign, x, new AST::Expr(AST::Expr::Add, x, Assign(), tk), tk);
  else if( consume("-=") )
    x = new AST::Expr(AST::Expr::Assign, x, new AST::Expr(AST::Expr::Sub, x, Assign(), tk), tk);
  else if( consume("*=") )
    x = new AST::Expr(AST::Expr::Assign, x, new AST::Expr(AST::Expr::Mul, x, Assign(), tk), tk);
  else if( consume("%=") )
    x = new AST::Expr(AST::Expr::Assign, x, new AST::Expr(AST::Expr::Mod, x, Assign(), tk), tk);
  else if( consume("/=") )
    x = new AST::Expr(AST::Expr::Assign, x, new AST::Expr(AST::Expr::Div, x, Assign(), tk), tk);


  return x;
}

void Debug(AST::Expr*);

AST::Expr* Expr()
{
  auto expr = Assign();

  //expr->Optimize();
  
  return expr;
}

AST::Block* Parse()
{
  auto ast = new AST::Block;

  while( check() )
  {
    ast->list.emplace_back(Statements());
  }

  return ast;
}