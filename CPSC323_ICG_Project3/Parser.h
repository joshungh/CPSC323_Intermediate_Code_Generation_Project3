#pragma once
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

#ifndef PARSER_H 
#define PARSER_H

#include "Lexer.h"
#include <string>
#include <vector>


vector <string> assemblyCommands;
string tRegister[10] = {};
vector<string> list;
vector<int> location;

int whileCount = 0;
int ifCount = 0;
int elsifCount = 0;

void Program(ifstream&, TokenType&);
void DeclList(ifstream&, TokenType&);
vector<string> Decl(ifstream&, TokenType&);
string Type(ifstream&, TokenType&);
vector<string> VarList(ifstream&, TokenType&);
void StmtList(ifstream&, TokenType&);
void Stmt(ifstream&, TokenType&);
void Assign(ifstream&, TokenType&);
void Read(ifstream&, TokenType&);
void Write(ifstream&, TokenType&);
void If(ifstream&, TokenType&);
void While(ifstream&, TokenType&);
string Cond(ifstream&, TokenType&);
string RelOp(ifstream&, TokenType&);
string Expr(ifstream&, TokenType&);
string Term(ifstream&, TokenType&);
string Factor(ifstream&, TokenType&);


// Parser function that starts the top down recursion for grammar rules that takes in the file
void Parser(ifstream &file) {

	//calls the first lexeme/token combo in the file
	TokenType curToken = lexer(file);

	// calls the Program function, checking if "program" is present
	Program(file, curToken);
}

// Display the error message
void writeError(string s, TokenType& check, ifstream &file) {
	cout << "Error: unexpected string: " << check.lexeme << ", expected " << s << endl;
}

// lexeme check
bool accept(string s, TokenType& check, ifstream &file) {
	// checks the string with the lexeme
	if (s.compare(check.lexeme) == 0) {
		check = lexer(file);
		return true;
	}
	else {
		return false;
	}
}
void expect(string s, TokenType& check, ifstream &file) {

	if (!accept(s, check, file)) {
		writeError(s, check, file);
	}
}


void Program(ifstream& file, TokenType& token) {

	// Consume token if present
	expect("program", token, file);

	// Check for DeclList

	if (token.token.compare("Type") == 0) {
		// Call DeclList
		DeclList(file, token);
	}

	// Consume token if present
	expect("begin", token, file);

	// begins the main assembly commands 
	assemblyCommands.push_back(".text");
	assemblyCommands.push_back("main:");

	// Check for StmtList
	if ((!(token.lexeme.compare("end"))) == 0) {

		// Call StmtList
		StmtList(file, token);
	}

	// Consume token if present
	expect("end", token, file);

	// Consume token if present
	expect(".", token, file);

	// the assembly commands to end the program are added at the end
	assemblyCommands.push_back("li $v0, 10");
	assemblyCommands.push_back("syscall");

	// output commands at end of programs
	bool txt = false;

	for (vector<string>::iterator it = assemblyCommands.begin(); it != assemblyCommands.end(); it++) {
		if (it->compare(".text") == 0) {
			txt = true;
		}
		if (txt == false) {
			cout << *it << endl;
		}
		else {

			if (it->back() != ':' && it->compare(".text") != 0) {
				cout << "\t" << *it << endl;
			}
			else {
				cout << *it << endl;
			}
		}
	}
}

void DeclList(ifstream& file, TokenType& token) {

	// Vector to hold the list of declarations
	vector<string> declL;

	// Starts off the declarations of variables
	assemblyCommands.push_back(".data");

	// Check for initial declaration and loops back if there are more
	do {

		// Call Decl and takes the list of declarations given to add to the main vector of commands
		declL = Decl(file, token);

		// loop to go through the vector 
		for (vector<string>::iterator it = declL.begin(); it != declL.end(); it++) {
			// adds them to the main assembly Command vector
			assemblyCommands.push_back(*it);
		}
	} while (token.token.compare("Type") == 0);
}
// Changing data type to return the data statements to save variable list
vector<string> Decl(ifstream& file, TokenType& token) {

	// declaration list vector
	vector<string> decl;

	// vector of variabls
	vector<string> vars;

	// string that takes in the type
	string t = "";

	// boolean for checking if declaration errors are present
	bool error = false;
	string varName = "";

	// Call Type function, assigns the type to t
	t = Type(file, token);

	// Call ValList function, passes the vector to vars
	vars = VarList(file, token);

	// Consume token if present
	expect(";", token, file);

	// Loop to set all the declarations
	for (vector<string>::iterator it = vars.begin(); it != vars.end(); it++) {
		string d = "";

		// gets the var
		d = d + *it;

		// check if the var being declared has already been declared
		if (list.empty() == true) { // if the list that checks if the vars is empty
			list.push_back(d); // add the var to the list
		}
		else { // otherwise, iterate through the vector to check if there is a duplicate present
			for (vector<string>::iterator il = list.begin(); il != list.end(); il++) {
				if (il->compare(d) == 0) { //if a duplicate is found
					error = true; //set error to true
					break; //break out of the iterator
				}
			}
		}
		// if error is found, output error message
		if (error == true) {

			cout << "error:" << d << " is declared more than once" << endl;

		}
		else { // otherwise, add the var to the list for checking

			list.push_back(d);
			varName = d;
		}
		// sets the rest of the declaration of the variable
		d = d + ":    ";
		d = d + t;
		d = d + "0";

		// saves it in the decl vector to pass back
		decl.push_back(d);

		// assign var to register (project 4 part 1)
		for (int i = 0; i < 10; i++) { // iterates through the register list

			if (tRegister[i].compare("") == 0) { // first register that is empty
				tRegister[i] = varName; // assigns the var to the register
				break; // breaks out of the loop
			}
		}
	}
	// returns decl vector
	return decl;

}

string Type(ifstream& file, TokenType& token) {

	// check for type
	string t = "";

	// if the type is int, then return
	if (token.lexeme.compare("int") == 0)

	{

		// word is only type available so it is automatically set
		t = ".word	";

		// consume the token first
		token = lexer(file);
	}
	// returns the asssembly for type
	return t;

}

vector<string> VarList(ifstream& file, TokenType& token) {

	// vector to hold the identifier list
	vector<string> ident;

	if (token.token.compare("Identifier") == 0)
	{
		// adds the identifier to the temp vector
		ident.push_back(token.lexeme);

		// moves to the next token
		token = lexer(file);
	}
	// if comma is found, then
	while (token.lexeme.compare(",") == 0) {

		// consume the token
		token = lexer(file);

		// check if next is another identifier
		if (token.token.compare("Identifier") == 0)

		{
			// push the next identifier to the vector
			ident.push_back(token.lexeme);
			// get next token
			token = lexer(file);
		}

	}// loop again if there is a comma following the identifier

	// return the vector
	return ident;

}

void StmtList(ifstream& file, TokenType& token) {

	// call Stmt function
	Stmt(file, token);

	// Continues to call stmt function if lexemes are Ident, read, write, if, while, do, return
	while (token.token.compare("Identifier") == 0 || token.lexeme.compare("read") == 0 || token.lexeme.compare("write") == 0 || token.lexeme.compare("if") == 0 || token.lexeme.compare("while") == 0) {
		Stmt(file, token);
	}
}

void Stmt(ifstream& file, TokenType& token) {
	// If token is identifier
	if (token.token.compare("Identifier") == 0)

	{	// Calls Assign function and output rule

		// Checks if identifier was declared
		// Bool for checking set to false
		bool err = false;

		// Iterator to check through list
		for (vector<string>::iterator it = list.begin(); it != list.end(); it++) {

			if (token.lexeme.compare(*it) != 0) { // If the identifier was not found in the list
				err = true; // Return that it is true
			}

			else { // Otherwise, if it is in the list, set to false
				err = false;
				break;// breaks out of loop
			}

		}// Outputs error if found
		if (err == true) {

			cout << "error: using " << token.lexeme << " without declaring first" << endl;
		}
		Assign(file, token);
	}

	// If token is read
	else if (token.lexeme.compare("read") == 0)

	{	// Calls Read function and output rule
		Read(file, token);
	}

	// If token is write

	else if (token.lexeme.compare("write") == 0)

	{
		// Calls Write function and output rule
		Write(file, token);

	}
	// If token is if

	else if (token.lexeme.compare("if") == 0)

	{	// calls If function and output rule
		// saves the count of if that is called

		ifCount++;
		If(file, token);

	}

	// If token is while
	else if (token.lexeme.compare("while") == 0)

	{	// Calls While function and output rule
		While(file, token);
	}
}

void Assign(ifstream& file, TokenType& token) {

	// Assign vector that holds the 
	// Vector<string> assign;

	// Variables to set the identifier and register to push to the assembly commands
	string ident = "";
	string reg = "";

	// If token is Identifier, consume token
	if (token.token.compare("Identifier") == 0)

	{
		// Gets the identifier
		// Ident = token.lexeme;

		// Check the identifier and assign the register instead
		for (int i = 0; i < 10; i++) {

			// If the identifier is found in the register

			if (token.lexeme.compare(tRegister[i]) == 0) {

				// Assign the register
				ident = "$t" + to_string(i);
				// Break out of the loop
				break;

			}
		}
		token = lexer(file);
	}
	expect(":=", token, file);

	// Call Expr function, and gets the register
	reg = Expr(file, token);

	// AssemblyCommands.push_back("sw " + reg + ", " + ident);
	assemblyCommands.push_back("move " + ident + ", " + reg + "");

	// Clear register after storing value back into the variable
	int rNum = reg[2] - '0';

	tRegister[rNum] = "";
	expect(";", token, file);

	for (int i = 0; i < 10; i++) {

		// Boolean to check if register used in conditional is part of the declared list
		bool declared = false;

		// Iterates throuhg the list of declared variables to see if it keeps 
		for (vector<string>::iterator it = list.begin(); it != list.end(); it++) {

			if ((*it).compare(tRegister[i]) == 0) {
				declared = true; //if found bool is true and breaks
				break;
			}

		} // If not in the declared list, clears the register instead
		if (declared == false) {
			tRegister[i] = "";
		}
	}
}

void Read(ifstream& file, TokenType& token) {

	// Vetor to hold the identifiers to read
	vector<string> idents;

	expect("read", token, file);
	expect("(", token, file);

	// Call VarList func, saves the varList
	idents = VarList(file, token);
	// Temp var to check which variable causes the error
	string errorVar = "";

	//bool var to check if it is in the declared list
	bool err = false;

	for (vector<string>::iterator ito = idents.begin(); ito != idents.end(); ito++) {

		for (vector<string>::iterator it = list.begin(); it != list.end(); it++) { // Double iterator to check through both the var list and the declared list

			if (ito->compare(*it) != 0) { // if not found, the var not found is saved
				err = true;
				errorVar = *ito;
			}
			else {
				err = false;
				break;
			}
		}
	}

	// If the error is found, the output
	if (err == true) {
		cout << "error: using " << errorVar << " without declaring first" << endl;
	}

	// For every entry in the varList, pushes these commands
	for (vector<string>::iterator it = idents.begin(); it != idents.end(); it++) {
		assemblyCommands.push_back("li $v0, 5");
		assemblyCommands.push_back("syscall");

		// Finds the register number that needs to be assigned to move the value
		bool reg = false;
		string regist = "$t";

		for (int i = 0; i < 10; i++) {
			if (tRegister[i].compare(*it) == 0) {
				reg = true;
				regist += to_string(i);
				break;
			}
		}
		if (reg == true) {
			assemblyCommands.push_back("move " + regist + ",$v0");
		}
		else {
			cout << "error" << endl;
		}
	}
	expect(")", token, file);
	expect(";", token, file);
}

void Write(ifstream& file, TokenType& token) {
	// The vector that holds the list of expressions
	vector<string> exprList;
	// Counter that counts how many expressions there are evertyome one is push to exprList
	int i = 0;


	// After consuming write lexeme
	expect("write", token, file);
	expect("(", token, file);

	// Keeps on finding an expression as long as the token matches with the comma
	exprList.push_back(Expr(file, token));

	// Adds the commands for each expression
	assemblyCommands.push_back("li $v0, 1");
	assemblyCommands.push_back("move $a0, " + exprList[i]);
	assemblyCommands.push_back("syscall");

	// Clear register used
	int regNum = exprList[i][2] - '0';
	tRegister[regNum] = "";

	while (token.lexeme.compare(",") == 0) {
		// Consumes comma token

		i++; // If anouther expression is detected, add one to the counter
		token = lexer(file);
		exprList.push_back(Expr(file, token));
		assemblyCommands.push_back("li $v0, 1");
		assemblyCommands.push_back("move $a0, " + exprList[i]);
		assemblyCommands.push_back("syscall");

		// Clear register used
		int regNum = exprList[i][2] - '0';
		tRegister[regNum] = "";

	}
	expect(")", token, file);
	expect(";", token, file);
}

void If(ifstream& file, TokenType& token) {

	// String to hold the conditional
	string cond = "";

	// Keeps the current count of how many if's have been encountered locally
	int countOfIf = ifCount;

	// Consumes if
	expect("if", token, file);
	expect("(", token, file);


	// Calls Cond function, saves the branch
	cond = Cond(file, token);
	expect(")", token, file);

	// Pushes the incomplete conditional back to the assemblyCommands
	assemblyCommands.push_back(cond);

	// saves the location of said conditional in the locations vector
	location.push_back(assemblyCommands.size() - 1);

	//ifOrder.push_back("");
	expect("begin", token, file);

	// Calls StmtList function
	StmtList(file, token);
	expect("end", token, file);

	// If there is an elsif/else present after the if, add a branch to the end of said If 
	if (token.lexeme.compare("elsif") == 0 || token.lexeme.compare("else") == 0) {
		assemblyCommands.push_back("b endIf" + to_string(countOfIf));
	}

	// elseif statements if there are any
	if (token.lexeme.compare("elsif") == 0) {

		do {
			elsifCount++;

			// Consume initial elsif
			expect("elsif", token, file);

			// Starts the elsif
			assemblyCommands.push_back("elseif" + to_string(elsifCount) + ":");

			// Add the label to the associated branch (like stack)
			assemblyCommands[location.back()] += "elseif" + to_string(elsifCount);
			location.pop_back();
			expect("(", token, file);

			// Call Cond function
			cond = Cond(file, token);
			expect(")", token, file);

			// Saves the next location the next label will be saves
			assemblyCommands.push_back(cond);
			location.push_back(assemblyCommands.size() - 1);
			expect("begin", token, file);

			// Call StmtList function
			StmtList(file, token);
			expect("end", token, file);

			// If elsif or else are present, add branch to end of If
			if (token.lexeme.compare("elsif") == 0 || token.lexeme.compare("else") == 0) {
				assemblyCommands.push_back("b endIf" + to_string(countOfIf));
			}

		} while (token.lexeme.compare("elsif") == 0); // Continues the loop if elseif token is present

	}

	if (token.lexeme.compare("else") == 0) {

		// Starts else commands
		assemblyCommands.push_back("else" + to_string(countOfIf) + ":");

		// Consume else token
		expect("else", token, file);

		// Add the label to the associated branch
		assemblyCommands[location.back()] += "else" + to_string(countOfIf);
		location.pop_back();
		expect("begin", token, file);

		// Call StmtList
		StmtList(file, token);
		expect("end", token, file);

		//assemblyCommands.push_back("b endIf" + to_string(countOfIf));
	}

	else { //if there is no else after the elsif(s), set the last branch with no label to this location
		assemblyCommands[location.back()] += "endIf" + to_string(countOfIf);
		location.pop_back();
	}
	// Branch to end the complete ifStatement
	assemblyCommands.push_back("endif" + to_string(countOfIf) + ":");
}
void While(ifstream& file, TokenType& token) {

	// Consume while token/keyword
	whileCount++;

	// Variables that hold how many whiles/iteration of while is active
	string whil = "while" + to_string(whileCount);
	string endWhil = "endWhl" + to_string(whileCount);

	// Saves the bracnh to jump if conditional is not met
	string cond = "";

	if (token.lexeme.compare("while") == 0) {

		// Starts the while loop
		assemblyCommands.push_back(whil + ":");
		token = lexer(file);
	}
	expect("(", token, file);

	// Calls Cond function, saves statement
	cond = Cond(file, token);

	// Adds the endWhile jump to the condition statement
	cond = cond + endWhil;

	// Add condition statement to the commands list
	assemblyCommands.push_back(cond);
	expect(")", token, file);
	expect("begin", token, file);

	// Checks for stmtList if there
	if (token.token.compare("Identifier") == 0 || token.lexeme.compare("read") == 0 || token.lexeme.compare("write") == 0 || token.lexeme.compare("if") == 0 || token.lexeme.compare("while") == 0) {
		// Call StmtList function
		StmtList(file, token);
	}

	expect("end", token, file);

	// Pushes the commands that ends the while loop
	assemblyCommands.push_back("b " + whil);
	assemblyCommands.push_back(endWhil + ":");
}

string Cond(ifstream& file, TokenType& token) {

	// String to return
	string cond = "";

	// Registers being compared
	string r1 = "";
	string r2 = "";

	// variable for what tyoe of branch basesd on conditional
	string relo = "";

	// Call Expr function. saves first register
	r1 = Expr(file, token);

	// Call RelOp function, saves the branch
	relo = RelOp(file, token);

	// Call Expr function, saves second register
	r2 = Expr(file, token);

	// Complete initial statement of branch
	cond = relo + " " + r1 + ", " + r2 + ", ";

	// Boolean to check if register used in conditional is part of the declared list
	bool declared = false;

	// Iterates throuhg the list of declared variables to see if it keeps 
	for (vector<string>::iterator it = list.begin(); it != list.end(); it++) {

		if ((*it).compare(tRegister[r1[2] - '0']) == 0) {
			declared = true; //if found bool is true and breaks
			break;
		}

	} // If not in the declared list, clears the register instead
	if (declared == false) {
		int mo = r1[2] - '0';
		tRegister[mo] = "";
	}
	declared = false;

	for (vector<string>::iterator it = list.begin(); it != list.end(); it++) {

		if ((*it).compare(tRegister[r2[2] - '0']) == 0) {
			declared = true; // If found bool is true and breaks
			break;
		}

	} // If not in the declared list, clears the register instead
	if (declared == false) {

		int mo = r2[2] - '0';
		tRegister[mo] = "";
	}

	return cond;
}

string RelOp(ifstream& file, TokenType& token) {

	// String to hold which branch to use
	string relator = "";

	// Check if it is a RelOp
	if (token.token.compare("RelOp") == 0)
	{
		if (token.lexeme.compare("=") == 0) {
			// Return opposite branch for the comparison
			relator = "bne";
		}
		else if (token.lexeme.compare("<>") == 0) {
			relator = "beq";
		}
		else if (token.lexeme.compare("<") == 0) {
			relator = "bge";
		}
		else if (token.lexeme.compare(">") == 0) {
			relator = "ble";
		}
		else if (token.lexeme.compare("<=") == 0) {
			relator = "bgt";
		}
		else {
			relator = "blt";
		}

		//Register lexeme
		token = lexer(file);
	}
	// Return branch
	return relator;
}

string Expr(ifstream& file, TokenType& token) {

	// Registers 1 and 2
	string r1 = "";
	string r2 = "";

	// The next empty register to store the value into
	string r3 = "";

	// Call Term function, set rregister 1
	r1 = Term(file, token);

	// If lexeme is + or -
	while (token.lexeme.compare("+") == 0 || token.lexeme.compare("-") == 0) {

		if (token.lexeme.compare("+") == 0) {

			// Register token
			token = lexer(file);

			// Call the factor if it is factor
			r2 = Term(file, token);

			// Assigns the 3rd register as the next empty register to move the calculated value to
			for (int i = 0; i < 10; i++) {
				if (tRegister[i].compare("") == 0) {
					// Assign it as the register to use for assignment
					r3 = "$t" + to_string(i);
					tRegister[i] = r1 + " + " + r2;
					break;
				}
			}
			// Gets add command
			assemblyCommands.push_back("add " + r3 + ", " + r1 + ", " + r2);
			// Clears the register that is unused after calc
			/*int mo = r3[2] - '0';
			tRegister[mo] = "";*/
		}
		else if (token.lexeme.compare("-") == 0) {

			//Register token
			token = lexer(file);

			// Call the factor if it is factor
			r2 = Term(file, token);

			// Assigns the 3rd register as the next empty register to move the calculated value to
			for (int i = 0; i < 10; i++) {
				if (tRegister[i].compare("") == 0) {
					// Assign it as the register to use for assignment
					r3 = "$t" + to_string(i);
					tRegister[i] = r1 + " + " + r2;
					break;
				}
			}
			// Gets sub command
			assemblyCommands.push_back("sub " + r3 + ", " + r1 + ", " + r2);
			// Clears the register that is unused after calc

			/*int mo = r3[2] - '0';
			tRegister[mo] = "";*/
		}
		// Returns the value of r3 after calculation to store into the variable
		return r3;
	}
	// Return the register with the value
	return r1;
}

string Term(ifstream& file, TokenType& token) {

	string r1 = "";
	string r2 = "";
	string r3 = "";

	// Call Factor function
	r1 = Factor(file, token);

	// Check if lexeme is * or /

	while (token.lexeme.compare("*") == 0 || token.lexeme.compare("/") == 0) {

		// Multiply
		if (token.lexeme.compare("*") == 0) {

			//Register token
			token = lexer(file);

			// Call the factor if it is factor
			r2 = Factor(file, token);

			// Assigns the 3rd register as the next empty register to move the calculated value to
			for (int i = 0; i < 10; i++) {
				if (tRegister[i].compare("") == 0) {
					// Assign it as the register to use for assignment
					r3 = "$t" + to_string(i);
					tRegister[i] = r1 + " + " + r2;
					break;
				}
			}

			// Set commands
			assemblyCommands.push_back("mult " + r1 + ", " + r2);
			assemblyCommands.push_back("mflo " + r3);
			// Clear unused register after use

			/*int mo = r2[2] - '0';
			tRegister[mo] = "";*/

			// Returns the register the value was stored
			return r3;

		}

		// Divide
		else if (token.lexeme.compare("/") == 0) {

			// Registered token
			token = lexer(file);

			// Call the factor if it is factor
			r2 = Factor(file, token);

			// Assigns the 3rd register as the next empty register to move the calculated value to
			for (int i = 0; i < 10; i++) {
				if (tRegister[i].compare("") == 0) {

					// Assign it as the register to use for assignment
					r3 = "$t" + to_string(i);
					tRegister[i] = r1 + " + " + r2;
					break;
				}
			}
			assemblyCommands.push_back("div " + r1 + ", " + r2);

			// Check if either is a variable register use to move value to the temp one
			// Boolean to check if register used in conditional is part of the declared list
			bool check = false;

			// Iterates throuhg the list of declared variables to see if it keeps 
			for (vector<string>::iterator it = list.begin(); it != list.end(); it++) {

				if ((*it).compare(tRegister[r1[2] - '0']) == 0 || (*it).compare(tRegister[r2[2] - '0']) == 0) {

					check = true;

				}

			} // If not in the declared list, returns mflo as r1 or r2(need to implement) /////TEST TEST

			if (check == false) {

				assemblyCommands.push_back("mflo " + r1);
				return r1;
			}
			else {
				assemblyCommands.push_back("mflo " + r3);
			}
			// Clear unused register after use

			/*int mo = r2[2] - '0';
			tRegister[mo] = "";*/

			// Returns the register the value was stored
			return r3;
		}
	}
	// Return register holding the value
	return r1;
}
string Factor(ifstream& file, TokenType& token) {

	string reg = "$t";
	string in = "";

	// Check if identifier, , realConst, strConst
	if (token.token.compare("Identifier") == 0)
	{
		in = token.lexeme;

		// Check if variable is declared
		bool err = false;
		// Iterates to check if the variable was declared before being used
		for (vector<string>::iterator it = list.begin(); it != list.end(); it++) {
			if (token.lexeme.compare(*it) != 0) { // If it is not found, then return that error is true
				err = true;
			}
			else { // Otherwise, return false for error and break out
				err = false;
				break;
			}
		}
		if (err == true) { //if not found in declared list, then the error is output
			cout << "error: using " << token.lexeme << " without declaring first" << endl;
		}
		// Register token
		token = lexer(file);
		for (int i = 0; i < 10; i++) {
			// If the temp register is empty/not used yet
			if (tRegister[i].compare("") == 0) {
				// Assign it as the register to use for assignment
				reg = reg + to_string(i);
				tRegister[i] = in;
				break;
			}
			else if (tRegister[i].compare(in) == 0) {
				reg = reg + to_string(i);
				//tRegister[i] = in;
				break;
			}
		}
		// Push back the load
		// AssemblyCommands.push_back("lw " + reg + ", " + in);
	}
	// Check if intConst
	else if (token.token.compare("IntConst") == 0)
	{
		in = token.lexeme;
		// Register token
		token = lexer(file);

		for (int i = 0; i < 10; i++) {
			// If the temp register is empty/not used yet
			if (tRegister[i].compare("") == 0) {
				// Assign it as the register to use for assignment
				reg = reg + to_string(i);
				tRegister[i] = in;
				break;
			}
		}

		//load immediate
		assemblyCommands.push_back("li " + reg + ", " + in);
	}

	else if (token.lexeme.compare("(") == 0)

	{
		expect("(", token, file);
		// Call Expr function
		reg = Expr(file, token);
		expect(")", token, file);

	}

	// Returns the register the value was stored in
	return reg;
}

#endif