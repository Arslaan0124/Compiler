#pragma once
#include<iostream>
#include<string>
#include <algorithm>
#include <vector>
#include <map>
#include<fstream>
#include<Stack>
using namespace std;

#define ErrorCondition -3
#define NullRule "Encountered Null, popping Stack 1 time"
#define ScanError -1
#define PopError -2


struct Token {
	string key;
	string value;
};

class Parser {

private:

	ifstream reader;

	vector<vector<string>> cfg;
	vector<vector<int>> parseTable;
	map<string, int> productionMap;
	map <string, int> terminalMap;

	stack<string> Stack;




public:

	vector <Token> tokens;
	map <int, string> symbolTable;

	vector<string> split(string s, string delimiter) {
		size_t pos_start = 0, pos_end, delim_len = delimiter.length();
		string token;
		vector<string> res;

		while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
			token = s.substr(pos_start, pos_end - pos_start);
			pos_start = pos_end + delim_len;
			res.push_back(token);
		}

		res.push_back(s.substr(pos_start));
		return res;
	}

	// Initializer Functions
	void init(string* files)
	{
		this->initCFG(files[0]);
		this->initParseTable(files[1]);
		this->initMap(files[2]);
	}
	void initCFG(string fileName) {

		reader.open(fileName);
		if (!reader.is_open())
			cerr << "Unable to open file : " << fileName << endl;
		string s;
		string delimeter = " ";
		int count = 1;
		while (getline(reader, s)) {
			vector<string> dummy = split(s, delimeter);
			cfg.push_back(dummy);
		}
		reader.close();
	}
	void initTokens(string fileName) {
		reader.open(fileName);
		if (!reader.is_open())
			cerr << "Unable to open file : " << fileName << endl;
		
		string key;
		string value;

		while (reader >> key && reader >> value) {
			Token token;
			token.key = key;
			token.value = value;
			tokens.push_back(token);

			//cout << token.key <<" "<< token.value << endl;
		}
		reader.close();
	}
	void initSymbolTable(string fileName) {

		string s;
		int t = 0;
		reader.open(fileName);
		int count = 0;
		while (!reader.eof())
		{
			reader >> t;
			reader >> s;
			symbolTable.insert({t,s});
		}
		reader.close();
		//for (auto i = symbolTable.begin(); i != symbolTable.end(); i++)
			//cout << i->first << " " << i->second << endl;
	}
	void initMap(string fileName) {

		string s;
		int t = 0;
		reader.open(fileName);
		int count = 0;
		while (!reader.eof())
		{
			reader >> s;
			productionMap.insert({ s,count++ });
		}
		reader.close();
	}
	void initTerminalMap(string fileName) {

		string s;
		int t = 0;
		reader.open(fileName);
		int count = 0;
		while (!reader.eof())
		{
			reader >> s;
			terminalMap.insert({ s,count++ });
		}
		reader.close();
	}
	void initParseTable(string fileName)
	{
		reader.open(fileName);
		if (!reader.is_open())
			cerr << "Unable to open file :" << fileName << endl;
		int row = 0;
		int col = 0;
		reader >> row;
		reader >> col;
		int temp = 0;
		for (int i = 0; i < row; i++)
		{
			vector<int>dummy;
			for (int j = 0; j < col; j++)
			{
				reader >> temp;
				dummy.push_back(temp);
			}
			parseTable.push_back(dummy);
		}

		reader.close();
	}

	//Display Functions.
	void DisplayCFG()
	{
		for (int i = 0; i < cfg.size(); i++)
		{
			cout << i + 1 << " :    ";
			for (auto j = cfg[i].begin(); j != cfg[i].end(); j++)
				cout << *j << " ";
			cout << endl << endl;
		}
	}
	void DisplayProductionMap()
	{
		for (auto i = productionMap.begin(); i != productionMap.end(); i++)
			cout << i->first << " " << i->second << endl;
	}
	void DisplayTerminalMap()
	{
		for (auto i = terminalMap.begin(); i != terminalMap.end(); i++)
			cout << i->first << " " << i->second << endl;
	}
	void DisplayParseTable()
	{
		for (int i = 0; i < parseTable.size(); i++)
		{
			for (int j = 0; j < parseTable[i].size(); j++)
				cout << parseTable[i][j] << " ";
			cout << endl;
		}
	}
	void DisplayStackStatus()
	{
		stack<string>tempStack = Stack;

		cout << endl;
		cout << ".......Stack STATUS......." << endl;
		while (!tempStack.empty())
		{
			cout << tempStack.top()<<"  ";
			tempStack.pop();
		}
		cout << endl;
		cout << ".......Stack END......." << endl;
		cout << endl;
	}
	//Helper Functions
	int DoesTerimnalExist(string key) {

		if (terminalMap.find(key) == terminalMap.end()) {
			cerr << "Error : Terminal value returned by LEXER in tokens.txt does not exist in terminals.txt" << endl;
			return ErrorCondition;
		}
		else {
			return terminalMap[key];
		}
	}
	int DoesVariableExist(string key) {
		
		if (productionMap.find(key) == productionMap.end()) {
			cerr << "Error : Variable value returned from Stack does not exist in productionMap (cfgmap.txt)" << endl;
			return ErrorCondition;
		}
		else {
			return productionMap[key];
		}
	}
	bool ScanException(bool &scan,int productionValue) {
		if (productionValue == ScanError) {
			tokens.erase(tokens.begin());
			scan = true;
			return true;
		}
		return false;
	}
	bool PopException(int productionValue) {
		if (productionValue == PopError) {
			Stack.pop();
			return true;
		}
		return false;
	}
	string GetStackTop(string & key,bool scan) {
		if (!scan) {
			key = Stack.top();
			Stack.pop();
		}
		else {
			scan = false;
		}
		return key;
	}
	string GetTokenName(Token token) {
		string x = symbolTable[stoi(token.key)];
		cout <<"Symbol Table Representation : "<< token.value << ", " << x << endl;
		//string x = symbolTable.at(24);
		return x;
	}
	void PushProduction(vector<string> production) {
		for (auto j = production.rbegin(); j != production.rend(); ++j)
			Stack.push(*j);
	}

	// Main loo
	void foo() {

		Stack.push("$");
		Stack.push("Function");

		int productionValue;
		bool scan = false;
		string key;

		cout << "Begin Parsing on tokens.txt"<<endl;
		DisplayStackStatus();
		while (Stack.top() != "$") {
			

			// Pop element from Stack.
			key = GetStackTop(key,scan);

			// Check if the popped variable/value is null
			if (key == "Null") {
				cout << NullRule << endl;
				continue;
			}
			// Get token
			Token token = tokens.front();
			string terminal;
			//If token is keyword, Get its Type from symbol table.
			if (token.value == "Keyword"){
				terminal =  GetTokenName(token);
			}
			// get keyword for identifier and number.
			else if(token.value == "identifier" || token.value == "number"){

				terminal = token.value;
			}
			// get value for all other tokens that remain.
			else {
				terminal = token.key;
			}
			cout <<"Variable : "<< key << "  " <<"Input : "<< terminal << endl;
			if (key == terminal) {

				tokens.erase(tokens.begin());
				cout << "MATCHED !" << endl;
			}
			else {
				int variableIndex = DoesVariableExist(key);
				int terminalIndex = DoesTerimnalExist(terminal);

				if (terminalIndex != ErrorCondition) {
					productionValue = parseTable[variableIndex][terminalIndex];

					if (PopException(productionValue) || ScanException(scan,productionValue))
						continue;
					cout << "Pushing Production no : "<< productionValue << endl;

					vector<string> production = cfg[productionValue - 1];
					PushProduction(production);
					DisplayStackStatus();
				}
			}
		}
	}


};