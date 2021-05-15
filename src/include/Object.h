#pragma once

namespace AST {
  class Class;
}

struct Object {
  enum Type {
    Int,
    Float,
    Char,
    Array,
    StructObj
  };

  Type type = Int;
  std::string name;
  
  int v_int = 0;
  float v_float = 0;
  char v_char = 0;
  std::vector<Object> list;

  Object* var_ptr = nullptr;
  AST::Struct* struct_ptr = nullptr;

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

