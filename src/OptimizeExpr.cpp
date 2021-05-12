#include "main.h"
#include <cassert>
#include <cmath>
#include <map>
#include <numeric>

/* internal types */
class ExprType
{
  public:
  enum Type
  {
    Expr,
    Term,
    Factor
  } type;

  bool isMatchedType(AST::Expr& src)
  {
    if(
        type == Expr and (src.type == AST::Expr::Add or src.type == AST::Expr::Sub) or type == Term and (src.type == AST::Expr::Mul or src.type == AST::Expr::Div) )
      return true;

    return false;
  }

  static bool isMatchedType(Type type, AST::Expr& src)
  {
    if(
        type == Expr and (src.type == AST::Expr::Add or src.type == AST::Expr::Sub) or type == Term and (src.type == AST::Expr::Mul or src.type == AST::Expr::Div) )
      return true;

    return false;
  }
};

class TypedExpr
{
  public:
  enum Kind
  {
    Expr,
    Term
  } kind;

  enum Type
  {
    Normal = 1,
    Innormal = -1,
  } type;
  AST::Expr* expr;

  template <typename T>
  TypedExpr(T type, Kind kind, AST::Expr* expr)
      : type((Type)type)
      , kind(kind)
      , expr(expr)
  {
  }
  static Kind getKindFromExprType(ExprType srctype)
  {
    if( srctype.type == ExprType::Expr )
    {
      return Kind::Expr;
    }
    else if( srctype.type == ExprType::Term )
    {
      return Kind::Term;
    }
    else
    {
      return Kind::Expr; // it's default
    }
  }
  static TypedExpr FromExprRight(AST::Expr* expr)
  {
    Type type;
    Kind kind;
    AST::Expr::Type srctype = expr->type;
    if( srctype == AST::Expr::Mul or srctype == AST::Expr::Add )
    {
      type = Normal;
    }
    else
    {
      type = Innormal;
    }

    if( srctype == AST::Expr::Mul or srctype == AST::Expr::Div )
    {
      kind = Term;
    }
    else
    {
      kind = Expr;
    }

    return TypedExpr(type, kind, expr->right);
  }
  int getSign()
  {
    return type == Normal ? 1 : -1;
  }
  AST::Expr::Type getType()
  {
    if( kind == Expr )
    {
      if( type == Normal )
      {
        return AST::Expr::Add;
      }
      else
      { // type == Innormal
        return AST::Expr::Sub;
      }
    }
    else
    { // kind == Term
      if( type == Normal )
      {
        return AST::Expr::Mul;
      }
      else
      { // type == Innormal
        return AST::Expr::Div;
      }
    }
  }
  bool operator==(TypedExpr& target)
  {
    return this->expr->equal(*target.expr) and kind == target.kind;
  }
};

using variableType = std::pair<int, std::string>;

/* internal types - end */

bool removeVariableOnce(AST::Expr& expr, int variable)
{
  if( !expr.right ) // it has not depth...
  {
    return false;
  }

  if( expr.right->type == AST::Expr::Variable and expr.right->varIndex == variable )
  {
    expr = *expr.left;
    return true;
  }
  else if( expr.left->type == AST::Expr::Variable and expr.left->varIndex == variable )
  {
    expr = *expr.right;
    return true;
  }
  else
  {
    return removeVariableOnce(*expr.right, variable) or removeVariableOnce(*expr.left, variable);
  }
}

/* internal functions */
AST::Expr* makeExprFromExprs(std::vector<AST::Expr*>& parts)
{
  auto ret = new AST::Expr;
  for( auto&& part : parts )
  {
    *ret += *part;
  }

  return ret;
}
void _getVariables(AST::Expr& expr, std::vector<variableType>& dest)
{
  if( expr.type == AST::Expr::Variable )
    dest.emplace_back(std::make_pair(expr.varIndex, expr.token->str));

  if( expr.left )
    _getVariables(*expr.left, dest);
  if( expr.right )
    _getVariables(*expr.right, dest);
}
void getAllParts(ExprType type, AST::Expr* expr, std::vector<TypedExpr>& parts)
{
  if( expr->isBinary() and expr->right != nullptr )
  {
    if( expr->right->isPrimary() )
      parts.emplace_back(TypedExpr::FromExprRight(expr->right));
  }

  if( expr->left )
    getAllParts(type, expr->left, parts);
  if( expr->right )
    getAllParts(type, expr->right, parts);
};

// parts = std::vector<TypedExpr(Term) >
void Expr_Summarize(std::vector<TypedExpr>& parts)
{
  if( parts.size() == 0 )
    return;
  std::vector<variableType> variables;
  for( auto&& expr : parts )
  {
    _getVariables(*expr.expr, variables);
  }
  Utils::VectorUnique(variables);
  std::vector<TypedExpr> terms;
  for( auto&& variable : variables )
  {
    terms.clear();
    for( auto it = parts.begin(); it != parts.end(); )
    {
      bool hasVariable = removeVariableOnce(*it->expr, variable.first);
      if( hasVariable )
      {
        terms.emplace_back(*it);
        parts.erase(it);
      }
      else
        it++;
    }

    auto dest = makeExprFromExprs(terms);
    std::cout << *dest << std::endl;
  }

  for( auto&& part : parts )
  {
    std::cout << "summarize     : " << *part.expr << std::endl;
  }
}
/* internal functions - end */
void AST::Expr::Optimize()
{
  if( type == Assign )
  {
    right->Optimize();
    std::cout << *this << std::endl;
    return;
  }
  //std::cout << "expropt before: " << *this << std::endl;

  // get expr type
  ExprType exprtype;
  if( type == Add or type == Sub )
    exprtype.type = ExprType::Expr;
  else if( type == Mul or type == Div )
    exprtype.type = ExprType::Term;
  else
    exprtype.type = ExprType::Factor;

  // get all parts
  std::vector<TypedExpr> parts;
  Expr* cur_left = this;
  while( exprtype.isMatchedType(*cur_left) ) // loop while cur_left kind = Expr
  {
    parts.emplace_back(TypedExpr::FromExprRight(cur_left));
    cur_left = cur_left->left;
  }
  parts.emplace_back(TypedExpr(TypedExpr::Normal, TypedExpr::getKindFromExprType(exprtype), cur_left));

  // clear this
  this->right = nullptr;
  this->left = nullptr;

  // optimize each types
  if( exprtype.type == ExprType::Expr )
  {
    // calculate immidiate
    double immidiate = 0;
    for( auto it = parts.begin(); it != parts.end(); )
    {
      it->expr->Optimize();
      if( it->expr->type == Immidiate )
      {
        immidiate += it->expr->token->obj.v_int * it->getSign();
        parts.erase(it);
      }
      else
        it++;
    }

    if( immidiate != 0 )
    {
      auto imm = new Expr();
      imm->token = new Token();
      imm->token->obj.v_int = immidiate;

      *this += *imm;
    }
    Expr_Summarize(parts);
  }
  else if( exprtype.type == ExprType::Term )
  {
    // remove immidiate!
    double imm_denom_dbl = 1;
    int imm_denom_int = 1;
    double imm_numer_dbl = 1;
    int imm_numer_int = 1;
    for( auto it = parts.begin(); it != parts.end(); )
    {
      if( it->expr->type != Immidiate )
      { // skip not immidiate
        it++;
        continue;
      }

      auto obj = it->expr->token->obj;
      if( it->type == TypedExpr::Normal )
      {
        if( obj.type == Object::Int )
          imm_numer_int *= obj.v_int;
        else
          imm_numer_dbl *= obj.as<double>();
      }
      else
      {
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
    if( imm_numer != 1.0 )
    {
      *this *= *(new Expr(imm_numer));
    }
    if( imm_denom != 1.0 )
    {
      *this /= *(new Expr(imm_denom));
    }

    // reduction!!!
    // reduct step1: split to denom and numer
    std::vector<TypedExpr> numers; // type == normal
    std::vector<TypedExpr> denoms; // type == innormal
    for( auto&& factor : parts )
    {
      if( factor.type == TypedExpr::Normal )
      {
        numers.emplace_back(factor);
      }
      else
      {
        denoms.emplace_back(factor);
      }
    }
    // reduct step2: remove duplicate(s)
    for( auto it = numers.begin(); it != numers.end(); )
    {
      auto numer = *it;
      if( Utils::VectorHasItem(denoms, numer) )
      {
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
  for( auto&& part : parts )
  {
    AST::Expr& target = *part.expr;
    AST::Expr::Type op;
    if( part.kind == TypedExpr::Kind::Expr )
    {
      if( part.type == TypedExpr::Type::Normal )
      {
        *this += target;
      }
      else
      {
        *this -= target;
      }
    }
    else if( part.kind == TypedExpr::Kind::Term )
    {
      if( part.type == TypedExpr::Type::Normal )
      {
        *this *= target;
      }
      else
      {
        *this /= target;
      }
    }
  }
  this->fix();
}