#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include <functional>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;


struct Token;
using ErrorInfo = std::pair<Token, std::string>;


template <class T, class F, class... Args>
int find_vector(std::vector<T>& vec, F compare, Args ...args)
{
  for( int i = 0; i < vec.size(); i++ )
    if( compare(vec[i], args...) )
      return i;

  return -1;
}

#include "Object.h"
#include "Token.h"

#include "AST.h"
#include "Tokenizer.h"
#include "ParserCore.h"
#include "AST_Runner.h"
#include "Program.h"

