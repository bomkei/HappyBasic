#include "main.h"

Program* Program::instance;

int main(int argc, char** argv)
{
  if( argc == 1 )
  {
    std::cout << "no input file. Good bye! :)";
    return 1;
  }

  Program prg;

  prg.OpenFile(argv[1]);

  prg.Tokenize();

  prg.Parse();

  auto res = prg.Run();


}