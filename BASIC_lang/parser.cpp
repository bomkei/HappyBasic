#include "main.h"

std::vector<Object> g_variables;

namespace
{
  std::vector<Token> g_tokens;
  size_t tkIndex;
  Token* csmtok;

  auto& curtok()
  {
    return g_tokens[tkIndex];
  }

  auto next(int n = 1)
  {
    tkIndex += n;
  }

  auto check()
  {
    return tkIndex < g_tokens.size();
  }

  auto consume(std::string const& str)
  {
    if( curtok().str == str )
    {
      csmtok = &curtok();
      next();
      return true;
    }

    return false;
  }

  auto expect(std::string const& str)
  {
    if( consume(str) == false )
    {
      if( str == "\n" )
      {
        tkIndex--;
        curtok().Error("expect new line after this token");
      }
      else
        curtok().Error("expect '" + str + "'");
    }
  }

  i64 find_var(std::string const& name)
  {
    for( i64 i = 0; i < g_variables.size(); i++ )
    {
      if( g_variables[i].name == name )
        return i;
    }

    return -1;
  }
}

Node::Node(Node::Type type)
  :type(type), lhs(nullptr), rhs(nullptr)
{

}

Node::Node(Node::Type type, Node* lhs, Node* rhs)
  :Node(type)
{
  this->lhs = lhs;
  this->rhs = rhs;

  this->tok.srcpos = csmtok->srcpos;
}


Node* Primary()
{
  if( curtok().type == Token::Number )
  {
    auto x = new Node;
    x->tok = curtok();

    next();
    return x;
  }

  if( curtok().type == Token::String )
  {
    auto x = new Node;
    x->tok.obj.type = Object::Array;

    for( int i = 1; i < curtok().str.length() - 1; i++ )
    {
      Object ch;
      ch.type = Object::Char;
      ch.v_char = curtok().str[i];
      x->tok.obj.list.emplace_back(ch);
    }

    next();
    return x;
  }

  if( curtok().type == Token::Ident )
  {
    auto x = new Node(Node::Variable);
    x->tok = curtok();

    auto find = find_var(x->tok.str);

    if( find == -1 )
    {
      find = g_variables.size();

      Object obj;
      obj.name = x->tok.str;
      g_variables.emplace_back(obj);
    }

    x->varIndex = find;

    next();
    return x;
  }

  curtok().Error("syntax error");
}

Node* Mul()
{
  auto x = Primary();

  while( check() )
  {
    if( consume("*") )
      x = new Node(Node::Mul, x, Primary());
    else if( consume("/") )
      x = new Node(Node::Div, x, Primary());
    else
      break;
  }

  return x;
}

Node* Add()
{
  auto x = Mul();

  while( check() )
  {
    if( consume("+") )
      x = new Node(Node::Add, x, Mul());
    else if( consume("-") )
      x = new Node(Node::Sub, x, Mul());
    else
      break;
  }
  
  return x;
}

Node* Expr()
{
  return Add();
}

Node* Stmt()
{
  if( consume("if") )
  {
    auto node = new Node(Node::If);
    node->tok = *csmtok;

    node->lhs = Expr();
    expect("then");
    expect("\n");

    auto closed = false;

    while( check() )
    {
      if( consume("endif") )
      {
        expect("\n");
        closed = true;
        break;
      }

      node->list.emplace_back(Stmt());
    }

    if( closed == false )
    {
      node->tok.Error("not closed with 'endif'");
    }

    return node;
  }


  if( curtok().type != Token::Ident )
    curtok().Error("syntax error");

  // トークンの先読みをして、代入式かどうか確認する
  if( tkIndex + 1 < g_tokens.size() && g_tokens[tkIndex + 1].str == "=" )
  {
    auto var = Primary();
    next();

    auto expr = Expr();
    expect("\n");

    return new Node(Node::Assign, var, expr);
  }

  //
  // 代入式でなければ、関数呼び出し
  
  auto node = new Node(Node::Callfunc);
  node->tok = curtok();

  next();

  if( !consume("\n") )
  {
    do
    {
      node->list.emplace_back(Expr());
    } while( consume(",") );

    expect("\n");
  }

  return node;
}

Node* Parse(std::vector<Token>&& tokens)
{
  g_tokens = std::move(tokens);

  for( int i = 0; i < g_tokens.size() - 1; )
  {
    if( g_tokens[i].str == "\n" && g_tokens[i + 1].str == "\n" )
    {
      g_tokens.erase(g_tokens.begin() + i);
    }
    else
      i++;
  }

  auto node = new Node(Node::Block);

  while( check() )
  {
    node->list.emplace_back(Stmt());
  }

  return node;
}