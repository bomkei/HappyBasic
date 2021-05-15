#pragma once

struct ProgramContext {
  static ProgramContext* Instance;

  ProgramContext();

  Token* token = nullptr;
  std::string source;

  std::vector<Object> variables;
  std::vector<AST::Function*> functions;
  std::vector<AST::Struct*> structs;

  [[noreturn]]
  static void Error(Token const&, std::string const&);
};

using PrgCtx = ProgramContext;

