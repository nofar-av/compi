%{

/* Declarations section */
#include <stdio.h>
#include "tokens.hpp"

%}

%option yylineno
%option noyywrap
relop           (==|!=|<|>|<=|>=)
binop           ([-+/*])
digit   		([0-9])
letter  		([a-zA-Z])
whitespace		([\t\n ])

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
\/\/[^\r\n]*                return COMMENT;
{letter}+{digit}*           return ID;
0|[1-9][0-9]*               return NUM;
{whitespace}				;
.		printf("Lex doesn't know what that is!\n");

%%

