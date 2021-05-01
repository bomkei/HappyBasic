#include "main.h"

struct ReservedToken
{
  std::string name;
  int value;

  ReservedToken(std::string const& name, int v = 0)
    :name(name), value(v)
  {
  }

  bool operator == (ReservedToken rs)
  {
    return name == rs.name;
  }
};

static std::vector<ReservedToken> Reserved_Words =
{
  { "COL_BLACK",        0x0 },
  { "COL_DARK_BLUE",    0x1 },
  { "COL_DARK_GREEN",   0x2 },
  { "COL_DARK_CYAN",    0x3 },
  { "COL_DARK_RED",     0x4 },
  { "COL_DARK_VIOLET",  0x5 },
  { "COL_DARK_YELLOW",  0x6 },
  { "COL_GRAY",         0x7 },
  { "COL_LIGHT_GRAY",   0x8 },
  { "COL_BLUE",         0x9 },
  { "COL_GREEN",        0xA },
  { "COL_CYAN",         0xB },
  { "COL_RED",          0xC },
  { "COL_VIOLET",       0xD },
  { "COL_YELLOW",       0xE },
  { "COL_WHITE",        0xF },
};

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

bool operator == (ReservedToken L, ReservedToken R)
{
  return L.name == R.name;
}

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

    // ����
    if( isdigit(c) )
    {
      tok.type = Token::Number;

      // "0x" ������� 16 �i��
      if( match("0x") )
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
    else if( isalnum(c) || c == '_' )
    {
      tok.type = Token::Ident;

      while( check() && (isalnum(c = peek()) || c == '_') )
        tok.str += c, next();

      int find = find_vector<ReservedToken>(Reserved_Words, tok.str);

      if( find != -1 )
      {
        tok.type = Token::Number;
        tok.obj.v_int = Reserved_Words[find].value;
      }
      else
      {
        for( auto&& c : tok.str )
          if( c >= 'A' && c <= 'Z' ) c += 0x20;
      }
    }

    // ����
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
        if( match(str) )
        {
          next(str.length());
          hit = true;
          tok.str = str;
          break;
        }
      }

      if( hit == false )
      {
        Program::Error(tok, "unknown token");
      }
    }

    tokens.emplace_back(tok);
    pass_space();
  }

  return tokens;
}

