%option noyywrap
%{
#include <stdio.h>
#include <stdlib.h>

#include "syntax_tree.h"
#include "syntax_analyzer.h"

int files_count;
int lines;
int pos_start;
int pos_end;

void pass_node(char *text){
     yylval.node = new_syntax_tree_node(text);
}
%}

/***************
 TO STUDENTS: Copy your Lab1 here. Make adjustments if necessary.

  Note: don't modify the prologue unless you know what you are doing.
***************/
IDENTIFIER [a-zA-Z]+
INTEGER 0|[1-9][0-9]*
FLOATPOINT [0-9]*\.[0-9]*
ARRAY \[\]
LETTER \'.\'
COMMENT  ("//"[^\n]*)|("/*"([^*]|\*+[^*/])*\*+"/")  
BLANK ([ ]|[\t])+
%%

"+" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return ADD;}
"-" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return SUB;}
"*" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return MUL;} 
"/" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return DIV;}
"<" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return LT;}
"<=" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return LTE;}
">" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return GT;}
">=" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return GTE;}
"==" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return EQ;}
"!=" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return NEQ;}
"=" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return ASSIN;}


";" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return SEMICOLON;}
"," {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return COMMA;}
"(" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return LPARENTHESE;}
")" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return RPARENTHESE;}
"[" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return LBRACKET;}
"]" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return RBRACKET;}
"{" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return LBRACE;}
"}" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return RBRACE;}


"else" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return ELSE;}
"if" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return IF;}
"int" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return INT;}
"float" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return FLOAT;}
"return" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return RETURN;}
"void" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return VOID;}
"while" {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return WHILE;}



{IDENTIFIER} {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return IDENTIFIER;}
{INTEGER} {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return INTEGER;}
{FLOATPOINT} {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return FLOATPOINT;}
{ARRAY} {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return ARRAY;}
{LETTER} {pos_start=pos_end;pos_end=pos_start+yyleng;pass_node(yytext);return LETTER;}


[\n]+ {pos_start=1;pos_end=1;lines+=yyleng;}
{COMMENT} {for(int i=0;i<yyleng;i++)
                if(yytext[i]=='\n'){
                        pos_end=1;
                        pos_start=1;
                        lines++;
                    }else pos_end++;
                    }
{BLANK} {pos_start = pos_end;pos_end+=yyleng;}
. {printf("error!\n");}


%%

// /* Example for you :-) */
// \+  { pos_start = pos_end; pos_end += 1; pass_node(yytext); return ADD; }


