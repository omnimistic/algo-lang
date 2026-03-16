#include "block.hpp"
#include "utils.hpp"

using namespace std;

//generic block finder for IF, ELIF, and ELSE
int findBlockEnd(int startIndex, const vector<string>& Buffer, string startCmd, string endCmd) {
    int count = 1;
    for (int i = startIndex + 1; i < Buffer.size(); i++) {
        string currentLine = trim(Buffer[i]);
        if (currentLine.empty() || (currentLine.length() >= 2 && currentLine.substr(0, 2) == "//")) continue;
        
        stringstream ss(currentLine);
        string command;
        ss >> command;
        
        if (command == startCmd) count++;
        else if (command == endCmd) {
            count--;
            if (count == 0) return i;
        }
    }
    return -1;
}

int findEndWhile(int startIndex, const vector<string>& Buffer) {
    int whileCount = 1;
    for (int i = startIndex + 1; i < Buffer.size(); i++) {
        string currentLine = trim(Buffer[i]);
        if (currentLine.empty() || (currentLine.length() >= 2 && currentLine.substr(0, 2) == "//")) continue;
        
        stringstream ss(currentLine);
        string command;
        ss >> command;
        
        if (command == "WHILE") whileCount++;
        else if (command == "ENDWHILE") {
            whileCount--;
            if (whileCount == 0) return i;
        }
    }
    return -1;
}

int findWhile(int startIndex, const vector<string>& Buffer) {
    int whileCount = 1;
    for (int i = startIndex - 1; i >= 0; i--) {
        string currentLine = trim(Buffer[i]);
        if (currentLine.empty() || (currentLine.length() >= 2 && currentLine.substr(0, 2) == "//")) continue;
        
        stringstream ss(currentLine);
        string command;
        ss >> command;
        
        if (command == "ENDWHILE") whileCount++;
        else if (command == "WHILE") {
            whileCount--;
            if (whileCount == 0) return i;
        }
    }
    return -1;
}
