#include <iostream>
#include <fstream>
#include <memory>

#include "Utils.h"

#include "Options.h"
#include "Parser.h"

std::string Options::FileName;
bool Options::ViewNodes;
bool Options::IsSafety;

std::string readfile(std::string path) {
  static auto is_empty = [] (std::string const& str) {
    for( auto&& c : str )
      if( c > ' ' )
        return false;

    return true;
  };

  std::ifstream ifs(path);
  std::string src, line;

  if( ifs.fail() ) {
    std::cout << "cannot open file: " << path << '\n';
    exit(1);
  }

  while( std::getline(ifs, line) ) {
    while( line.length() && line[line.length() - 1] <= ' ' )
      line.pop_back();

    while( line.length() && line[0] <= ' ' )
      line.erase(line.begin());

    src += line + '\n';
  }

  if( is_empty(src) ) {
    std::cout << "empty source file";
    exit(1);
  }

  for( size_t i = 0; i < src.length(); ) {
    if( src[i] == '"' ) {
      for( ++i; src[i] != '"'; i++ );
      i++;
    }
    else if( i + 2 <= src.length() && src.substr(i, 2) == "//" ) {
      src[i] = src[i + 1] = ' ';
      i += 2;

      while( src[i] != '\n' ) {
        src[i] = ' ';
        i++;
      }
    }
    else if( i + 2 <= src.length() && src.substr(i, 2) == "/*" ) {
      src[i] = src[i + 1] = ' ';
      i += 2;

      while( i < src.length() ) {
        if( i + 2 <= src.length() && src.substr(i, 2) == "*/" ) {
          src[i] = src[i + 1] = ' ';
          i += 2;
          break;
        }
        else {
          src[i] = ' ';
          i++;
        }
      }
    }
    else i++;
  }

  return src;
}

int main(int argc, char** argv) {
  alart;
  srand((int)time(nullptr));

  alart;
  std::unique_ptr<ProgramContext> context;
  context.reset(new ProgramContext);

  alart;
  context->source = readfile("C:/Users/mrzkr/Desktop/test.txt");

  alart;
  context->token = Tokenize(context->source);

  alart;
  auto parser = ParserCore(
    context->token,
    context->variables,
    context->functions,
    context->structs
  );

  alart;
  auto ast = parser.Parse();

  alart;
  auto obj = AST_Runner::Stmt(ast);

  std::cout << "variables:\n";
  for( auto&& i : context->variables ) {
    std::cout << " " << i.name << ": " << i.ToString() << '\n';
  }


}