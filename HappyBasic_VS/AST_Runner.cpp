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
  namespace {
    size_t CallCount;
    bool* LoopBreaked;
    bool* LoopContinued;
    bool* FuncReturned;
    Object* ReturnValue;

    AST::Function* cur_func_ast; // 実行中の関数
  }

  // グローバル変数オブジェクトを variables に追加します
  // 同じ名前の物が存在する場合、追加しません
  void make_var(AST::Expr* var) {
    // var->type == AST::Expr::Variable

    auto find = find_vector(variables, [] (auto x, auto y) { return x.name == y; }, var->token->str);
    
    if( find == -1 ) {
      variables.emplace_back(var->token->obj);
    }
  }

  // 変数を探して、見つかった場合それへのポインタを返す
  // 見つからない場合 nullptr です
  Object* find_var(std::string const& name) {


    // グローバル変数
    for( auto&& x : variables ) {
      if( x.name == name )
        return &x;
    }

    return nullptr;
  }

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
    else {
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

  // -------------------------------------------------- //
  //  ユーザー定義関数の呼び出し
  // -------------------------------------------------- //
  Object UserFunc(AST::Callfunc* fun) {
    // ユーザー定義関数を探す
    auto find = find_vector(functions, [] (auto x, auto y) { return x->name == y; }, fun->token->str);

    std::vector<Object> args_obj;
    bool returned = false;
    Object retVal;

    if( find == -1 ) { // 見つからない場合エラー
      Error(*fun->token, "undefined function");
    }

    auto func_ast = functions[find];

    if( fun->args.size() != func_ast->args.size() ) {
      Error(*fun->token, "no matching args");
    }

    for( size_t i = 0; i < fun->args.size(); i++ ) {
      args_obj.emplace_back(func_ast->args[i]->token->obj);
      func_ast->args[i]->token->obj = AST_Runner::Expr(fun->args[i]);
    }

    auto oldptr1 = FuncReturned;
    auto oldptr2 = ReturnValue;

    FuncReturned = &returned;
    ReturnValue = &retVal;

    CallCount++;

    AST_Runner::Stmt(func_ast->code);

    CallCount--;

    for( size_t i = 0; i < fun->args.size(); i++ ) {
      func_ast->args[i]->token->obj = args_obj[i];
    }

    FuncReturned = oldptr1;
    ReturnValue = oldptr2;

    return retVal;
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
      // 即値
      case AST::Expr::Immidiate:
        return ast->token->obj;

      // 変数
      case AST::Expr::Variable:
      {
        auto ptr = find_var(ast->token->str);

        if( !ptr ) {
          Error(*ast->token, "cannot use variable before assignment");
        }

        ptr->var_ptr = ptr;
        return *ptr;
      }

      // 関数呼び出し
      case AST::Expr::Callfunc:
        return Function((AST::Callfunc*)ast);

      // 配列
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

      // 配列添え字参照
      case AST::Expr::IndexRef:
      {
        auto obj = Expr(ast->left);
        auto sub = Expr(ast->right);

        // 左辺が配列じゃない場合エラー
        if( obj.type != Object::Array )
          Error(*ast->left->token, "this is not array");

        // インデックスは整数のみ
        if( sub.type != Object::Int )
          Error(*ast->right->token, "subscript type mismatch");

        // 範囲外だったらエラー
        if( sub.v_int < 0 || sub.v_int >= obj.list.size() )
          Error(*ast->right->token, "subscript out of range");

        return obj.list[sub.v_int];
      }

      // 代入
      case AST::Expr::Assign:
      {
        if( ast->left->type == AST::Expr::Variable ) {
          make_var(ast->left);
        }




        break;
      }

      case AST::Expr::MemberAccess:
      {
        break;
      }

      case AST::Expr::Statements:
      {
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

              case Object::Array:
              {
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
          {
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
          }

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

      case AST::Stmt::Block:
      {
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