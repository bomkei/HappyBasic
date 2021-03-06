#include <iostream>

#include "Utils.h"

#include "Object.h"
#include "Token.h"
#include "AST.h"

#include "Parser.h"
#include "Global.h"

using namespace Global;

std::string Mangle(std::string fun_name, std::vector<std::string> class_layers, std::vector<std::string> args);

namespace ParserCore {

  //
  // 構文解析で使用される変数です
  namespace {
    Token* token;   // 指しているトークン
    Token* csmtok;  // consume() が呼ばれた時点でのトークン

    // ------------------------------------------------
    // 関数に関する情報
    bool in_function;          // 関数を解析中かどうか
    AST::Function* cur_func;   // 解析中の関数へのポインタ

    // ------------------------------------------------
    // 解析中の構造体またはクラスに関する情報です
    bool in_struct;            // 構造体を解析中かどうか
    bool in_class;             // クラスかどうか
    AST::Struct* cur_struct;   // 解析している構造体へのポインタ
    AST::Expr* this_ptr;       // クラス内で関数の解析を開始するとき、自動で引数として追加される this へのポインタ
                               // また、パーサがその関数内でメンバ変数の参照に当たった場合、this に対してのメンバアクセスとして解析されます
    
    std::vector<std::string> struct_layer;
    

    //------------------------------------------------------------
    // 変数の自動配置を制御するフラグです
    // true になっている場合は、自動配置をしません
    // これは主にメンバアクセスをパースしてるときに使用します
    // 例: A.b.c という場合、A のみが 確認 & 自動配置 される
    //     b と c はされません
    //------------------------------------------------------------
    bool DontPlace_v;


    Token& get_tok()
    {
      return *token;
    }

    bool check()
    {
      return token->type != Token::End;
    }

    void next()
    {
      token = token->next;
    }

    bool match(std::string const& str)
    {
      return token->str == str;
    }

    bool consume(std::string const& str)
    {
      if( token->str == str )
      {
        csmtok = token;
        next();
        return true;
      }

      return false;
    }

    void expect(std::string const& str)
    {
      if( !consume(str) )
        Error(*token, "expect '" + str + "'");
    }

    int find_var(std::string const& name)
    {
      for( int i = 0; i < variables.size(); i++ )
        if( variables[i].name == name )
          return i;

      return -1;
    }
  } // unnamed namespace


  void Init(Token* tok) {
    token = tok;
  }

  AST::Expr* Primary()
  {
    if( consume("(") )
    {
      // 複文
      if( get_tok().str == "{" ) {
        auto x = new AST::Expr;
        x->stmt = Statements();
        x->type = AST::Expr::Statements;
        expect(")");
        return x;
      }

      auto x = Expr();
      expect(")");
      return x;
    }

    //
    // 配列
    if( consume("[") )
    {
      auto x = new AST::Array;
      x->token = csmtok;

      if( !consume("]") )
      {
        do
        {
          x->elems.emplace_back(Expr());
        } while( consume(",") );

        expect("]");
      }

      return x;
    }

    auto tok = &get_tok();

    switch( tok->type )
    {
      // 即値
      case Token::Char:
      case Token::String:
      case Token::Number:
      {
        auto ast = new AST::Expr;

        ast->type = AST::Expr::Immidiate;
        ast->token = tok;
        next();

        return ast;
      }

      // 識別子
      case Token::Ident:
      {
        AST::Struct* st_ptr = nullptr;

        auto find = find_vector(structs, [] (auto x, auto y) { return x->name == y; }, get_tok().str);
        if( find != -1 ) st_ptr = structs[find];

        next();

        // スコープ解決演算子
        while( consume("::") ) {
          if( !st_ptr )
            Error(get_tok(), "no");

          auto find_s = find_vector(st_ptr->member_list,
            [] (auto x, auto y) { return x->type == AST::Stmt::Struct && ((AST::Struct*)x)->name == y; }, get_tok().str);

          if( find_s != -1 )
            st_ptr = (AST::Struct*)st_ptr->member_list[find_s];
          else
            Error(get_tok(), "huh");

          next();
        }


        // 関数呼び出し
        if( consume("(") ) {
          auto ast = new AST::Callfunc;
          ast->token = tok;
          ast->st_ptr = st_ptr;

          // 不完全である場合、使用不可能 //
          if( cur_struct && cur_struct->name == tok->str ) {
            Error(*tok, "cannot use incomplete struct");
          }

          if( !consume(")") ) {
            auto _d = DontPlace_v;
            DontPlace_v = true;

            do {
              ast->args.emplace_back(Expr());
            } while( consume(",") );
            
            DontPlace_v = _d;

            expect(")");
          }

          return ast;
        }

        //// 関数の中にいる場合、引数を探す
        //if( in_function ) {
        //  for( auto&& i : cur_func->args )
        //    if( i->token->str == tok->str ) {
        //      return i;
        //    }
        //}

        //// クラス / 構造体
        //if( in_struct ) {
        //  for( auto&& i : cur_struct->member_list ) {
        //    if( i->type == AST::Stmt::Var &&
        //      i->expr->left->token->str == tok->str ) {

        //      if( in_class ) { // クラス
        //        auto xx = new AST::Expr;
        //        xx->type = AST::Expr::MemberAccess;
        //        xx->left = this_ptr;
        //        xx->right = i->expr->left;

        //        return xx;
        //      }
        //      else { // 構造体
        //        return i->expr->left;
        //      }
        //    }
        //  }
        //}

        auto ast = new AST::Expr;
        ast->type = AST::Expr::Variable;
        ast->token = tok;

        //if( !DontPlace_v ) {
        //  if( (ast->varIndex = find_var(tok->str)) == -1 ) {
        //    ast->varIndex = variables.size();
        //    variables.emplace_back(tok->obj);
        //  }
        //}

        return ast;
      }

    }

    Error(*tok, "syntax error ,,,,");
  }

  AST::Expr* IndexRef()
  {
    auto x = Primary();

    while( check() )
    {
      if( consume("[") )
      {
        x = new AST::Expr(AST::Expr::IndexRef, x, Expr(), csmtok);
        expect("]");
      }
      else
        break;
    }

    return x;
  }

  AST::Expr* MemberAccess() {
    auto x = IndexRef();

    DontPlace_v = true;

    while( check() ) {
      if( consume(".") ) {
        x = new AST::Expr(AST::Expr::MemberAccess, x, IndexRef(), csmtok);

        //if( x->right->type == AST::Expr::Callfunc ) {
        //  auto& args = ((AST::Callfunc*)x->right)->args;
        //  args.insert(args.begin(), x->left);
        //  x = x->right;

        //  //alart;
        //  //std::cout << ((AST::Callfunc*)x)->st_ptr->name << '\n';
        //  

        //}
      }
      else
        break;
    }

    DontPlace_v = false;

    return x;
  }

  AST::Expr* Unary()
  {
    if( consume("-") )
      return new AST::Expr(AST::Expr::Sub, AST::Expr::FromInt(0), MemberAccess(), csmtok);

    //if( consume("new") ) {
    //  DontPlace_v = true;

    //  auto x = new AST::Expr(AST::Expr::New, MemberAccess(), nullptr, csmtok);
    //  DontPlace_v = false;

    //  return x;
    //}

    return MemberAccess();
  }

  AST::Expr* Mul()
  {
    auto x = Unary();

    while( check() )
    {
      if( consume("*") )
        x = new AST::Expr(AST::Expr::Mul, x, Unary(), csmtok);
      else if( consume("%") )
        x = new AST::Expr(AST::Expr::Mod, x, Unary(), csmtok);
      else if( consume("/") )
        x = new AST::Expr(AST::Expr::Div, x, Unary(), csmtok);
      else
        break;
    }

    return x;
  }

  AST::Expr* Add()
  {
    auto x = Mul();

    while( check() )
    {
      if( consume("+") )
        x = new AST::Expr(AST::Expr::Add, x, Mul(), csmtok);
      else if( consume("-") )
        x = new AST::Expr(AST::Expr::Sub, x, Mul(), csmtok);
      else
        break;
    }

    return x;
  }

  AST::Expr* Shift()
  {
    auto x = Add();

    while( check() )
    {
      if( consume("<<") )
        x = new AST::Expr(AST::Expr::Shift, x, Add(), csmtok);
      else if( consume(">>") )
        x = new AST::Expr(AST::Expr::Shift, Add(), x, csmtok);
      else
        break;
    }

    return x;
  }

  AST::Expr* Compare() {
    auto x = Shift();

    while( check() ) {
      if( consume(">") )
        x = new AST::Expr(AST::Expr::Bigger, x, Shift(), csmtok);
      else if( consume("<") )
        x = new AST::Expr(AST::Expr::Bigger, Shift(), x, csmtok);
      else if( consume(">=") )
        x = new AST::Expr(AST::Expr::BiggerOrEqual, x, Shift(), csmtok);
      else if( consume("<=") )
        x = new AST::Expr(AST::Expr::BiggerOrEqual, Shift(), x, csmtok);
      else
        break;
    }

    return x;
  }

  AST::Expr* Equal() {
    auto x = Compare();

    while( check() ) {
      if( consume("==") )
        x = new AST::Expr(AST::Expr::Equal, x, Compare(), csmtok);
      else if( consume("!=") )
        x = new AST::Expr(AST::Expr::NotEqual, x, Compare(), csmtok);
      else break;
    }

    return x;
  }

  AST::Expr* LogAND() {
    auto x = Equal();

    while( check() ) {
      if( consume("and") )
        x = new AST::Expr(AST::Expr::LogAND, x, Equal(), csmtok);
      else break;
    }

    return x;
  }

  AST::Expr* LogOR() {
    auto x = LogAND();

    while( check() ) {
      if( consume("or") )
        x = new AST::Expr(AST::Expr::LogOR, x, LogAND(), csmtok);
      else break;
    }

    return x;
  }

  AST::Expr* Assign() {
    auto x = LogOR();
    auto tk = &get_tok();

    if( consume("=") )
      x = new AST::Expr(AST::Expr::Assign, x, Assign(), tk);

    else if( consume("+=") )
      x = new AST::Expr(AST::Expr::Assign, x, new AST::Expr(AST::Expr::Add, x, Assign(), tk), tk);
    else if( consume("-=") )
      x = new AST::Expr(AST::Expr::Assign, x, new AST::Expr(AST::Expr::Sub, x, Assign(), tk), tk);
    else if( consume("*=") )
      x = new AST::Expr(AST::Expr::Assign, x, new AST::Expr(AST::Expr::Mul, x, Assign(), tk), tk);
    else if( consume("%=") )
      x = new AST::Expr(AST::Expr::Assign, x, new AST::Expr(AST::Expr::Mod, x, Assign(), tk), tk);
    else if( consume("/=") )
      x = new AST::Expr(AST::Expr::Assign, x, new AST::Expr(AST::Expr::Div, x, Assign(), tk), tk);


    return x;
  }

  void Debug(AST::Expr*);

  AST::Expr* Expr()
  {
    auto expr = Assign();

    //expr->Optimize();

    return expr;
  }

  AST::Stmt* Statements() {
    //
    // ブロック文
    if( consume("{") ) {
      auto ast = new AST::Block;
      ast->token = csmtok;

      auto closed = false;

      while( 1 ) {
        if( consume("}") ) {
          closed = true;
          break;
        }

        ast->list.emplace_back(Statements());
      }

      if( !closed ) {
        Error(*ast->token, "not closed");
      }

      return ast;
    }

    //
    // if
    if( consume("if") ) {
      auto ast = new AST::If;

      expect("(");
      ast->cond = Expr();
      expect(")");

      ast->code = Statements();

      if( consume("else") ) {
        ast->elseCode = Statements();
      }

      return ast;
    }

    //
    // for
    if( consume("for") ) {
      auto ast = new AST::For;
      ast->token = csmtok;

      expect("(");
      auto it = Expr();

      expect("in");
      auto list = Expr();

      expect(")");

      auto code = Statements();

      ast->iterator = it;
      ast->list = list;
      ast->code = code;

      return ast;
    }

    //
    // while
    if( consume("while") ) {
      auto ast = new AST::While;

      expect("(");
      ast->cond = Expr();
      expect(")");

      ast->code = Statements();

      return ast;
    }

    //
    // do - while
    // while 文として置き換えます
    // README を参照
    if( consume("do") ) {
      auto ast = new AST::While;
      ast->cond = AST::Expr::FromInt(1);

      auto block = new AST::Block;
      block->list.emplace_back(Statements());

      auto cmp = new AST::If;
      expect("while");
      expect("(");
      cmp->cond = Expr();
      expect(")");
      expect(";");

      cmp->code = new AST::Stmt;
      cmp->code->type = AST::Stmt::Break;

      block->list.emplace_back(cmp);

      ast->code = block;
      return ast;
    }

    //
    // ユーザー定義関数
    // def - end
    if( consume("def") ) {
      
      auto ast = new AST::Function;

      // TODO: check if it identifier
      ast->name = token->str;
      next();

      expect("(");
      if( !consume(")") ) {
        do {
          // TODO: check if it identifier
          
          auto param = new AST::Expr;
          param->type = AST::Expr::Variable;
          param->token = token;
          ast->args.emplace_back(param);
          
          next();

        } while( consume(",") );
        expect(")");
      }

      ast->code = Statements();

      functions.emplace_back(ast);

      return ast;
    }

    //
    // class
    if( consume("class") ) {
      
      // todo

      return nullptr;
    }

    //
    // return
    if( consume("return") )
    {
      auto s = new AST::Stmt;
      s->type = AST::Stmt::Return;
      s->token = csmtok;

      if( consume(";") ) {
        return s;
      }

      s->expr = Expr();
      expect(";");

      return s;
    }

    //
    // break
    if( consume("break") ) {
      expect(";");

      auto s = new AST::Stmt;
      s->type = AST::Stmt::Break;
      return s;
    }

    //
    // continue
    if( consume("continue") ) {
      expect(";");

      auto s = new AST::Stmt;
      s->type = AST::Stmt::Continue;
      return s;
    }


    auto st = new AST::Stmt;

    st->expr = Expr();
    expect(";");

    return st;
  }


  AST::Block* Parse()
  {
    auto ast = new AST::Block;

    while( check() )
    {
      ast->list.emplace_back(Statements());
    }

    return ast;
  }
}