#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>

using namespace std;

// Language ka RAM that we use for storing variables
map<string, int> variables;

bool isRunning = false;


int doMaths(const string& expression) {

    //local struct so that i can write functions inside a fucntion because cant do that by default in C++
    struct Parser {

        //moves the pointer forward to ignore spaces and tabs
        void skipWhitespace(const string& str, size_t& pos) {
            while (pos < str.length() && isspace(str[pos])) {
                pos++;
            }
        }

        //reads a word, if digit, converts to integer, if letters, pulls the value from variables map
        //also handles negative sign and restarts if it sees ( ).
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
                return sign * variables[token]; // Looks up the global variables map
            }
        }
        
        //handles multiplication, division, and modulus
        //makes sure these happen before addition or subtraction.
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
        
        //handles addition and subtraction.
        //starting funciton of the parser
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

    //copies the file text into buffer
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
        if (command == "INPUT") {
            string varName;
            ss >> varName;
            
            cout << "Enter value for " << varName << ": ";
            int value;
            cin >> value;
            
            //save it to map
            variables[varName] = value;
        }
        else if (command == "OUTPUT") {
            string varName;
            ss >> varName;
            
            //find in the map and print it
            cout << ">> " << variables[varName] << endl;
        }
        else {
            //ARITHMETIC LOGIC
            string targetVar = command;
            string equalsSign;
            ss >> equalsSign;

            if (equalsSign == "=") {
                string expression;
                // Grab the entire rest of the mathematical equation
                getline(ss, expression); 
                
                // Pass it to the Do_maths function and save the answer!
                variables[targetVar] = doMaths(expression);
            }
        }
    }

    return 0;
}
