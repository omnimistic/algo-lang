#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <cctype>
#include <cmath>

using namespace std;

//language ka RAM that we use for storing variables

map<string, varValue> variables;

bool isRunning = false;
bool previousIfBranchExecuted = false; //tracks if an IF/ELIF was true to know if we should run ELSE

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
        string currentLine = trim(Buffer[i]);
        if(currentLine.empty()) continue;
        
        //tokenizer
        stringstream ss(currentLine);
        string command;
        ss >> command;

        //check for start and end points
        if (command == "START") {
            isRunning = true; 
            continue;
        }

        //skip to next line if not executing yet
        if (!isRunning) continue;

        //pass line to the execution engine
        executeLine(currentLine, i, Buffer);

        //break out completely if an END command was hit
        if (!isRunning) break;
    }

    return 0;
}
