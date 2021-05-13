#include "main.h"

AST::Stmt* ParserCore::Statements() {
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
      Program::Error(*ast->token, "not closed");
    }

    return ast;
  }


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
    auto ast = new AST::While;
    ast->cond = AST::Expr::FromInt(1);

    ast->code = new AST::Block;
    ast->code->list.emplace_back(Statements());
    
    auto cmp = new AST::If;
    expect("while");
    expect("(");
    cmp->cond = Expr();
    expect(")");
    expect(";");

    cmp->code = new AST::Stmt;
    cmp->code->type = AST::Stmt::Break;

    ast->code->list.emplace_back(cmp);

    return ast;
  }


  if( consume("def") )
  {
    auto ast = new AST::Function;
    ast->token = &get_tok();

    next();
    expect("(");

    if( !consume(")") ) {
      while( check() ) {
        auto& tk = get_tok();

        if( tk.type != Token::Ident )
          Program::Error(tk, "syntax error");

        auto prm = new AST::Expr;
        prm->type = AST::Expr::Param;
        prm->token = &tk;
        next();

        ast->args.emplace_back(prm);

        if( !consume(",") )
          break;
      }

      expect(")");
    }

    in_function = true;
    ast->code = Statements();
    in_function = false;

    if( !in_class ) {
      functions.emplace_back(ast);
    }

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
    auto s = new AST::Stmt;
    s->type = AST::Stmt::Return;

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

