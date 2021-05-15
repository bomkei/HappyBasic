#pragma once

//class Tokenizer
//{
//  std::string const& source;
//  size_t position;
//
//  bool check() const;
//  char peek() const;
//  bool match(std::string const& str) const;
//  void next(int n = 1);
//  void pass_space();
//
//public:
//
//  Tokenizer(std::string const& src);
//
//  Token* Tokenize();
//};


Token* Tokenize(std::string const&);

