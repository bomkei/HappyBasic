#pragma once

#define  _DEBUG_BUILD_    1

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <utility>
#include <ctime>
#include <algorithm>

#include "include/common.h"
#include "include/utils.h"

#include "include/Object.h"
#include "include/Token.h"
#include "include/AST.h"

#include "include/Tokenizer.h"
#include "include/Parser.h"
#include "include/AST_Runner.h"
#include "include/Program.h"

#include "include/Optimizer.h"
#include "include/Options.h"

Object MakeObjectFromString(std::string const& str);

void CheckExpr(AST::Expr* expr);

