#include "main.h"

Program* Program::instance;

int main(int argc, char** argv)
{
  srand((int)time(nullptr));



  Program prg;

  prg.OpenFile();

  prg.Tokenize();
  
  prg.Parse();

  auto res = prg.Run();


}