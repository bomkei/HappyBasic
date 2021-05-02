#include "main.h"

void Program::OpenFile(std::string const& path)
{
  static auto is_empty = [] (std::string const& str)
  {
    for( auto&& c : str )
      if( c > ' ' ) return false;

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

int main()
{
  try
  {
    Program prg;
    
    prg.OpenFile("test.txt");
    
    prg.Tokenize();
    prg.Parse();
    
    auto res = prg.Run();
    
  }
  catch( ErrorInfo const& info )
  {
    std::cout << std::get<1>(info) << '\n';


  }

}


