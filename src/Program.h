#pragma once

class Program
{
  std::vector<Object> variables;
  std::string source;

  Tokenizer* tokenizer;
  ParserCore* parser;

  AST::Stmt* prs_result;

public:
  Program()
  {
    tokenizer = new Tokenizer(source);
    parser = new ParserCore();
  }

  void OpenFile(std::string const& path)
  {
    static auto is_empty = [] (std::string const& str) {
      for( auto&& c : str )
        if( c > ' ' )
          return false;

      return true;
    };

    std::ifstream ifs(path);
    std::string line;

    // ファイルを開けなかった
    if( ifs.fail() )
    {
      std::cout << "cannot open file";
      exit(1);
    }

    // 一行ずつ読み取っていく
    while( std::getline(ifs, line) )
    {
      // 右端の空白以下の文字を削除
      while( line.length() && line[line.length() - 1] <= ' ' )
        line.pop_back();

      // 左端の空白以下の文字を削除
      while( line.length() && line[0] <= ' ' )
        line.erase(line.begin());

      if( line.empty() )
        continue;

      // 改行文字をつけて ret に追加する
      source += line + '\n';
    }

    // ソースが空
    if( is_empty(source) )
    {
      std::cout << "empty source file";
      exit(1);
    }
  }

  void Tokenize()
  {
    parser->Initialize(tokenizer->Tokenize());
  }

  void Parse()
  {
    prs_result = parser->Parse();
  }

  Object Run()
  {

    return AST_Runner::Run_Stmt(prs_result);
  }
};
