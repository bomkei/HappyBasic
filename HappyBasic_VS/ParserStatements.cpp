#include "main.h"

AST::Stmt* ParserCore::Statements() {
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
      PrgCtx::Error(*ast->token, "not closed");
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
    ast->token = &get_tok();

    next();
    expect("(");

    if( !consume(")") ) {
      while( check() ) {
        auto& tk = get_tok();

        if( tk.type != Token::Ident )
          PrgCtx::Error(tk, "syntax error");

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

    auto ptr = func_args;

    in_function = true;
    func_args = &(ast->args);
    
    ast->code = Statements();
    
    in_function = false;
    func_args = ptr;

    functions.emplace_back(ast);

    return ast;
  }

  if( consume("class") ) {
    // TODO

    return nullptr;
  }

  if( consume("struct") ) {
    auto tok = csmtok;
    auto name_tok = &get_tok();

    auto ast = new AST::Struct;
    ast->name = name_tok->str;

    alart;
    debugmsg("%s", ast->name.c_str());

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
        PrgCtx::Error(*tk, "002r03u4290tu4930tu");
      }

      auto member = Statements();
      ast->member_list.emplace_back(member);
    }

    cur_struct = oldptr;
    in_struct = oldflag;
    PrgCtx::Instance->structs.emplace_back(ast);

    return ast;
  }

  //
  // var
  if( consume("var") ) {
    auto tok = csmtok;
    auto var_tok = &get_tok();

    if( var_tok->type != Token::Ident ) {
      PrgCtx::Error(*tok, "expected variable name when after this token");
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

