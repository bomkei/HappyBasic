#include "main.h"

Object Callfunc(Node *node)
{
  auto const& name = node->tok.str;
  auto const& args = node->list;
  
  Object ret;

  //
  // make array with range
  if( name == "range" )
  {
    ret.type = Object::Array;

    switch( args.size() )
    {
      case 1:
      {
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

      case 2:
      case 3:
      {
        auto begin = RunExpr(args[0]);
        auto end = RunExpr(args[1]);

        Object inc;
        inc.v_int = 1;
        if( args.size() == 3 ) inc = RunExpr(args[2]);

        if( begin.type != Object::Int || end.type != Object::Int || inc.type != Object::Int )
          node->tok.Error("parameter is must be a integer");

        if( inc.v_int < 1 )
          args[2]->tok.Error("cannot use negative number or zero for inclement value");

        for( int i = begin.v_int; i < end.v_int; i += inc.v_int )
        {
          Object x;
          x.v_int = i;
          ret.list.emplace_back(x);
        }

        return ret;
      }

    }

    node->tok.Error("illegal call function");
  }

  //
  // casting to integer
  if( name == "int" )
  {
    if( args.size() != 1 )
      node->tok.Error("illegal call function");

    auto str = RunExpr(args[0]).to_string();

    try
    {
      // hex
      if( is_hex(str) )
        ret.v_int = std::stoi(str, nullptr, 16);
      
      // dec
      else
        ret.v_int = std::stoi(str);
    }
    catch( ... )
    {
      args[0]->tok.Error("cannot cast to integer");
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

bool AdjustObjectType(Object& a, Object& b)
{
  if( (a.type == Object::Array) != (b.type == Object::Array) )
    return false;

  if( a.type == Object::Float || b.type == Object::Float )
  {
    for( auto&& x : { &a, &b } )
    {
      if( x->type == Object::Int )
        x->v_float = x->v_int;
      else if( x->type == Object::Char )
        x->v_float = x->v_char;

      x->type = Object::Float;
    }
  }
  else
  {
    for( auto&& x : { &a, &b } )
    {
      if( x->type == Object::Char )
        x->v_int = x->v_char;

      x->type = Object::Int;
    }
  }

  return true;
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

    case Node::IndexRef:
    {
      auto obj = RunExpr(node->lhs);

      if( obj.type != Object::Array )
        node->tok.Error("this is not array");

      auto index = RunExpr(node->rhs);

      if( index.type != Object::Int )
        node->rhs->tok.Error("subscript is must be a integer");

      if( index.v_int < 0 || index.v_int >= obj.list.size() )
        node->rhs->tok.Error("subscript out of range");

      return obj.list[index.v_int];
    }

    case Node::MemberAccess:
    {
      auto obj = RunExpr(node->lhs);

      node->tok.Error("not implemented");
    }

    default:
    {
      auto&& lhs = std::move(RunExpr(node->lhs));
      auto&& rhs = std::move(RunExpr(node->rhs));

      if( AdjustObjectType(lhs, rhs) == false )
        node->tok.Error("type mismatch");

      switch( node->type )
      {
        case Node::Add:
          lhs.v_int += rhs.v_int;
          lhs.v_char += rhs.v_char;
          lhs.v_float += rhs.v_float;
          break;

        case Node::Sub:
          lhs.v_int -= rhs.v_int;
          lhs.v_char -= rhs.v_char;
          lhs.v_float -= rhs.v_float;
          break;

        case Node::Mul:
          lhs.v_int *= rhs.v_int;
          lhs.v_char *= rhs.v_char;
          lhs.v_float *= rhs.v_float;
          break;

        case Node::Div:
          lhs.v_int /= rhs.v_int;
          lhs.v_char /= rhs.v_char;
          lhs.v_float /= rhs.v_float;
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