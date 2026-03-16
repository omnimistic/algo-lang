#pragma once
#include "common.hpp"
#include "utils.hpp"
#include "parse.hpp"
#include "block.hpp"
#include "types.hpp"

using namespace std;

float doMaths(const string& expression, map<string, varValue>* variables);

//engine that processes a single line, moved here so IF can call it recursively
void executeLine(string line, int& i, const vector<string>& Buffer, map<string, varValue>* variables, bool* previousIfBranchExecuted, bool* isRunning);
