#include "execute.hpp"

#include <algorithm>
#include <cstdlib>

using namespace std;
//page level functions
namespace {
//thing to verify the comparative token
bool isComparisonOperator(const string& token) {
    return token == "==" || token == "!=" || token == "<" || token == ">" || token == "<=" || token == ">=";
}
//thing to verify the boolean token
bool isBooleanOperator(const string& token) {
    string normalized = token;
    transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
    return normalized == "and" || normalized == "or" || normalized == "xor";
}

string joinTokens(const vector<string>& tokens, size_t start, size_t endExclusive) {
    string joined;
    for (size_t i = start; i < endExclusive; ++i) {
        if (!joined.empty()) joined += " ";
        joined += tokens[i];
    }
    return joined;
}
//thing to check if the statements are valid or not
bool splitConditionAndInlineCommand(const string& rawConditionAndRest, string* conditionText, string* inlineCommand) {
    vector<string> tokens;
    string token;
    stringstream tokenStream(rawConditionAndRest);

    while (tokenStream >> token) {
        tokens.push_back(token);
    }

    if (tokens.size() < 3 || !isComparisonOperator(tokens[1])) {
        return false;
    }

    size_t idx = 3;
    while (idx < tokens.size() && isBooleanOperator(tokens[idx])) {
        if (idx + 3 >= tokens.size() || !isComparisonOperator(tokens[idx + 2])) {
            return false;
        }
        idx += 4;
    }

    *conditionText = joinTokens(tokens, 0, idx);
    *inlineCommand = joinTokens(tokens, idx, tokens.size());
    return true;
}
}

float doMaths(const string& expression, map<string, varValue>* variables) {
    //instantiate the local parser and do the calculation
    size_t pos = 0;
    return parseExpression(expression, pos, variables);
}

//engine that processes a single line, moved here so IF can call it recursively
void executeLine(string line, int& i, const vector<string>& Buffer, map<string, varValue>* variables, bool* previousIfBranchExecuted, bool* isRunning) {
    stringstream ss(line);
    string command;
    ss >> command;
    
    //if the line starts with //, completely ignore it and stop processing
    if (command.length() >= 2 && command.substr(0, 2) == "//") {
        return; 
    }

    if (command == "END") {
        *isRunning = false; //stop executing
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
                if (variables->count(varName) > 0) {
                    cout << "Error: Variable '" << varName << "' already declared!" << endl;
                } else {
		    (*variables)[varName] = varValue();
                }
            }
        }
    }
    else if (command == "INPUT") {
        string varName;
        ss >> varName;
        if (!checkVarExists(varName, variables)) return;

        cout << "Enter value for " << varName << ": ";
        string rawInput;
        getline(cin >> ws, rawInput);
        varValue newVal;

        //checks if starts with quote for string
        if (rawInput[0] == '"') {
            newVal.type = TYPE_STRING;
            size_t lastQuote = rawInput.find_last_of('"');
            if (lastQuote != string::npos && lastQuote > 0) newVal.s_val = rawInput.substr(1, lastQuote - 1);
            else newVal.s_val = rawInput.substr(1);
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
        (*variables)[varName] = newVal;
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
            } else cout << "Error: Missing closing quote in OUTPUT!" << endl;
        }
        //if raw number
        else if (isdigit(restOfLine[0]) || (restOfLine[0] == '-' && restOfLine.length() > 1 && isdigit(restOfLine[1]))) {
            cout << ">> " << restOfLine << endl;
        }
        //else must be a variable
        else {
	  if (!checkVarExists(restOfLine, variables)) return;
            //find in the map and print it
	    varValue v = (*variables)[restOfLine];
            if (v.type == TYPE_INT) cout << ">> " << v.i_val << endl;
            else if (v.type == TYPE_FLOAT) cout << ">> " << v.f_val << endl;
            else if (v.type == TYPE_STRING) cout << ">> " << v.s_val << endl;
        }
    }
    else if (command == "GOTO") {
        int targetLine;
        ss >> targetLine;
        //make sure the line exists in Buffer
        if (targetLine > 0 && static_cast<size_t>(targetLine) <= Buffer.size()) {
            //fun trick here, target line - 1 to adjust for 0 index and
            //then another -1 so that the ++i of loop runs the actual line required
            i = targetLine - 2;
        } else cout << "Error: Line " << targetLine << " does not exist!" << endl;
    }
    else if (command == "IF") {
        string conditionAndInline;
        getline(ss, conditionAndInline);
        conditionAndInline = trim(conditionAndInline);

        string conditionText;
        string inlineCommand;
        if (!splitConditionAndInlineCommand(conditionAndInline, &conditionText, &inlineCommand)) {
            cout << "Error: Malformed IF condition!" << endl;
            exit(1);
        }

        stringstream conditionStream(conditionText);
        bool conditionMet = parseBooleanConditions(conditionStream, variables);
        if (conditionMet) {
            *previousIfBranchExecuted = true;
            if (!inlineCommand.empty()) executeLine(inlineCommand, i, Buffer, variables, previousIfBranchExecuted, isRunning);
        } else {
            *previousIfBranchExecuted = false;
            int endIdx = findBlockEnd(i, Buffer, "IF", "ENDIF");
            if (endIdx != -1) i = endIdx; 
        }
    }
    else if (command == "ELIF") {
        if (*previousIfBranchExecuted) {
            int endIdx = findBlockEnd(i, Buffer, "ELIF", "ENDELIF");
            if (endIdx != -1) i = endIdx;
            return;
        }

        string conditionAndInline;
        getline(ss, conditionAndInline);
        conditionAndInline = trim(conditionAndInline);

        string conditionText;
        string inlineCommand;
        if (!splitConditionAndInlineCommand(conditionAndInline, &conditionText, &inlineCommand)) {
            cout << "Error: Malformed ELIF condition!" << endl;
            exit(1);
        }

        stringstream conditionStream(conditionText);
        bool conditionMet = parseBooleanConditions(conditionStream, variables);

        if (conditionMet) {
            *previousIfBranchExecuted = true;
            if (!inlineCommand.empty()) executeLine(inlineCommand, i, Buffer, variables, previousIfBranchExecuted, isRunning);
        } else {
            int endIdx = findBlockEnd(i, Buffer, "ELIF", "ENDELIF");
            if (endIdx != -1) i = endIdx; 
        }
    }
    else if (command == "ELSE") {
        if (*previousIfBranchExecuted) {
            int endIdx = findBlockEnd(i, Buffer, "ELSE", "ENDELSE");
            if (endIdx != -1) i = endIdx;
            return;
        }
        
        *previousIfBranchExecuted = true;
        string restOfLine;
        getline(ss, restOfLine);
        restOfLine = trim(restOfLine);
        
        if (!restOfLine.empty()) executeLine(restOfLine, i, Buffer, variables, previousIfBranchExecuted, isRunning);
    }
    else if (command == "ENDIF" || command == "ENDELIF" || command == "ENDELSE") {
        return; 
    }
    else if (command == "WHILE") {
        string conditionAndInline;
        getline(ss, conditionAndInline);
        conditionAndInline = trim(conditionAndInline);

        string conditionText;
        string inlineCommand;
        if (!splitConditionAndInlineCommand(conditionAndInline, &conditionText, &inlineCommand) || !inlineCommand.empty()) {
            cout << "Error: Malformed WHILE condition!" << endl;
            exit(1);
        }

        stringstream conditionStream(conditionText);
        bool conditionMet = parseBooleanConditions(conditionStream, variables);
        
        if (!conditionMet) {
            int endIdx = findEndWhile(i, Buffer);
            if (endIdx != -1) {
                i = endIdx; 
            } else {
                cout << "Error: Missing ENDWHILE for WHILE!" << endl;
                *isRunning = false;
            }
        }
    }
    else if (command == "ENDWHILE") {
        int startIdx = findWhile(i, Buffer);
        if (startIdx != -1) {
            i = startIdx - 1; 
        } else {
            cout << "Error: Missing WHILE for ENDWHILE!" << endl;
            *isRunning = false;
        }
    }
    else {
        //ARITHMETIC LOGIC
        string targetVar = command;
        string equalsSign;
        ss >> equalsSign;

        if (equalsSign == "=") {
	  if (!checkVarExists(targetVar, variables)) return;

            string expression;
            //take the mathematical expression and pass it to doMaths
            getline(ss, expression);
            
            float result = doMaths(expression, variables);
            
            (*variables)[targetVar].f_val = result;
            (*variables)[targetVar].i_val = (int)result;

            if (result == (int)result) (*variables)[targetVar].type = TYPE_INT;
            else (*variables)[targetVar].type = TYPE_FLOAT;
        }
    }
}
