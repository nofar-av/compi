#include <iostream>
#include <map>
#include <string>
#include "tokens.hpp"
#include <regex>

extern int yylex();

void showError(std::string name)
{
    if(name == "Error unclosed string")
    {
        std::cout << name << std::endl;
    }
    else
    {    
        std::cout << name << " " << yytext << std::endl;
    }
    exit(0);
}
void showToken(std::string name)
{
    std::string str(yytext);
    if(name == "STRING")
    {
        str = str.substr(1, str.length() - 2);
        std::regex reg("[\\]{2,}");
        str = std::regex_replace(str, reg, "\\");
    }
	std::cout << yylineno << " " << name << " " << str << std::endl;
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
    {OVERRIDE, "OVERRIDE"},
    {CHAR_ERROR,"Error"},
    {STRING_ERROR,"Error unclosed string"},
    {UNCLOSED_STRING_ERROR,"Error unclosed string"},
    {UNDEFINED_ESCAPE_SEQ,"Error undefined escape sequence"}
	};
	
	while((token = yylex()))
	{
		// Your code here
        if(token > 30)
            showError(tokenMap.at(token));
		showToken(tokenMap.at(token));
	}
	return 0;
}