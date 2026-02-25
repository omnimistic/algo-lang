#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>


using namespace std;

int main(int argc, char* argv[]) {

    if (argc < 2){
        cerr << "Usage: algo <filename>" << endl;
        return 1;
    }

    string filename = argv[1];
    ifstream file(filename);

    if (!file.is_open()){
        cerr << "Error: Could not open file '" << filename << "'" << endl;
        return 1;
    }

    //buffer
    vector<string> Buffer;
    string line;
    while (getline(file, line)){
        Buffer.push_back(line);
    }

    file.close();

    
    for(int i = 0; i < Buffer.size(); ++i){
        
        string currentLine = Buffer[i];
        
        //skip blank
        if(currentLine.empty()) continue; 

        //seperate words
        stringstream ss(currentLine); 
        
        //get the fist word
        string command;
        ss >> command;

        cout << "Line " << (i+1) << " Command: " << command << endl;
    }

    return 0;
}
