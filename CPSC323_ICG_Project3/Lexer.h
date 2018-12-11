/*
Semantic_Project3

Joshua Ungheanu
Derek Dorr
Adam Weesner

CPSC 323 Fall 2018
Shohrat Geldiyev

*********************************************
1) Symbol table handling
2) Generating an  assembly code for the simplified version of Rat18S
*********************************************/


#ifndef LEXER_H
#define LEXER_H

#include<iostream>
#include<fstream>
#include<vector>
#include<cctype>
#include<string>


using namespace std;


struct TokenType {
	string lexeme;
	string token;
};


TokenType lexer(ifstream &file) {
	string keyWord[] = { "while", "if", "elsif", "else", "return", "print", "end", "program", "begin", "end", "function", "read", "write" };
	vector<string> keyword(keyWord, keyWord + sizeof(keyWord) / sizeof(string));

	string text = "";
	char value;

	TokenType current;

	if (file >> value) {
		text += value;
		if (isalpha(value)) { //checks to see is its an identifier, type or keyword
			value = file.peek();

			while (isalpha(value) || isdigit(value) || value == '_') { //reads through the string
				value = file.get();
				text += value;
				value = file.peek();
			}
			current.lexeme = text; //registers the string as lexeme

			if (current.lexeme.compare("int") == 0 || current.lexeme.compare("real") == 0 || current.lexeme.compare("string") == 0) { // check for type
				current.token = "Type";
			}

			else
				for (unsigned int i = 0; i < keyword.size(); i++) {
					if (current.lexeme.compare(keyword[i]) == 0) {
						current.token = "Keyword";
						break;
					}

					else {
						current.token = "Identifier";
					}
				}
		}

		else if (isdigit(value)) {
			value = file.peek();
			while (isdigit(value)) {
				value = file.get();
				text += value;
				value = file.peek();
			}

			current.lexeme = text;
			current.token = "IntConst";

			if (value == '.') {

				value = file.get();
				value = file.peek();

				if (isdigit(value)) {
					text += '.';
					value = file.get();
					text += value;
					value = file.peek();

					while (isdigit(value)) {
						value = file.get();
						text += value;
						value = file.peek();
					}
					current.lexeme = text;
					current.token = "RealConst";

				}

				else {
					file.putback(value);
					current.lexeme = text;
					current.token = "IntConst";
				}
			}
		}

		else if (value == '"') {
			value = file.peek();
			while (value != '"' && !file.eof()) {
				value = file.get();
				text += value;
				value = file.peek();
			}

			if (!file.eof()) {
				value = file.get();
				text += value;
				current.lexeme = text;
				current.token = "StrConst";
			}
			else {
				current.lexeme = text;
				current.token = "error";
			}
		}

		else if (value == '=' || value == '>' || value == '<') {
			if (value == '=') {
				current.lexeme = value;
				current.token = "RelOp";
			}
			else if (value == '>') {
				value = file.peek();
				if (value == '=') {
					value = file.get();
					text += value;
					current.lexeme = text;
					current.token = "RelOp";
				}
				else {
					current.lexeme = text;
					current.token = "RelOp";
				}
			}
			else if (value == '<') {

				value = file.peek();
				if (value == '=') {
					value = file.get();
					text += value;
					current.lexeme = text;
					current.token = "RelOp";
				}
				else if (value == '>') {
					value = file.get();
					text += value;
					current.lexeme = text;
					current.token = "RelOp";
				}
				else {
					current.lexeme = text;
					current.token = "RelOp";
				}

			}
		}
		// Normal Operator List
		else if (value == '.' || value == ',' || value == ';' || value == ':' || value == '(' || value == ')' || value == '+' || value == '-' || value == '*' || value == '/') {
			value = file.peek();
			if (text.compare(":") == 0 && value == '=') {
				value = file.get();
				text += value;
			}

			current.lexeme = text;
			current.token = "Operator";
		}


		else {
			current.lexeme = value;
			current.token = "error";
		}

		return current;
	}

	else {
		current.lexeme = "";
		current.token = "eof";

		return current;
	}


}


#endif
