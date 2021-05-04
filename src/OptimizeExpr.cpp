#include "main.h"
#include <cassert>

/* internal types */
class SignedExpr{
public:
  enum Sign{
    Positive=1,
    Negative=-1,
  } sign;
  AST::Expr* expr;

  template <typename T>
  SignedExpr(T sign,AST::Expr* expr)
    : sign((Sign)sign), expr(expr)
  {
  }

  static SignedExpr FromExprRight(AST::Expr* expr){
    auto right=expr->right;
    int sign=(expr->type==AST::Expr::Add ? 1 : -1); // default sign (expr->type)
    if(right->type == AST::Expr::Immidiate){
      sign*=right->token->obj.v_int > 0 ? 1 : -1; // if immidiate is negative, invert sign
    }
    return SignedExpr(sign,expr->right);
  }
};

/* internal types - end */

void AST::Expr::Optimize(){
  if(type==Add or type==Sub){
    // Expr
    // get all term
    std::vector<SignedExpr> terms;
    
    Expr *cur_left=this;
    while(cur_left->type == Add or cur_left->type==Sub)// loop while cur_left kind = Expr
    {
      assert(cur_left->type == Add or cur_left->type==Sub); // cur_left must be Add/Sub

      terms.emplace_back(SignedExpr::FromExprRight(cur_left));
      cur_left=cur_left->left;
    }
    terms.emplace_back(SignedExpr(SignedExpr::Positive,cur_left));

    // remove immidiate! (to immidiatePart)
    Expr immidiatePart{};
    immidiatePart.token=new Token();
    for( auto it = terms.begin(); it != terms.end(); ) {
      if(it->expr->type==Immidiate){
        immidiatePart.token->obj.v_int+=it->expr->token->obj.v_int*it->sign;
        terms.erase(it);
      }else it++;
    }
    
  }else if(type==Mul or type==Div){
    // Term
  }else{
    // Factor
  }
}