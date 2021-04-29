#include "main.h"

namespace
{
  std::vector<Token> g_tokens;
  size_t tkIndex;
  size_t csmpos;

  auto curtok()
  {
    return g_tokens[tkIndex];
  }

  auto next(int n = 1)
  {
    tkIndex += n;
  }

  auto check()
  {
    return tkIndex < g_tokens.size();
  }

  auto consume(std::string const& str)
  {
    if( curtok().str == str )
    {
      csmpos = curtok().srcpos;
      next();
      return true;
    }

    return false;
  }

  auto expect(std::string const& str)
  {
    if( consume(str) == false )
    {

    }
  }
}

Node::Node(Node::Type type)
  :type(type), lhs(nullptr), rhs(nullptr)
{

}

Node::Node(Node::Type type, Node* lhs, Node* rhs)
  :Node(type)
{
  this->lhs = lhs;
  this->rhs = rhs;
}

