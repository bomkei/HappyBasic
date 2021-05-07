#include "main.h"

Program* Program::instance;

int main(int argc, char** argv)
{
  srand((int)time(nullptr));

  for( auto&& arg : Utils::ToVector<std::string>(argc, argv) )
  {
    if( arg == "-safety" && !Options::IsSafety )
    {
      Options::IsSafety = true;
    }
    else if( Options::FileName.empty() )
    {
      Options::FileName = arg;
    }
    else
    {
      std::cout
        << "fatal: this argument is specified or unknown.\n"
        << "  " << arg << '\n';

      exit(2);
    }
  }

  Program prg;

  prg.OpenFile();

  prg.Tokenize();
  
  prg.Parse();

  auto res = prg.Run();


}