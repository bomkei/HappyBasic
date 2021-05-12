#include "../main.h"

AST::Class *ParserCore::Class()
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
    if( consume("End") ) {
      expect("Class");
      expect("\n");
      closed = true;
      break;
    }

    auto tk = &get_tok(); {
      if( tk->str != "Var" &&
        tk->str != "Func" &&
        tk->str != "Class" ) {
        Program::Error(*tk, "at9032j0t2j40");
      }
    }

    if( tk->str == "Var" ) {
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

