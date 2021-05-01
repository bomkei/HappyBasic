#pragma once

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include <utility>

#include "common.h"
#include "utils.h"
#include "color.h"
#include "object.h"
#include "token.h"
#include "AST.h"

using ErrorInfo = std::pair<Token*, std::string>;

class Tokenizer
{
  std::string source;
  size_t position;

  bool check() const;
  char peek() const;
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
  
  Token& get_tok() const;

  bool check();
  bool consume(std::string const&);
  void expect(std::string const);
  void next();

public:

  void Initialize(std::vector<Token>&&);

  AST* Primary();
  AST* Mul();
  AST* Add();
  AST* Expr();

  AST* Parse();


};

class Program
{
  std::vector<Object> variables;
  std::string source;

  Tokenizer tokenizer;
  ParserCore parser;

public:

  void OpenFile(std::string const& path);

  void Tokenize();
  void Parse();

  Object Run() const;

  [[noreturn]]
  void Error(Token* tok, std::string const& msg);



};


//
// ソースコード
//extern std::string g_source;

//
// 変数
//struct Object;
//extern std::vector<Object> g_variables;


std::vector<Token> Tokenize(std::string&& source);
