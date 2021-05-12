#include "main.h"

ParserCore::ParserCore(
  std::vector<Object>& variables,
  std::vector<AST::Function*>& functions,
  std::vector<AST::Class*>& classes
)
  :variables(variables),
  functions(functions),
  classes(classes),
  in_function(false),
  func_args(nullptr)
{

}

Token& ParserCore::get_tok()
{
  return tokens[index];
}

bool ParserCore::check()
{
  return index < tokens.size();
}

void ParserCore::swaptoken(std::string const& str)
{
  get_tok().str = str;
}

bool ParserCore::match(std::string const& str)
{
  return get_tok().str == str;
}

bool ParserCore::consume(std::string const& str)
{
  if( match(str) )
  {
    csmtok = &get_tok();
    next();
    return true;
  }

  return false;
}

void ParserCore::expect(std::string const& str)
{
  if( !consume(str) )
  {
    if( str == "\n" )
    {
      index--;
      Program::Error(get_tok(), "expect new line after this token");
    }
    else
      Program::Error(get_tok(), "expect '" + str + "'");
  }
}

void ParserCore::next()
{
  index++;
}

int ParserCore::find_var(std::string const& name)
{
  for( int i = 0; i < variables.size(); i++ )
    if( variables[i].name == name )
      return i;

  return -1;
}

void ParserCore::Initialize(std::vector<Token>&& tokens)
{
  this->tokens = std::move(tokens);
}

AST::Expr* ParserCore::Primary()
{
  if( consume("(") )
  {
    auto x = Expr();
    expect(")");
    return x;
  }

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
    case Token::Char:
    case Token::String:
    case Token::Number:
    {
      auto ast = new AST::Expr();

      ast->type = AST::Expr::Immidiate;
      ast->token = tok;
      next();

      return ast;
    }

    case Token::Ident:
    {
      // callfunc
      if( index + 1 < tokens.size() && tokens[index + 1].str == "(" )
      {
        auto ast = new AST::Callfunc;
        ast->token = tok;
        index += 2;

        if( !consume(")") )
        {
          do
          {
            ast->args.emplace_back(Expr());
          } while( consume(",") );
          expect(")");
        }

        return ast;
      }

      // find parameter
      if( in_function )
      {
        for( auto&& i : *func_args )
        {
          if( i->token->str == tok->str )
          {
            next();
            return i;
          }
        }
      }

      // class
      if( in_class ) {
        for( auto&& i : cur_class->member_list ) {
          if( i->type == AST::Stmt::Var && i->expr->left->token->str == tok->str ) {
            next();
            return i->expr->left;
          }
        }
      }

      auto ast = new AST::Expr;
      ast->type = AST::Expr::Variable;
      ast->token = tok;
      next();

      auto find = find_var(tok->str);

      if( find == -1 )
      {
        find = variables.size();
        variables.emplace_back(tok->obj);
      }

      ast->varIndex = find;

      return ast;
    }

  }

  Program::Error(*tok, "syntax error");
}

AST::Expr* ParserCore::IndexRef()
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

AST::Expr* ParserCore::MemberAccess() {
  auto x = IndexRef(); {
    while( check() ) {
      if( consume(".") ) x = new AST::Expr(AST::Expr::MemberAccess, x, IndexRef(), csmtok);
      else
        break;
    }
  }
  return x;
}

AST::Expr* ParserCore::Unary()
{
  if( consume("-") )
    return new AST::Expr(AST::Expr::Sub, AST::Expr::FromInt(0), MemberAccess(), csmtok);

  if( consume("New") )
  {
    auto tk = csmtok;
    auto fun = Primary();

    if( fun->type != AST::Expr::Callfunc )
      Program::Error(*tk, "expect call function expression after 'new'");

    return new AST::Expr(AST::Expr::New, fun, nullptr, tk);
  }

  return MemberAccess();
}

AST::Expr* ParserCore::Mul()
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

AST::Expr* ParserCore::Add()
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

AST::Expr* ParserCore::Shift()
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

AST::Expr* ParserCore::Compare()
{
  auto x = Shift();

  while( check() )
  {
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

AST::Expr* ParserCore::Equal()
{
  auto x = Compare();

  while( check() )
  {
    if( consume("==") )
      x = new AST::Expr(AST::Expr::Equal, x, Compare(), csmtok);
    else if( consume("!=") )
      x = new AST::Expr(AST::Expr::NotEqual, x, Compare(), csmtok);
    else
      break;
  }

  return x;
}

AST::Expr* ParserCore::Assign()
{
  auto x = Equal();
  auto tk = &get_tok();

  if( consume("=") ) {
    x = new AST::Expr(AST::Expr::Assign, x, Assign(), tk);
  }
  else if( consume("+=") ) {
    x = new AST::Expr(AST::Expr::Assign, x, new AST::Expr(AST::Expr::Add, x, Assign(), tk), tk);
  }
  else if( consume("-=") ) {
    x = new AST::Expr(AST::Expr::Assign, x, new AST::Expr(AST::Expr::Sub, x, Assign(), tk), tk);
  }
  else if( consume("*=") ) {
    x = new AST::Expr(AST::Expr::Assign, x, new AST::Expr(AST::Expr::Mul, x, Assign(), tk), tk);
  }
  else if( consume("%=") ) {
    x = new AST::Expr(AST::Expr::Assign, x, new AST::Expr(AST::Expr::Mod, x, Assign(), tk), tk);
  }
  else if( consume("/=") ) {
    x = new AST::Expr(AST::Expr::Assign, x, new AST::Expr(AST::Expr::Div, x, Assign(), tk), tk);
  }

  return x;
}

void Debug(AST::Expr*);

AST::Expr* ParserCore::Expr()
{
  auto expr = Assign();

  expr->Optimize();
  
  return expr;
}

AST::Block* ParserCore::Parse()
{
  auto ast = new AST::Block;

  while( check() )
  {
    ast->list.emplace_back(Statements());
  }

  return ast;
}