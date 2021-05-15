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

  void OpenFile();  // ファイルを開く ( Options::FileName )

  void Tokenize();  // トークナイズ
  void Parse();     // パース
  //void Check();     // チェック

  void ViewNodes();


  // 変数を探し、見つかった場合はそれへのポインタを返します
  // 存在しない場合は nullptr を返します
  Object* GetVariable(std::string const&) const;
  
  // 変数リストへの参照
  std::vector<Object>& GetVariables() const;

  // 構造体リストへの参照
  std::vector<AST::Struct*> GetStructs() const;

  // 構造体を探す
  // 無い場合は nullptr
  AST::Struct* GetStruct(std::string const&) const;

  // 関数を探す
  // 無い場合は nullptr
  AST::Function* GetFunction(std::string const&) const;
  
  // 現在実行中の関数
  AST::Function*& GetCurrentFunction() const;

  // AST 実行
  Object Run();

  static Program* GetInstance();

  [[noreturn]]
  static void Error(Token const& tok, std::string const& msg);
};
