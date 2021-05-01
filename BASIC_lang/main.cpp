#include "main.h"

void Program::OpenFile(std::string const& path)
{
  static auto is_empty = [] (std::string const& str)
  {
    for( auto&& c : str )
      if( c > ' ' ) return false;

    return true;
  };

  std::ifstream ifs(path);
  std::string line;

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

    if( line.empty() )
      continue;

    // ���s���������� ret �ɒǉ�����
    source += line + '\n';
  }

  // �\�[�X����
  if( is_empty(source) )
  {
    std::cout << "empty source file";
    exit(1);
  }
}

int main()
{
  try
  {
    Program prg;

    prg.OpenFile("C:/Users/mrzkr/Desktop/test.txt");
    
    prg.Tokenize();
    prg.Parse();

    auto res = prg.Run();

  }
  catch( ErrorInfo const& info )
  {
    std::cout << std::get<1>(info) << '\n';


  }

}


