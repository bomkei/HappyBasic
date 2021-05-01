#include "main.h"

Program::Program()
{
  tokenizer = new Tokenizer(source);
  parser = new ParserCore();
}

void Program::Tokenize()
{
  parser->Initialize(tokenizer->Tokenize());
}

void Program::Parse()
{

}

Object Program::Run() const
{



  return Object();
}

