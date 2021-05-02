#include "main.h"

int main()
{
  try
  {
    Program prg;
    
    prg.OpenFile("test.txt");
    
    prg.Tokenize();
    prg.Parse();
    
    auto res = prg.Run();
    
    std::cout << res.to_string();
  }
  catch( ErrorInfo const& info )
  {
    std::cout << std::get<1>(info) << '\n';


  }

}


