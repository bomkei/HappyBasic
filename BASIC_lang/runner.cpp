#include "main.h"

Object Callfunc(Node *node)
{
  auto const& name = node->tok.str;
  auto const& args = node->list;
  
  Object ret;

  if( name == "range" )
  {
    ret.type = Object::Array;

    if( args.size() != 1 )
      node->tok.Error("illegal call function");

    auto max = RunExpr(args[0]);

    if( max.type != Object::Int )
      node->tok.Error("parameter is must be a integer");

    for( int i = 0; i < max.v_int; i++ )
    {
      Object x;
      x.v_int = i;
      ret.list.emplace_back(x);
    }

    return ret;
  }

  node->tok.Error("undefined function");
}

void Instruction(Node* node)
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
    return;
  }



  node->tok.Error("undefined instruction");
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
    {
      auto &var = g_variables[node->varIndex];
      var.var_ptr = &var;
      return var;
    }

    case Node::Array:
    {
      Object obj;
      obj.type = Object::Array;

      for( auto&& i : node->list )
        obj.list.emplace_back(RunExpr(i));

      return obj;
    }

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
      auto var = RunExpr(node->lhs);

      if( var.var_ptr == nullptr )
        node->tok.Error("cannot assignment to not a lvalue object");

      *var.var_ptr = RunExpr(node->rhs);
      break;
    }

    case Node::Block:
      for( auto&& x : node->list )
        RunStmt(x);

      break;

    case Node::Instruction:
      Instruction(node);
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

    case Node::For:
    {
      auto it = RunExpr(node->lhs);
      auto arr = RunExpr(node->rhs);

      if( it.var_ptr == nullptr )
        node->lhs->tok.Error("cannot use not a lvalue for iterator");

      if( arr.type != Object::Array )
        node->rhs->tok.Error("cannot iterate not array object");

      for( auto&& i : arr.list )
      {
        *it.var_ptr = i;
        RunStmt(node->list[0]);
      }

      break;
    }

    default:
      return RunExpr(node);
  }

  return { };
}