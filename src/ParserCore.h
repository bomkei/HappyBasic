#pragma once


class ParserCore
{
  std::vector<Object>& variables;

  std::vector<Token> tokens;
  size_t index;

  Token* csmtok;

  Token& get_tok()
  {
    return tokens[index];
  }

  bool check()
  {
    return index < tokens.size();
  }

  void swaptoken(std::string const& str)
  {
    get_tok().str = str;
  }

  bool match(std::string const& str)
  {
    return get_tok().str == str;
  }

  bool consume(std::string const& str)
  {
    if( match(str) )
    {
      csmtok = &get_tok();
      next();
      return true;
    }

    return false;
  }

  void expect(std::string const& str)
  {
    if( !consume(str) )
    {
      if( str == "\n" )
        Error(get_tok().srcpos, "expect new line");
      else
        Error(get_tok().srcpos, "expect '" + str + "'");
    }
  }

  void next()
  {
    index++;
  }

  int find_var(std::string const& name)
  {
    for( int i = 0; i < variables.size(); i++ )
      if( variables[i].name == name )
        return i;

    return -1;
  }

public:

  ParserCore(std::vector<Object>& variables)
    :variables(variables) { }

  void Initialize(std::vector<Token>&& tokens)
  {
    this->tokens = std::move(tokens);
  }

  AST::Expr* Primary()
  {
    auto tok = &get_tok();

    switch( tok->type )
    {
    case Token::Number:
    {
      auto ast = new AST::Expr();

      ast->token = tok;
      next();

      return ast;
    }

    case Token::Ident:
    {
      auto ast = new AST::Expr;
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

    Error(tok->srcpos, "syntax error");
  }

  AST::Expr* Mul()
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

  AST::Expr* Expr()
  {
    return Add();
  }

  AST::Stmt* Stmt()
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

      expect("then");
      expect("\n");

      auto closed = false;

      while( check() )
      {
        if( consume("endif") )
        {
          expect("\n");
          closed = true;
          break;
        }
        else if( consume("else") )
        {
          expect("\n");

          while( check() )
          {
            if( consume("endif") )
            {
              expect("\n");
              closed = true;
              break;
            }
            else
              ast->if_false->list.emplace_back(Stmt());
          }
          
          break;
        }
        else if( consume("elseif") )
        {
          auto old = ast;
          tk = csmtok;

          ast = new AST::If;
          ast->if_false = old;
          ast->cond = Expr();

          expect("then");
          expect("\n");
        }
        else
          ast->if_true->list.emplace_back(Stmt());
      }

      if( !closed )
        Error(tk->srcpos, "not closed");

      return ast;
    }


    auto& tok = get_tok();

    if( tok.type != Token::Ident )
      Error(tok.srcpos, "expect instruction name or variable here");

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

  AST::Stmt* Parse()
  {
    auto ast = new AST::Stmt(AST::Stmt::Block);

    while( check() )
    {
      ast->list.emplace_back(Stmt());
    }

    return ast;
  }





};
