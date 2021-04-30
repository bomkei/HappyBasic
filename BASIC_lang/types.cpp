#include "main.h"

Token::Token(Token const& tok)
{
  *this = tok;
}

Token::Token(Token&& tok)
{
  *this = std::move(tok);
}

Token& Token::operator = (Token const& tok)
{
  type = tok.type;
  str = tok.str;
  obj = tok.obj;
  srcpos = tok.srcpos;

  return *this;
}

Token& Token::operator = (Token&& tok)
{
  type = tok.type;
  str = std::move(tok.str);
  obj = std::move(tok.obj);
  srcpos = tok.srcpos;

  return *this;
}


Object::Object(Object::Type type)
  :type(type), v_int(0), v_float(0), v_char(0), var_ptr(nullptr)
{

}

Object::Object(Object const& obj)
{
  *this = obj;
}

Object::Object(Object&& obj)
{
  *this = std::move(obj);
}

Object& Object::operator = (Object const& obj)
{
  type = obj.type;
  v_int = obj.v_int;
  v_float = obj.v_float;
  v_char = obj.v_char;

  name = obj.name;
  list = obj.list;
  var_ptr = obj.var_ptr;

  return *this;
}

Object& Object::operator = (Object&& obj)
{
  type = obj.type;
  v_int = obj.v_int;
  v_float = obj.v_float;
  v_char = obj.v_char;

  name = std::move(obj.name);
  list = std::move(obj.list);
  var_ptr = obj.var_ptr;

  Object::Clear(obj);

  return *this;
}

std::string Object::to_string() const
{
  switch( type )
  {
    case Int:
      return std::to_string(v_int);

    case Float:
      return std::to_string(v_float);

    case Char:
      return std::string(1, v_char);

    case Array:
    {
      std::string s = "";

      if( is_string() )
      {
        for( auto&& c : list )
          s += c.v_char;

        return s;
      }

      for( int i = 0; i < list.size(); i++ )
      {
        s += list[i].to_string();
        if( i < list.size() - 1 ) s += ", ";
      }

      return  "[" + s + "]";
    }
  }

  return "";
}

void Object::Clear(Object& obj)
{
  obj.type = Int;
  obj.v_int = obj.v_float = obj.v_char = 0;
  obj.list.clear();
  obj.name.clear();
}

bool Object::is_string() const
{
  if( this->type != Array )
    return false;

  for( auto&& c : list )
    if( c.type != Char )
      return false;

  return list.size() != 0;
}

bool Object::eval() const
{
  switch( type )
  {
    case Int: return v_int != 0;
    case Float: return v_float != 0;
    case Char: return v_char != 0;
    case Array: return list.size() != 0;
  }

  return false;
}

