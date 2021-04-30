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


Node* Expr();
Node* Primary()
{
  if( consume("(") )
  {
    auto x = Expr();
    expect(")");
    return x;
  }

  if( consume("[") )
  {
    auto x = new Node(Node::Array);

    if( !consume("]") )
    {
      do
      {
        x->list.emplace_back(Expr());
      } while( consume(",") );
      expect("]");
    }

    return x;
  }

  switch( curtok().type )
  {
    case Token::Number:
    case Token::String:
    case Token::Char:
    {
      auto x = new Node;
      x->tok = curtok();

      next();
      return x;
    }

    case Token::Ident:
    {
      auto x = new Node(Node::Variable);
      x->tok = curtok();

      next();

      if( consume("(") )
      {
        x->type = Node::Callfunc;

        if( !consume(")") )
        {
          do
          {
            x->list.emplace_back(Expr());
          } while( consume(",") );
          expect(")");
        }

        return x;
      }

      auto find = find_var(x->tok.str);

      if( find == -1 )
      {
        find = g_variables.size();

        Object obj;
        obj.name = x->tok.str;
        g_variables.emplace_back(obj);
      }

      x->varIndex = find;

      return x;
    }
  }

  curtok().Error("syntax error");
}

Node* IndexRef()
{
  auto x = Primary();

  while( check() )
  {
    if( consume("[") )
    {
      x = new Node(Node::IndexRef, x, Expr());
      expect("]");
    }
    else
      break;
  }

  return x;
}

Node* MemberAccess()
{
  auto x = IndexRef();

  while( check() )
  {
    if( consume(".") )
      x = new Node(Node::MemberAccess, x, IndexRef());
    else
      break;
  }

  return x;
}

Node* Unary()
{
  if( consume("-") )
    return new Node(Node::Sub, Node::FromInt(0), MemberAccess());

  return MemberAccess();
}

Node* Mul()
{
  auto x = Unary();

  while( check() )
  {
    if( consume("*") )
      x = new Node(Node::Mul, x, Unary());
    else if( consume("/") )
      x = new Node(Node::Div, x, Unary());
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
  //
  // if
  if( consume("if") )
  {
    auto tk = csmtok;

    auto cond = Expr();
    expect("then");
    expect("\n");

    std::vector<Node*> true_block;

    bool have_else = false;
    bool closed = false;

    Node* elseN = nullptr;

    while( check() )
    {
      if( consume("endif") )
      {
        closed = true;
        expect("\n");
        break;
      }
      else if( consume("else") )
      {
        if( curtok().str == "if" )
        {
          elseN = Stmt();
          closed = true;
        }
        else
        {
          elseN = new Node(Node::Block);
          expect("\n");

          while( check() )
          {
            if( consume("endif") )
            {
              closed = true;
              expect("\n");
              break;
            }

            elseN->list.emplace_back(Stmt());
          }
        }

        break;
      }

      true_block.emplace_back(Stmt());
    }

    if( closed == false )
    {
      tk->Error("not closed");
    }

    auto trueN = new Node(Node::Block);
    trueN->list = std::move(true_block);

    auto node = new Node(Node::If);
    node->lhs = cond;
    node->rhs = trueN;
    node->list.emplace_back(elseN);

    return node;
  }

  //
  // for
  if( consume("for") )
  {
    auto tk = csmtok;
    auto var = Expr();

    expect("in");
    auto list = Expr();
    expect("\n");

    auto node = new Node(Node::For);
    bool closed = false;

    node->tok = *tk;
    node->lhs = var;
    node->rhs = list;
    node->list.emplace_back(new Node(Node::Block));

    while( check() )
    {
      if( consume("next") )
      {
        closed = true;
        expect("\n");
        break;
      }

      node->list[0]->list.emplace_back(Stmt());
    }

    if( closed == false )
      tk->Error("not closed");

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
  
  auto node = new Node(Node::Instruction);
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

  for( size_t i = 0; i < g_tokens.size() - 1; )
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

  g_tokens.clear();

  return node;
}