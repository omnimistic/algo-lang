#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <cctype>
#include <cmath>

using namespace std;

enum DataType { TYPE_INT, TYPE_FLOAT, TYPE_STRING };

struct varValue {
    DataType type = TYPE_INT;
    int i_val = 0;
    float f_val = 0.0f;
    string s_val = "";
};

//language ka RAM that we use for storing variables
map<string, varValue> variables;

bool isRunning = false;

//removes leading and trailing spaces from a string
//scans for the first and last actual characters, then extracts only that portion.
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

bool checkVarExists(const string& varName) {
    if (variables.count(varName) == 0) {
        cout << "Error: Undeclared variable '" << varName << "'!" << endl;
        return false;
    }
    return true;
}

//helper function to evaluate a raw number or pull a variable's value for the IF statement
float getValue(const string& token) {
    if (isdigit(token[0]) || (token[0] == '-' && token.length() > 1 && isdigit(token[1]))) {
        return stof(token);
    } else {
        if (checkVarExists(token)) return variables[token].f_val;
        return 0.0f; 
    }
}


float doMaths(const string& expression) {

    //local struct so that i can write functions inside a fucntion because cant do that by default in C++
    struct Parser {

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
    };

    //instantiate the local parser and do the calculation
    Parser p;
    size_t pos = 0;
    return p.parseExpression(expression, pos);
}

//engine that processes a single line, moved here so IF can call it recursively
void executeLine(string line, int& i, const vector<string>& Buffer) {
    stringstream ss(line);
    string command;
    ss >> command;
    //if the line starts with //, completely ignore it and stop processing
    if (command.length() >= 2 && command.substr(0, 2) == "//") {
        return; 
    }

    if (command == "END") {
        isRunning = false; //strop executing
        return;
    }
    else if (command == "DECLARE") {
        string restOfLine;
        getline(ss, restOfLine);
        stringstream declStream(restOfLine);
        string varName;
        while (getline(declStream, varName, ',')) {
            varName = trim(varName);
            if (!varName.empty()) {
                if (variables.count(varName) > 0) {
                    cout << "Error: Variable '" << varName << "' already declared!" << endl;
                } else {
                    variables[varName] = varValue();
                }
            }
        }
    }
    else if (command == "INPUT") {
        string varName;
        ss >> varName;
        
        if (!checkVarExists(varName)) return;

        cout << "Enter value for " << varName << ": ";
        string rawInput;
        getline(cin >> ws, rawInput);
        
        varValue newVal;

        //checks if starts with quote for string
        if (rawInput[0] == '"') {
            newVal.type = TYPE_STRING;
            size_t lastQuote = rawInput.find_last_of('"');
            if (lastQuote != string::npos && lastQuote > 0) {
                newVal.s_val = rawInput.substr(1, lastQuote - 1);
            } else {
                newVal.s_val = rawInput.substr(1);
            }
        }
        //checks if it has a dot for float
        else if (rawInput.find('.') != string::npos) {
            newVal.type = TYPE_FLOAT;
            try { newVal.f_val = stof(rawInput); } catch(...) { newVal.f_val = 0.0f; }
            newVal.i_val = (int)newVal.f_val; 
        }
        //otherwise assumes its an integer
        else {
            newVal.type = TYPE_INT;
            try { newVal.i_val = stoi(rawInput); } catch(...) { newVal.i_val = 0; }
            newVal.f_val = (float)newVal.i_val; 
        }
        
        //save it to map
        variables[varName] = newVal;
    }
    else if (command == "OUTPUT") {
        string restOfLine;
        getline(ss, restOfLine);
        restOfLine = trim(restOfLine); 

        if (restOfLine.empty()) return;

        //if string literal
        if (restOfLine[0] == '"') {
            size_t lastQuote = restOfLine.find_last_of('"');
            if (lastQuote != string::npos && lastQuote > 0) {
                cout << ">> " << restOfLine.substr(1, lastQuote - 1) << endl;
            } else {
                cout << "Error: Missing closing quote in OUTPUT!" << endl;
            }
        }
        //if raw number
        else if (isdigit(restOfLine[0]) || (restOfLine[0] == '-' && restOfLine.length() > 1 && isdigit(restOfLine[1]))) {
            cout << ">> " << restOfLine << endl;
        }
        //else must be a variable
        else {
            if (!checkVarExists(restOfLine)) return;
            
            //find in the map and print it
            varValue v = variables[restOfLine];
            if (v.type == TYPE_INT) cout << ">> " << v.i_val << endl;
            else if (v.type == TYPE_FLOAT) cout << ">> " << v.f_val << endl;
            else if (v.type == TYPE_STRING) cout << ">> " << v.s_val << endl;
        }
    }
    else if (command == "GOTO") {
        int targetLine;
        ss >> targetLine;

        //make sure the line exists in Buffer
        if (targetLine > 0 && targetLine <= Buffer.size()) {
            //fun trick here, target line - 1 to adjust for 0 index and
            //then another -1 so that the ++i of loop runs the actual line required
            i = targetLine - 2;
        }
        else {
            cout << "Error: Line " << targetLine << " does not exist!" << endl;
        }
    }
    else if (command == "IF") {
        string leftSide, op, rightSide;
        ss >> leftSide >> op >> rightSide;

        float leftVal = getValue(leftSide);
        float rightVal = getValue(rightSide);

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

        //if the condition is met, extract the rest of the line and run it
        if (conditionMet) {
            string restOfLine;
            getline(ss, restOfLine);
            restOfLine = trim(restOfLine);
            
            if (!restOfLine.empty()) {
                executeLine(restOfLine, i, Buffer);
            }
        }
    }
    else {
        //ARITHMETIC LOGIC
        string targetVar = command;
        string equalsSign;
        ss >> equalsSign;

        if (equalsSign == "=") {
            if (!checkVarExists(targetVar)) return;

            string expression;
            //take the mathematical expression and pass it to doMaths
            getline(ss, expression);
            
            float result = doMaths(expression);
            
            variables[targetVar].f_val = result;
            variables[targetVar].i_val = (int)result;

            if (result == (int)result) {
                variables[targetVar].type = TYPE_INT;
            } else {
                variables[targetVar].type = TYPE_FLOAT;
            }
        }
    }
}


int main(int argc, char* argv[]) {

    if (argc < 2){
        cerr << "Usage: algo <filename>" << endl;
        return 1;
    }
    
    //opens the file
    string filename = argv[1];
    ifstream file(filename);

    if (!file.is_open()){
        cerr << "Error: Could not open file '" << filename << "'" << endl;
        return 1;
    }

    //copies the file text into "Buffer"
    vector<string> Buffer;
    string line;

    while (getline(file, line)){
        Buffer.push_back(line);
    }
   
    //we stop using the file as we already have all the code in the buffer
    file.close();
   
    //code execution loop
    for(int i = 0; i < Buffer.size(); ++i){
        
        //access current line from buffer and skip if blank
        string currentLine = Buffer[i];
        if(currentLine.empty()) continue;
        
        //tokenizer
        stringstream ss(currentLine);
        string command;
        ss >> command;

        //check for start and end points
        if (command == "START") {
            isRunning = true; //start executing
            continue;
        }

        //skip to next line if not executing yet
        if (!isRunning) {
            continue;
        }

        //pass line to the execution engine
        executeLine(currentLine, i, Buffer);

        //break out completely if an END command was hit
        if (!isRunning) {
            break;
        }
    }

    return 0;
}
