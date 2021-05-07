#include "main.h"

Program* Program::instance;

int main(int argc, char** argv)
{
  srand((int)time(nullptr));

  Program prg;

  prg.OpenFile(argc == 1 ? "./plugin/BASIC_lang/test.txt" : argv[1]);

  prg.Tokenize();
  
  prg.Parse();

  auto res = prg.Run();


}