#include "main.h"

PrgCtx* PrgCtx::Instance;

ProgramContext::ProgramContext() {
  Instance = this;
}

void ProgramContext::Error(Token const& tok, std::string const& msg) {
  auto& source = PrgCtx::Instance->source;
  
  size_t
    line = 1,
    begin = 0,
    end = Instance->source.length();

  for( size_t i = 0; i < tok.srcpos; i++ )   {
    if( source[i] == '\n' )     {
      line++;
      begin = i + 1;
    }
  }

  for( auto i = begin; i < end; i++ ) {
    if( source[i] == '\n' ) {
      end = i;
      break;
    }
  }

  std::cout
    << '\n'
    << "Error:\n"
    << format("%6zd|", line) + source.substr(begin, end - begin) << '\n'
    << "      |" << std::string(tok.srcpos - begin, ' ')
    << "^" << std::string(tok.str.length() > 1 ? tok.str.length() - 1 : 0, '~')
    << "   " << msg << "\n\n";

  exit(1);
}