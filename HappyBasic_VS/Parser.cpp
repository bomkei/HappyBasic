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
  // �\����͂Ŏg�p�����ϐ��ł�
  namespace {
    Token* token;   // �w���Ă���g�[�N��
    Token* csmtok;  // consume() ���Ă΂ꂽ���_�ł̃g�[�N��

    // ------------------------------------------------
    // �֐��Ɋւ�����
    bool in_function;          // �֐�����͒����ǂ���
    AST::Function* cur_func;   // ��͒��̊֐��ւ̃|�C���^

    // ------------------------------------------------
    // ��͒��̍\���̂܂��̓N���X�Ɋւ�����ł�
    bool in_struct;            // �\���̂���͒����ǂ���
    bool in_class;             // �N���X���ǂ���
    AST::Struct* cur_struct;   // ��͂��Ă���\���̂ւ̃|�C���^
    AST::Expr* this_ptr;       // �N���X���Ŋ֐��̉�͂��J�n����Ƃ��A�����ň����Ƃ��Ēǉ������ this �ւ̃|�C���^
                               // �܂��A�p�[�T�����̊֐����Ń����o�ϐ��̎Q�Ƃɓ��������ꍇ�Athis �ɑ΂��Ẵ����o�A�N�Z�X�Ƃ��ĉ�͂���܂�
    
    std::vector<std::string> struct_layer;
    

    //------------------------------------------------------------
    // �ϐ��̎����z�u�𐧌䂷��t���O�ł�
    // true �ɂȂ��Ă���ꍇ�́A�����z�u�����܂���
    // ����͎�Ƀ����o�A�N�Z�X���p�[�X���Ă�Ƃ��Ɏg�p���܂�
    // ��: A.b.c �Ƃ����ꍇ�AA �݂̂� �m�F & �����z�u �����
    //     b �� c �͂���܂���
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
      // ����
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
    // �z��
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
      // ���l
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

      // ���ʎq
      case Token::Ident:
      {
        AST::Struct* st_ptr = nullptr;

        auto find = find_vector(structs, [] (auto x, auto y) { return x->name == y; }, get_tok().str);
        if( find != -1 ) st_ptr = structs[find];

        next();

        // �X�R�[�v�������Z�q
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


        // �֐��Ăяo��
        if( consume("(") ) {
          auto ast = new AST::Callfunc;
          ast->token = tok;
          ast->st_ptr = st_ptr;

          // �s���S�ł���ꍇ�A�g�p�s�\ //
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

        // �֐��̒��ɂ���ꍇ�A������T��
        if( in_function ) {
          for( auto&& i : cur_func->args )
            if( i->token->str == tok->str ) {
              return i;
            }
        }

        // �N���X / �\����
        if( in_struct ) {
          for( auto&& i : cur_struct->member_list ) {
            if( i->type == AST::Stmt::Var &&
              i->expr->left->token->str == tok->str ) {

              if( in_class ) { // �N���X
                auto xx = new AST::Expr;
                xx->type = AST::Expr::MemberAccess;
                xx->left = this_ptr;
                xx->right = i->expr->left;

                return xx;
              }
              else { // �\����
                return i->expr->left;
              }
            }
          }
        }

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
    // �u���b�N��
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
    // while ���Ƃ��Ēu�������܂�
    // README ���Q��
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
    // ���[�U�[��`�֐�
    // def - end
    if( consume("def") ) {
      auto oldptr = this_ptr;
      
      auto ast = new AST::Function;
      ast->token = &get_tok();
      ast->name = ast->token->str;

      if( in_class ) {
        auto x = AST::Expr::ToLOBJ_FromName("this");
        ast->args.emplace_back(x);
        this_ptr = x;
      }

      next();
      expect("(");

      if( !consume(")") ) {
        while( check() ) {
          auto& tk = get_tok();

          if( tk.type != Token::Ident )
            Error(tk, "syntax error 11");

          auto prm = new AST::Expr;
          prm->type = AST::Expr::Lv_obj;
          prm->token = &tk;
          next();

          ast->args.emplace_back(prm);

          if( !consume(",") )
            break;
        }

        expect(")");
      }

      auto ptr = cur_func;

      in_function = true;
      cur_func = ast;

      ast->code = Statements();

      in_function = false;
      cur_func = ptr;

      functions.emplace_back(ast);

      this_ptr = oldptr;

      // mangle
      {
        std::vector<std::string> _Args;
        for( auto&& i : ast->args ) _Args.emplace_back(i->token->str);
        ast->not_mangled = ast->name;
        ast->name = Mangle(ast->name, struct_layer, _Args);

        alart;
        std::cout << ast->name << '\n';
      }

      return ast;
    }

    if( consume("struct") ) {
      auto tok = csmtok;
      auto name_tok = &get_tok();

      auto ast = new AST::Struct;
      ast->name = name_tok->str;

      struct_layer.emplace_back(ast->name);

      if( !in_struct )
        structs.emplace_back(ast);

      auto oldptr = cur_struct;
      auto oldflag = in_struct;
      cur_struct = ast;
      in_struct = true;

      next();
      expect("{");

      while( !consume("}") ) {
        auto tk = &get_tok();

        if( tk->str != "var" &&
          tk->str != "struct" ) {
          Error(*tk, "002r03u4290tu4930tu");
        }

        auto member = Statements();
        ast->member_list.emplace_back(member);
      }

      cur_struct = oldptr;
      in_struct = oldflag;

      struct_layer.pop_back();

      return ast;
    }

    //
    // class
    if( consume("class") ) {
      auto tok = csmtok;
      auto name_tok = &get_tok();

      auto ast = new AST::Struct;
      ast->name = name_tok->str;
      ast->token = name_tok;

      struct_layer.emplace_back(ast->name);

      if( !in_struct )
        structs.emplace_back(ast);

      auto oldptr = cur_struct;
      auto oldflag = in_struct;
      auto oldflag2 = in_class;
      cur_struct = ast;
      in_struct = true;
      in_class = true;

      next();
      expect("{");

      while( !consume("}") ) {
        auto tk = &get_tok();

        if( tk->str != "var" &&
          tk->str != "def" &&
          tk->str != "struct" &&
          tk->str != "class" ) {
          Error(*tk, "dji9042j r9pw4");
        }

        auto member = Statements();
        ast->member_list.emplace_back(member);
      }

      cur_struct = oldptr;
      in_struct = oldflag;
      in_class = oldflag2;

      struct_layer.pop_back();

      return ast;
    }

    //
    // var
    if( consume("var") ) {
      auto tok = csmtok;
      auto var_tok = &get_tok();

      if( var_tok->type != Token::Ident ) {
        Error(*tok, "expected variable name when after this token");
      }

      next();
      expect("=");

      auto ast = new AST::Stmt;
      ast->type = AST::Stmt::Var;
      ast->expr = new AST::Expr(AST::Expr::Assign, AST::Expr::ToLOBJ_FromName(var_tok->str), Expr(), tok);

      expect(";");

      return ast;
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