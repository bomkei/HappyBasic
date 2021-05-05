#include "main.h"

Program* Program::instance;

/// mainコード
int main(int argc, char** argv)
{
  Program prg;

  alart;
  prg.OpenFile(argc == 1 ? "./plugin/BASIC_lang/test.txt" : argv[1]);

  alart;
  prg.Tokenize();
  
  alart;
  prg.Parse();

  alart;
  auto res = prg.Run();


}