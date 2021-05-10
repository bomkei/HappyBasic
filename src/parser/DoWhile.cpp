#include "../main.h"

AST::While* ParserCore::DoWhile()
{
  std::vector<AST::Stmt*> list;
  AST::Expr* cond = nullptr;
  auto closed = false;

  while( check() ) {
    if( consume("While") ) {
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
