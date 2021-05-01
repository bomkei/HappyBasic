#include "main.h"


Token& ParserCore::get_tok()
{
  return tokens[index];
}

bool ParserCore::check()
{
  return index < tokens.size();
}

bool ParserCore::match(std::string const& str)
{
  return get_tok().str == str;
}

bool ParserCore::consume(std::string const& str)
{
  if( match(str) )
  {
    next();
    return true;
  }

  return false;
}

void ParserCore::expect(std::string const& str)
{
  if( !consume(str) )
    Program::Error(get_tok(), "expect '" + str + "'");
}

void ParserCore::next()
{
  index++;
}



void ParserCore::Initialize(std::vector<Token>&& tokens)
{
  this->tokens = std::move(tokens);
}

AST* ParserCore::Primary()
{
  return nullptr;
}

AST* ParserCore::Mul()
{
  return nullptr;
}

AST* ParserCore::Add()
{
  return nullptr;
}

AST* ParserCore::Expr()
{
  return nullptr;
}

AST_Block* ParserCore::Parse()
{
  return nullptr;
}


