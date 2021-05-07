#pragma once

struct Object
{
  enum Type
  {
    Int,
    Float,
    Char,
    Array,
  };

  Type type = Int;
  std::string name;

  int v_int = 0;
  float v_float = 0;
  char v_char = 0;
  std::vector<Object> list;

  Object* var_ptr = nullptr;

  Object& operator = (Object const& obj);

  std::string to_string() const;
  bool is_string() const;
  bool eval() const'
  bool equal(Object const&) const;

  template<typename T>
  T as(){
    T ret;
    switch( type )
    {
    case Int:
      return static_cast<T>(v_int);
      break;

    case Float:
      return static_cast<T>(v_float);
      break;

    case Char:
      return static_cast<T>(v_char);
      break;

    default:
      return T();
    }
  }
};

