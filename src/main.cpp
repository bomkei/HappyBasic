#include "main.h"

Program* Program::instance;

int main(int argc, char** argv)
{
  Program prg;

#ifdef _MSC_VER
  prg.OpenFile("C:/Users/mrzkr/Desktop/Sources/C++/HappyBasic/script.txt");
#else
  prg.OpenFile(argc == 1 ? "./plugin/BASIC_lang/test.txt" : argv[1]);
#endif

  prg.Tokenize();
  
  prg.Parse();

  auto res = prg.Run();


}