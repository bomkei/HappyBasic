#include "main.h"

Program::Program()
{
  tokenizer = new Tokenizer(source);
  parser = new ParserCore(variables);

  instance = this;
}

void Program::OpenFile(std::string const& path)
{
  static auto is_empty = [] (std::string const& str) {
    for( auto&& c : str )
      if( c > ' ' )
        return false;

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

void Program::Tokenize()
{
  parser->Initialize(std::move(tokenizer->Tokenize()));
}

void Program::Parse()
{
  prs_result = parser->Parse();
}

Object Program::Run()
{

  return AST_Runner::Stmt(prs_result);
}

void Program::Error(Token const& tok, std::string const& msg)
{
  alart;

  size_t
    line = 1,
    begin = 0,
    end = instance->source.length();

  for( size_t i = 0; i < tok.srcpos; i++ )
  {
    if( instance->source[i] == '\n' )
    {
      line++;
      begin = i + 1;
    }
  }

  for( auto i = begin; i < end; i++ )
  {
    if( instance->source[i] == '\n' )
    {
      end = i;
      break;
    }
  }

  alart;

  std::cout
    << '\n'
    << format("%6zd|", line) + instance->source.substr(begin, end - begin) << '\n'
    << "      |" << std::string(tok.srcpos - begin, ' ') << "^" << msg;

  exit(1);
}