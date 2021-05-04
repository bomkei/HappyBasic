#include "main.h"

void AST_Runner::Instruction(AST::Instruction* ast)
{
  auto const& name = ast->name;
  std::vector<Object> args;

  for( auto&& i : ast->args )
    args.emplace_back(AST_Runner::Expr(i));

  if( name == "print" )
  {
    for( auto&& i : args )
      std::cout << i.to_string();

    std::cout << '\n';
    return;
  }

  Program::Error(*ast->token, "undefined instruction");
}


