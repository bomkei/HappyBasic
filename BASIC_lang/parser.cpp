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
    for( size_t i = 0; i < g_variables.size(); i++ )
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

Node::Node(Node::Type type, Node* lhs, Node* rhs, Token* tok)
  : Node(type)
{
  this->lhs = lhs;
  this->rhs = rhs;

  if( tok != nullptr )
    this->tok = *tok;
  else
    this->tok = *csmtok;
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
    x->tok = *csmtok;

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
      x = new Node(Node::IndexRef, x, Expr(), csmtok);
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
      x = new Node(Node::MemberAccess, x, IndexRef(), csmtok);
    else
      break;
  }

  return x;
}

Node* Unary()
{
  if( consume("-") )
    return new Node(Node::Sub, Node::FromInt(0), MemberAccess(), csmtok);

  if( consume("ref") )
    return new Node(Node::Reference, MemberAccess(), nullptr, csmtok);


  return MemberAccess();
}

Node* Mul()
{
  auto x = Unary();

  while( check() )
  {
    if( consume("*") )
      x = new Node(Node::Mul, x, Unary(), csmtok);
    else if( consume("/") )
      x = new Node(Node::Div, x, Unary(), csmtok);
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
      x = new Node(Node::Add, x, Mul(), csmtok);
    else if( consume("-") )
      x = new Node(Node::Sub, x, Mul(), csmtok);
    else
      break;
  }

  return x;
}

Node* Expr()
{
  return Add();
}

Node* Instruction(bool expect_new_line = true)
{
  if( curtok().type != Token::Ident )
    curtok().Error("syntax error");

  // �g�[�N���̐�ǂ݂����āA��������ǂ����m�F����
  if( tkIndex + 1 < g_tokens.size() && g_tokens[tkIndex + 1].str == "=" )
  {
    auto var = Primary();
    next();

    auto expr = Expr();

    if( expect_new_line )
      expect("\n");

    return new Node(Node::Assign, var, expr);
  }

  //
  // ������łȂ���΁A���ߕ�

  auto node = new Node(Node::Instruction);
  node->tok = curtok();

  next();

  if( !consume("\n") )
  {
    do
    {
      node->list.emplace_back(Expr());
    } while( consume(",") );

    if( expect_new_line )
      expect("\n");
  }
  else if( !expect_new_line )
    tkIndex--;

  return node;
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

    // ?
    if( consume("endif") )
    {
      expect("\n");
      return nullptr;
    }

    if( !consume("\n") )
    {
      auto xx = new Node(Node::If);
      xx->lhs = cond;
      xx->rhs = Instruction(false);

      if( consume("else") )
        xx->list.emplace_back(Instruction());
      else
      {
        expect("\n");
        xx->list.emplace_back(nullptr);
      }

      return xx;
    }

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
    
    auto counter = Primary();
    if( counter->type != Node::Variable )
      tk->Error("expect counter variable after this token");

    expect("=");
    auto begin = Expr();

    expect("to");
    auto end = Expr();
    expect("\n");

    auto closed = false;
    std::vector<Node*> block;

    while( check() )
    {
      if( consume("next") )
      {
        expect("\n");
        closed = true;
        break;
      }

      block.emplace_back(Stmt());
    }

    if( closed == false )
      tk->Error("not closed statement");

    auto block_n = new Node(Node::Block);
    block_n->list = std::move(block);

    auto node = new Node(Node::For);
    node->tok = *tk;
    node->lhs = begin;
    node->rhs = end;
    node->list.emplace_back(counter);
    node->list.emplace_back(block_n);

    return node;
  }

  //
  // While
  if( consume("while") )
  {
    auto cond = Expr();
    expect("\n");

    std::vector<Node*> block;
    bool closed = false;

    while( check() )
    {
      if( consume("wend") )
      {
        expect("\n");
        closed = true;
        break;
      }

      block.emplace_back(Stmt());
    }

    auto node = new Node(Node::While);
    node->lhs = cond;
    node->rhs = new Node(Node::Block);
    node->rhs->list = std::move(block);

    return node;
  }

  //
  // do - while
  if( consume("do") )
  {
    auto tk = csmtok;

    expect("\n");
    std::vector<Node*> block;

    Node* cond = nullptr;
    auto closed = false;

    while( check() )
    {
      if( consume("while") )
      {
        cond = Expr();
        expect("\n");
        closed = true;
        break;
      }

      block.emplace_back(Stmt());
    }

    if( closed == false )
      tk->Error("not closed");

    auto block_main = new Node(Node::Block);
    block_main->list = std::move(block);

    auto if_n = new Node(Node::If);
    if_n->lhs = new Node(Node::Equal, cond, Node::FromInt(0));
    if_n->rhs = new Node(Node::Break);
    if_n->list.emplace_back(nullptr);

    auto check_n = new Node(Node::Block);
    check_n->list.emplace_back(block_main);
    check_n->list.emplace_back(if_n);
    
    auto node = new Node(Node::While);
    node->lhs = Node::FromInt(1);
    node->rhs = check_n;

    return node;
  }


  return Instruction();
}

Node* Parse(std::vector<Token>&& tokens)
{
  g_tokens = std::move(tokens);

  auto node = new Node(Node::Block);

  while( check() )
  {
    node->list.emplace_back(Stmt());
  }

  g_tokens.clear();

  return node;
}