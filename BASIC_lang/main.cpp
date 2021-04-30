#include "main.h"


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
    // �E�[�̋󔒈ȉ��̕������폜
    while( line.length() && line[line.length() - 1] <= ' ' )
      line.pop_back();

    // ���[�̋󔒈ȉ��̕������폜
    while( line.length() && line[0] <= ' ' )
      line.erase(line.begin());

    // ���s���������� ret �ɒǉ�����
    ret += line + '\n';
  }
  
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
  
  auto tokens = std::move(Tokenize(std::move(source)));

  auto node = Parse(std::move(tokens));

  RunStmt(node);

}


