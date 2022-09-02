#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <iconv.h>
#include <conio.h>

#define TOTAL_CNT 867024

char* encoding(const char* text_input, const char* source, const char* target)
{
    iconv_t it;

    int input_len = strlen(text_input) + 1;
    int output_len = input_len * 2;

    size_t in_size = input_len;
    size_t out_size = output_len;

    char* output = (char*)malloc(output_len);

    char* output_buf = output;
    const char* input_buf = text_input;

    it = iconv_open(target, source);
    int ret = iconv(it, &input_buf, &in_size, &output_buf, &out_size);


    iconv_close(it);

    return output;
}


using namespace std;

class Link {
public:
    int id;
    string display = "";
    Link(int id) {
        this->id = id;
    }
};

class Document {
public:
    vector<Link> links;
    int redirect = 0;
    void addLink(Link link) {
        links.push_back(link);
    }
    void addDisplayToLastLink(string display) {
        links[links.size() - 1].display = display;
    }
};

class Bfs {
public:
    int depth;
    int id;
    vector<Link> parents;
    Bfs(int depth, int id, vector<Link> parents) {
        this->depth = depth;
        this->id = id;
        this->parents = parents;
    }
    Bfs() {
        this->depth = -1;
        this->id = 0;
    }
};

unordered_map<string, int> IdByTitle;
vector<Document> documents(TOTAL_CNT + 1);

string getTitleById(int id) {
    ifstream titleList;
    titleList.open("./title_List.txt");
    int cnt = 1;
    string line;
    while (getline(titleList, line)) {
        if (cnt == id) return line;
        cnt++;
    }
    titleList.close();
    return "";
}

void mappingId() {
    ifstream titleList;
    titleList.open("./title_list.txt");
    int id = 1;
    string line;
    while (getline(titleList, line)) {
        IdByTitle[line] = id;
        id++;
    }
}

void fillDocumentList() {
    ifstream namulink;
    namulink.open("./namulink_id.txt");
    string line;
    int id;
    bool isLink = false;
    while (getline(namulink, line)) {
        if (line == "##--TITLE") {
        title:
            isLink = false;
            getline(namulink, line);
            id = stoi(line);
            getline(namulink, line);
            if (line == "##--LINK") {
                isLink = true;
            }
            else if (line == "##--REDIRECT") {
                getline(namulink, line);
                documents[id].redirect = stoi(line);
            }
            else if (line == "##--TITLE") {
                goto title;
            }
        }
        else if (isLink) {
            if (line == "##--DISPLAY") {
                getline(namulink, line);
                documents[id].addDisplayToLastLink(line);
            }
            else if (line == "##--REDIRECT") isLink = false;
            else documents[id].addLink(stoi(line));
        }
    }
    namulink.close();
}

Bfs runBfs(int start, int end) {
    if (documents[end].redirect != 0) end = documents[end].redirect;
    queue<Bfs> Q;
    vector<bool> visited(TOTAL_CNT + 1);
    if (documents[start].redirect == 0) {
        Q.push(Bfs(0, start, vector<Link>()));
        visited[start] = true;
    }
    else {
        Q.push(Bfs(0, documents[start].redirect, vector<Link>()));
        visited[documents[start].redirect] = true;
    }
    while (!Q.empty()) {
        Bfs current = Q.front();
        Q.pop();
        for (int i = 0; i < documents[current.id].links.size(); i++) {
            vector<Link> parents(current.parents);
            parents.push_back(documents[current.id].links[i]);
            int nextId = documents[current.id].links[i].id;
            if (documents[nextId].redirect == 0) {
                if (!visited[nextId]) {
                    Q.push(Bfs(current.depth + 1, nextId, parents));
                    visited[nextId] = true;
                    if (nextId == end) return Q.back();
                }
            }
            else {
                if (!visited[documents[nextId].redirect]) {
                    parents.push_back(Link(documents[nextId].redirect));
                    Q.push(Bfs(current.depth + 1, documents[nextId].redirect, parents));
                    visited[documents[nextId].redirect] = true;
                    if (documents[nextId].redirect == end) return Q.back();
                }
            }
        }
    }
    return Bfs();
}

int main() {
    cout << "로딩중..." << endl;
    mappingId();
    fillDocumentList();
    system("cls");

    while (1) {
        system("cls");
        string start, end;
        int startId, endId;
        while (1) {
            fflush(stdin);
            cout << "시작할 문서의 제목을 입력해주세요: ";
            getline(cin, start);
            char* start_utf8 = encoding(start.c_str(), "cp949", "UTF-8");
            startId = IdByTitle[string(start_utf8)];
            if (startId == 0) {
                system("cls");
                cout << "\"" << start << "\" 이름의 문서를 찾을 수 없습니다." << endl;
            }
            else {
                system("cls");
                break;
            }
        }
        while (1) {
            fflush(stdin);
            cout << "시작할 문서의 제목을 입력해주세요: " << start << endl;
            cout << "도착할 문서의 제목을 입력해주세요: ";
            getline(cin, end);
            char* end_utf8 = encoding(end.c_str(), "cp949", "UTF-8");
            endId = IdByTitle[string(end_utf8)];
            if (endId == 0) {
                system("cls");
                cout << "\"" << end << "\" 이름의 문서를 찾을 수 없습니다." << endl;
            }
            else if (start == end) {
                system("cls");
                cout << "시작 문서와 도착 문서의 이름이 같습니다." << endl;
            }
            else {
                system("cls");
                break;
            }
        }
        cout << "\"" << start << "\" ---> \"" << end << "\"" << endl << endl << "탐색중..." << endl << endl;
        Bfs result = runBfs(startId, endId);
        if (result.depth == -1) {
            cout << "경로가 존재하지 않습니다." << endl;
        }
        else {
            cout << "경로 길이: " << result.depth << endl;
            cout << "--------------------" << endl << start << endl;
            for (int i = 0; i < result.parents.size(); i++) {
                bool isLast = (i == result.parents.size() - 1);
                int id = result.parents[i].id;
                string display = result.parents[i].display;
                string display_cp949 = encoding(display.c_str(), "UTF-8", "cp949");
                string title = encoding(getTitleById(id).c_str(), "UTF-8", "cp949");
                if (documents[result.parents[i].id].redirect != 0) {
                    i++;
                    int id_ = result.parents[i].id;
                    string title_ = encoding(getTitleById(id_).c_str(), "UTF-8", "cp949");
                    if (display == "") {
                        cout << title << " --> " << title_ << endl;
                    }
                    else {
                        cout << display_cp949 << " --> " << title_ << endl;
                    }
                }
                else if (display == "") {
                    cout << title << endl;
                }
                else {
                    cout << display_cp949 << " --> " << title << endl;
                }
            }
            cout << "--------------------" << endl;
        }
        cout << endl << "엔터키를 눌러서 계속하기";
        fflush(stdin);
        char ch;
        do {
            ch = _getch();
        } while (ch != 13);
    }
    return 0;
}