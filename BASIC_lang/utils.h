#pragma once

// ----------------------------------------------- //
//  文字列置き換え
// 
//  str       = 置き換えを実行する文字列
//  find      = 検索する文字列
//  replace   = 新しく置き換える文字列
// 
//  戻り      = すべての find が replace として置き換えられた文字列
// 
//  注意   第一引数に渡された文字列を変更し、戻り値として返します
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


//
// vector 検索
template <class T, class... Args>
i64 find_vector(std::vector<T> const& vec, Args ...args)
{
  for( i64 i = 0; i < vec.size(); i++ )
    if( vec[i] == T(args...) )
      return i;

  return -1;
}


// ファイル名の部分のみ取得する
inline auto GetFileName(const char* s)
{
  auto i = strlen(s);

  while( s[i] != '\\' )
    i--;

  return s + i + 1;
}

#define  alart fprintf(stderr,"\t%s:%d\n",GetFileName(__FILE__),__LINE__)


bool is_hex(std::string);
void SetConsoleColor(int col);

