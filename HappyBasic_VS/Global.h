#pragma once

namespace AST {
  class Function;
  class Struct;
}

namespace Global {
  extern std::string source;

  extern std::vector<Object> variables;
  extern std::vector<AST::Function*> functions;
  extern std::vector<AST::Struct*> structs;
}

namespace Options {
  extern std::string FileName;
  extern bool ViewNodes;
  extern bool IsSafety;
}

[[noreturn]]
void Error(Token const&, std::string const&);

