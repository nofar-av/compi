%{

    /* Declarations section */
    #include <stdio.h>
    #include "parser.tab.hpp"
    #include "output.hpp"

%}

%option yylineno
%option noyywrap
relop           (==|!=|<|>|<=|>=)
binop           ([-+/*])
digit   		([0-9])
letter  		([a-zA-Z])
id              ([0-9a-zA-Z])
whitespace		([\r\t\n ])
string_chars    ([\x09\x20-\x21\x23-\x5B\x5D-\x7E]|\\\"|\\r|\\n|\\0|\\t|\\x|\\\\)
%%

void                        return VOID;
int                         return INT;
byte                        return BYTE;
b                           return B;
bool                        return BOOL;
override                    return OVERRIDE;
and                         return AND;
or                          return OR;
not                         return NOT;
true                        return TRUE;
false                       return FALSE;
return                      return RETURN;
if                          return IF;
else                        return ELSE;
while                       return WHILE;
break                       return BREAK;
continue                    return CONTINUE;
;                           return SC;
,                           return COMMA;
\(                          return LPAREN;
\)                          return RPAREN;
\{                          return LBRACE;
\}                          return RBRACE;
=                           return ASSIGN;
{relop}                     return RELOP;
{binop}                     return BINOP;
\/\/[^\r\n]*[\r|\n|\r\n]?   ;
{letter}+{id}*              return ID;
0|[1-9][0-9]*               return NUM;
\"([^\n\r\"\\]|\\[rnt"\\])+\"         return STRING;
{whitespace}				;
.                           { output::errorLex(yylineno); exit(0);};	

%%

