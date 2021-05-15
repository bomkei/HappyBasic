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

#include "common.h"
#include "utils.h"

#include "Object.h"
#include "Token.h"
#include "AST.h"

#include "Tokenizer.h"
#include "Parser.h"
#include "AST_Runner.h"
#include "ProgramContext.h"

#include "Options.h"

std::string readfile(std::string path);

