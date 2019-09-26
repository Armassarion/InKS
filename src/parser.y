%code requires {

#include <V_Function_application_t.hpp>
#include <Function_application_t.hpp>
#include <V_Dependence_t.hpp>
#include <Dependence_t.hpp>
#include <Constraint_t.hpp>
#include <Union_Poly_t.hpp>
#include <List_var_t.hpp>
#include <Array_Map.hpp>
#include <Point_t.hpp>
#include <Expr_t.hpp>
#include <Array.hpp>

typedef struct yy_buffer_state * YY_BUFFER_STATE;

}

%code provides {
#include <Inequality_t.hpp>
#include <Reference_t.hpp>
#include <Set_Poly_t.hpp>
#include <Equality_t.hpp>
#include <Nombre_t.hpp>
#include <Modulo_t.hpp>
#include <Minus_t.hpp>
#include <globals.hpp>
#include <List_t.hpp>
#include <Mult_t.hpp>
#include <Plus_t.hpp>
#include <Neg_t.hpp>
#include <Var_t.hpp>

extern int autolineno;
extern char* autotext;

YY_BUFFER_STATE auto_scan_string(const char * str);
void auto_delete_buffer(YY_BUFFER_STATE buffer);
int autoparse();

extern "C" {
		int yylex();
		int yyerror(const char *s);
	}
}

%define api.prefix {auto}
%define parse.error verbose

%union{
	const char* val;
	Array* ar;
	Array_Map* amap;
	Dependence_t* io;
	V_Dependence_t* vio;
	Function_application_t* fa;
	V_Function_application_t* vfa;
	
	Expr_t* expr;
	List_t* list;
	Point_t* p;
	Var_t* f;
	Constraint_t* c;
	List_var_t* lvar;
	Set_Poly_t* poly;
	Union_Poly_t* upoly;
};
%type <f> Named_Range
%type <val> ParNombre RestrictSet
%type <expr> Scalar Expression
%type <list> List ParList
%type <p> List_of_expr Point
%type <c> Range Constraint
%type <poly> Poly DptsUP
%type <upoly> Union_Poly 
%type <lvar> List_var
%type <io> InOutline
%type <vio> KernelArrayline
%type <fa> Applyline
%type <ar> Arrayline
%type <vfa> Applyset
%type <amap> Arrayset

%token <val> ID NOMBRE C_CODE INCLUDE RESTRICT
%token KERNEL SIMULATION PATTERN IN OUT
%token EQUAL PLUS MINUS MULT MODULO MOD
%token INKS_CODE END_CODE DOMAREA DOM
%token PARG PARD /* ( ) */
%token CURLYG CURLYD /* { } */
%token SQUAREG SQUARED /* [ ] */
%token PIPE NEXT COMMA DPTS /* | ; , : */

%left PLUS MINUS
%precedence MULT
%precedence NEG

%start Input
%%

Input:
	  %empty
	| Input INCLUDE 			{ Includes.push($2); }
	| Input InKSLine
;

InKSLine:
	  KERNEL Kernelline
	| SIMULATION Generateline
;
/* Func_name(x, y, z, ...)*/
Kernelline:
	  ID ParList DPTS PARG KernelArrayline PARD C_CODE	{ Functions.push($1, $2, $5, $7); }
;
/* TYPE ID_ARR(DIM_SIZE)*/
KernelArrayline:
	  ID ID CURLYG InOutline CURLYD							{ $$ = new V_Dependence_t($4->add_arr_info($2, $1)); }
	| KernelArrayline COMMA ID ID CURLYG InOutline CURLYD	{ $$ = $1->push($6->add_arr_info($4, $3)); }
;
InOutline:
	  %empty						{ $$ = new Dependence_t; }
	| IN  DptsUP PIPE OUT DptsUP	{ $$ = new Dependence_t($2, $5); }
	| OUT DptsUP PIPE IN  DptsUP	{ $$ = new Dependence_t($5, $2); }
	| IN  DptsUP					{ $$ = new Dependence_t($2, nullptr); }
	| OUT DptsUP					{ $$ = new Dependence_t(nullptr, $2); }
;
DptsUP:
	  %empty			{ $$ = new Point_t();}
	| DPTS Union_Poly	{ $$ = $2;}
;
/*func_name (...) : (arrays in/out) #CODE (INKS) {apply} #END*/
Generateline:
	  ID ParList DPTS PARG Arrayset PARD RestrictSet DomArea INKS_CODE Applyset END_CODE { Gen.init($1, $2, $5, $10, $7); }
;
/* x, y, ...*/
List:
	  %empty			{$$ = new List_t;}
	| ID				{$$ = new List_t($1);}
	| List COMMA ID		{$$ = $1->push($3);}
;
ParList:
	  PARG List PARD		{$$ = $2;}	
;
Arrayset:
	  Arrayline					{ $$ = new Array_Map($1); }
	| Arrayset COMMA Arrayline	{ $$ = $1->push($3);   }
;	
Arrayline:
	  ID ID ParNombre							{ $$ = new Array($1, $2, $3);     }
	| ID ID ParNombre CURLYG InOutline CURLYD	{ $$ = new Array($1, $2, $3, $5->add_arr_info($2, $1)); }
;
ParNombre:
	  %empty			{$$ = nullptr;}
	| PARG NOMBRE PARD	{$$ = $2;}
;
RestrictSet:
	  %empty		{$$ = "";}
	| RESTRICT NEXT {$$ = $1;}
;
DomArea:
	  %empty
	| DOMAREA DomSet
;
DomSet:
	  DOM List_var			{Gen.add_vars($2);}
	| DomSet DOM List_var	{Gen.add_vars($3);}
;
Applyset:
	  Applyline					{$$ = new V_Function_application_t($1);}
	| Applyset COMMA Applyline	{$$ = $1->push($3);}
;
Applyline:
	 ID Union_Poly DPTS ParList {$$ = new Function_application_t($1, $2, $4);}
;
List_var:
	  Named_Range			{$$ = new List_var_t($1);}
	| List_var Named_Range	{$$ = $1->push($2);}
;
/*****************************POLYHEDRON*/
Scalar:
	  NOMBRE						{ $$ = new Nombre_t($1); }
	| ID /*constant or variable*/	{ $$ = new Reference_t($1); }
;
Expression:
	  Scalar								{$$ = $1;}
	| NOMBRE MULT Expression				{$$ = new Mult_t($1, $3); }
	| Expression PLUS Expression			{$$ = new Plus_t($1, $3); }
	| Expression MINUS Expression			{$$ = new Minus_t($1, $3); }
	| MINUS Expression %prec NEG			{$$ = new Neg_t($2); }
;
Constraint:
	  Expression					{$$ = new Equality_t($1);}
	| 			 DPTS				{$$ = new Equality_t(nullptr);}
	| Expression DPTS Expression	{$$ = new Inequality_t($1, $3);}
	| 			 DPTS Expression	{$$ = new Inequality_t(nullptr, $2);}
	| Expression DPTS 				{$$ = new Inequality_t($1, nullptr);}
	| Constraint MOD NOMBRE			{$$ = new ModuloT_t($1, $3);}
;
List_of_expr:
	  Constraint					{$$ = new Point_t($1);}
	| List_of_expr COMMA Constraint {$$ = $1->push($3);}
;
Point:
	  PARG PARD						{ $$ = new Point_t();}
	| PARG List_of_expr PARD		{ $$ = $2; }
;
Range:
	  SQUAREG Constraint SQUAREG	{$$ = $2;}
	| SQUAREG Constraint SQUARED	{$$ = $2->unset_supremum();}
;
Named_Range:
	  ID EQUAL Range 			{$$ = new Var_t($1, $3);}
;
Poly:
	  Point									{ $$ = $1; }
	| Named_Range CURLYG Union_Poly CURLYD	{ $$ = $1->set_inner($3); }
	| Named_Range Poly 						{ $$ = $1->set_inner($2); }
;
Union_Poly:
	  Poly					{ $$ = new Union_Poly_t($1); }
	| Union_Poly NEXT Poly	{ $$ = $1->push($3); }
;
%%

int autoerror(const char *s) {
	printf("%s: '%s' in line %d\n", s, autotext, autolineno);
	exit(EXIT_FAILURE);
}
