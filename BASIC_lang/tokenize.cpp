#include "main.h"

std::string g_source;
size_t srcpos;

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

std::vector<Token> Tokenize(std::string&& src)
{
  g_source = std::move(src);
  srcpos = 0;

  std::vector<Token> tokens;

  static auto peek = []
  {
    return g_source[srcpos];
  };

  static auto check = []
  {
    return srcpos < g_source.length();
  };

  static auto next = [](int n = 1)
  {
    srcpos += n;
  };

  static auto pass_space = []
  {
    while( peek() != '\n' && peek() <= ' ' )
      next();
  };

  static auto isident = [](char c)
  {
    return isalnum(c) || c == '_';
  };

  pass_space();

  while( check() )
  {
    auto c = peek();

    Token tok;
    tok.srcpos = srcpos;

    // ����
    if( isdigit(c) )
    {
      tok.type = Token::Number;

      // "0x" ������� 16 �i��
      if( srcpos + 2 <= g_source.length() && g_source.substr(srcpos, 2) == "0x" )
      {
        next(2);

        while( check() && (isdigit(c = peek()) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) )
          tok.str += c, next();

        tok.obj.v_int = std::stoi(tok.str, nullptr, 16);
      }

      // �����ꍇ�A10 �i��
      else
      {
        while( check() && isdigit(c = peek()) )
          tok.str += c, next();

        // ���������_��
        if( peek() == '.' )
        {
          next();
          tok.str += '.';

          while( check() && isdigit(c = peek()) )
            tok.str += c, next();

          tok.obj.type = Object::Float;
          tok.obj.v_float = std::stof(tok.str);
        }

        // ����
        else
        {
          tok.obj.v_int = std::stoi(tok.str);
        }
      }
    }

    // ���ʎq
    else if( isident(c) )
    {
      tok.type = Token::Ident;

      while( check() && isident(c = peek()) )
        tok.str += c, next();
    }

    // ������
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

    // ���Z�q
    else
    {
      auto hit = false;
      tok.type = Token::Operator;

      for( std::string str : op_tokens )
      {
        if( srcpos + str.length() <= g_source.length() && g_source.substr(srcpos, str.length()) == str )
        {
          next(str.length());
          hit = true;
          tok.str = str;
          break;
        }
      }

      if( hit == false )
      {
        tok.Error("unknown token");
      }
    }

    tokens.emplace_back(tok);
    pass_space();
  }

  return tokens;
}

