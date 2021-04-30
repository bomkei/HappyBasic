#include <Windows.h>
#include "main.h"
#include "color.h"

// 文字色指定 for Windows Console
void SetConsoleColor(int col)
{
  HANDLE hCons = GetStdHandle(STD_OUTPUT_HANDLE);    // コンソールハンドル取得
  WORD attr = 0;                                     // API に渡すアトリビュート

  if( col & COL_INTENSITY )        // 高輝度ビットが立っていれば
    attr |= FOREGROUND_INTENSITY;  // アトリビュートの高輝度ビットを立てる
  
  if( col & COL_RED_MASK )
    attr |= FOREGROUND_RED;
  
  if( col & COL_GREEN_MASK )
    attr |= FOREGROUND_GREEN;
  
  if( col & COL_BLUE_MASK )
    attr |= FOREGROUND_BLUE;
  
  // 色指定
  SetConsoleTextAttribute(hCons, attr);
}

void Token::Error(std::string const& msg)
{
  size_t line = 1;
  size_t begin = 0;
  size_t end = g_source.length();

  for( size_t i = 0; i < srcpos; i++ )
  {
    if( g_source[i] == '\n' )
    {
      line++;
      begin = i + 1;
    }
  }

  for( size_t i = srcpos; i < end; i++ )
  {
    if( g_source[i] == '\n' )
    {
      end = i;
      break;
    }
  }

  auto line_str = g_source.substr(begin, end - begin);
  auto pos = srcpos - begin;

  SetConsoleColor(COL_BLUE);
  printf("%s\n", std::string(50, '-').c_str());

  SetConsoleColor(COL_RED);
  printf("Error:\n");

  SetConsoleColor(COL_WHITE);
  printf("%6zd|%s\n", line, line_str.c_str());
  printf("      |");

  SetConsoleColor(COL_RED);
  printf("%s^%s  %s\n", std::string(pos, ' ').c_str(), std::string(str.length() ? str.length() - 1 : 0, '~').c_str(), msg.c_str());

  SetConsoleColor(COL_GRAY);
  exit(1);
}