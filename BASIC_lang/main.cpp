#include "main.h"

// ----------------------------------------------- //
//  文字列置き換え
// 
//  str       = 置き換えを実行する文字列
//  find      = 検索する文字列
//  replace   = 新しく置き換える文字列
// 
//  戻り      = すべての find が replace として置き換えられた文字列
// 
//  注意   第一引数に渡した文字列は変更されます
// ----------------------------------------------- //
template <class STR>
auto& string_replace(STR& str, STR find, STR replace)
{
  for( int i = 0; i < str.length() - find.length(); )
  {
    // find と一致したら
    if( str.substr(i, find.length()) == find )
    {
      str.erase(i, find.length());  // 元の文字列を削除
      str.insert(i, replace);       // 新しい文字列を挿入
      i += replace.length();        // 操作位置を、挿入した文字列の長さの分進める
    }
    else
      i++;
  }

  return str;
}


// ----------------------------------------------- //
//  ファイルの中身をソースコードとして読み取る
// ----------------------------------------------- //
auto readfile(std::string const& path)
{
  static auto is_empty = [](std::string const& str)
  {
    for( auto&& c : str )
      if( c > ' ' ) return false;

    return true;
  };

  std::ifstream ifs(path);
  std::string ret, line;

  // ファイルを開けなかった
  if( ifs.fail() )
  {
    std::cout << "cannot open file";
    exit(1);
  }

  // 一行ずつ読み取っていく
  while( std::getline(ifs, line) )
  {
    // 文字が無ければスキップ
    if( is_empty(line) )
      continue;

    // 右端の空白以下の文字を削除
    while( line.length() && line[line.length() - 1] <= ' ' )
      line.pop_back();

    // 左端の空白以下の文字を削除
    while( line.length() && line[0] <= ' ' )
      line.erase(line.begin());

    // 改行文字をつけて ret に追加する
    ret += line + '\n';
  }


  // 行をつなげる記号
  string_replace<std::string>(ret, "\\\n", "");

  // ソースが空
  if( is_empty(ret) )
  {
    std::cout << "empty source file";
    exit(1);
  }

  return ret;
}

int main()
{
  auto source = std::move(readfile("C:/users/mrzkr/desktop/test.txt"));
  std::cout << source << '\n';

  auto tokens = std::move(Tokenize(std::move(source)));

  for( auto&& tok : tokens )
  {
    std::cout << ":" << tok.str << '\n';
  }



}


