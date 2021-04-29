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
      return Callfunc(node);

    case Node::Variable:
      return g_variables[node->varIndex];

    default:
    {

    }
  }

  return { };
}

Object RunStmt(Node* node)
{
  if( node == nullptr )
    return { };

  switch( node->type )
  {
    case Node::Assign:
    {
      auto& var = g_variables[node->lhs->varIndex];
      var.var_ptr = &var;

      var = RunExpr(node->rhs);

      return var;
    }

    case Node::If:
    {
      if( RunExpr(node->lhs).eval() )
      {

      }
      else
      {

      }

      break;
    }

    default:
      return RunExpr(node);
  }

  return { };
}