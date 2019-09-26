%code requires {

#include <List_constraints_t.hpp>
#include <List_named_poly_t.hpp>
#include <Named_Poly_t.hpp>
#include <Constraint_t.hpp>
#include <Union_Poly_t.hpp>
#include <Set_Poly_t.hpp>
#include <List_var_t.hpp>
#include <Point_t.hpp>
#include <Expr_t.hpp>
#include <List_t.hpp>
#include <Var_t.hpp>

#include <Combo/DD_Valloc_t.hpp>
#include <Combo/DD_Vamap_t.hpp>
#include <Combo/DD_Amap_t.hpp>

#include <DDFor_utils/Options/OMP_For_option_t.hpp>
#include <DDFor_utils/Options/OMP_TaskLoop_option_t.hpp>
#include <DD_OMP_Parallel_Region_t.hpp>
#include <V_DD_Block_t.hpp>
#include <DD_Update_t.hpp>
#include <DD_Block_t.hpp>
#include <DD_Param_t.hpp>
#include <DD_Unmap_t.hpp>
#include <DD_Code_t.hpp>
#include <DD_Line_t.hpp>
#include <DD_Auto_t.hpp>
#include <DD_Copy_t.hpp>
#include <DD_Decl_t.hpp>
#include <DD_Mem_t.hpp>
#include <DD_Map_t.hpp>
#include <DD_For_t.hpp>
#include <DD_Dom_t.hpp>
#include <DD_Free.hpp>
#include <vector>

#include <DDFor_utils/V_DDForNamedVar.hpp>
#include <DDFor_utils/Options/DDForOptions_t.hpp>
#include <DDFor_utils/Options/DDForOption_t.hpp>

typedef struct yy_buffer_state * YY_BUFFER_STATE;

}

%code provides {

#include <Inequality_t.hpp>
#include <Reference_t.hpp>
#include <Equality_t.hpp>
#include <Nombre_t.hpp>
#include <Modulo_t.hpp>
#include <Minus_t.hpp>
#include <Mult_t.hpp>
#include <Plus_t.hpp>
#include <Neg_t.hpp>
#include <Var_t.hpp>

extern int DDlineno;
extern char* DDtext;

YY_BUFFER_STATE DD_scan_string(const char * str);
void DD_delete_buffer(YY_BUFFER_STATE buffer);
int DDparse(V_DD_Block_t*);

extern "C"{
		int yylex();
		int yyerror(V_DD_Block_t*, const char *s);
	}
}

%define api.prefix {DD}
%define parse.error verbose
%parse-param {V_DD_Block_t* DD_Blocks}

%union{
	const char* val;
	DD_Alloc_param_t* ddlistalloc;
	V_DDForNamedVar* vfnv;
	DD_Block_t* ddblock;
	DD_Param_t* ddparam;
	DDForOptions_t* fos;
	DDForNamedVar* fnv;
	DDForOption_t* fo;
	DD_Code_t* ddcode;
	DD_Line_t* ddline;
	List_var_t* lvar;
	DD_Map_t* ddmap;
	DD_Mem_t* ddmem;
	IOMap_t* iomap;
	
	Var_t* f;
	Point_t* p;
	Expr_t* expr;
	List_t* list;
	Constraint_t* c;
	Set_Poly_t* poly;
	Union_Poly_t* upoly;
	Named_Poly_t* npoly;
	List_constraints_t* lcs;
	List_named_poly_t* lns;
};
%type <f> Named_Range
%type <expr> Scalar Expression
%type <list> List ParList Private
%type <p> List_of_expr Point
%type <c> Range Constraint
%type <poly> Poly
%type <upoly> Union_Poly 
%type <npoly> Named_Poly
%type <lns> List_Named_Poly
%type <lcs> List_of_constraints

%type <val> Collapse
%type <val> GrainSize
%type <iomap> InOutMap
%type <ddlistalloc> DD_List_alloc
%type <lvar> List_var
%type <ddblock> DDBlock
%type <ddcode> DDCode
%type <ddline> DDLine DDDecl DDUnmap DDUpdate DDFor DDOmpRegion
%type <ddmem> DDMem
%type <ddmap> DDMap
%type <ddparam> DDParam
%type <vfnv> DDForNamedList;
%type <fnv> DDForNamedVar;
%type <fos> DDForOptions;
%type <fo> DDForOption;

%token <val> ID NOMBRE C_CODE INCLUDE 
%token FOR MAP UNMAP ALLOC PARAM FREE DECL UPDATE 
%token DOM VALLOC VAMAP AMAP
%token GIVE TAKE KEEP IN OUT USING WITH COPY AUTO
%token EQUAL PLUS MINUS MULT MODULO MOD
%token END_CODE
%token PARG PARD /* ( ) */
%token CURLYG CURLYD /* { } */
%token SQUAREG SQUARED /* [ ] */
%token PIPE NEXT COMMA DPTS AND /* | ; , : */

%token OMP OMP_FOR OMP_TASKLOOP PARALLEL COLLAPSE GRAINSIZE PRIVATE /*FOR options*/

%left PLUS MINUS
%precedence MULT
%precedence NEG

%start Input
%%

Input:
	  %empty
	| Input DDBlock	{DD_Blocks->push($2);}
;
DDBlock:
	  ID PARG 	DD_List_alloc COMMA 	List	PARD CURLYG DDCode CURLYD {$$ = new DD_Block_t($1, $3, $5, $8);}
	| ID PARG 							List 	PARD CURLYG DDCode CURLYD {$$ = new DD_Block_t($1, $3, $6);}
	| ID PARG 	DD_List_alloc					PARD CURLYG DDCode CURLYD {$$ = new DD_Block_t($1, $3, $6);}
;
DD_List_alloc:
	  DDParam						{$$ = new DD_Alloc_param_t($1);}
	| DD_List_alloc COMMA DDParam	{$$ = $1->push($3);}
;
DDCode:
	  DDLine			{$$ = new DD_Code_t($1);}
	| DDCode DDLine		{$$ = $1->push($2);}
;
DDLine:
	  UPDATE DDFor		{$$ = $2;}
	| UPDATE DDUpdate	{$$ = $2;}
	| ALLOC DDMem		{$$ = $2;}
	| DECL DDDecl		{$$ = $2;}
	| MAP DDMap			{$$ = $2;}
	| UNMAP DDUnmap		{$$ = $2;}
	| OMP DDOmpRegion	{$$ = $2;}
	| FREE ID			{$$ = new DD_Free_t(DDlineno, $2);}
	| DOM List_var		{$$ = new DD_Dom_t(DDlineno, $2);}
	| VALLOC DDMem		{$$ = new DD_Valloc_t($2);}
	| VAMAP ID DDMap	{$$ = new DD_Vamap_t($2, $3);}
	| AMAP ID DDMap		{$$ = new DD_Amap_t($2, $3);}
;
DDFor:
	  List_Named_Poly USING FOR PARG DDForNamedList PARD CURLYG DDCode CURLYD	{$$ = new DD_For_t(DDlineno, $1, $5, $8);}
;
DDForNamedList:
	  DDForNamedVar							{$$ = new V_DDForNamedVar($1);}
	| DDForNamedList COMMA DDForNamedVar	{$$ = $1->push($3);}
;
DDForNamedVar:
	  DDForOptions ID CURLYG List_of_constraints		            CURLYD	{$$ = new DDForNamedVar($1, $2, $4);}
	| DDForOptions ID CURLYG Expression DPTS Expression DPTS NOMBRE CURLYD	{$$ = new DDForNamedVar($1, $2, new Modulo_t(new Inequality_t($4, $6), $4->clone(), $8));}
;
DDForOptions:
	  %empty										{$$ = new DDForOptions_t();}
	| CURLYG DDForOption CURLYD 					{$$ = new DDForOptions_t($2);}
	| CURLYG DDForOptions NEXT DDForOption CURLYD	{$$ = $2->push($4);}
;
DDForOption:
	  OMP_FOR Collapse Private							{$$ = new OMP_For_option_t($2, $3);}
	| OMP_FOR PARALLEL Collapse Private					{$$ = new OMP_For_option_t($3, $4, true);}
	| OMP_TASKLOOP GrainSize Collapse Private			{$$ = new OMP_TaskLoop_option_t($2, $3, $4);}
	| OMP_TASKLOOP PARALLEL GrainSize Collapse Private	{$$ = new OMP_TaskLoop_option_t($3, $4, $5, true);}
;
Collapse:
	  %empty						{$$ = strdup("1");}
	| COLLAPSE PARG NOMBRE PARD		{$$ = $3;}
;
GrainSize:
	%empty							{$$ = strdup("1");}
	| GRAINSIZE PARG NOMBRE PARD	{$$ = $3;}
;
Private:
	  %empty			{$$ = nullptr;}
	| PRIVATE ParList	{$$ = $2;}
;
DDOmpRegion:
	PARALLEL Private CURLYG DDCode CURLYD	{$$ = new DD_OMP_Parallel_Region_t(DDlineno, $2, $4);}
;
DDUpdate:
	  List_Named_Poly USING List	{$$ = new DD_Update_t(DDlineno, $1, $3);}
	| List_Named_Poly USING AUTO	{$$ = new DD_Auto_t(DDlineno, $1);}
	| List_Named_Poly USING COPY	{$$ = new DD_Copy_t(DDlineno, $1);}
;	
DDMem:
	  ID ID Point	{$$ = new DD_Mem_t(DDlineno, $1, $2, $3);}
	| ID ID			{$$ = new DD_Mem_t(DDlineno, $1, $2);}
;
DDMap:
	  List_var ID Point EQUAL ID Point	{$$ = new DD_Map_t(DDlineno, $1, $2, $3, $5, $6);}
	| List_var ID       EQUAL ID Point	{$$ = new DD_Map_t(DDlineno, $1, $2, $4, $5);}	  
	|  	       ID Point EQUAL ID Point	{$$ = new DD_Map_t(DDlineno, $1, $2, $4, $5);}
	| 		   ID       EQUAL ID Point	{$$ = new DD_Map_t(DDlineno, $1, $3, $4);}
	| 		   ID       EQUAL ID		{$$ = new DD_Map_t(DDlineno, $1, $3);}
;
DDUnmap:
	  ID			{$$ = new DD_Unmap_t(DDlineno, $1);}
	| ID Union_Poly	{$$ = new DD_Unmap_t(DDlineno, $1, $2);}
;
DDParam:
	  GIVE ID ID Point InOutMap	{$$ = new DD_Param_t(DDlineno, DD_Param_t::OWN::Give, $2, $3, $4, $5);}
	| KEEP ID ID Point InOutMap {$$ = new DD_Param_t(DDlineno, DD_Param_t::OWN::Keep, $2, $3, $4, $5);}
	| TAKE ID ID Point InOutMap {$$ = new DD_Param_t(DDlineno, DD_Param_t::OWN::Take, $2, $3, $4, $5);}
	| GIVE ID ID InOutMap		{$$ = new DD_Param_t(DDlineno, DD_Param_t::OWN::Give, $2, $3, $4);}
	| KEEP ID ID InOutMap		{$$ = new DD_Param_t(DDlineno, DD_Param_t::OWN::Keep, $2, $3, $4);}
	| TAKE ID ID InOutMap		{$$ = new DD_Param_t(DDlineno, DD_Param_t::OWN::Take, $2, $3, $4);}
;
InOutMap:
	  %empty 											{$$ = new IOMap_t(nullptr, nullptr);}
	| CURLYG IN  DPTS DDMap PIPE OUT DPTS DDMap CURLYD	{$$ = new IOMap_t($4, $8);}
	| CURLYG OUT DPTS DDMap CURLYD						{$$ = new IOMap_t(nullptr, $4);}
	| CURLYG IN  DPTS DDMap CURLYD						{$$ = new IOMap_t($4, nullptr);}
;
DDDecl:
	  ID ID PARG NOMBRE PARD	{$$ = new DD_Decl_t(DDlineno, $1, $2, atoi($4));}
	| ID ID						{$$ = new DD_Decl_t(DDlineno, $1, $2);}
;
List:
	  %empty			{$$ = new List_t;}
	| ID				{$$ = new List_t($1);}
	| List COMMA ID		{$$ = $1->push($3);}
;
ParList:
	  PARG List PARD		{$$ = $2;}	
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
	| Constraint MOD NOMBRE 		{$$ = new ModuloT_t($1, $3);}
;
List_of_constraints:
	  Constraint							{$$ = new List_constraints_t($1);}
	| List_of_constraints AND Constraint	{$$ = $1->push($3);}
;
List_of_expr:
	  List_of_constraints						{$$ = new Point_t($1);}
	| List_of_expr COMMA List_of_constraints	{$$ = $1->push($3);}
;
Point:
	  PARG PARD						{ $$ = new Point_t();}
	| PARG List_of_expr PARD		{ $$ = $2; }
;
Range:
	  SQUAREG List_of_constraints SQUAREG	{$$ = $2;}
	| SQUAREG List_of_constraints SQUARED	{$$ = $2->unset_supremum();}
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
Named_Poly:
	  ID Union_Poly		{ $$ = new Named_Poly_t($1, $2); }
	| ID				{ $$ = new Named_Poly_t($1); }
;
List_Named_Poly:
	  Named_Poly						{$$ = new List_named_poly_t($1); }
	| List_Named_Poly COMMA Named_Poly	{$$ = $1->push($3); }
;
%%

int DDerror(V_DD_Block_t* DD_Blocks, const char *s) {
	printf("%s: '%s' in line %d\n", s, DDtext, DDlineno);
	exit(EXIT_FAILURE);
}
