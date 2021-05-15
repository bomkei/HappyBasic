#include "main.h"

class ProgramImpl
{
  Token* token;
  std::vector<Object> variables;
  std::vector<AST::Function*> functions;
  std::vector<AST::Struct*> structs;

  std::string source;

  Tokenizer* tokenizer;
  ParserCore* parser;

  AST::Stmt* prs_result;
  AST::Function* cur_func;

  friend class Program;

  ProgramImpl() {
    tokenizer = new Tokenizer(source);
    parser = new ParserCore(token, variables, functions, structs);

    prs_result = nullptr;
    cur_func = nullptr;
  }

public:
  ~ProgramImpl() {

  }
};

Program::Program()
  :_impl(new ProgramImpl())
{
  _instance = this;
}

Program::~Program() = default;

AST::Function* Program::GetFunction(std::string const& name) const
{
  for( auto&& i : _impl->functions ) {
    if( i->name == name )
      return i;
  }

  return nullptr;
}

Object* Program::GetVariable(std::string const& name) const {
  for( auto&& i : _impl->variables ) {
    if( i.name == name )
      return &i;
  }

  return nullptr;
}

std::vector<Object>& Program::GetVariables() const {
  return _impl->variables;
}

std::vector<AST::Struct*> Program::GetStructs() const {
  return _impl->structs;
}

AST::Function*& Program::GetCurrentFunction() const {
  return _impl->cur_func;
}

void Program::OpenFile()
{
  static auto is_empty = [] (std::string const& str) {
    for( auto&& c : str )
      if( c > ' ' )
        return false;

    return true;
  };

  std::ifstream ifs(Options::FileName);
  std::string line;
  auto& src = _impl->source;

  if( Options::FileName.empty() )
  {
    std::cout << "fatal: no input file\n";
    exit(1);
  }

  if( ifs.fail() )
  {
    std::cout << "cannot open file: " << Options::FileName << '\n';
    exit(1);
  }

  while( std::getline(ifs, line) )
  {
    while( line.length() && line[line.length() - 1] <= ' ' )
      line.pop_back();

    while( line.length() && line[0] <= ' ' )
      line.erase(line.begin());

    src += line + '\n';
  }

  if( is_empty(src) )
  {
    std::cout << "empty source file";
    exit(1);
  }

  for( size_t i = 0; i < src.length(); ) {
    if( src[i] == '"' ) {
      for( ++i; src[i] != '"'; i++ );
      i++;
    }
    else if( i + 2 <= src.length() && src.substr(i, 2) == "//" ) {
      src[i] = src[i + 1] = ' ';
      i += 2;

      while( src[i] != '\n' ) {
        src[i] = ' ';
        i++;
      }
    }
    else if( i + 2 <= src.length() && src.substr(i, 2) == "/*" ) {
      src[i] = src[i + 1] = ' ';
      i += 2;

      while( i < src.length() ) {
        if( i + 2 <= src.length() && src.substr(i, 2) == "*/" ) {
          src[i] = src[i + 1] = ' ';
          i += 2;
          break;
        }
        else {
          src[i] = ' ';
          i++;
        }
      }
    }
    else i++;
  }
}

void Program::Tokenize()
{
  _impl->token = _impl->tokenizer->Tokenize();

}

void Program::Parse()
{
  _impl->prs_result = _impl->parser->Parse();
}

void Program::ViewNodes() {

  std::cout << _impl->prs_result->ToString() << '\n';

}

Object Program::Run()
{

  return AST_Runner::Stmt(_impl->prs_result);
}

Program* Program::GetInstance() {
  return _instance;
}

void Program::Error(Token const& tok, std::string const& msg)
{
  size_t
    line = 1,
    begin = 0,
    end = GetInstance()->_impl->source.length();
  
  for( size_t i = 0; i < tok.srcpos; i++ )
  {
    if( GetInstance()->_impl->source[i] == '\n' )
    {
      line++;
      begin = i + 1;
    }
  }

  for( auto i = begin; i < end; i++ ) {
    if( GetInstance()->_impl->source[i] == '\n' ) {
      end = i;
      break;
    }
  }

  std::cout
    << '\n'
    << "Error:\n"
    << format("%6zd|", line) + Program::GetInstance()->_impl->source.substr(begin, end - begin) << '\n'
    << "      |" << std::string(tok.srcpos - begin, ' ')
    << "^" << std::string(tok.str.length() > 1 ? tok.str.length() - 1 : 0, '~')
    << "   " << msg << "\n\n";

  exit(1);
}