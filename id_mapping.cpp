#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string>
#include <unordered_map>

using namespace std;

unordered_map<string, int> IdByTitle;

void mappingId() {
    ifstream titleList;
    titleList.open("./title_list.txt");
    int id = 1;
    string line;
    while(getline(titleList, line)) {
        IdByTitle[line] = id;
        id++;
    }
}

int getIdByTitle(string title) {
    return IdByTitle[title];
}

void mappingNamulink() {
    ifstream namulink;
    ofstream namulinkId;
    namulink.open("./namulink.txt");
    namulinkId.open("./namulink_id.txt");
    string line;
    bool isDisplay = false;
    bool isWrongLink = false;
    bool isRedirect = false;
    while(getline(namulink, line)) {
        line = (line.find("#s-") == string::npos) ? line : line.substr(0, line.find("#s-"));
        if(isDisplay) {
            namulinkId << line << endl;
            isDisplay = false;
        }
        else if(line == "##--DISPLAY") {
            if(isWrongLink) {
                getline(namulink, line);
                isWrongLink = false;
            }
            else{
                namulinkId << line << endl;
                isDisplay = true;
            }
            continue;
        }

        isWrongLink = false;

        if(line == "##--REDIRECT") {
            namulinkId << line << endl;
            isRedirect = true;
        }
        else if(line.substr(0, 4) == "##--") {
            namulinkId << line << endl;
            isRedirect = false;
        }
        else if(IdByTitle[line] != 0) namulinkId << IdByTitle[line] << endl;
        else {
            if(isRedirect) namulinkId << 0 << endl;
            else isWrongLink = true;
        }
    }
    namulink.close();
    namulinkId.close();
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    mappingId();
    mappingNamulink();

    return 0;
}