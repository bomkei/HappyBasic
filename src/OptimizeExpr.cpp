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
      terms.emplace_back(
        SignedExpr(
          (cur_left->type==Add or cur_left->type==Sub) ? 1 : -1
          ,cur_left->right
        )  
      );
      cur_left=cur_left->left;
    }

    
  }else if(type==Mul or type==Div){
    // Term
  }else{
    // Factor
  }
}