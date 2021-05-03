#pragma once

struct Object
{
  enum Type
  {
    Int,
    Float,
    Char,
    Array,
    Pointer
  };

  Type type = Int;
  std::string name;

  int v_int = 0;
  float v_float = 0;
  char v_char = 0;
  std::vector<Object> list;

  Object* var_ptr = nullptr;

  Object& operator = (Object const& obj)
  {
    type = obj.type;
    v_int = obj.v_int;
    v_float = obj.v_float;
    v_char = obj.v_char;

    list = obj.list;
    var_ptr = obj.var_ptr;

    return *this;
  }

  std::string to_string() const
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

    case Pointer:
      return "{" + var_ptr->name + "}";
    }

    return "";
  }

  bool is_string() const
  {
    if( type != Array )
      return false;

    for( auto&& c : list )
      if( c.type != Char )
        return false;

    return list.size() != 0;
  }

  bool eval() const
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
};

