%option noyywrap
%{
/*****************声明和选项设置  begin*****************/
#include <stdio.h>
#include <stdlib.h>

#include "lexical_analyzer.h"

int lines;
int pos_start;
int pos_end;
//IDENTIFIER ([a-zA-Z]|_)([a-zA-Z0-9]|_)*
/*****************声明和选项设置  end*****************/

%}
IDENTIFIER [a-zA-Z]+
INTEGER 0|[1-9][0-9]*
FLOATPOINT [0-9]*\.[0-9]*
ARRAY \[\]
LETTER \'.\'
COMMENT  ("//"[^\n]*)|("/*"([^*]|\*+[^*/])*\*+"/")  
BLANK ([ ]|[\t])+
%%

 /******************TODO*********************/
 /****请在此补全所有flex的模式与动作  start******/
 //STUDENT TO DO




"+" {pos_start=pos_end;pos_end=pos_start+yyleng;return ADD;}
"-" {pos_start=pos_end;pos_end=pos_start+yyleng;return SUB;}
"*" {pos_start=pos_end;pos_end=pos_start+yyleng;return MUL;} 
"/" {pos_start=pos_end;pos_end=pos_start+yyleng;return DIV;}
"<" {pos_start=pos_end;pos_end=pos_start+yyleng;return LT;}
"<=" {pos_start=pos_end;pos_end=pos_start+yyleng;return LTE;}
">" {pos_start=pos_end;pos_end=pos_start+yyleng;return GT;}
">=" {pos_start=pos_end;pos_end=pos_start+yyleng;return GTE;}
"==" {pos_start=pos_end;pos_end=pos_start+yyleng;return EQ;}
"!=" {pos_start=pos_end;pos_end=pos_start+yyleng;return NEQ;}
"=" {pos_start=pos_end;pos_end=pos_start+yyleng;return ASSIN;}


";" {pos_start=pos_end;pos_end=pos_start+yyleng;return SEMICOLON;}
"," {pos_start=pos_end;pos_end=pos_start+yyleng;return COMMA;}
"(" {pos_start=pos_end;pos_end=pos_start+yyleng;return LPARENTHESE;}
")" {pos_start=pos_end;pos_end=pos_start+yyleng;return RPARENTHESE;}
"[" {pos_start=pos_end;pos_end=pos_start+yyleng;return LBRACKET;}
"]" {pos_start=pos_end;pos_end=pos_start+yyleng;return RBRACKET;}
"{" {pos_start=pos_end;pos_end=pos_start+yyleng;return LBRACE;}
"}" {pos_start=pos_end;pos_end=pos_start+yyleng;return RBRACE;}


"else" {pos_start=pos_end;pos_end=pos_start+yyleng;return ELSE;}
"if" {pos_start=pos_end;pos_end=pos_start+yyleng;return IF;}
"int" {pos_start=pos_end;pos_end=pos_start+yyleng;return INT;}
"float" {pos_start=pos_end;pos_end=pos_start+yyleng;return FLOAT;}
"return" {pos_start=pos_end;pos_end=pos_start+yyleng;return RETURN;}
"void" {pos_start=pos_end;pos_end=pos_start+yyleng;return VOID;}
"while" {pos_start=pos_end;pos_end=pos_start+yyleng;return WHILE;}



{IDENTIFIER} {pos_start=pos_end;pos_end=pos_start+yyleng;return IDENTIFIER;}
{INTEGER} {pos_start=pos_end;pos_end=pos_start+yyleng;return INTEGER;}
{FLOATPOINT} {pos_start=pos_end;pos_end=pos_start+yyleng;return FLOATPOINT;}
{ARRAY} {pos_start=pos_end;pos_end=pos_start+yyleng;return ARRAY;}
{LETTER} {pos_start=pos_end;pos_end=pos_start+yyleng;return LETTER;}


"\n" {return EOL;}
{COMMENT} {return COMMENT;}
{BLANK}  {pos_start=pos_end;pos_end+=yyleng;return BLANK;} 
. {return ERROR;}

 /****请在此补全所有flex的模式与动作  end******/
%%
/****************C代码 start*************/

/// \brief analysize a *.cminus file
///
/// \param input_file, 需要分析的文件路径
/// \param token stream, Token_Node结构体数组，用于存储分析结果，具体定义参考lexical_analyer.h

void analyzer(char* input_file, Token_Node* token_stream){
    lines = 1;
    pos_start = 1;
    pos_end = 1;
    if(!(yyin = fopen(input_file,"r"))){
        printf("[ERR] No input file\n");
        exit(1);
    }
    printf("[START]: Read from: %s\n", input_file);

    int token;
    int index = 0;

    while(token = yylex()){
        switch(token){
            case COMMENT:
                for(int i=0;i<yyleng;i++)
                    if(yytext[i]=='\n'){
                        pos_end=1;
                        pos_start=1;
                        lines++;
                    }else pos_end++;
            case BLANK:
                break;
            case EOL:
                pos_start=1;
                pos_end=1;
                lines++;
                break;
            case ERROR:
                printf("[ERR]: unable to analysize %s at %d line, from %d to %d\n", yytext, lines, pos_start, pos_end);
            default :
                if (token == ERROR){
                    sprintf(token_stream[index].text, "[ERR]: unable to analysize %s at %d line, from %d to %d", yytext, lines, pos_start, pos_end);
                } else {
                    strcpy(token_stream[index].text, yytext);
                }
                token_stream[index].token = token;
                token_stream[index].lines = lines;
                token_stream[index].pos_start = pos_start;
                token_stream[index].pos_end = pos_end;
                index++;
                if (index >= MAX_NUM_TOKEN_NODE){
                    printf("%s has too many tokens (> %d)", input_file, MAX_NUM_TOKEN_NODE);
                    exit(1);
                }
        }
    }
    printf("[END]: Analysis completed.\n");
    return;
}



/****************C代码 end*************/
