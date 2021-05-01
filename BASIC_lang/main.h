#pragma once

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "common.h"
#include "utils.h"
#include "color.h"
#include "object.h"
#include "token.h"
#include "AST.h"

class Tokenizer
{
  std::string source;
  size_t position;

  char peek() const;
  bool check() const;
  bool match(std::string const&) const;
  void next();
  void pass_space();

public:
  void Initialize(std::string&&);

  std::vector<Token> Tokenize() const;


};

class ParserCore
{
  std::vector<Token> tokens;
  size_t index;
  
public:

  void Initialize(std::vector<Token>&&);


};

class ProgramContext
{

};


//
// ソースコード
//extern std::string g_source;

//
// 変数
//struct Object;
//extern std::vector<Object> g_variables;


std::vector<Token> Tokenize(std::string&& source);

bool is_hex(std::string);

void SetConsoleColor(int col);
