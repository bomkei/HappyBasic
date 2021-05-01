#pragma once

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "common.h"
#include "color.h"
#include "object.h"
#include "token.h"
#include "AST.h"


//
// �\�[�X�R�[�h
extern std::string g_source;

//
// �ϐ�
struct Object;
extern std::vector<Object> g_variables;


std::vector<Token> Tokenize(std::string&& source);

bool is_hex(std::string);

void SetConsoleColor(int col);
