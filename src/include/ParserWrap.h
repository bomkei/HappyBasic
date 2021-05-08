#pragma once

class ParserWrap : public ParserCore
{
public:
  ParserWrap(
    std::vector<Object>& variables,
    std::vector<AST::Function*>& functions,
    std::vector<AST::Class*>& classes
  )
    :ParserCore(variables, functions, classes)
  {

  }


  AST::Block* Parse();


};