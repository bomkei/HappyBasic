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

AST::Expr* ParserCore::Unary()
{
  if( consume("-") )
    return new AST::Expr(AST::Expr::Sub, AST::Expr::FromInt(0), IndexRef(), csmtok);

  return IndexRef();
}

AST::Expr* ParserCore::Mul()
{
  auto x = Unary();

  while( check() )
  {
    if( consume("*") )
      x = new AST::Expr(AST::Expr::Mul, x, Unary(), csmtok);
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

AST::Expr* ParserCore::Expr()
{
  auto expr = Equal();

  //Debug(expr);

  return expr;
}

void Debug(AST::Expr*);

AST::Stmt* ParserCore::Stmt()
{
  //
  // if
  if( consume("if") )
  {
    AST::If::Pair pair;
    auto closed = false;

    auto ast = new AST::If;
    ast->token = csmtok;

    pair = std::make_pair(Expr(), new AST::Block);

    expect("then");
    expect("\n");

    while( check() )
    {
      if( consume("endif") )
      {
        expect("\n");
        ast->pairs.emplace_back(pair);
        closed = true;
        break;
      }
      else if( consume("elseif") )
      {
        ast->pairs.emplace_back(pair);

        pair = std::make_pair(Expr(), new AST::Block);

        expect("then");
        expect("\n");
      }
      else if( consume("else") )
      {
        ast->pairs.emplace_back(pair);
        pair = std::make_pair(AST::Expr::FromInt(1), new AST::Block);

        expect("\n");

        while( check() )
        {
          if( consume("endif") )
          {
            expect("\n");
            closed = true;
            break;
          }

          std::get<1>(pair)->list.emplace_back(Stmt());
        }

        ast->pairs.emplace_back(pair);
        break;
      }
      else
      {
        std::get<1>(pair)->list.emplace_back(Stmt());
      }
    }

    if( !closed )
    {
      Program::Error(*ast->token, "not closed");
    }

    return ast;
  }


  //
  // for
  if( consume("for") )
  {
    auto ast = new AST::For;
    ast->token = csmtok;

    ast->counter = Expr();

    expect("=");
    ast->begin = Expr();

    expect("to");
    ast->end = Expr();

    ast->code = new AST::Block;

    expect("\n");

    auto closed = false;

    while( check() )
    {
      if( consume("next") )
      {
        expect("\n");
        closed = true;
        break;
      }

      ast->code->list.emplace_back(Stmt());
    }

    if( !closed )
    {
      Program::Error(*ast->token, "not closed");
    }

    return ast;
  }

  //
  // def
  if( consume("def") )
  {
    auto tk = csmtok;

    if( get_tok().type != Token::Ident )
      Program::Error(get_tok(), "expect identifier after 'def' keyword");

    auto& name = get_tok().str;
    next();

    std::vector<AST::Expr*> args;
    std::vector<AST::Stmt*> block;

    expect("(");

    if( !consume(")") )
    {
      do
      {
        if( get_tok().type != Token::Ident )
          Program::Error(get_tok(), "syntax error");
        
        args.emplace_back(Primary());
      } while( consume(",") );

      expect(")");
    }

    auto closed = false;

    while( check() )
    {
      if( consume("enddef") )
      {
        closed = true;
        break;
      }

      block.emplace_back(Stmt());
    }

    auto ast = new AST::Function;
    ast->token = tk;
    ast->name = name;
    ast->args = std::move(args);

    ast->code = new AST::Block;
    ast->code->list = std::move(block);
    
    return ast;
  }


  auto& tok = get_tok();

  if( tok.type != Token::Ident )
    Program::Error(tok, "expect instruction name or variable here");

  // assign
  if( index + 1 < tokens.size() && tokens[index + 1].str == "=" )
  {
    auto ast = new AST::Assign;
    ast->token = &tok;

    ast->var = Primary();
    next();

    ast->value = Expr();
    expect("\n");

    return ast;
  }

  // instruction
  auto ast = new AST::Instruction;
  ast->name = tok.str;
  ast->token = &tok;
  next();

  if( !consume("\n") )
  {
    do
    {
      ast->args.emplace_back(Expr());
    } while( consume(",") );
    expect("\n");
  }

  //alart;
  //Debug(ast->args[0]);

  //alart;

  //std::cout << ast->args[0]->equal(*ast->args[1]) << '\n';
  
  return ast;
}

AST::Stmt* ParserCore::Parse()
{
  auto ast = new AST::Block;

  while( check() )
  {
    ast->list.emplace_back(Stmt());
  }

  //alart;
  return ast;
}