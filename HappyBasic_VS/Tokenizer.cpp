#include "main.h"

static auto op_tokens =
{
  "...",
  "::",
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
};


Token* Tokenize(std::string const& source)
{
  size_t position = 0;
  Token top;
  auto cur = &top;

  static auto check = [&] {return position < source.length(); };
  static auto peek = [&] {return source[position]; };
  static auto match = [&] (std::string str) { return position + str.length() <= source.length() && source.substr(position, str.length()) == str; };
  static auto next = [&] (int n = 1) {position += n; };
  static auto pass_space = [&] { while( check() && peek() <= ' ' )next(); };

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

      // 16 進数
      if( match("0x") )
      {
        next(2);

        while( check() && (isdigit(c = peek()) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) )
          tok.str += c, next();

        tok.obj.v_int = std::stoi(tok.str, nullptr, 16);
      }

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
      {
        tok.str += c, next();
      }

      tok.obj.name = tok.str;
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
        PrgCtx::Error(tok, "unclosed char literal");

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

    // 演算子 / 記号
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

      if( !hit )
      {
        PrgCtx::Error(tok, "unknown token");
      }
    }

    auto tk = new Token(tok);
    cur->next = tk;
    cur = tk;
    
    pass_space();
  }

  auto tk = new Token;
  tk->type = Token::End;
  cur->next = tk;
  
  return top.next;
}