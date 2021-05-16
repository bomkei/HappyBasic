#pragma once

namespace AST_Runner
{
  /*
    Object::Type を調節します

    どちらかが配列の場合、何も行いません

    どちらかが Float の場合、両方 Float になるように調節します
      たとえば、`3.14 + 10` という式の場合は 10 を 10.0 に変換します

    そうでない場合、どちらも Int に変換します
  */
  void ObjectAdjuster(Object& L, Object& R);


  Object Expr(AST::Expr* ast);
  Object Stmt(AST::Stmt* ast);

  Object Function(AST::Callfunc* ast);
  Object UserFunc(AST::Callfunc* fun);

  Object BuiltInMember(AST::Expr*);
};