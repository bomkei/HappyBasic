#include "main.h"

bool is_hex(std::string str)
{
  for( auto&& c : str )
    if( isalpha(c) ) return true;

  return false;
}

