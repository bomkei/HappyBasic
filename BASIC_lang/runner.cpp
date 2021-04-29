#include "main.h"

Object Callfunc(Node *node)
{

}

Object RunExpr(Node* node)
{
  if( node == nullptr )
    return { };

  switch( node->type )
  {
    case Node::Immidiate:
      return node->tok.obj;

    case Node::Callfunc:

  }

  return { };
}

Object RunStmt(Node* node)
{
  if( node == nullptr )
    return { };

  switch( node->type )
  {

  }

  return { };
}