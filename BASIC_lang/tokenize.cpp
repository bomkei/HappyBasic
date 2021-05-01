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

std::string g_source;
size_t srcpos;

bool operator == (ReservedToken L, ReservedToken R)
{
  return L.name == R.name;
}

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

  static auto next = [] (size_t n = 1)
  {
    srcpos += n;
  };

  static auto match = [] (std::string const& s)
  {
    return
      srcpos + s.length() <= g_source.length()
      &&
      g_source.substr(srcpos, s.length()) == s;
  };

  static auto pass_space = []
  {
    while( check() )
    {
      if( match("\\\n") )
        srcpos += 2;
      else if( peek() <= ' ' && peek() != '\n' )
        srcpos++;
      else
        break;
    }
  };

  static auto isident = [] (char c)
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
      {
        //if( c >= 'A' && c <= 'Z' )
        //  c += ('a' - 'A');

        tok.str += c, next();
      }

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
        tok.Error("unclosed char literal");

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

