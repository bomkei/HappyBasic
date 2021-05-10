#pragma once

struct Object
{
  enum Type
  {
    Int,
    Float,
    Char,
    Array,
    ClassObj
  };

  Type type = Int;
  std::string name;
  
  int v_int = 0;
  float v_float = 0;
  char v_char = 0;
  std::vector<Object> list;

  Object* var_ptr = nullptr;
  AST::Class* class_ptr = nullptr;

  Object& operator = (Object const& obj);

  std::string ToString() const;
  bool IsString() const;
  bool Eval() const;
  bool Equal(Object const&) const;

  template<typename T>
  T as()
  {
    switch( type )
    {
      case Int:
        return static_cast<T>(v_int);

      case Float:
        return static_cast<T>(v_float);

      case Char:
        return static_cast<T>(v_char);

      default:
        break;
    }
    
    return T();
  }
};

