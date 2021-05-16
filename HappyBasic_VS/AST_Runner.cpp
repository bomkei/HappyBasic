#include <iostream>

#include "Utils.h"

#include "Object.h"
#include "Token.h"
#include "AST.h"

#include "Parser.h"
#include "AST_Runner.h"

#include "Global.h"

using namespace Global;

namespace AST_Runner {
  size_t CallCount;
  bool* LoopBreaked;
  bool* LoopContinued;
  bool* FuncReturned;
  Object* ReturnValue;

  void ObjectAdjuster(Object& L, Object& R) {
    if( L.type == Object::Array || R.type == Object::Array )
      return;

    if( L.type == Object::Float || R.type == Object::Float ) {
      for( auto&& obj : { &L, &R } ) {
        if( obj->type == Object::Int )
          obj->v_float = obj->v_int;
        else if( obj->type == Object::Char )
          obj->v_float = obj->v_char;

        obj->type = Object::Float;
      }
    }
    else if( L.type != R.type ) {
      for( auto&& obj : { &L, &R } ) {
        if( obj->type == Object::Char )
          obj->v_int = obj->v_char;

        obj->type = Object::Int;
      }
    }
  }

  Object Function(AST::Callfunc* ast) {
    auto& name = ast->token->str;
    std::vector<Object> args;
    Object ret;

    for( auto&& i : ast->args ) {
      args.emplace_back(Expr(i));
    }

    //
    // print
    if( name == "print" ) {
      for( auto&& i : args )
        std::cout << i.ToString();

      std::cout << '\n';
    }

    //
    // range
    else if( name == "range" ) {
      ret.type = Object::Array;

      switch( args.size() ) {
        case 1:
          if( args[0].type != Object::Int )
            Error(*(ast->args[0]->token), "must be a integer");

          for( int i = 0; i < args[0].v_int; i++ ) {
            Object obj;
            obj.v_int = i;
            ret.list.emplace_back(obj);
          }

          break;

        case 2:
          if( args[0].type != Object::Int )
            Error(*(ast->args[0]->token), "must be a integer");

          if( args[1].type != Object::Int )
            Error(*(ast->args[1]->token), "must be a integer");

          for( int i = args[0].v_int; i < args[1].v_int; i++ ) {
            Object obj;
            obj.v_int = i;
            ret.list.emplace_back(obj);
          }

          break;

        default:
          Error(*ast->token, "no matching args");
      }
    }

    //
    // length
    else if( name == "length" ) {
      if( args.size() != 1 )
        Error(*ast->token, "no matching args");

      if( args[0].type != Object::Array )
        Error(*(ast->args[0]->token), "this is not array");

      ret.v_int = args[0].list.size();
    }

    //
    // random
    else if( name == "random" ) {
      for( auto&& i : args )
        if( i.type != Object::Int )
          Error(*ast->token, "only use integer in args of random()");

      auto begin = args.size() > 1 ? args[0].v_int : 0;
      auto end = args[args.size() > 1].v_int;

      if( begin > end )
        Error(*ast->args[0]->token, "begin > end");

      if( begin == end )
        ret.v_int = begin;
      else
        ret.v_int = Utils::Random(begin, end);
    }

    //
    // randomstr
    else if( name == "randomstr" ) {
      if( args.size() > 1 )
        Error(*ast->token, "no matching args");

      auto len = args.size() ? args[0].v_int : 30;

      auto str = Utils::GetRandomStr<std::string>(len);

      ret.type = Object::Array;

      for( auto&& c : str ) {
        Object ch;
        ch.type = Object::Char;
        ch.v_char = c;
        ret.list.emplace_back(ch);
      }
    }

    //
    // int
    else if( name == "int" ) {
      if( args.size() != 1 )
        Error(*ast->token, "no matching args");

      switch( args[0].type ) {
        case Object::Int:
          break;

        case Object::Float:
          args[0].v_int = args[0].v_float;
          break;

        case Object::Char:
          args[0].v_int = args[0].v_char;
          break;

        case Object::Array:
        {
          try {
            auto str = args[0].ToString();
            args[0].v_int = std::stoi(str, nullptr, str.find("0x") == std::string::npos ? 10 : 16);
          }
          catch( ... ) {
            Error(*ast->args[0]->token, "cannot cast to integer");
          }
        }
      }

      args[0].type = Object::Int;
      ret = args[0];
    }

    //
    // to_string
    else if( name == "to_string" ) {
      if( args.size() != 1 )
        Error(*ast->token, "no matching args");

      try {
        auto str = args[0].ToString();

        ret.type = Object::Array;

        for( auto&& c : str ) {
          Object ch;
          ch.type = Object::Char;
          ch.v_char = c;
          ret.list.emplace_back(ch);
        }
      }
      catch( ... ) {
        Error(*ast->args[0]->token, SERIOUS_ERROR);
      }
    }

    else {
      return AST_Runner::UserFunc(ast);
    }

    return ret;
  }

  Object UserFunc(AST::Callfunc* fun) {

    return { };
  }

  Object BuiltInMember(AST::Expr* expr) {
    

    return { };
  }

  Object Expr(AST::Expr* ast)
  {
    if( !ast )
      return { };
    
    switch( ast->type )
    {
      case AST::Expr::Lv_obj: {
        auto& v = ast->token->obj;
        v.var_ptr = &v;
        return v;
      }

      case AST::Expr::Immidiate:
        return ast->token->obj;

      case AST::Expr::Variable: {
        if( ast->varIndex == -1 ) {
          Error(*ast->token, SERIOUS_ERROR);
        }

        auto& var = variables[ast->varIndex];
        var.var_ptr = &var;
        return var;
      }

      case AST::Expr::Callfunc:
        return Function((AST::Callfunc*)ast);

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
          Error(*ast->left->token, "this is not array");

        if( sub.type != Object::Int )
          Error(*ast->right->token, "subscript type mismatch");

        if( sub.v_int < 0 || sub.v_int >= obj.list.size() )
          Error(*ast->right->token, "subscript out of range");

        return obj.list[sub.v_int];
      }

      case AST::Expr::Param:
      {
        auto& p = ast->token->obj;
        p.var_ptr = &p;
        return p;
      }

      case AST::Expr::Assign:
      {
        auto dest = Expr(ast->left);
        auto src = Expr(ast->right);

        if( !dest.var_ptr )
          Error(*ast->token, "cannot assign to rvalue");

        *dest.var_ptr = src;
        dest.var_ptr->name = dest.name;

        return src;
      }

      case AST::Expr::New: {
        if( ast->left->type != AST::Expr::Callfunc ) {
          Error(*ast->token, "expected call func after this token");
        }

        auto& name = ast->left->token->str;
        auto find = find_vector(structs, [] (auto s, auto n) { return s->name == n; }, name);

        if( find == -1 ) {
          Error(*ast->left->token, "this is doesnt exists");
        }

        auto ptr = structs[find];

        Object ret;
        ret.type = Object::StructObj;
        ret.struct_ptr = ptr;

        for( auto&& i : ptr->member_list ) {
          switch( i->type ) {
            case AST::Stmt::Var: {
              auto vv = Expr(i->expr);
              vv.name = i->expr->left->token->str;
              ret.list.emplace_back(vv);
              break;
            }
          }
        }

        return ret;
      }

      case AST::Expr::MemberAccess:
      {
        auto obj = Expr(ast->left);

        if( obj.type != Object::StructObj ) {
          // TODO: BuiltInMember
          Error(*ast->token, "not struct");
        }

        if( ast->right->type != AST::Expr::Variable ) {
          Error(*ast->right->token, "syntax error");
        }

        auto& name = ast->right->token->str;

        for( auto&& i : obj.list ) {
          if( i.name == name )
            return i;
        }

        Error(*ast->right->token, "dont have the member '" + name + "'");
      }

      case AST::Expr::MemberVariable: {

        break;
      }

      case AST::Expr::Statements: {
        return Stmt(ast->stmt);
      }

      default:
      {
        auto left = Expr(ast->left);
        auto right = Expr(ast->right);

        ObjectAdjuster(left, right);

        switch( ast->type )
        {
          case AST::Expr::Add:
            switch( left.type ) {
              case Object::Int: left.v_int += right.v_int; break;
              case Object::Char: left.v_char += right.v_char; break;
              case Object::Float: left.v_float += right.v_float; break;
              
              case Object::Array: {
                if( right.type == Object::Array ) {
                  for( auto&& i : right.list )
                    left.list.emplace_back(i);
                }
                else {
                  left.list.emplace_back(right);
                }
                break;
              }
            }
            break;

          case AST::Expr::Sub:
            switch( left.type ) {
              case Object::Int: left.v_int -= right.v_int; break;
              case Object::Char: left.v_char -= right.v_char; break;
              case Object::Float: left.v_float -= right.v_float; break;
              case Object::Array: Error(*ast->token, "type mismatch");
            }
            break;

          case AST::Expr::Mul:
            if( (left.type == Object::Array) != (right.type == Object::Array) )
            {
              if( left.type != Object::Array )
                std::swap(left, right);

              if( right.type != Object::Int )
                Error(*ast->token, "type mismatch");
              
              auto list = left.list;

              for( int i = 1; i < right.v_int; i++ )
              {
                for( auto&& obj : list )
                  left.list.emplace_back(obj);
              }

              break;
            }
            
            if( !left.Eval() || !right.Eval() )
            {
              left.v_int = 0;
              break;
            }

            switch( left.type ) {
              case Object::Int: left.v_int *= right.v_int; break;
              case Object::Char: left.v_char *= right.v_char; break;
              case Object::Float: left.v_float *= right.v_float; break;
            }
            break;

          case AST::Expr::Mod:
            if( !right.Eval() )
            {
              Error(*ast->token, "cant division with zero");
            }
            else if( !left.Eval() )
              break;
            else if( right.type != Object::Int )
              Error(*ast->token, "only can use integer at mod operator");

            switch( left.type ) {
              case Object::Int: left.v_int /= right.v_int; break;
              case Object::Char: left.v_char /= right.v_char; break;
              case Object::Float: Error(*ast->token, "cannot mod float");
              case Object::Array: Error(*ast->token, "type mismatch");
            }
            break;
            
          case AST::Expr::Div:
            if( !right.Eval() )
            {
              Error(*ast->token, "cant division with zero");
            }
            else if( !left.Eval() )
              break;

            switch( left.type ) {
              case Object::Int: left.v_int /= right.v_int; break;
              case Object::Char: left.v_char /= right.v_char; break;
              case Object::Float: left.v_float /= right.v_float; break;
              case Object::Array: Error(*ast->token, "type mismatch");
            }
            break;

          case AST::Expr::Shift:
            switch( left.type ) {
              case Object::Int: left.v_int <<= right.v_int; break;
              case Object::Char: left.v_char <<= right.v_char; break;
              case Object::Float: Error(*ast->token, "cannot shift float-type object");
              case Object::Array: Error(*ast->token, "type mismatch");
            }
            break;

          case AST::Expr::Bigger:
            switch( left.type ) {
              case Object::Int: left.v_int = left.v_int > right.v_int; break;
              case Object::Char: left.v_int = left.v_char > right.v_char; break;
              case Object::Float: left.v_int = left.v_float > right.v_float; break;
              case Object::Array: Error(*ast->token, "type mismatch");
            }
            left.type = Object::Int;
            break;

          case AST::Expr::BiggerOrEqual:
            switch( left.type ) {
              case Object::Int: left.v_int = left.v_int >= right.v_int; break;
              case Object::Char: left.v_int = left.v_char >= right.v_char; break;
              case Object::Float: left.v_int = left.v_float >= right.v_float; break;
              case Object::Array: Error(*ast->token, "type mismatch");
            }
            left.type = Object::Int;
            break;

          case AST::Expr::Equal:
            left.v_int = left.Equal(right);
            left.type = Object::Int;
            break;

          case AST::Expr::NotEqual:
            left.v_int = !left.Equal(right);
            left.type = Object::Int;
            break;

          case AST::Expr::LogAND:
            left.v_int = left.Eval() && right.Eval();
            left.type = Object::Int;
            break;
            
          case AST::Expr::LogOR:
            left.v_int = left.Eval() || right.Eval();
            left.type = Object::Int;
            break;


        }

        return left;
      }
    }

    return { };
  }

  Object Stmt(AST::Stmt* ast)
  {
    if( !ast )
      return { };

    switch( ast->type ) {
      case AST::Stmt::Struct:
      case AST::Stmt::Function:
        break;

      case AST::Stmt::Block: {
        Object obj;

        for( auto&& i : ((AST::Block*)ast)->list ) {
          obj = Stmt(i);

          if( LoopBreaked && *LoopBreaked ) break;
          if( LoopContinued && *LoopContinued ) break;
          if( FuncReturned && *FuncReturned ) break;
        }

        return obj;
      }

      case AST::Stmt::Break:
        if( !LoopBreaked )
          Error(*ast->token, "cannot use 'break' here");

        *LoopBreaked = true;
        break;

      case AST::Stmt::Continue:
        if( !LoopContinued )
          Error(*ast->token, "cannot use 'continue' here");

        *LoopContinued = true;
        break;

      case AST::Stmt::Return:
        if( !ReturnValue )
          Error(*ast->token, "cannot use 'return' here");

        *FuncReturned = true;
        return *ReturnValue = Expr(ast->expr);

      case AST::Stmt::If:
      {
        if( Expr(((AST::If*)ast)->cond).Eval() ) {
          return Stmt(((AST::If*)ast)->code);
        }
        else if( ((AST::If*)ast)->elseCode ) {
          return Stmt(((AST::If*)ast)->elseCode);
        }

        break;
      }

      //
      // For-statement
      case AST::Stmt::For:
      {
        Object obj;
        size_t index = 0;

        // save pointers
        auto oldptr1 = LoopBreaked;
        auto oldptr2 = LoopContinued;

        // make new flags
        bool flag1 = false, flag2 = false;

        // set new pointer
        LoopBreaked = &flag1;
        LoopContinued = &flag2;

        while( 1 ) {
          auto it = Expr(((AST::For*)ast)->iterator);
          auto list = Expr(((AST::For*)ast)->list);

          if( !it.var_ptr )
            Error(*(((AST::For*)ast)->iterator->token), "this isnt a lvalue");

          if( list.type != Object::Array )
            Error(*ast->token, "cannot iterate in not an array type object");

          if( index >= list.list.size() )
            break;

          *it.var_ptr = list.list[index];
          *LoopBreaked = *LoopContinued = false;

          obj = Stmt(((AST::For*)ast)->code);
          index++;

          if( *LoopBreaked )
            break;

          if( FuncReturned && *FuncReturned )
            break;
        }

        // restore pointers
        LoopBreaked = oldptr1;
        LoopContinued = oldptr2;

        return obj;
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

        while( Expr(while_ast->cond).Eval() )
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

      case AST::Stmt::Var:
      default:
        return Expr(ast->expr);
    }

    if( ReturnValue )
      return *ReturnValue;

    return { };
  }
}