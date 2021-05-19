#include <iostream>
#include <fstream>
#include <ctime>

#include "Utils.h"

#include "Object.h"
#include "Token.h"
#include "AST.h"

#include "Tokenizer.h"
#include "Parser.h"
#include "AST_Runner.h"
#include "Compiler.h"

#include "Global.h"

using namespace Global;

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
  srand((int)time(nullptr));

  auto args = Utils::ToVector<std::string>(argc - 1, argv + 1);

  for( auto&& i : args ) {
    if( i == "-safety" ) {
      Options::IsSafety = true;
    }
    else if( i == "-view_ast" ) {
      Options::ViewNodes = true;
    }
    else if( Options::FileName.empty() ) {
      Options::FileName = i;
    }
    else {
      std::cout << "unknown arg";
      return 1;
    }
  }

  auto source = readfile(
#ifndef _MSC_VER
    Options::FileName
#else
    "C:/Users/mrzkr/Desktop/test.txt"
#endif
  );

  auto token = Tokenize(source);

  ParserCore::Init(token);
  auto ast = ParserCore::Parse();

  auto obj = AST_Runner::Stmt(ast);


}