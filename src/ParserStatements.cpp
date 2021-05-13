#include "main.h"

AST::Stmt* ParserCore::Statements()
{
  if( consume("if") )
  {
    AST::If::Pair pair;
    auto closed = false;

    auto ast = new AST::If;
    ast->token = csmtok;

    pair = std::make_pair(Expr(), new AST::Block);

    expect("then");
    expect("\n");

    while( check() ) {
      if( consume("endif") ) {
        expect("\n");
        ast->pairs.emplace_back(pair);
        closed = true;
        break;
      }
      else if( consume("elseif") ) {
        ast->pairs.emplace_back(pair);

        pair = std::make_pair(Expr(), new AST::Block);

        expect("then");
        expect("\n");
      }
      else if( consume("else") ) {
        ast->pairs.emplace_back(pair);
        pair = std::make_pair(AST::Expr::FromInt(1), new AST::Block);

        expect("\n");

        while( check() ) {
          if( consume("endif") ) {
            expect("\n");
            closed = true;
            break;
          }

          std::get<1>(pair)->list.emplace_back(Statements());
        }

        ast->pairs.emplace_back(pair);
        break;
      }
      else
      {
        std::get<1>(pair)->list.emplace_back(Statements());
      }
    }

    if( !closed ) {
      Program::Error(*ast->token, "not closed");
    }

    return ast;
  }


  if( consume("for") ) {
    auto ast = new AST::For;
    ast->token = csmtok;

    ast->counter = IndexRef();

    expect("=");
    ast->begin = Expr();

    expect("to");
    ast->end = Expr();

    ast->code = new AST::Block;

    expect("\n");

    auto closed = false;

    while( check() ) {
      if( consume("next") ) {
        expect("\n");
        closed = true;
        break;
      }

      ast->code->list.emplace_back(Statements());
    }

    if( !closed ) {
      Program::Error(*ast->token, "not closed");
    }

    return ast;
  }

  if( consume("while") ) {
    auto tk = csmtok;

    auto cond = Expr();
    expect("\n");

    std::vector<AST::Stmt*> block;
    auto closed = false;

    while( check() ) {
      if( consume("wend") ) {
        expect("\n");
        closed = true;
        break;
      }

      block.emplace_back(Statements());
    }

    if( !closed )
      Program::Error(*tk, "not closed");

    auto ast = new AST::While;
    ast->token = tk;
    ast->cond = cond;
    ast->code = new AST::Block;
    ast->code->list = std::move(block);

    return ast;
  }

  if( consume("do") ) {
    std::vector<AST::Stmt*> list;
    AST::Expr* cond = nullptr;
    auto closed = false;

    while( check() ) {
      if( consume("while") ) {
        cond = Expr();
        expect("\n");
        closed = true;
        break;
      }

      list.emplace_back(Statements());
    }

    auto ast = new AST::While;

    ast->cond = AST::Expr::FromInt(1);
    ast->code = new AST::Block;

    {
      auto bb = new AST::Block;
      bb->list = std::move(list);
      ast->code->list.emplace_back(bb);
    }

    {
      auto iff = new AST::If;

      {
        auto breakk = new AST::Block; {
          auto bruh = new AST::Stmt;
          bruh->type = AST::Stmt::Break;
          breakk->list.emplace_back(bruh);
        }

        iff->pairs.emplace_back(std::make_pair(cond, breakk));
      }
      ast->code->list.emplace_back(iff);
    }

    return ast;
  }


  if( consume("def") )
  {
    auto tk = csmtok;

    if( in_function )
      Program::Error(*tk, "nested def");

    if( get_tok().type != Token::Ident )
      Program::Error(get_tok(), "expect identifier after 'def' keyword");

    auto& name = get_tok().str;
    next();

    std::vector<AST::Expr*> args;
    std::vector<AST::Stmt*> block;

    func_args = &args;

    expect("(");

    if( !consume(")") )
    {
      do
      {
        if( get_tok().type != Token::Ident )
          Program::Error(get_tok(), "syntax error");

        auto prm = new AST::Expr;
        prm->type = AST::Expr::Param;
        prm->token = &get_tok();
        next();

        args.emplace_back(prm);
      } while( consume(",") );

      expect(")");
    }

    expect("\n");

    auto closed = false;
    in_function = true;

    while( check() )
    {
      if( consume("end") )
      {
        closed = true;
        expect("\n");
        break;
      }

      block.emplace_back(Statements());
    }

    if( !closed )
      Program::Error(*tk, "not closed");

    in_function = false;

    auto ast = new AST::Function;
    ast->token = tk;
    ast->name = name;

    if( args.size() )
      ast->args = args;

    ast->code = new AST::Block;
    ast->code->list = std::move(block);

    if( !in_class )
      functions.emplace_back(ast);

    return ast;
  }

  if( consume("class") )
  {
    auto ast = new AST::Class;
    ast->token = &get_tok();

    auto closed = false;

    next();
    expect("\n");

    auto flag = in_class;
    auto ptr = cur_class;

    in_class = true;
    cur_class = ast;

    while( check() ) {
      if( consume("end") ) {
        expect("\n");
        closed = true;
        break;
      }

      auto tk = &get_tok(); {
        if( tk->str != "var" &&
          tk->str != "def" &&
          tk->str != "class" ) {
          Program::Error(*tk, "only can place var or def or class statements on here");
        }
      }

      if( tk->str == "var" ) {
        auto ss = new AST::Stmt;
        ss->type = AST::Stmt::Var;
        ss->token = tk;
        next();
        if( get_tok().type != Token::Ident ) {
          Program::Error(get_tok(), "expect identifier");
        }
        auto v_tok = &get_tok();
        auto var = new AST::Expr;
        var->type = AST::Expr::MemberVariable;
        var->token = v_tok;
        next();
        expect("=");
        auto expr = Expr();
        expect("\n");
        ss->expr = new AST::Expr(AST::Expr::Assign, var, expr, v_tok + 1);
        ast->member_list.emplace_back(ss);
      }
      else ast->member_list.emplace_back(Statements());
    }

    if( !closed )
      Program::Error(*ast->token, "not closed");

    classes.emplace_back(ast);

    in_class = flag;
    cur_class = ptr;

    return ast;
  }



  if( consume("var") ) {
    auto tk = csmtok;
    auto cc = Expr();
    if( cc->type != AST::Expr::Assign ) {
      Program::Error(*tk, "89t4308tg5342");
    }
    auto jjj = new AST::Stmt;
    jjj->type = AST::Stmt::Var;
    jjj->expr = cc;
    jjj->token = tk;
    expect("\n");
    return jjj;



  }

  //
  // return
  if( consume("return") )
  {
    auto ast = new AST::Stmt;
    ast->type = AST::Stmt::Return;
    ast->token = csmtok;

    if( !consume("\n") )
    {
      ast->expr = Expr();
      expect("\n");
    }
    else
    {
      ast->expr = nullptr;
    }

    return ast;
  }

  //
  // break
  if( consume("break") ) {
    auto ast = new AST::Stmt;
    ast->type = AST::Stmt::Break;
    ast->token = csmtok;
    expect("\n");
    return ast;
  }

  //
  // continue
  if( consume("continue") ) {
    auto ast = new AST::Stmt;
    ast->type = AST::Stmt::Continue;
    ast->token = csmtok;
    expect("\n");
    return ast;
  }


  auto st = new AST::Stmt;

  st->expr = Expr();
  expect("\n");

  return st;
}

