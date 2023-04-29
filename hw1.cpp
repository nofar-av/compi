#include <iostream>
#include <map>
#include <string>
#include "tokens.hpp"
#include <regex>

extern int yylex();
std::string unescape(const std::string& s);

void showUndefinedEscapeSeq()
{
    std::string s(yytext);
    int i = 0;
    while (i != s.length())
    {
        char c = s[i++];
        if(i + 1 < s.length() && c == '\\' && s[i] == 'x' && s[i + 1] == '\\')
        {
            std::cout << "Error undefined escape sequence " << s.substr(i, 3) << std::endl;                
            exit(0);
        }
        else if (c == '\\' && i != s.length() && s[i] != 'r' && s[i] != 'n' && s[i] != 't' && s[i] != '0' && s[i] != '"' && s[i] != '\\'
            && s[i] != 'x')
        {
            std::cout << "Error undefined escape sequence " << s[i] << std::endl;
            exit(0);
        }    
    }
    std::cout << "mistake" << std::endl;
    exit(0);
}
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
        str = unescape(str);
    }
    else if(name == "COMMENT")
    {
        std::cout << yylineno << " " << name << " //" << std::endl;
        return;  
    }
	std::cout << yylineno << " " << name << " " << str.c_str() << std::endl;
}

std::string unescape(const std::string& s)
{
    std::string res;
    int i = 0;
    while (i != s.length())
    {
        char c = s[i++];
        if (c == '\\' && i != s.length())
        {
            switch (s[i++]) {
            case '\\': c = '\\'; break;
            case 'n': c = '\n'; break;
            case 't': c = '\t'; break;
            case '0': c = '\0'; break;
            case 'r': c = '\r'; break;
            case '"': c = '\"'; break;
            case 'x': {
                // check both characters are legal hex characters
                int c1 = s[i]; 
                int c2 = s[i+1];
                if (('0' <= c1 && c1 <= '7') &&
                        (('0' <= c2 && c2 <= '9') || ('A' <= c2 && c2 <= 'F') || ('a' <= c2 && c2 <= 'f')))
                {
                    c = (char)std::stoi(s.substr(i, 2), nullptr, 16);
                    i += 2;
                    break;
                }
                std::cout << "Error undefined escape sequence " << s.substr(i-1, 3) << std::endl;
                exit(0);
            }
            // all other escapes
            default: { 
            // invalid escape sequence - skip it. alternatively you can copy it as is, throw an exception...
                std::cout << "Error undefined escape sequence " << s[i - 1] << std::endl;
                exit(0);
            }
            }
        }
        else if (c == '\\' && i == s.length())
        {
            std::cout << "Error unclosed string" << std::endl;
            exit(0);
        }
        res += c;
  }

  return res;
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
        if(token == 34)
        {
            showUndefinedEscapeSeq();
        }
        else if(token > 30)
            showError(tokenMap.at(token));
		showToken(tokenMap.at(token));
	}
	return 0;
}