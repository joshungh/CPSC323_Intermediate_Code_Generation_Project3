/*
Parser_Project3

Joshua Ungheanu
Derek Dorr
Adam Weesner

CPSC 323 Fall 2018
Shohrat Geldiyev

*********************************************
1) Symbol table handling
2) Generating an  assembly code for the simplified version of Rat18S
*********************************************/

#include <iostream>
#include <fstream>
#include "parser.h"

using namespace std;

int main(int argc, char* argv[]) {

	
	if (argc < 2) {

		cout << "Missing file name..." << endl;
		return 1;
	}

	ifstream fin(argv[1]);

	if (!fin) {
		cout << "file not found..." << endl;
		return 1;

	}

	Parser(fin); //parser function which takes in the file
				 //closes file after program completes
	fin.close();
	system("pause");
	return 0;
}

//int main() {
//	ifstream fin("six.txt");
//
//	if (!fin) {
//		cout << "file not found...";
//		return 1;
//	}
//	parser(fin);
//	fin.close();
//}