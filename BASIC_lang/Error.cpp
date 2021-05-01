#include "main.h"

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


void Program::Error(Token* tok, std::string const& msg)
{

  throw std::make_pair(tok, msg);

}