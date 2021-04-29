#include "main.h"

Object Callfunc(Node *node)
{
  auto const& name = node->tok.str;
  auto const& args = node->list;
  
  if( name == "print" )
  {
    for( auto&& i : args )
    {
      std::cout << RunExpr(i).to_string();
    }

    std::cout << '\n';
    return { };
  }

  node->tok.Error("undefined function");
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
      auto&& lhs = std::move(RunExpr(node->lhs));
      auto&& rhs = std::move(RunExpr(node->rhs));

      switch( node->type )
      {
        case Node::Add:
          lhs.v_int += rhs.v_int;
          break;

        case Node::Sub:
          lhs.v_int -= rhs.v_int;
          break;

        case Node::Mul:
          lhs.v_int *= rhs.v_int;
          break;

        case Node::Div:
          lhs.v_int /= rhs.v_int;
          break;
      }

      return lhs;
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

    case Node::Block:
      for( auto&& x : node->list )
        RunStmt(x);

      break;

    case Node::If:
    {
      if( RunExpr(node->lhs).eval() )
      {
        RunStmt(node->rhs);
      }
      else
      {
        RunStmt(node->list[0]);
      }

      break;
    }

    default:
      return RunExpr(node);
  }

  return { };
}