#include "main.h"

Program* Program::instance;

int main()
{
  alart;
  Program prg;

  alart;
  prg.OpenFile("script.txt");

  alart;
  prg.Tokenize();

  alart;
  prg.Parse();

  alart;
  auto res = prg.Run();

  alart;
  //std::cout << res.to_string();



}