#include "main.h"

// ----------------------------------------------- //
//  ������u������
// 
//  str       = �u�����������s���镶����
//  find      = �������镶����
//  replace   = �V�����u�������镶����
// 
//  �߂�      = ���ׂĂ� find �� replace �Ƃ��Ēu��������ꂽ������
// 
//  ����   �������ɓn����������͕ύX����܂�
// ----------------------------------------------- //
template <class STR>
auto& string_replace(STR& str, STR find, STR replace)
{
  for( int i = 0; i < str.length() - find.length(); )
  {
    // find �ƈ�v������
    if( str.substr(i, find.length()) == find )
    {
      str.erase(i, find.length());  // ���̕�������폜
      str.insert(i, replace);       // �V�����������}��
      i += replace.length();        // ����ʒu���A�}������������̒����̕��i�߂�
    }
    else
      i++;
  }

  return str;
}


// ----------------------------------------------- //
//  �t�@�C���̒��g���\�[�X�R�[�h�Ƃ��ēǂݎ��
// ----------------------------------------------- //
auto readfile(std::string const& path)
{
  static auto is_empty = [](std::string const& str)
  {
    for( auto&& c : str )
      if( c > ' ' ) return false;

    return true;
  };

  std::ifstream ifs(path);
  std::string ret, line;

  // �t�@�C�����J���Ȃ�����
  if( ifs.fail() )
  {
    std::cout << "cannot open file";
    exit(1);
  }

  // ��s���ǂݎ���Ă���
  while( std::getline(ifs, line) )
  {
    // ������������΃X�L�b�v
    if( is_empty(line) )
      continue;

    // �E�[�̋󔒈ȉ��̕������폜
    while( line.length() && line[line.length() - 1] <= ' ' )
      line.pop_back();

    // ���[�̋󔒈ȉ��̕������폜
    while( line.length() && line[0] <= ' ' )
      line.erase(line.begin());

    // ���s���������� ret �ɒǉ�����
    ret += line + '\n';
  }


  // �s���Ȃ���L��
  string_replace<std::string>(ret, "\\\n", "");

  // �\�[�X����
  if( is_empty(ret) )
  {
    std::cout << "empty source file";
    exit(1);
  }

  return ret;
}

int main()
{
  auto source = std::move(readfile("C:/users/mrzkr/desktop/test.txt"));
  std::cout << source << '\n';

  auto tokens = std::move(Tokenize(std::move(source)));

  for( auto&& tok : tokens )
  {
    std::cout << ":" << tok.str << '\n';
  }



}


