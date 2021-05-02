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
      Error(get_tok(), "expect '" + str + "'");
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
    
    Error(*tok, "syntax error");
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
    return nullptr;
  }

  AST::Expr* Expr()
  {
    return nullptr;
  }
  
  
  AST::If* Parse_if()
  {
    
  }
  
  AST::For* Parse_for()
  {
    
  }
  
  AST::Stmt* Parse()
  {
    auto x = new AST::Stmt();
    
//    x->list.emplace_back();
    
    return x;
  }
  
  
  
  
  
};
