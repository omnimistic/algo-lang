#pragma once

//moves the pointer forward to ignore spaces and tabs
void skipWhitespace(const string& str, size_t& pos) {
  while (pos < str.length() && isspace(str[pos])) {
    pos++;
  }
}

//reads a word, if digit, converts to float, if letters, pulls the value from variables map
//also handles negative sign and restarts if it sees ( ).
float parseFactor(const string& str, size_t& pos) {
  skipWhitespace(str, pos);
  if (pos >= str.length()) return 0.0f;

  float sign = 1.0f;
  if (str[pos] == '-') {
    sign = -1.0f;
    pos++;
    skipWhitespace(str, pos);
  }

  if (str[pos] == '(') {
    pos++;
    float val = parseExpression(str, pos);
    skipWhitespace(str, pos);
    if (pos < str.length() && str[pos] == ')') pos++;
    return sign * val;
  }

  string token = "";
  while (pos < str.length() && (isalnum(str[pos]) || str[pos] == '_' || str[pos] == '.')) {
    token += str[pos++];
  }

  if (token.empty()) return 0.0f;

  if (isdigit(token[0]) || token[0] == '.') {
    return sign * stof(token);
  } else {
    if (!checkVarExists(token)) return 0.0f;
    return sign * variables[token].f_val;
  }
}
        
//handles multiplication, division, and modulus
//makes sure these happen before addition or subtraction.
float parseTerm(const string& str, size_t& pos) {
  float val = parseFactor(str, pos);
  while (true) {
    skipWhitespace(str, pos);
    if (pos >= str.length()) break;
                
    char op = str[pos];
    if (op != '*' && op != '/' && op != '%') break;
                
    pos++;
    float nextVal = parseFactor(str, pos);
                
    if (op == '*') val *= nextVal;
    else if (op == '/') {
      if (nextVal != 0.0f) val /= nextVal;
      else cout << "Error: Division by zero!" << endl;
    }
    else if (op == '%') {
      // % symbol doesn't work on floats in C++ so fmod is used
      if (nextVal != 0.0f) val = fmod(val, nextVal);
      else cout << "Error: Modulo by zero!" << endl;
    }
  }
  return val;
}
        
//handles addition and subtraction.
//starting funciton of the parser
float parseExpression(const string& str, size_t& pos) {
  float val = parseTerm(str, pos);
  while (true) {
    skipWhitespace(str, pos);
    if (pos >= str.length()) break;
                
    char op = str[pos];
    if (op != '+' && op != '-') break;
                
    pos++;
    float nextVal = parseTerm(str, pos);
                
    if (op == '+') val += nextVal;
    else if (op == '-') val -= nextVal;
  }
  return val;
}

