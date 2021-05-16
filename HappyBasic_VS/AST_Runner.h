#pragma once

namespace AST_Runner
{
  /*
    Object::Type �𒲐߂��܂�

    �ǂ��炩���z��̏ꍇ�A�����s���܂���

    �ǂ��炩�� Float �̏ꍇ�A���� Float �ɂȂ�悤�ɒ��߂��܂�
      ���Ƃ��΁A`3.14 + 10` �Ƃ������̏ꍇ�� 10 �� 10.0 �ɕϊ����܂�

    �����łȂ��ꍇ�A�ǂ���� Int �ɕϊ����܂�
  */
  void ObjectAdjuster(Object& L, Object& R);


  Object Expr(AST::Expr* ast);
  Object Stmt(AST::Stmt* ast);

  Object Function(AST::Callfunc* ast);
  Object UserFunc(AST::Callfunc* fun);

  Object BuiltInMember(AST::Expr*);
};