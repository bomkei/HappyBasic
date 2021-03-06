#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <cmath>
#include <map>
#include <numeric>

#include "Utils.h"

#include "Object.h"
#include "Token.h"
#include "AST.h"


/* internal types */
class ExprType {
public:
  enum Type {
    Expr,
    Term,
    Factor
  } type;
  ExprType(AST::Expr expr) {
    if( expr.type == AST::Expr::Add or expr.type == AST::Expr::Sub )
      type = ExprType::Expr;
    else if( expr.type == AST::Expr::Mul or expr.type == AST::Expr::Div )
      type = ExprType::Term;
    else
      type = ExprType::Factor;
  }
  bool isMatchedType(AST::Expr& src) {
    if(
      type == Expr and (src.type == AST::Expr::Add or src.type == AST::Expr::Sub) or type == Term and (src.type == AST::Expr::Mul or src.type == AST::Expr::Div) )
      return true;

    return false;
  }

  static bool isMatchedType(Type type, AST::Expr& src) {
    if(
      type == Expr and (src.type == AST::Expr::Add or src.type == AST::Expr::Sub) or type == Term and (src.type == AST::Expr::Mul or src.type == AST::Expr::Div) )
      return true;

    return false;
  }
};

class TypedExpr {
public:
  enum Kind {
    Expr,
    Term
  } kind;

  enum Type {
    Normal = 1,
    Innormal = -1,
  } type;
  AST::Expr* expr;

  template <typename T>
  TypedExpr(T type, Kind kind, AST::Expr* expr)
    : type((Type)type)
    , kind(kind)
    , expr(expr) {
  }
  static Kind getKindFromExprType(ExprType srctype) {
    if( srctype.type == ExprType::Expr ) {
      return Kind::Expr;
    }
    else if( srctype.type == ExprType::Term ) {
      return Kind::Term;
    }
    else {
      return Kind::Expr; // it's default
    }
  }
  static TypedExpr FromExprRight(AST::Expr* expr) {
    Type type;
    Kind kind;
    AST::Expr::Type srctype = expr->type;
    if( srctype == AST::Expr::Mul or srctype == AST::Expr::Add ) {
      type = Normal;
    }
    else {
      type = Innormal;
    }

    if( srctype == AST::Expr::Mul or srctype == AST::Expr::Div ) {
      kind = Term;
    }
    else {
      kind = Expr;
    }

    return TypedExpr(type, kind, expr->right);
  }
  int getSign() {
    return type == Normal ? 1 : -1;
  }
  AST::Expr::Type getType() {
    if( kind == Expr ) {
      if( type == Normal ) {
        return AST::Expr::Add;
      }
      else { // type == Innormal
        return AST::Expr::Sub;
      }
    }
    else { // kind == Term
      if( type == Normal ) {
        return AST::Expr::Mul;
      }
      else { // type == Innormal
        return AST::Expr::Div;
      }
    }
  }
  bool operator==(TypedExpr& target) {
    return this->expr->equal(*target.expr) and kind == target.kind;
  }
};

using variableType = std::pair<int, std::string>;

/* internal types - end */
bool hasVariable(AST::Expr& expr, variableType variable) {
  if( expr.type == AST::Expr::Variable and expr.varIndex == variable.first ) {
    return true;
  }

  if( expr.isBinary() ) {
    return hasVariable(*expr.right, variable) or hasVariable(*expr.left, variable);
  }
  else {
    return false;
  }
}
void removeVariableOnce(AST::Expr& expr, variableType variable) {
  auto var = new AST::Expr(AST::Expr::Variable);
  var->varIndex = variable.first;
  var->token = new Token();
  var->token->type = Token::Ident;
  var->token->str = variable.second;

  expr /= *var;
  expr.Optimize();
}

/* internal functions */
AST::Expr* makeExprFromExprs(std::vector<TypedExpr>& parts) {
  auto ret = new AST::Expr;
  ret->clear();
  for( auto&& part : parts ) {
    *ret += *part.expr;
  }
  ret->fix();
  return ret;
}
void _getVariables(AST::Expr& expr, std::vector<variableType>& dest) {
  if( expr.type == AST::Expr::Variable )
    dest.emplace_back(std::make_pair(expr.varIndex, expr.token->str));

  if( expr.left )
    _getVariables(*expr.left, dest);
  if( expr.right )
    _getVariables(*expr.right, dest);
}
void getAllParts(ExprType type, AST::Expr* expr, std::vector<TypedExpr>& parts, bool isFirst = true) {
  AST::Expr* cur = expr;
  while( type.isMatchedType(*cur) ) {
    parts.emplace_back(TypedExpr::FromExprRight(cur));
    cur = cur->left;
  }
  parts.emplace_back(TypedExpr(TypedExpr::Normal, TypedExpr::getKindFromExprType(type), cur));
};

// parts = std::vector<TypedExpr(Term) >
void Expr_Summarize(std::vector<TypedExpr>& parts) {
  if( parts.size() == 0 )
    return;
  std::vector<variableType> variables;
  for( auto&& expr : parts ) {
    _getVariables(*expr.expr, variables);
  }
  Utils::VectorUnique(variables);
  std::vector<TypedExpr> terms;
  for( auto&& variable : variables ) {
    terms.clear();
    for( auto it = parts.begin(); it != parts.end(); ) {
      if( hasVariable(*it->expr, variable) ) {
        removeVariableOnce(*it->expr, variable);
        terms.emplace_back(*it);
        parts.erase(it);
      }
      else
        it++;
    }

    auto dest = makeExprFromExprs(terms);
    dest->Optimize();

    auto var = new AST::Expr(AST::Expr::Variable);
    var->varIndex = variable.first;
    var->token = new Token();
    var->token->type = Token::Ident;
    var->token->str = variable.second;
    *dest *= *var;

    dest->Optimize();

    parts.emplace_back(TypedExpr(TypedExpr::Normal, TypedExpr::Expr, dest));
  }
}
/* internal functions - end */
void AST::Expr::Optimize() {
  if( type == Assign ) {
    right->Optimize();
    return;
  }

  // get expr type
  ExprType exprtype(*this);
  // get all parts
  std::vector<TypedExpr> parts;
  auto thisClone = new AST::Expr();
  *thisClone = *this;
  getAllParts(exprtype, thisClone, parts);

  //clear this
  this->clear();

  // optimize each types
  if( exprtype.type == ExprType::Expr ) {
    // calculate immidiate
    float immidiate = 0;
    for( auto it = parts.begin(); it != parts.end(); ) {
      it->expr->Optimize();
      if( it->expr->type == Immidiate ) {
        immidiate += it->expr->token->obj.as<float>() * it->getSign();
        parts.erase(it);
      }
      else
        it++;
    }

    if( immidiate != 0 ) {
      auto imm = new Expr(immidiate);

      *this += *imm;
    }
    Expr_Summarize(parts);
  }
  else if( exprtype.type == ExprType::Term ) {
    // remove immidiate!
    double imm_denom_dbl = 1;
    int imm_denom_int = 1;
    double imm_numer_dbl = 1;
    int imm_numer_int = 1;
    for( auto it = parts.begin(); it != parts.end(); ) {
      if( it->expr->type != Immidiate ) { // skip not immidiate
        it++;
        continue;
      }

      auto obj = it->expr->token->obj;
      if( it->type == TypedExpr::Normal ) {
        if( obj.type == Object::Int )
          imm_numer_int *= obj.v_int;
        else
          imm_numer_dbl *= obj.as<double>();
      }
      else {
        if( obj.type == Object::Int )
          imm_denom_int *= obj.v_int;
        else
          imm_denom_dbl *= obj.as<double>();
      }
      parts.erase(it);
    }

    // imm
    int gcd = std::gcd(imm_denom_int, imm_numer_int);
    float imm_numer = imm_numer_dbl * (float)imm_numer_int / gcd;
    float imm_denom = imm_denom_dbl * (float)imm_denom_int / gcd;
    if( imm_numer != 1.0 ) {
      *this *= *(new Expr(imm_numer));
    }
    if( imm_denom != 1.0 ) {
      *this /= *(new Expr(imm_denom));
    }

    // reduction!!!
    // reduct step1: split to denom and numer
    std::vector<TypedExpr> numers; // type == normal
    std::vector<TypedExpr> denoms; // type == innormal
    for( auto&& factor : parts ) {
      if( factor.type == TypedExpr::Normal ) {
        numers.emplace_back(factor);
      }
      else {
        denoms.emplace_back(factor);
      }
    }
    // reduct step2: remove duplicate(s)
    for( auto it = numers.begin(); it != numers.end(); ) {
      auto numer = *it;
      if( Utils::VectorHasItem(denoms, numer) ) {
        denoms.erase(denoms.begin() + Utils::VectorFindItem(denoms, numer));
        numers.erase(it);
      }
      else
        it++;
    }
    // reduct step3: reconstructing parts
    parts.clear();
    for( auto&& numer : numers )
      parts.emplace_back(numer);
    for( auto&& denom : denoms )
      parts.emplace_back(denom);
  }

  // reconstructing Expr (to ret)
  for( auto&& part : parts ) {
    AST::Expr::Type op;
    // clang-format off
    op =
      part.kind == TypedExpr::Kind::Expr ?
      part.type == TypedExpr::Type::Normal ? Add : Sub

      : part.kind == TypedExpr::Kind::Term ?
      part.type == TypedExpr::Type::Normal ? Mul : Div

      : Add;
    // clang-format on  
    _oprator(op, *part.expr);
  }
  this->fix();
}