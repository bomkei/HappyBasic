#include "main.h"

size_t AST_Runner::CallCount = false;
bool* AST_Runner::LoopBreaked = nullptr;
bool* AST_Runner::LoopContinued = nullptr;
bool* AST_Runner::FuncReturned = nullptr;
Object* AST_Runner::ReturnValue = nullptr;

void ObjectAdjuster(Object& L, Object& R)
{
  if( L.type == Object::Array || R.type == Object::Array )
    return;

  if( L.type == Object::Float || R.type == Object::Float )
  {
    for( auto&& obj : { &L, &R } )
    {
      if( obj->type == Object::Int )
        obj->v_float = obj->v_int;
      else if( obj->type == Object::Char )
        obj->v_float = obj->v_char;

      obj->type = Object::Float;
    }
  }
  else if( L.type != R.type )
  {
    for( auto&& obj : { &L, &R } )
    {
      if( obj->type == Object::Char )
        obj->v_int = obj->v_char;

      obj->type = Object::Int;
    }
  }
}

Object AST_Runner::Expr(AST::Expr* ast)
{
  if( !ast )
    return { };
  
  switch( ast->type )
  {
    case AST::Expr::Immidiate:
      return ast->token->obj;

    case AST::Expr::Variable:
    {
      auto& var = Program::instance->variables[ast->varIndex];
      var.var_ptr = &var;
      return var;
    }

    case AST::Expr::Callfunc:
      return Function(reinterpret_cast<AST::Callfunc*>(ast));

    case AST::Expr::Array:
    {
      Object ret;
      ret.type = Object::Array;

      for( auto&& i : ((AST::Array*)ast)->elems )
      {
        ret.list.emplace_back(Expr(i));
      }

      return ret;
    }

    case AST::Expr::IndexRef:
    {
      auto obj = Expr(ast->left);
      auto sub = Expr(ast->right);

      if( obj.type != Object::Array )
        Program::Error(*ast->left->token, "this is not array");

      if( sub.type != Object::Int )
        Program::Error(*ast->right->token, "subscript type mismatch");

      if( sub.v_int < 0 || sub.v_int >= obj.list.size() )
        Program::Error(*ast->right->token, "subscript out of range");

      return obj.list[sub.v_int];
    }

    case AST::Expr::Param:
    {
      auto& p = ast->token->obj;
      p.var_ptr = &p;
      return p;
    }

    default:
    {
      auto left = Expr(ast->left);
      auto right = Expr(ast->right);

      ObjectAdjuster(left, right);

      switch( ast->type )
      {
        case AST::Expr::Add:
          left.v_int += right.v_int;
          left.v_char += right.v_char;
          left.v_float += right.v_float;
          break;

        case AST::Expr::Sub:
          left.v_int -= right.v_int;
          left.v_char -= right.v_char;
          left.v_float -= right.v_float;
          break;

        case AST::Expr::Mul:
          if( (left.type == Object::Array) != (right.type == Object::Array) )
          {
            if( left.type != Object::Array )
              std::swap(left, right);

            if( right.type != Object::Int )
              Program::Error(*ast->token, "type mismatch");
            
            auto list = left.list;

            for( int i = 1; i < right.v_int; i++ )
            {
              for( auto&& obj : list )
                left.list.emplace_back(obj);
            }

            break;
          }
          
          if( !left.eval() || !right.eval() )
          {
            left.v_int = 0;
            break;
          }

          left.v_int *= right.v_int;
          left.v_char *= right.v_char;
          left.v_float *= right.v_float;
          break;

        case AST::Expr::Div:
          if( !right.eval() )
          {
            Program::Error(*ast->token, "cant division with zero");
          }
          else if( !left.eval() )
            break;

          left.v_int /= right.v_int;
          left.v_char /= right.v_char;
          left.v_float /= right.v_float;
          break;

        case AST::Expr::Shift:
          left.v_int <<= right.v_int;
          left.v_char <<= right.v_char;
          break;

        case AST::Expr::Bigger:
          switch( left.type ) {
            case Object::Int: left.v_int = left.v_int > right.v_int; break;
            case Object::Char: left.v_int = left.v_char > right.v_char; break;
            case Object::Float: left.v_int = left.v_float > right.v_float; break;
          }
          left.type = Object::Int;
          break;

        case AST::Expr::BiggerOrEqual:
          switch( left.type ) {
            case Object::Int: left.v_int = left.v_int >= right.v_int; break;
            case Object::Char: left.v_int = left.v_char >= right.v_char; break;
            case Object::Float: left.v_int = left.v_float >= right.v_float; break;
          }
          left.type = Object::Int;
          break;

        case AST::Expr::Equal:
          switch( left.type ) {
            case Object::Int: left.v_int = left.v_int == right.v_int; break;
            case Object::Char: left.v_int = left.v_char == right.v_char; break;
            case Object::Float: left.v_int = left.v_float == right.v_float; break;
          }
          left.type = Object::Int;
          break;

        case AST::Expr::NotEqual:
          switch( left.type ) {
            case Object::Int: left.v_int = left.v_int != right.v_int; break;
            case Object::Char: left.v_int = left.v_char != right.v_char; break;
            case Object::Float: left.v_int = left.v_float != right.v_float; break;
          }
          left.type = Object::Int;
          break;



      }

      return left;
    }
  }

  return { };
}

Object AST_Runner::Stmt(AST::Stmt* ast)
{
  if( !ast )
    return { };

  switch( ast->type )
  {
    case AST::Stmt::Function:
      break;

    case AST::Stmt::Block:
    {
      for( auto&& i : ((AST::Block*)ast)->list )
      {
        Stmt(i);

        if( LoopBreaked && *LoopBreaked )
          break;
        
        if( LoopContinued && *LoopContinued )
          break;

        if( FuncReturned && *FuncReturned )
          break;
      }

      break;
    }

    case AST::Stmt::Assign:
    {
      auto var = Expr(((AST::Assign*)ast)->var);
      auto value = Expr(((AST::Assign*)ast)->value);

      return *(var.var_ptr) = value;
    }

    case AST::Stmt::Instruction:
    {
      Instruction((AST::Instruction*)(ast));
      break;
    }
    
    case AST::Stmt::Break:
      if( !LoopBreaked )
        Program::Error(*ast->token, "cannot use 'break' here");

      *LoopBreaked = true;
      break;

    case AST::Stmt::Continue:
      if( !LoopContinued )
        Program::Error(*ast->token, "cannot use 'continue' here");

      *LoopContinued = true;
      break;

    case AST::Stmt::Return:
      if( !ReturnValue )
        Program::Error(*ast->token, "cannot use 'return' here");

      *ReturnValue = Expr(((AST::Return*)ast)->expr);
      *FuncReturned = true;
      break;

    case AST::Stmt::If:
    {
      for( auto&& pair : ((AST::If*)ast)->pairs )
      {
        auto cond = Expr(std::get<0>(pair));

        if( cond.eval() )
          return Stmt(std::get<1>(pair));
      }

      break;
    }

    case AST::Stmt::For:
    {
      auto for_ast = (AST::For*)(ast);

      auto counter = Expr(for_ast->counter);
      auto begin = Expr(for_ast->begin);

      // save pointers
      auto oldptr1 = LoopBreaked;
      auto oldptr2 = LoopContinued;

      // make new flags
      bool flag1 = false, flag2 = false;

      // set new pointer
      LoopBreaked = &flag1;
      LoopContinued = &flag2;

      if( !counter.var_ptr )
        Program::Error(*(for_ast->counter->token), "not a lvalue");

      if( begin.type != Object::Int )
        Program::Error(*(for_ast->begin->token), "must be a integer");

      *(counter.var_ptr) = begin;

      while( 1 )
      {
        auto end = Expr(for_ast->end);

        if( end.type != Object::Int )
          Program::Error(*(for_ast->end->token), "must be a integer");

        if( counter.var_ptr->v_int > end.v_int )
          break;

        *LoopBreaked = *LoopContinued = false;
        Stmt(for_ast->code);

        if( *LoopBreaked )
          break;

        if( FuncReturned && *FuncReturned )
          break;

        counter.var_ptr->v_int++;
      }
      
      // restore pointers
      LoopBreaked = oldptr1;
      LoopContinued = oldptr2;

      break;
    }

    case AST::Stmt::While:
    {
      auto while_ast = (AST::While*)ast;

      // save pointers
      auto oldptr1 = LoopBreaked;
      auto oldptr2 = LoopContinued;

      // make new flags
      bool flag1 = false, flag2 = false;

      // set new pointer
      LoopBreaked = &flag1;
      LoopContinued = &flag2;

      while( Expr(while_ast->cond).eval() )
      {
        *LoopBreaked = *LoopContinued = false;
        Stmt(while_ast->code);

        if( *LoopBreaked )
          break;

        if( FuncReturned && *FuncReturned )
          break;

      }

      // restore pointers
      LoopBreaked = oldptr1;
      LoopContinued = oldptr2;

      break;
    }

  }

  if( ReturnValue )
    return *ReturnValue;

  return { };
}



