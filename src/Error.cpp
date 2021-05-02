#include "main.h"


void Program::Error(Token const& tok, std::string const& msg)
{

  throw std::make_pair(tok, msg);

}