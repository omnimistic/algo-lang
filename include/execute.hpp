#pragma once
#include "common.hpp"

float doMaths(const string& expression);

//engine that processes a single line, moved here so IF can call it recursively
void executeLine(string line, int& i, const vector<string>& Buffer);
