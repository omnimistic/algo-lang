#include "utils.hpp"

using namespace std;

//removes leading and trailing spaces from a string
//scans for the first and last actual characters, then extracts only that portion.
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

bool checkVarExists(const string& varName, map<string, varValue>* variables) {
    if (variables->count(varName) == 0) {
        cout << "Error: Undeclared variable '" << varName << "'!" << endl;
        return false;
    }
    return true;
}

//helper function to evaluate a raw number or pull a variable's value for the IF statement
float getValue(const string& token, map<string, varValue>* variables) {
    if (isdigit(token[0]) || (token[0] == '-' && token.length() > 1 && isdigit(token[1]))) {
        return stof(token);
    } else {
        if (checkVarExists(token)) return variables[token].f_val;
        return 0.0f; 
    }
}
