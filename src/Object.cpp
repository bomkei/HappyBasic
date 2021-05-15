#include "main.h"

std::string Object::ToString() const
{
  switch( type )
  {
    case Int:
      return std::to_string(v_int);

    case Float:
      return std::to_string(v_float);

    case Char:
      return std::string(1, v_char);

    case Array: {
      std::string s = "";

      if( IsString() ) {
        for( auto&& c : list )
          s += c.v_char;

        return s;
      }

      for( int i = 0; i < list.size(); i++ ) {
        s += list[i].ToString();
        if( i < list.size() - 1 ) s += ", ";
      }

      return  "[" + s + "]";
    }

    case StructObj: {
      std::string s;

      for( size_t i = 0; i < list.size(); i++ ) {
        s += list[i].name + ":" + list[i].ToString();
        if( i < list.size() - 1 ) s += ", ";
      }

      return "struct<" + struct_ptr->name + ">{" + s + "}";
    }
  }

  return "";
}

bool Object::IsString() const {
  if( type != Array )
    return false;

  for( auto&& c : list )
    if( c.type != Char )
      return false;

  return list.size() != 0;
}

bool Object::Eval() const
{
  switch( type )
  {
    case Int:
      return v_int != 0;

    case Float:
      return v_float != 0;

    case Char:
      return v_char != 0;

    case Array:
      return list.size() != 0;
  }

  return false;
}

bool Object::Equal(Object const& obj) const
{
  if( obj.type != type )
    return false;

  switch( type ) {
    case Int: return obj.v_int == v_int;
    case Float: return obj.v_float == v_float;
    case Char: return obj.v_char == v_char;

    case Array:
      if( obj.list.size() != list.size() )
        return false;
      
      for( int i = 0; i < obj.list.size(); i++ )
        if( !obj.list[i].Equal(list[i]) ) return false;
      
      return true;
  }

  return false;
}

Object MakeObjectFromString(std::string const& str) {
  Object ret;
  ret.type = Object::Array;

  for( auto&& c : str ) {
    Object cc;
    cc.type = Object::Char;
    cc.v_char = c;
    ret.list.emplace_back(cc);
  }

  return ret;
}