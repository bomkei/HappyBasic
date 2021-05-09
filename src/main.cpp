#include "main.h"

Program* Program::instance;

int main(int argc, char** argv)
{
  srand((int)time(nullptr));

  auto&& args = Utils::ToVector<std::string>(argc, argv);

  args.erase(args.begin());

  for( auto&& arg : args )
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
        << "fatal: this is already specified or unknown.\n"
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