#include "main.h"

ParserCore::ParserCore(std::vector<Object>& variables)
  :variables(variables) { }

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

AST::Expr* ParserCore::Mul()
{
  auto x = Primary();

  while( check() )
  {
    if( consume("*") )
      x = new AST::Expr(AST::Expr::Mul, x, Primary(), csmtok);
    else if( consume("/") )
      x = new AST::Expr(AST::Expr::Div, x, Primary(), csmtok);
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

AST::Expr* ParserCore::Expr()
{
  return Add();
}

AST::Stmt* ParserCore::Stmt()
{
  //
  // if
  if( consume("if") )
  {
    auto tk = csmtok;

    auto ast = new AST::If;
    ast->cond = Expr();
    ast->if_true = new AST::Stmt;
    ast->if_false = new AST::Stmt;

    alart;
    expect("then");
    expect("\n");

    alart;
    auto closed = false;

    while( check() )
    {
      alart;

      if( consume("endif") )
      {
        alart;

        expect("\n");
        closed = true;
        break;
      }
      else if( consume("else") )
      {
        alart;

        expect("\n");

        while( check() )
        {
          alart;

          if( consume("endif") )
          {
            alart;

            expect("\n");
            closed = true;
            break;
          }
          else
          {
            alart;

            ast->if_false->list.emplace_back(Stmt());
          }
        }

        break;
      }
      else if( consume("elseif") )
      {
        alart;

        auto old = ast;
        tk = csmtok;

        alart;

        ast = new AST::If;
        ast->if_true = new AST::Stmt;
        ast->if_false = old;
        ast->cond = Expr();

        alart;

        expect("then");
        expect("\n");
      }
      else
      {
        alart;

        ast->if_true->list.emplace_back(Stmt());
      }
    }

    alart;

    if( !closed )
      Program::Error(*tk, "not closed");

    alart;

    return ast;
  }


  auto& tok = get_tok();

  if( tok.type != Token::Ident )
    Program::Error(tok, "expect instruction name or variable here");

  // assign
  if( index + 1 < tokens.size() && tokens[index + 1].str == "=" )
  {
    auto ast = new AST::Assign;

    ast->var = Primary();
    next();

    ast->value = Expr();
    expect("\n");

    return ast;
  }

  // instruction
  auto ast = new AST::Instruction;
  ast->name = tok.str;
  next();

  if( !consume("\n") )
  {
    do
    {
      ast->args.emplace_back(Expr());
    } while( consume(",") );
    expect("\n");
  }

  return ast;
}

AST::Stmt* ParserCore::Parse()
{
  auto ast = new AST::Stmt(AST::Stmt::Block);

  while( check() )
  {
    ast->list.emplace_back(Stmt());
  }

  return ast;
}

