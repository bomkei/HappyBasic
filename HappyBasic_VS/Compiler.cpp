#include <iostream>
#include <fstream>
#include <ctime>

#include "Utils.h"

#include "Object.h"
#include "Token.h"
#include "AST.h"

#include "Tokenizer.h"
#include "Parser.h"
#include "AST_Runner.h"
#include "Compiler.h"

#include "Global.h"

using namespace Global;

// Object を Codes に配置するときは、ポインタとして配置する

namespace Compiler {
#define type2u8(t)   static_cast<u8>(t)

  namespace {
    std::vector<u8> Codes;

    auto add(CodeType t) {
      Codes.emplace_back(type2u8(t));
    }

    auto add(Object const& obj) {
      constexpr auto byte = sizeof(Object*);
      
      for( int i = 0; i < byte; i++ ) {
        
      }
    }
  }
  
  std::vector<u8> const& GetCodes() {
    return const_cast<std::vector<u8>&>(Codes);
  }

  void CompileExpr(AST::Expr* expr) {
    if( !expr )
      return;

    switch( expr->type ) {
      case AST::Expr::Immidiate:
        add(CodeType::MoveImmidiate);

    }
  }

  void CompileStmt(AST::Stmt* stmt) {

  }
}