#pragma once

namespace Global {
  extern std::string source;

  extern std::vector<Object> variables;
  extern std::vector<AST::Function*> functions;
  extern std::vector<AST::Struct*> structs;
}

[[noreturn]]
void Error(Token const&, std::string const&);

