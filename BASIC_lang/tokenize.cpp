#include "main.h"

static auto op_tokens =
{
  "...",
  "&=",
  "|=",
  "^=",
  "+=",
  "-=",
  "*=",
  "/=",
  "%=",
  ">>",
  "<<",
  "==",
  "!=",
  ">=",
  "<=",
  "&&",
  "||",
  "!",
  "~",
  "&",
  "|",
  ">",
  "<",
  "=",
  "+",
  "-",
  "*",
  "/",
  "%",
  "(",
  ")",
  "[",
  "]",
  "{",
  "}",
  ",",
  ".",
  ";",
  ":",
  "\n",
};

Tokenizer::Tokenizer(std::string const& src)
  :source(src), position(0)
{

}

bool Tokenizer::check() const
{
  return position < source.length();
}

char Tokenizer::peek() const
{
  return source[position];
}

bool Tokenizer::match(std::string const& str) const
{
  return position + str.length() <= source.length() && source.substr(position, str.length()) == str;
}

void Tokenizer::next(int n)
{
  position += n;
}

void Tokenizer::pass_space()
{
  while( check() && peek() <= ' ' )
    next();
}

std::vector<Token> Tokenizer::Tokenize()
{
  std::vector<Token> tokens;

  pass_space();

  while( check() )
  {
    auto c = peek();

    Token tok;
    tok.srcpos = position;

    // 数字
    if( isdigit(c) )
    {
      tok.type = Token::Number;

      // "0x" があれば 16 進数
      if( match("0x") )
      {
        next(2);

        while( check() && (isdigit(c = peek()) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) )
          tok.str += c, next();

        tok.obj.v_int = std::stoi(tok.str, nullptr, 16);
      }

      // 無い場合、10 進数
      else
      {
        while( check() && isdigit(c = peek()) )
          tok.str += c, next();

        // 浮動小数点数
        if( peek() == '.' )
        {
          next();
          tok.str += '.';

          while( check() && isdigit(c = peek()) )
            tok.str += c, next();

          tok.obj.type = Object::Float;
          tok.obj.v_float = std::stof(tok.str);
        }

        // 整数
        else
        {
          tok.obj.v_int = std::stoi(tok.str);
        }
      }
    }

    // 識別子
    else if( isalnum(c) || c == '_' )
    {
      tok.type = Token::Ident;

      while( check() && (isalnum(c = peek()) || c == '_') )
        tok.str += c, next();

//      int find = find_vector<ReservedToken>(Reserved_Words, tok.str);

//      if( find != -1 )
//      {
//        tok.type = Token::Number;
//        tok.obj.v_int = Reserved_Words[find].value;
//      }
//      else
//      {
//        for( auto&& c : tok.str )
//          if( c >= 'A' && c <= 'Z' ) c += 0x20;
//      }
    }

    // 文字
    else if( c == '\'' )
    {
      next();
      tok.type = Token::Char;

      tok.obj.type = Object::Char;
      tok.obj.v_char = peek();

      next();
      if( peek() != '\'' )
        Program::Error(tok, "unclosed char literal");

      next();
    }

    // 文字列
    else if( c == '"' )
    {
      tok.type = Token::String;
      tok.obj.type = Object::Array;

      next();
      tok.str += '"';

      while( check() && (c = peek()) != '"' )
      {
        tok.str += c;

        Object ch;
        ch.type = Object::Char;
        ch.v_char = c;
        tok.obj.list.emplace_back(ch);

        next();
      }

      next();
      tok.str += '"';
    }

    // 演算子
    else
    {
      auto hit = false;
      tok.type = Token::Operator;

      for( std::string str : op_tokens )
      {
        if( match(str) )
        {
          next(str.length());
          hit = true;
          tok.str = str;
          break;
        }
      }

      if(!hit)
      {
        Program::Error(tok, "unknown token");
      }
    }

    tokens.emplace_back(tok);
    pass_space();
  }

  return tokens;
}

