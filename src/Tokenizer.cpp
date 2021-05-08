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

static std::vector<std::pair<std::string, int>> ReservedWords =
{
  { "COL_WHITE", 0 },
  { "COL_BLACK", 0x00 },
  { "COL_DARK_BLUE", 0x01 },
  { "COL_DARK_GREEN", 0x02 },
  { "COL_DARK_CYAN", 0x03 },
  { "COL_DARK_RED", 0x04 },
  { "COL_DARK_VIOLET", 0x05 },
  { "COL_DARK_YELLOW", 0x06 },
  { "COL_GRAY", 0x07 },
  { "COL_LIGHT_GRAY", 0x08 },
  { "COL_BLUE", 0x09 },
  { "COL_GREEN", 0x0a },
  { "COL_CYAN", 0x0b },
  { "COL_RED", 0x0c },
  { "COL_VIOLET", 0x0d },
  { "COL_YELLOW", 0x0e },
  { "COL_WHITE", 0x0f },

};


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
  while( check() && peek() <= ' ' && peek() != '\n' )
    next();
}

Tokenizer::Tokenizer(std::string const& src)
  :source(src), position(0)
{

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

    if( isdigit(c) )
    {
      tok.type = Token::Number;

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

        if( peek() == '.' )
        {
          next();
          tok.str += '.';

          while( check() && isdigit(c = peek()) )
            tok.str += c, next();

          tok.obj.type = Object::Float;
          tok.obj.v_float = std::stof(tok.str);
        }

        else
        {
          tok.obj.v_int = std::stoi(tok.str);
        }
      }
    }

    else if( isalnum(c) || c == '_' )
    {
      tok.type = Token::Ident;

      while( check() && (isalnum(c = peek()) || c == '_') )
      {
        if( c >= 'A' && c <= 'Z' ) c += 0x20;

        tok.str += c, next();
      }

      tok.obj.name = tok.str;
    }

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
        Program::Error(tok, "unknown token");
      }
    }

    tokens.emplace_back(tok);
    pass_space();
  }

  for( size_t i = 0; i < tokens.size() - 1;)
  {
    if( tokens[i].str == "\n" && tokens[i + 1].str == "\n" )
      tokens.erase(tokens.begin() + i);
    else
      i++;
  }

  for( auto&& tk : tokens )
  {
    if( tk.str == "__happybasic__" )
    {
      tk.type = Token::String;
      tk.obj.type = Object::Array;

      for( auto&& c : "HappyBasic" )
      {
        Object ch;
        ch.type = Object::Char;
        ch.v_char = c;
        tk.obj.list.emplace_back(ch);
      }
    }
  }

  return tokens;
}