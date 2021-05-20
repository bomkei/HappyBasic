#include <iostream>

#include "Utils.h"

#include "Object.h"
#include "Token.h"
#include "AST.h"

#include "Parser.h"
#include "Global.h"

using namespace Global;

std::string Mangle(std::string fun_name, std::vector<std::string> class_layers, std::vector<std::string> args) {

  std::string ret = "_?@_";

  for( auto&& L : class_layers )
    ret += L + "_";

  ret += "?@;" + std::to_string(args.size());

  for( auto&& arg : args )
    ret += arg + "`";

  return ret;
}