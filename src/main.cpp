#include "main.h"

Program* Program::instance;

int main(int argc, char** argv)
{
  srand((int)time(nullptr));

  for( auto&& arg : Utils::ToVector<std::string>(argc, argv) )
  {
    if( arg == "-safety" )
    {

    }
  }

  Program prg;

  prg.OpenFile();

  prg.Tokenize();
  
  prg.Parse();

  auto res = prg.Run();


}