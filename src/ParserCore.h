#pragma once


class ParserCore
{
  std::vector<Token> tokens;
  size_t index;
  
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
    return nullptr;
  }

  AST::Expr* Mul()
  {
    return nullptr;
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
    return nullptr;
  }
  
  
  
  
  
};
