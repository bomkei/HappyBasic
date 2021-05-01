#pragma once

//
// オブジェクト
// 即値や変数、文字列などとして使用する
// さらに、動的な型変更もこれにより実現されている
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

  Type type;
  std::string name;

  int v_int;
  float v_float;
  char v_char;
  std::vector<Object> list;

  Object* var_ptr;

  Object(Type type = Int);

  Object& operator = (Object const& obj);

  std::string to_string() const;

  static void Clear(Object& obj);

  bool is_string() const;
  bool eval() const;
};
