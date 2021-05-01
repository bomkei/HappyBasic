#include "main.h"


void Program::Error(Token* tok, std::string const& msg)
{

  throw std::make_pair(tok, msg);

}