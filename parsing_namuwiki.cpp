#include <iostream>
#include <cstdio>
#include <Windows.h>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

class WordFinder {
private:
	string word;
	string tmpWord;

public:
	WordFinder(string word) {
		this->word = word;
	}

	bool find(char c) {
		if (tmpWord.length() < word.length()) tmpWord += c;
		else tmpWord = tmpWord.substr(1) + c;
		if (tmpWord == word) return true;
		return false;
	}
};

vector<string> split(string s, string divid) {
	vector<string> v;
	int start = 0;
	int d = s.find(divid);
	while (d != -1 && s[d - 1] != '\\') {
		v.push_back(s.substr(start, d - start));
		start = d + 1;
		d = s.find(divid, start);
	}
	v.push_back(s.substr(start, d - start));

	return v;
}

int main() {
	SetConsoleOutputCP(CP_UTF8);
	
	ofstream namulink;
	ofstream title_list;
	namulink.open("namulink.txt");
	title_list.open("title_list.txt");

	FILE* namuwiki;
	namuwiki = fopen("Namuwiki.json", "r");
	
	string title;
	string link;
	string redirect;
	bool isTitle = false;
	bool isLink = false;
	bool wasLink = false;
	WordFinder titleFinder(",\"title\":\"");
	WordFinder textFinder("\",\"text\":\"");
	WordFinder openBracketFinder("[[");
	WordFinder closeBracketFinder("]]");
	WordFinder redirectFinder("\":\"#redirect");
	WordFinder contributorsFinder("\",\"contributors\":[\"");
	bool isFirstLink = true;
	bool isRedirect = false;
	int id = 1;

	int a = 1;
	
	char ch;

	while ((ch = fgetc(namuwiki)) != EOF) {
		if (titleFinder.find(ch)) {
		    isTitle = true;
		} 
		if (textFinder.find(ch)) {
			wasLink = false;
			isTitle = false;
			isFirstLink = true;
			title = title.substr(1, title.length() - 10);
			namulink << "##--TITLE" << endl;
			namulink << title << endl;
			title_list << title << endl;
			title = "";
		}
		if(contributorsFinder.find(ch) && isRedirect) {
			redirect = redirect.substr(2, redirect.length() - 22);
			namulink << "##--REDIRECT" << endl ;
			namulink << redirect << endl;
			redirect = "";
			isRedirect = false;
		}
		if (redirectFinder.find(ch)) {
			isRedirect = true;
		}
		if (openBracketFinder.find(ch)) {
			link = "";
			isLink = true;
		}
		if (closeBracketFinder.find(ch) && isLink) {
			isLink = false;
			link = link.substr(1, link.length() - 2);
				if (link.find("https://") == string::npos && 
				link.find("http://") == string::npos &&
				link.find("파일:") == string::npos &&
				link.find("분류") == string::npos &&
				link.find("나무위키:") == string::npos &&
				link[0] != '|') {
					wasLink = true;
					vector<string> linkArr = split(link, "|");
					if(linkArr[0] != "") {
						if (isFirstLink) namulink << "##--LINK" << endl;
						if (linkArr.size() == 1) {
							namulink << linkArr[0] << endl;
						}
						else {
							namulink << linkArr[0] << endl;
							namulink << "##--DISPLAY" << endl;
							namulink << linkArr[1] << endl;
						}
						isFirstLink = false;
					}
				}
			link = "";
		}
		if (isTitle) title += ch;
		if (isLink) link += ch;
		if (isRedirect) redirect += ch;
	}

	fclose(namuwiki);
	namulink.close();
	title_list.close();

	return 0;
}
