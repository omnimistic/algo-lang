#pragma once

//generic block finder for IF, ELIF, and ELSE
int findBlockEnd(int startIndex, const vector<string>& Buffer, string startCmd, string endCmd);

int findEndWhile(int startIndex, const vector<string>& Buffer);

int findWhile(int startIndex, const vector<string>& Buffer);
