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
            //pass
        }
    }

    return 0;
}
