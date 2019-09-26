%option prefix="DD"
%option yylineno
%option noyywrap
%option never-interactive

%{
#include <dd_parser.tab.hh>
#include <cstring>
%}

c_code		"#CODE"[[:space:]]*("(C""XX"?")")(#EN[^D]|#E[^N]|#[^E]|[^#])*#END
include		"#HEADER"[[:space:]]*("(C)")(#EN[^D]|#E[^N]|#[^E]|[^#])*#END
integer		[[:digit:]]+
id			([[:alnum:]]|_)+

%x C_COMMENT
%x INLINE_C_COMMENT

%%
[[:space:]] { /* Ignored */ }

"/*"            { BEGIN(C_COMMENT); }
<C_COMMENT>"*/" { BEGIN(INITIAL); }
<C_COMMENT>.    { }
<C_COMMENT>\n   { }

"//"           			{ BEGIN(INLINE_C_COMMENT); }
<INLINE_C_COMMENT>"\n" 	{ BEGIN(INITIAL); }
<INLINE_C_COMMENT>.    	{ }

{c_code} {
	DDlval.val=strdup(yytext);
	return(C_CODE);
}
{include} {
	DDlval.val=strdup(yytext);
	return(INCLUDE);
}

{integer} {
	DDlval.val=strdup(yytext);
	return(NOMBRE);
}

"for"		return(FOR);
"map"		return(MAP);
"unmap"		return(UNMAP);
"update"	return(UPDATE);
"var"		return(DECL);
"alloc"		return(ALLOC);
"valloc"	return(VALLOC);
"vamap"		return(VAMAP);
"amap"		return(AMAP);
"free"		return(FREE);
"param"		return(PARAM);
"using" 	return(USING);
"with"	 	return(WITH);
"in"		return(IN);
"out"		return(OUT);
"copy"		return(COPY);
"auto"		return(AUTO);
"dom"		return(DOM);

"give"		return(GIVE);
"keep"		return(KEEP);
"take"		return(TAKE);

"|"			return(PIPE);
"+"			return(PLUS);
"-"			return(MINUS);
"*"			return(MULT);
"%"			return(MODULO);
"mod"		return(MOD);
":"			return(DPTS);
"="			return(EQUAL);
";"			return(NEXT);
","			return(COMMA);
"{"			return(CURLYG);
"}"			return(CURLYD);
"["			return(SQUAREG);
"]"			return(SQUARED);
"("			return(PARG);
")"			return(PARD);
"and"		return(AND);
"#END"		return(END_CODE);


"OMP"               return(OMP);
"OMP_FOR"           return(OMP_FOR);
"OMP_TASKLOOP"      return(OMP_TASKLOOP);
"parallel"	return(PARALLEL);
"collapse"	return(COLLAPSE);
"grainsize"  return(GRAINSIZE);
"private"	return(PRIVATE);


{id} {
	DDlval.val=strdup(yytext);
	return(ID);
}

%%
