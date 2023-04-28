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
id              ([0-9a-zA-Z])
whitespace		([\t\n ])
printable_chars ([\x20-\x7E])
string_chars   ([\x20-\x21\x23-\x7E]|\\\")
new_line         ([\x0A])
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
{letter}+{id}*              return ID;
0|[1-9][0-9]*               return NUM;
\"{string_chars}*\"         return STRING;
\"{string_chars}*         return STRING_ERROR;
{whitespace}				;
{printable_chars}           return CHAR_ERROR;
.		printf("Lex doesn't know what that is!\n");

%%

