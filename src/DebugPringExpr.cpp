#include "main.h"
#include <ostream>

std::ostream& operator<<(std::ostream& ss, const AST::Expr &expr){
  if(&expr==nullptr){
    ss<<"null";
    return ss;
  }
  if (expr.type >= AST::Expr::Add && AST::Expr::Div >= expr.type){
    ss<<*expr.left;
    ss<<(
        expr.type == AST::Expr::Add ? "+"
      : expr.type == AST::Expr::Sub ? "-"
      : expr.type == AST::Expr::Mul ? "*"
      : expr.type == AST::Expr::Div ? "/"
      : "?");
    ss<<*expr.right;
  }else if(expr.type==AST::Expr::Immidiate){
    auto token=expr.token;
    if(token==nullptr){
      ss<<"null";
      return ss;
    }
    switch(token->type){
      case Token::Operator:
        ss<<"{"<<token->str<<"}";
        break;
      case Token::Ident:
        ss<<token->str<<"'";
        break;

      case Token::Char:
      case Token::Number:
      case Token::String:
        ss<<token->obj.to_string();
        break;
    }
  }else if(expr.type==AST::Expr::Variable){
    ss<<"var["<<expr.varIndex<<"]";
  }else if(expr.type==AST::Expr::Callfunc){
    auto callfunc=reinterpret_cast<AST::Callfunc*>((AST::Expr*)&expr);
    ss<<callfunc->token->str<<"(";
    for(auto arg:callfunc->args){
      ss<<arg<<",";
    }
    ss<<")";
  }
  
  return ss;
}