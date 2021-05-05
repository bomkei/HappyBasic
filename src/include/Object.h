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

  Object& operator = (Object const& obj);

  std::string to_string() const;
  bool is_string() const;
  bool eval() const;
  bool equal(Object const&) const;
};

