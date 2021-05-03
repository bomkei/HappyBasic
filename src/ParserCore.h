#pragma once


class ParserCore
{
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
      Error(get_tok().srcpos, "expect '" + str + "'");
  }

  void next()
  {
    index++;
  }

public:
  
  void Initialize(std::vector<Token>&& tokens)
  {
    this->tokens = std::move(tokens);
  }

  AST::Expr* Primary()
  {
    if( !check() )
      Error(tokens[index - 1].srcpos, "syntax error");
    
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
  
  AST::If* Parse_if()
  {
    
  }
  
  AST::For* Parse_for()
  {
    
  }
  
  AST::Stmt* Parse()
  {
    auto ast = new AST::Stmt(AST::Stmt::Block);
    
    while( check() )
    {
      
      
      auto x = new AST::Stmt(AST::Stmt::Default);
      x->expr = Expr();
      ast->list.emplace_back(x);
    }
    
    return ast;
  }
  
  
  
  
  
};
