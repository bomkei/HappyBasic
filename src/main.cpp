#include "main.h"

Program* Program::instance;

int main()
{
  Program prg;

  prg.OpenFile("script.txt");

  prg.Tokenize();

  prg.Parse();

  auto res = prg.Run();


}