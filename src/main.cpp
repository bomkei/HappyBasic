#include "main.h"

int main(int argc, char** argv)
{
  srand((int)time(nullptr));

  auto&& args = Utils::ToVector<std::string>(argc - 1, argv + 1);

  for( auto&& arg : args ) {
    if( arg == "-safety" && !Options::IsSafety ) {
      Options::IsSafety = true;
    }
    else if( arg == "-node" && !Options::ViewNodes ) {
      Options::ViewNodes = true;
    }
    else if( Options::FileName.empty() ) {
      Options::FileName = arg;
    }
    else {
      std::cout
        << "fatal: this argument is already specified or unknown.\n"
        << "  " << arg << '\n';

      exit(2);
    }
  }

  Program prg;

  alart;
  prg.OpenFile();

  alart;
  prg.Tokenize();
  
  alart;
  prg.Parse();

  //alart;
  //prg.Check();

  alart;
  return 0;

  if( Options::ViewNodes ) {
    prg.ViewNodes();
    return 0;
  }

  alart;
  auto res = prg.Run();


  alart;
  debugmsg("good bye.");
}