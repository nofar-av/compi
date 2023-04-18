#include <iostream>
#include <map>
#include <string>
#include "tokens.hpp"
extern int yylex();

void showToken(std::string name)
{
	std::cout << yylineno << " " << name << " " << yytext << std::endl;
}

int main()
{
	int token;
	std::map<int, std::string> tokenMap = {
	{VOID, "VOID"},
    {INT, "INT"},
    {BYTE, "BYTE"},
    {B, "B"},
    {BOOL, "BOOL"},
    {AND, "AND"},
    {OR, "OR"},
    {NOT, "NOT"},
    {TRUE, "TRUE"},
    {FALSE, "FALSE"},
    {RETURN, "RETURN"},
    {IF, "IF"},
    {ELSE, "ELSE"},
    {WHILE, "WHILE"},
    {BREAK, "BREAK"},
    {CONTINUE, "CONTINUE"},
    {SC, "SC"},
    {COMMA, "COMMA"},
    {LPAREN, "LPAREN"},
    {RPAREN, "RPAREN"},
    {LBRACE, "LBRACE"},
    {RBRACE, "RBRACE"},
    {ASSIGN, "ASSIGN"},
    {RELOP, "RELOP"},
    {BINOP, "BINOP"},
    {COMMENT, "COMMENT"},
    {ID, "ID"},
    {NUM, "NUM"},
    {STRING, "STRING"},
    {OVERRIDE, "OVERRIDE"}
	};
	
	while((token = yylex()))
	{
		// Your code here
		showToken(tokenMap.at(token));
	}
	return 0;
}