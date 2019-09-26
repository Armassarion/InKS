%option prefix="auto"
%option yylineno
%option noyywrap
%option never-interactive

%{
#include <parser.tab.hh>
#include <cstring>
%}

inks_code	"#CODE"[[:space:]]*("("[iI][nN][kK][sS]")")
c_code		"#CODE"[[:space:]]*("(C""XX"?")")(#EN[^D]|#E[^N]|#[^E]|[^#])*#END
restrict	"#RESTRICT"([[:space:]]|[[:alnum:]]|_|>|<|=|\(|\))+
include		"#HEADER"[[:space:]]*("(C)")(#EN[^D]|#E[^N]|#[^E]|[^#])*#END
integer		[[:digit:]]+
id			([[:alnum:]]|_)+

%x C_COMMENT

%%
[[:space:]] { /* Ignored */ }

"/*"            { BEGIN(C_COMMENT); }
<C_COMMENT>"*/" { BEGIN(INITIAL); }
<C_COMMENT>.    { }
<C_COMMENT>\n   { }

{inks_code} { return(INKS_CODE); }
{c_code} {
	autolval.val=strdup(yytext);
	return(C_CODE);
}
{include} {
	autolval.val=strdup(yytext);
	return(INCLUDE);
}
{restrict} {
	autolval.val=strdup(yytext);
	return(RESTRICT);
}

{integer} {
	autolval.val=strdup(yytext);
	return(NOMBRE);
}

"pattern"			return(PATTERN);
"in"				return(IN);
"out"				return(OUT);
"kernel"			return(KERNEL);
"simulation"		return(SIMULATION);
"dom"				return(DOM);
"#DOM"				return(DOMAREA);

"|"					return(PIPE);
"+"					return(PLUS);
"-"					return(MINUS);
"*"					return(MULT);
"%"					return(MODULO);
"mod"				return(MOD);
":"					return(DPTS);
"="					return(EQUAL);
";"					return(NEXT);
","					return(COMMA);
"{"					return(CURLYG);
"}"					return(CURLYD);
"["					return(SQUAREG);
"]"					return(SQUARED);
"("					return(PARG);
")"					return(PARD);
"#END"				return(END_CODE);

{id} {
	autolval.val=strdup(yytext);
	return(ID);
}

%%
