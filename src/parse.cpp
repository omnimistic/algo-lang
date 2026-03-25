#include "parse.hpp"
using namespace std;

//moves the pointer forward to ignore spaces and tabs
void skipWhitespace(const string& str, size_t& pos) {
  while (pos < str.length() && isspace(str[pos])) {
    pos++;
  }
}

//reads a word, if digit, converts to float, if letters, pulls the value from variables map
//also handles negative sign and restarts if it sees ( ).
float parseFactor(const string& str, size_t& pos, map<string, varValue>* variables) {
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
    float val = parseExpression(str, pos, variables);
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
    if (!checkVarExists(token, variables)) return 0.0f;
    return sign * (*variables)[token].f_val;
  }
}
        
//handles multiplication, division, and modulus
//makes sure these happen before addition or subtraction.
float parseTerm(const string& str, size_t& pos, map<string, varValue>* variables) {
  float val = parseFactor(str, pos, variables);
  while (true) {
    skipWhitespace(str, pos);
    if (pos >= str.length()) break;
                
    char op = str[pos];
    if (op != '*' && op != '/' && op != '%') break;
                
    pos++;
    float nextVal = parseFactor(str, pos, variables);
                
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
float parseExpression(const string& str, size_t& pos, map<string, varValue>* variables) {
  float val = parseTerm(str, pos, variables);
  while (true) {
    skipWhitespace(str, pos);
    if (pos >= str.length()) break;
                
    char op = str[pos];
    if (op != '+' && op != '-') break;
                
    pos++;
    float nextVal = parseTerm(str, pos, variables);
                
    if (op == '+') val += nextVal;
    else if (op == '-') val -= nextVal;
  }
  return val;
}
//helper function to evaluate condition inside if statement
bool parseComparisionConditions(string& conditionalStatement, map<string, varValue>* variables){
    stringstream ss(conditionalStatement);
    string leftSide, op, rightSide;
    ss >> leftSide >> op >> rightSide;
    float leftVal = getValue(leftSide, variables);
    float rightVal = getValue(rightSide, variables);

    bool conditionMet = false;
    if (op == "==") conditionMet = (leftVal == rightVal);
    else if (op == "!=") conditionMet = (leftVal != rightVal);
    else if (op == "<") conditionMet = (leftVal < rightVal);
    else if (op == ">") conditionMet = (leftVal > rightVal);
    else if (op == "<=") conditionMet = (leftVal <= rightVal);
    else if (op == ">=") conditionMet = (leftVal >= rightVal);
    else {
        cout << "Error: Unknown operator '" << op << "' in IF statement!" << endl;
        return;
    }
    return conditionMet;
}
bool parseBooleanConditions(stringstream& nestedConditionalStatement, map<string, varValue>* variables) {
    
  vector<string> parts = separateThestringstream(nestedConditionalStatement);
  vector<string> temp;

  // PASS 1: AND / XOR
  for (int i = 0; i < parts.size(); i++) {
      string token = parts[i];

      if (token == "and" || token == "xor") {

          if (temp.empty()) return false;

          bool left = parseComparisionConditions(temp.back(), variables);
          temp.pop_back();

          bool right = parseComparisionConditions(parts[i + 1], variables);
          i++;

          bool result = (token == "and") ? (left && right) : (left ^ right);

          temp.push_back(result ? "true" : "false");
      } else {
          temp.push_back(token);
      }
  }

  // PASS 2: OR
  bool result = parseComparisionConditions(temp[0], variables);

  for (int i = 1; i < temp.size(); i += 2) {
      string op = temp[i];
      bool right = parseComparisionConditions(temp[i + 1], variables);

      if (op == "or") {
          result = result || right;
      }
  }

  return result;
}