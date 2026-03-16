#pragma once
#include "common.hpp"

//removes leading and trailing spaces from a string
//scans for the first and last actual characters, then extracts only that portion.
string trim(const string& str);

// check if variable exists
bool checkVarExists(const string& varName, map<string, varValue>* variables);

//helper function to evaluate a raw number or pull a variable's value for the IF statement
float getValue(const string& token, map<string, varValue>* variables);
