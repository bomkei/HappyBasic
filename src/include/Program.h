#pragma once

#include <memory>

class ProgramImpl;
class Program
{
  std::unique_ptr<ProgramImpl> _impl;
  static inline Program* _instance;

public:
  Program();
  ~Program();

  void OpenFile();  // �t�@�C�����J�� ( Options::FileName )

  void Tokenize();  // �g�[�N�i�C�Y
  void Parse();     // �p�[�X
  //void Check();     // �`�F�b�N

  void ViewNodes();


  // �ϐ���T���A���������ꍇ�͂���ւ̃|�C���^��Ԃ��܂�
  // ���݂��Ȃ��ꍇ�� nullptr ��Ԃ��܂�
  Object* GetVariable(std::string const&) const;
  
  // �ϐ����X�g�ւ̎Q��
  std::vector<Object>& GetVariables() const;

  // �\���̃��X�g�ւ̎Q��
  std::vector<AST::Struct*> GetStructs() const;

  // �\���̂�T��
  // �����ꍇ�� nullptr
  AST::Struct* GetStruct(std::string const&) const;

  // �֐���T��
  // �����ꍇ�� nullptr
  AST::Function* GetFunction(std::string const&) const;
  
  // ���ݎ��s���̊֐�
  AST::Function*& GetCurrentFunction() const;

  // AST ���s
  Object Run();

  static Program* GetInstance();

  [[noreturn]]
  static void Error(Token const& tok, std::string const& msg);
};
