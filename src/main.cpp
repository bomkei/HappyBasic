#include "main.h"

Program* Program::instance;

/// mainコード
int main(int argc, char** argv)
{
  Program prg;

  alart;

#ifdef _MSC_VER
  prg.OpenFile("C:/Users/mrzkr/Desktop/Sources/C++/HappyBasic/script.txt");
#else
  prg.OpenFile(argc == 1 ? "./plugin/BASIC_lang/test.txt" : argv[1]);
#endif

  alart;
  prg.Tokenize();
  
  alart;
  prg.Parse();

  alart;
  auto res = prg.Run();


}