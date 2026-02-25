#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <cctype>

using namespace std;

map<string, int> variables;

bool isRunning = false;

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

int doMaths(const string& expression) {

    struct Parser {

        void skipWhitespace(const string& str, size_t& pos) {
            while (pos < str.length() && isspace(str[pos])) {
                pos++;
            }
        }

        int parseFactor(const string& str, size_t& pos) {
            skipWhitespace(str, pos);
            if (pos >= str.length()) return 0;

            int sign = 1;
            if (str[pos] == '-') {
                sign = -1;
                pos++;
                skipWhitespace(str, pos);
            }

            if (str[pos] == '(') {
                pos++;
                int val = parseExpression(str, pos);
                skipWhitespace(str, pos);
                if (pos < str.length() && str[pos] == ')') pos++;
                return sign * val;
            }

            string token = "";
            while (pos < str.length() && (isalnum(str[pos]) || str[pos] == '_')) {
                token += str[pos++];
            }

            if (token.empty()) return 0;

            if (isdigit(token[0])) {
                return sign * stoi(token);
            } else {
                if (!checkVarExists(token)) return 0;
                return sign * variables[token]; // Looks up the global variables map
            }
        }
        
        int parseTerm(const string& str, size_t& pos) {
            int val = parseFactor(str, pos);
            while (true) {
                skipWhitespace(str, pos);
                if (pos >= str.length()) break;
                
                char op = str[pos];
                if (op != '*' && op != '/' && op != '%') break;
                
                pos++;
                int nextVal = parseFactor(str, pos);
                
                if (op == '*') val *= nextVal;
                else if (op == '/') {
                    if (nextVal != 0) val /= nextVal;
                    else cout << "Error: Division by zero!" << endl;
                }
                else if (op == '%') {
                     if (nextVal != 0) val %= nextVal;
                     else cout << "Error: Modulo by zero!" << endl;
                }
            }
            return val;
        }
        
        int parseExpression(const string& str, size_t& pos) {
            int val = parseTerm(str, pos);
            while (true) {
                skipWhitespace(str, pos);
                if (pos >= str.length()) break;
                
                char op = str[pos];
                if (op != '+' && op != '-') break;
                
                pos++;
                int nextVal = parseTerm(str, pos);
                
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
        else if (command == "END") {
            break;//strop executing
        }

        //skip to next line if not executing yet
        if (!isRunning) {
            continue;
        }

        //COMMANDS logic
        if (command == "DECLARE") {
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
                        variables[varName] = 0;
                    }
                }
            }
        }
        else if (command == "INPUT") {
            string varName;
            ss >> varName;
            
            if (!checkVarExists(varName)) continue;

            cout << "Enter value for " << varName << ": ";
            int value;
            cin >> value;
            
            //save it to map
            variables[varName] = value;
        }
        else if (command == "OUTPUT") {
            string varName;
            ss >> varName;
            
            if (!checkVarExists(varName)) continue;

            //find in the map and print it
            cout << ">> " << variables[varName] << endl;
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
        else {
            //ARITHMETIC LOGIC
            string targetVar = command;
            string equalsSign;
            ss >> equalsSign;

            if (equalsSign == "=") {
                if (!checkVarExists(targetVar)) continue;

                string expression;
                //take the mathematical expression and pass it to doMaths
                getline(ss, expression);
                variables[targetVar] = doMaths(expression);
            }
        }
    }

    return 0;
}
