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
\/\/[^\r\n]*                return COMMENT;
{letter}+{id}*              return ID;
0|[1-9][0-9]*               return NUM;
\"{string_chars}*\"         return STRING;
\"{string_chars}*\\[^nrxt0\\\"]{string_chars}* return UNDEFINED_ESCAPE_SEQ;
\"{string_chars}*           return STRING_ERROR;
{whitespace}				;
.                           return CHAR_ERROR;		

%%

