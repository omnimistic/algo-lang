#pragma once
#include "common.hpp"

//moves the pointer forward to ignore spaces and tabs
void skipWhitespace(const string& str, size_t& pos);

//reads a word, if digit, converts to float, if letters, pulls the value from variables map
//also handles negative sign and restarts if it sees ( ).
float parseFactor(const string& str, size_t& pos);
        
//handles multiplication, division, and modulus
//makes sure these happen before addition or subtraction.
float parseTerm(const string& str, size_t& pos);
        
//handles addition and subtraction.
//starting funciton of the parser
float parseExpression(const string& str, size_t& pos);
