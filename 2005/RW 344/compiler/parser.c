/********************************************************
 * 	Parser.c					*
 *							*
 * 	Written by Abrie Greeff				*
 *							*
 * 	Part of Software design: Compiler project (2005)*
 *							*
 * 	Group: Andrew Mori, Eduard Bergh, Mia Meyer	*
 ********************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/types.h"
#include "include/scanner.h"
#include "include/error.h"
#include "include/symboltable.h"
#include "include/opcodes.h"
#include "include/codegen.h"                             
#include "include/parser.h"


//#include "include/codegen.h"

static token_str* token;
static char last_expected[MAX_LEN+1], last_found[MAX_LEN+1], proc[MAX_LEN], def_name[MAX_LEN+1], const_name[MAX_LEN+1],call_name[MAX_LEN+1];
static int need_priority, level, val;
static enum types type;
//static int express = FALSE;
enum symtype sym_type;
enum opcodes op;
varlist* list;
var_node *params;
//int isInt;

#ifdef DEBUG_PARSER
	static int spacing, spacing_count;
	#define PRINT_PARSE char func[MAX_LEN];\
		strcpy(func,__FUNCTION__);\
		spacing_count = 0;\
		while (spacing_count < spacing){\
			spacing_count = spacing_count + 1;\
			printf(" ");\
		}\
		printf("enter: %s\n",__FUNCTION__);\
		spacing = spacing + 2;
	#define END_PARSE_PRINT end_parse_print(func)
	#define PRINT_PARSE_NAME spacing_count = 0;\
	while (spacing_count < spacing){\
		spacing_count = spacing_count + 1;\
		printf(" ");\
	}
	/*
	 * ends the parse print
	 */
	int end_parse_print(char* func){
		spacing = spacing - 2;
		spacing_count = 0;
		while (spacing_count < spacing){
			spacing_count = spacing_count + 1;
			printf(" ");
		}
		printf("exit: %s\n",func);
		return FALSE;
	}
#else
	#define PRINT_PARSE
	#define END_PARSE_PRINT FALSE
	#define PRINT_PARSE_NAME
#endif

#ifdef PARSER_PRINTING
	#define print(x, ...) printf(x, ## __VA_ARGS__)
#else
	#define print(x, ...)
#endif


/*
 * The main function of parser.c
 * This function sets us up for parsing
 * and then parses the file
 * an error is returned when there's a problem
 */
int parse(){
	set_count();
	//read the first token
	token = getToken();
	//was a token read?
	if (token != NULL){
		//we need priorities
		need_priority = TRUE;
		#ifdef DEBUG_PARSER
			spacing = 0;
		#endif
		//parse the file
		return Program();
	}else{
		//there was a error
		return FALSE;
	}
}

/*
 * This function is called by the function to tell the parser
 * what token is expected to be read. This function then returns
 * according to what token was found. If a prioritized expect
 * isn't satisfied we must save what was expected and what was found.
 * Because this means that there is a parsing error.
 */ 
int expect(enum symtype sym_type,int priority){
	
	//check if the token expected is this one
	if (token->sym_type == sym_type){
		//if it is then get the next token and return True
		token = getToken();		
		return TRUE;
	}else{
		//it wasn't. so return false
		//if a prioritized expect wasn't satisfied go into the if
		if (priority && need_priority){
			//what was the last expected symbol?
			sprintf(last_expected,"%s",set_error(sym_type));
			//what was the symbol that was found?
			sprintf(last_found,"%s",set_error(token->sym_type));
			//call the error handler
			ERR_HANDLER(5);
			//we don't need to handle a priority case again
			need_priority = FALSE;
		}
		return FALSE;
	}
}

/*
 * Returns what was expected for the last error
 */
char* expect_err(){
	return last_expected;
}

/*
 * Return what was found for the last error
 */
char* found(){
	return last_found;
}

void closeItem(cg_item *x,int clear)
{
	if ((x->mode == reg) && (clear))
	{
		clearReg(x->r);
	}
	free(x);
}

void clear_list()
{
	varlist* temp;	
	while (list != NULL)
	{
		temp = list;
		list = list->next;
		closeItem(temp->x,TRUE);
		free(temp);
	}
}

cg_item* newItem()
{
	cg_item *x;
	x = malloc(sizeof(struct cg_item));
	x->type = no_type;
	x->a = 0;
	x->c = 0;
	x->mode = nomode;
	x->baseAddr = 0;
	x->isArray = FALSE;
	x->varIndex = FALSE;
	return x;
}


/* ====================================================================	*
 * From here every function is a production rule as defined in the EBNF	*
 * If a rule isn't satisfied false is returned, else true		*
 * When expect is called with high priority then that symbol must	*
 * be part of the rule. This corresponds with the follow sets		*
 * when the first and follow sets of the EBNF is computed		*
 * ====================================================================	*/


/* 
 * The main calling function of the parser.
 * All programs that we parse must be parsed from here
 * The parser will descend recursively in this function
 * We also check if the end of the file was found after the last fullstop
 * Production rule: Program = Block "."
 */
int Program(){
	PRINT_PARSE
	level = 0;
	list = NULL;
	sprintf(proc,"_main");
	print("Defining default constants\n");
	procedure_node * temp = lookup_proc(proc,FALSE);
	temp->level = 1;
	insert_var(proc,"true",c_bool,1);
	insert_var(proc,"false",c_bool,0);
	return (Block() && expect(sym_stop,1) && expect(sym_eof,1)) | END_PARSE_PRINT ;
}

/*
 * Every procedure and the main program always contains this
 * block. Everything in a block happens between a begin
 * and an end.
 * Production rule: Block = "begin" DefinitionPart StatementPart "end"
 */
int Block(){
	PRINT_PARSE
	
	if (expect(sym_begin,1)){
		level = level + 1;
		if (DefinitionPart())
		{
			procedure_node * temp = lookup_proc(proc,FALSE);
			int disp = temp->disp;
			print("final displacement for %s is %d\n",proc,disp);
			if (strcmp("_main",proc) != 0)
			{
				temp->pc = procEnter(-disp);
				printf("starts at %lu\n",temp->pc);
			}else
			{
				start(-disp);
			}
			if  (StatementPart())
			{
				level = level - 1;
				//printf("*****************************************************\n");
				//printSymTable();
				
				
				//printf("=====================================================\n");
				//printSymTable();
				if (strcmp("_main",proc) != 0)
				{
					print("\tPROC %s disp %d\n",proc,-disp);
					procReturn(-disp);
				}else
				{
					print("proc %s\n",proc);
					Close();
					//Put(F2_RET,0,0,0);
				}
				remove_proc(proc);
				return (expect(sym_end,1)) | END_PARSE_PRINT;
			}else
			{
				return END_PARSE_PRINT;
			}
			
		}else{
			return END_PARSE_PRINT;
		}
	}else{
		return END_PARSE_PRINT;
	}
}

/*
 * All definitions in a program is handled by this
 * production rule. Because there's curl brackets in the 
 * rule this means there may by zero or more definitions.
 * Production rule: DefinitionPart = {Definition ";"}
 */
int DefinitionPart(){
	PRINT_PARSE
	if (Definition()){		
		if (expect(sym_semicolon,1)){
			//a complete definition
			//now look if there's a next one
			return DefinitionPart() | END_PARSE_PRINT ;
		}else{
			//invalid definition
			return END_PARSE_PRINT ;
		}
	}else{
		//no definition
		return TRUE | END_PARSE_PRINT ;
	}	
}

/*
 * Checks which of the 3 types of definitions we have
 * Production rule: Definition = ConstantDefinition | VariableDefinition | ProcedureDefinition
 */
int Definition(){
	PRINT_PARSE
	return (ConstantDefinition() || VariableDefinition() || ProcedureDefinition()) | END_PARSE_PRINT ;
}
/*
 * A constant is being defined
 * production rule: ConstantDefinition = "const" ConstantName "=" Constant
 */
int ConstantDefinition(){
	PRINT_PARSE
	if (expect(sym_const,0))
	{
		
		if (ConstantName(NULL,1) && expect(sym_equal,1))
		{
			char name[MAX_LEN];
			cg_item *x = newItem();
			sprintf(name,"%s",const_name);
				
			if (Constant(x,FALSE)){
				
				print("in level %d, procedure %s want to define constant %s to %d\n",level,proc,def_name,x->val);
				if (insert_var(proc,name,type,val) == FALSE)
				{
					ERR_HANDLER(6);
				}
				closeItem(x,TRUE);
				return TRUE | END_PARSE_PRINT;
			}else{
				return END_PARSE_PRINT ;
			}
		}else{
			return END_PARSE_PRINT;
		}
	}else{
		return FALSE;
	}
}

/*
 * A Variable is being declared
 * production rule: VariableDefinition = TypeSymbol VariableList | 
 * 					TypeSymbol "array" VariableList "[" Constant "]"
 */
int VariableDefinition(){
	PRINT_PARSE
	if (TypeSymbol()){
		if (VariableList()){
			return TRUE | END_PARSE_PRINT ;
		}else{
			
			cg_item *x = newItem();
			int temp;
			temp = FALSE;
			if ((expect(sym_array,1) && VariableName(x,1) && expect(sym_leftblock,1) && Constant(x,&temp) && expect(sym_rightblock,1))){
				print("\tdefine array %s length %d\n",def_name,val);
					int ret = FALSE;
					if (sym_type == 9)
					{
						ret = insert_var(proc,def_name,a_int,val);
					}else if (sym_type == 2)
					{
						ret = insert_var(proc,def_name,a_bool,val);
					}
				if (ret == FALSE)
				{
					ERR_HANDLER(7);
				}
				if (val <= 0)
				{
					ERR_HANDLER(8);
				}
				closeItem(x,TRUE);
				return TRUE | END_PARSE_PRINT;
			}else{
				closeItem(x,TRUE);
				return END_PARSE_PRINT;
			}
		}
	}else{
		return END_PARSE_PRINT ;
	}
}

/* 
 * We expect a type of symbol
 * integer or boolean
 * production rule: TypeSymbol = "integer" | "boolean"
 */
int TypeSymbol(){
	PRINT_PARSE
	sym_type = token->sym_type;
	return (expect(sym_int,0) || expect(sym_bool,0)) | END_PARSE_PRINT ;
}
/*
 * A list of one or more variables is expected
 * Variables are seperated by commas
 * production rule: VariableName {, VariableName}
 */
int VariableList(){
	PRINT_PARSE
	
	if (VariableName(NULL,0)){
		
		int ret = FALSE;
		if (sym_type == 9)
		{
			ret = insert_var(proc,def_name,t_int,0);
		}else if (sym_type == 2)
		{
			ret = insert_var(proc,def_name,t_bool,0);
		}
		if (ret == FALSE)
		{
			ERR_HANDLER(9);
		}
		print("in level %d, procedure %s want to define variable %s of type %d (int is 9 and bool 2)\n",level,proc,def_name,sym_type);
		//while a comma is found look for the next variable
		while (expect(sym_comma,0)){
			return VariableList() | END_PARSE_PRINT ;
		}
		//we parsed the whole list
		return TRUE | END_PARSE_PRINT ;		
	}else{
	
		return END_PARSE_PRINT ;
	}
}

/*
 * This is the definition of a procedure
 * production rule: ProcedureDefinition = "proc" ProcedureName Block
 */
int ProcedureDefinition(){
	PRINT_PARSE
	
	if (expect(sym_proc,0) && ProcedureName(NULL,1)){
	
		sprintf(proc,"%s",def_name);
		if (insert_proc(def_name,level+1) == FALSE)
		{
			ERR_HANDLER(10);
		}
		
		print("defining procedure %s at level %d\n",def_name,level);
		
	    return FormalParameters() && Block() | END_PARSE_PRINT ;
	}else{
	
		return END_PARSE_PRINT;
	}
}

int FormalParameters()
{
	PRINT_PARSE
	if (expect(sym_leftbracket,0))
	{
		if ( FPSection())
		{
			while (expect(sym_semicolon,0))
			{
				if (!(FPSection()))
				{
					ERR_HANDLER(30);
				}
			}
		}
		return (expect(sym_rightbracket,1)) | END_PARSE_PRINT;
	}else
	{
		return TRUE | END_PARSE_PRINT;
	}
	
}

int FPSection()
{
	PRINT_PARSE
	if (TypeSymbol())
	{
		int isValue;
		isValue = FALSE;
		if (expect(sym_value,0))
		{
			isValue = TRUE;
		}
		print("Declaring a parameter\n");
		int ret;
		if (!(VariableName(NULL,1)))
		{
			return END_PARSE_PRINT;
		}
		if (sym_type == 9)
		{
			ret = insert_param(proc,def_name,t_int,0,level+1,isValue);
		}else if (sym_type == 2)
		{
			ret = insert_param(proc,def_name,t_bool,0,level+1,isValue);
		}
		if (ret == FALSE)
		{
			ERR_HANDLER(7);
		}
		return TRUE | END_PARSE_PRINT;
	}
	return END_PARSE_PRINT;
}

/* This is a block of zero or more statements separated by semi-colons
 * production rule: StatementPart = {Statement ";"}
 */ 
int StatementPart(){
	PRINT_PARSE
	if (Statement()){
		//is the statement closed by a semi-colon?
		if (expect(sym_semicolon,1)){
			//do the next Statement
			return StatementPart() | END_PARSE_PRINT ;
		}else{
			return END_PARSE_PRINT ;
		}
	}else{
		return TRUE | END_PARSE_PRINT ;
	}
}

/* A statement
 * A statement can be one of the 7 options in the production rule
 * production rule: Statement =  EmptyStatement || 
				ReadStatement || 
				WriteStatement || 
				AssignmentStatement || 
				ProcedureStatement || 
				IfStatement || 
				DoStatement
 */
int Statement(){
	PRINT_PARSE
 	int goback = FALSE;
	return (ProcedureStatement(&goback) || 
		EmptyStatement() || 
		ReadStatement() || 
		WriteStatement() || 
		AssignmentStatement(goback) || 
		IfStatement() || 
		DoStatement()) | END_PARSE_PRINT ;
}
/* An empty statement
 * production rule: EmptyStatement = "skip"
 */
int EmptyStatement(){
	PRINT_PARSE
	return expect(sym_skip,0) | END_PARSE_PRINT ;
}
/* A read statement
 * production rule: ReadStatement = "read" VariableAccessList
 */ 
int ReadStatement(){
	PRINT_PARSE
	clear_list();
	if (expect(sym_read,0) && VariableAccessList(FALSE))
	{
		varlist *temp;
		temp = list;
		while (temp != NULL)
		{
			if (temp->x->isArray)
			{
				printf("is array\n");
				printItem(temp->x);
				if (temp->x->varIndex)
				{
					printf ("WHAT TO DO\n");
					//Put(
				}
				//Put(F1_ADD,temp->x->r,temp->x->baseAddr,temp->x->r);
				//Put(F3_WRD,0,0,temp->x->r);
				int r = getReg();
				Put(F1_RD,r,temp->x->r,0);
				clearReg(r);
				temp->x->isArray = FALSE;
				clearReg(temp->x->r);
				//free(temp->x);
			}else
			{
				op1(F1_RD,temp->x);
			}
			//closeItem(temp->x,FALSE);
			temp = temp->next;
		}
		return TRUE | END_PARSE_PRINT ;
	}
	return END_PARSE_PRINT ;
}

/* A list of variables that needs to be accessed
 * it must contain one or more variables separated by commas
 * production rule: VariableAccessList = VariableAccess { "," VariableAccess }
 */ 
int VariableAccessList(int goback){
	PRINT_PARSE
	cg_item *x = newItem();
	
	if (VariableAccess(x,goback)){
		varlist* temp;
		temp = malloc(sizeof(struct varlist));
		temp->x = malloc(sizeof(struct cg_item));
		memcpy(temp->x,x,sizeof(struct cg_item));
		//temp->x = x;
		temp->next = NULL;
		if (list == NULL)
		{
			list = temp;
		}else
		{
			varlist* cur;
			cur = list;
			while (cur->next != NULL)
			{
				cur = cur->next;
			}
			cur->next = temp;
		}
		x->mode = nomode;
		
		closeItem(x,FALSE);
		//is there another variableaccess?
		if (expect(sym_comma,0)){
			return VariableAccessList(FALSE) | END_PARSE_PRINT ;
		}else{
			return TRUE | END_PARSE_PRINT ;
		}
	}else{
		return END_PARSE_PRINT ;
	}
}

/* A write statement
 * production rule: WriteStatement = "write" ExpressionList
 */
int WriteStatement(){
	PRINT_PARSE
	return (expect(sym_write,0) && ExpressionList(TRUE)) | END_PARSE_PRINT ;
}

/* A list of one or more expressions separated by commas
 * production rule: ExpressionList = Expression {"," Expression }
 */
int ExpressionList(int nolist){
	PRINT_PARSE
	cg_item *x = newItem();
	if (Expression(x)){
		print("value = %d\n",value);
		if (!nolist)
		{
		
			if (list == NULL)
			{
				ERR_HANDLER(16);
			}else if (list->x->type == x->type)
			{
				varlist* temp;
				temp = list;
				list = list->next;
				print("Stoor die antwoord\n");
				//printItems(temp->x,x);
				if (temp->x->mode == var)
				{
					if (temp->x->level == level)
					{
						temp->x->baseAddr = resetLevel();
					}else
					{
						temp->x->baseAddr = rootLevel();
						//temp->x->baseAddr = changeLevel();
					}
				}
				if (x->mode == var)
				{
					if (x->level == level)
					{
						x->baseAddr = resetLevel();
					}else
					{
						x->baseAddr = rootLevel();
						//x->baseAddr = changeLevel();
					}
				}

				print("\t\t\t befire here %d\n",x->isArray);
				op2(F1_STW,temp->x,x);
				closeItem(temp->x,TRUE);
				free(temp);
			}else
			{
				ERR_HANDLER(17);
			}
		}else
		{
			//printf("Mode = %d var level %d, func level %d",x->mode,x->level,level);
			if (x->mode == var)
			{
				printf("Mode = %d var level %d, func level %d\n",x->mode,x->level,level);
				if (x->level == level)
				{
					x->baseAddr = resetLevel();
				}else
				{
					x->baseAddr = rootLevel();
					//x->baseAddr = changeLevel();
					
				}
			}
			print("type = %d\n",x->type);
			if (x->isArray)
			{	print("\twant to write array\n");
 				Put(F1_LDW,x->r,x->r,0);
			}
			op1(F3_WRD,x);
			closeItem(x,TRUE);
			clear_list();
		}
		//is there another expression?
		if (expect(sym_comma,0)){
			return ExpressionList(nolist) | END_PARSE_PRINT ;
		}else{
			if (list != NULL)
			{
				ERR_HANDLER(18);
			}
			return TRUE | END_PARSE_PRINT ;
		}
	}else{
		return END_PARSE_PRINT ;
	}	
}

/* An assignment
 * production rule: AssignmentStatement = VariableAccessList ":=" ExpressionList
 */
int AssignmentStatement(int goback){
	PRINT_PARSE
	clear_list();
	return (VariableAccessList(goback) && expect(sym_assign,1) && ExpressionList(FALSE)) | END_PARSE_PRINT ;
}

/* A procedure that is called
 * production rule: ProcedureStatement = "call" ProcedureName
 */
int ProcedureStatement(int *goback){
	PRINT_PARSE
	procedure_node *temp = NULL;
	if (ProcedureName(NULL,0)){
		temp = lookup_proc(def_name,TRUE);
		if ( temp == NULL)
		{
			*goback = TRUE;
			return END_PARSE_PRINT;
			//ERR_HANDLER(11);
		}
		call_node *call;
		call = (call_node*)temp;

		printf("\t\tSuccessful call to %s\n",def_name);
		sprintf(call_name,"%s",def_name);
		if (ActualParameters())
		{
			pushParams(params);
			procCall(call);
			popParams(params);
			params = NULL;
			return TRUE | END_PARSE_PRINT;
		}
		return END_PARSE_PRINT;			
	}else{
		return END_PARSE_PRINT;
	}
}

var_node* newParam(cg_item *x)
{
	var_node *params = malloc(sizeof (struct var_node));
	params->next = NULL;
	params->prev = NULL;
	params->type = x->type;
	params->value = x->c;
	
	params->x = malloc(sizeof (struct cg_item));
	memcpy(params->x,x,sizeof (struct cg_item));
	return params;
}

int ActualParameters()
{
	PRINT_PARSE
	var_node *temp;
	params = NULL;
	if (expect(sym_leftbracket,0))
	{
		cg_item *x = newItem();
		
		if (Expression(x))
		{
			params = newParam(x);
			temp = params;
			print("expr type %d, val %d\n",x->type,x->val);
			while (expect(sym_comma,0))
			{
				if (!(Expression(x)))
				{
					ERR_HANDLER(31);
				}
				//load(x);
				temp->next = newParam(x);
				temp = temp->next;
				print("expr type %d, val %d\n",x->type,x->val);
				
			}
		}
		checkParams(call_name,params);
		return expect(sym_rightbracket,1) | END_PARSE_PRINT;
	}
	checkParams(call_name,params);
	return TRUE | END_PARSE_PRINT;
}

/* An if statement
 * production rule: IfStatement = "if" GuardedCommandList "fi"
 */
int IfStatement(){
	PRINT_PARSE
	return (expect(sym_if,0) && GuardedCommandList() && expect(sym_fi,1)) | END_PARSE_PRINT ;
}

/* A do statement
 * production rule: DoStatement = "do" GuardedCommandList "od"
 */
int DoStatement(){
	PRINT_PARSE
	return (expect(sym_do,0) && GuardedCommandList() && expect(sym_od,1)) | END_PARSE_PRINT ;
}
/* a list of one or more guarded commands
 * refer to the next function's comments
 * to see what a guarded command is
 * production rule: GuardedCommandList = GuardedCommand { "[]" GuardedCommand }
 */
int GuardedCommandList(){
	PRINT_PARSE
	if (GuardedCommand()){
		//does another guarded command follow?
		if (expect(sym_guard,0)){
			return GuardedCommandList() || END_PARSE_PRINT ;
		}else{
			return TRUE || END_PARSE_PRINT ;
		}
	}else{
		return FALSE || END_PARSE_PRINT ;
	}	
}

/* a guarded command
 * This is a boolean expression followed by a statement to be executed
 * if true
 * production rule: GuardedCommand = Expression "->" StatementPart
 */
int GuardedCommand(){
	PRINT_PARSE
	cg_item *x = newItem();
	if (Expression(x))
	{
		if (x->type != t_bool)
		{
			ERR_HANDLER(27);
		}
		return (expect(sym_arrow,1) && StatementPart()) | END_PARSE_PRINT ;
	}
	return END_PARSE_PRINT;	 
}

/* A boolean expression
 * production rule: Expression = PrimaryExpression {PrimaryOperator PrimaryExpression}
 */
int Expression(cg_item *x){
	PRINT_PARSE
	
	if (PrimaryExpression(x)){
		cg_item *y = malloc(sizeof(struct cg_item));
		memcpy(y,x,sizeof(struct cg_item));
		//enum types temp = x->type;
		while (PrimaryOperator()){
			if (PrimaryExpression(y))
			{
				if ((y->type == t_int) || (x->type == t_int))
				{
					ERR_HANDLER(26);
				}
				if (y->type != x->type)
				{
					ERR_HANDLER(25);
				}
				op2(op,x,y);
				x->type = t_bool;
				//return TRUE | END_PARSE_PRINT ;
			}else
			{
				return END_PARSE_PRINT;
			}
		}
		return TRUE | END_PARSE_PRINT ;
	}else{
		return END_PARSE_PRINT ;
	}
}

/* Primary Operators
 * it may be an AND or an OR
 * production rule: PrimaryOperator = "&" | "|"
 */
int PrimaryOperator(){
	PRINT_PARSE
	if (expect(sym_and,0))
	{
		op = F1_AND;
		return TRUE | END_PARSE_PRINT;
	}
	if (expect(sym_or,0))
	{
		op = F1_OR;
		return TRUE | END_PARSE_PRINT;
	}
	return  END_PARSE_PRINT ;
}

/* Primary expressions
 * may contain "RelationalOperator SimpleExpression" zero or one times
 * production rule: PrimaryExpression = SimpleExpression [RelationalOperator SimpleExpression]
 */
int PrimaryExpression(cg_item *x){
	PRINT_PARSE
	
	if (SimpleExpression(x)){
	
		if (RelationalOperator()){
			
			cg_item *y = malloc(sizeof(struct cg_item));
			memcpy(y,x,sizeof(struct cg_item));
			if (SimpleExpression(y))
			{
				if (y->type != x->type)
				{
					ERR_HANDLER(24);
					return END_PARSE_PRINT;
				}else
				{
					x->type = t_bool;
					op2(F1_CMP,x,y);
					op1(op,x);
					return TRUE  | END_PARSE_PRINT ;
				}
			}
			return END_PARSE_PRINT ;
		}else{
			return TRUE | END_PARSE_PRINT ;
		}
	}else{
		return END_PARSE_PRINT ;
	}	
}

/* Relational Operators
 * can be "<", "=" or ">"
 * production rule: RelationalOperator = "<" | "=" | ">"
 */
int RelationalOperator(){
	PRINT_PARSE
	if (expect(sym_less,0))
	{
		op = F1_BLT;
		return TRUE | END_PARSE_PRINT;
	}
	if (expect(sym_equal,0))
	{
		op = F1_BEQ;
		return TRUE | END_PARSE_PRINT;

	}
	if (expect(sym_greater,0))
	{
		op = F1_BGT;
		return TRUE | END_PARSE_PRINT;

	}
	return END_PARSE_PRINT ;
}

/* A simple expression
 * Can be one or more terms added/subtracted
 * from each other. The first term can contain a unary minus
 * production rule: SimpleExpression = ["-"] Term {AddingOperator Term}
 */
int SimpleExpression(cg_item *x){
	PRINT_PARSE
	//Does the term have a minus in front
	if (expect(sym_minus,0))
	{
		if (Term(x))
		{
			if (x->type != t_int)
			{
				ERR_HANDLER(20);
			}
			/*if (x->isInt == FALSE)
			{
				if (x->isArray)
				{
					printf("want to unary minus array");
					Put(F1_LDW,x->r,x->r,0);
				}
				
			}else
			{
				x->c = -x->c;
			}*/
			if (x->isArray)
			{
					//printf("want to unary minus array\n");
				//	Put(F1_LDW,x->r,x->r,0);
			}
			op1(F3_UNARY,x);
		}else
		{printf("ERROR ?\n");
		////////////////////////////////////////////////////////////////
			return END_PARSE_PRINT ;
		}
	}else
	{
		if (!(Term(x)))
		{
			return END_PARSE_PRINT ;
		}
	}	
	cg_item *y = malloc(sizeof (struct cg_item));
	memcpy(y,x,sizeof (struct cg_item));
	y->isArray = FALSE;
	
	
	int add;
	
	while (AddingOperator(&add))
	{
		print("\t\t\t\t\tAdd\n");
		if (!(Term(y)))
		{
			return END_PARSE_PRINT ;
		}
		print("\t\t\tis Array %d\n",y->isArray);
		if ((x->type != t_int) || (y->type != t_int))
		{
			ERR_HANDLER(21);
		}
		//printItems(x,y);
		if ((x->isInt == FALSE) || (y->isInt == FALSE))
		{
			if ((x->mode != cons) && (y->mode != cons))
			{print("isInt = %d, mode = %d\n",y->isInt,y->mode);
				if (add)
				{
					print("voor x = %d, y = %d\n",x->mode,y->mode);
					printReg();
					op2(F1_ADD,x,y);
					print("na x = %d, y = %d\n",x->r,y->r);
					printReg();
				}else
				{
					
					op2(F1_SUB,x,y);
				}
			
			}else
			{
				if (add)
				{
					//op2(F2_ADDI,x,y);
					print("isInt = %d, mode = %d\n",x->isInt,x->mode);
					if (x->mode != cons)
					{//printItems(x,y);
						op2(F2_ADDI,x,y);
					}else
					{
						op2(F2_ADDI,y,x);
						memcpy(x,y,sizeof(struct cg_item));
						//op1(F3_UNARY,x);
					}
				}else
				{
					if (x->mode != cons)
					{
						op2(F2_SUBI,x,y);
					}else
					{
					
						if (x->isArray)
						{
							print("want to  minus array\n");
						}
					
						op2(F2_SUBI,y,x);
						memcpy(x,y,sizeof(struct cg_item));
						op1(F3_UNARY,x);
					}
				}
			}
			
			print("op:= %d; Op2(op,%ld,%ld)\n",add,x->a,y->a);
			
			//printItems(x,y);
			x->isInt = FALSE;
			y->isInt = FALSE;
			y->c = 0;
		}else
		{
			if (add)
			{
				x->c = y->c + x->c;
			}else
			{
				x->c = x->c - y->c;
			}
		}
	}
	y->c = x->c;
	return TRUE | END_PARSE_PRINT ;

	

		
}

/* The adding operators, + and -
 * production rule: AddingOperator = "+" or "-"
 */
int AddingOperator(int * add){
	PRINT_PARSE
	
	if (expect(sym_plus,0))
	{
		*add = TRUE;
		return TRUE | END_PARSE_PRINT;
	}
	if (expect(sym_minus,0))
	{
		*add = FALSE;
		return TRUE | END_PARSE_PRINT;
	}
	return END_PARSE_PRINT ;
}

/* A term
 * This is factors that may be separated by multiplying operators
 * production rule: Term = Factor {MultiplyingOperator Term}
 */
int Term(cg_item *x){
	PRINT_PARSE
	
	if (Factor(x))
	{
		cg_item *y = malloc(sizeof (struct cg_item));
		memcpy(y,x,sizeof (struct cg_item));
		//enum types type = x->type;
		//int temp_val = x->c;
		int mult;
		print("\t\t\tis Array %d\n",x->isArray);
		//int inttemp = x->isInt;
		while (MultiplyingOperator(&mult))
		{
			if (Factor(y))
			{
				if ((x->type != t_int) || (y->type != t_int))
				{
					ERR_HANDLER(22);
				}
				if ((x->isInt == FALSE) || (y->isInt == FALSE))
				{
					print("op:=mult %d; op2(op,%ld,%ld);\n",mult,x->a,y->a);
					//printItems(x,y);
					
					if ((x->mode != cons) && (y->mode != cons))
					{
						switch (mult)
						{
							case 0:
								print("voor x = %d, y = %d\n",x->mode,y->mode);
								printReg();
								op2(F1_MUL,x,y);
								printReg();
								print("na x = %d, y = %d\n",x->mode,y->mode);
								break;
							case 1:
								op2(F1_DIV,x,y);
								break;
							case 2:
								op2(F1_MOD,x,y);
								break;
							
						}
					
					}else
					{
						if (x->mode != cons)
						{
							switch (mult)
							{
								case 0:
								print("voor x = %d r=%d, y = %d\n",x->mode,x->r,y->mode);
								printReg();
									op2(F2_MULI,x,y);
									print("voor x = %d, y = %d\n",x->mode,y->mode);
								printReg();
									break;
								case 1:
									op2(F2_DIVI,x,y);
									break;
								case 2:
									op2(F2_MODI,x,y);
									break;
							}
						}else
						{
							/*y->r = getReg();
							y->mode = reg;
							Put(F1_LDW,y->r,29,y->a);*/
							switch (mult)
							{
								
								case 0:
								print("voor x = %d, y = %d\n",x->mode,y->mode);
								
									op2(F1_MUL,x,y);
									break;
								case 1:
									op2(F1_DIV,x,y);
									break;
								case 2:
									op2(F1_MOD,x,y);
									break;
							}
							//memcpy(x,y,sizeof(struct cg_item));
						}
						/*if (add)
						{
							op2(F2_ADDI,x,y);
						}else
						{
							if (x->isInt == FALSE)
							{
								op2(F2_SUBI,x,y);
							}else
							{
								op2(F2_SUBI,x,y);
								op1(F3_UNARY,x);
							}
						}*/
					}
					
					x->isInt = FALSE;
					y->isInt = FALSE;
					x->c = 0;
				}else
				{
					if (mult == 0)
					{
						print("Multiplying\n");
						x->c = y->c * x->c;
					}
					//if (*value
					if (mult == 1)
					{
						print("Division\n");
						if (y->c == 0)
						{
							ERR_HANDLER(29);
						}
						x->c = x->c / y->c;
					}
					if (mult == 2)
					{
						print("Modulus\n");
						if (x->c == 0)
						{
							ERR_HANDLER(29);
						}
						x->c = x->c % y->c;
					}
				}
				y->c = x->c;
				//return TRUE | END_PARSE_PRINT ;
			}else
			{
				return END_PARSE_PRINT ;
			}
		}//else{
			return TRUE | END_PARSE_PRINT ;
		//}
	}else{
		return END_PARSE_PRINT ;
	}
}

/* The multiplying operators
 * production rule: MultiplyingOperators = "*" | "/" | "\"
 */
int MultiplyingOperator(int * mult){
	PRINT_PARSE	
	if (expect(sym_mult,0))
	{
		*mult = 0;
		return TRUE | END_PARSE_PRINT;
	} 
	if (expect(sym_div,0))
	{
		*mult = 1;
		return TRUE | END_PARSE_PRINT;
	}
	if (expect(sym_mod,0))
	{
		*mult = 2;
		return TRUE | END_PARSE_PRINT;
	}
		 
	return END_PARSE_PRINT ;
}

/*A factor
 * A factor can be a constant, a variable, a expression or not a factor
 * production rule: Factor = Constant | VariableAccess | "(" Expression ")" | "~" Factor
 */
int Factor(cg_item *x){
	PRINT_PARSE
	int goback = TRUE;
	if (Constant(x,&goback) || VariableAccess(x,goback) ||
		(expect(sym_leftbracket,0) && Expression(x) && expect(sym_rightbracket,1)))
	{
		return TRUE | END_PARSE_PRINT;
	}
	if ((expect(sym_not,0) && Factor(x)))
	{
		if (x->type != t_bool)
		{
			ERR_HANDLER(23);
		}
		op1(F3_NOT,x);
		return TRUE | END_PARSE_PRINT;
	}
	return END_PARSE_PRINT ;
}

/* VariableAccess
 * access to a variable
 * production rule: VariableAccess = VariableName [IndexedSelector]
 */
int VariableAccess(cg_item *x, int goback){
	PRINT_PARSE	
	if ((goback) || (VariableName(x,0))){
		var_node* temp = lookup_var(proc,def_name);
		print("Lookup needed for %s in procedure %s at level %d, val %d type %d\n",def_name,proc,level,temp->value,temp->type);
		if (temp == NULL)
		{
			ERR_HANDLER(12);
			return END_PARSE_PRINT;
		}else
		{
			x->c = 0;
			x->a = temp->disp;
			type = temp->type;
			x->mode = var;
			if (temp->value != 0)
			{
				if ((temp->type == c_bool) || (temp->type == c_int))
				{
					//expr_type = temp->type;
					ERR_HANDLER(15);
					return TRUE | END_PARSE_PRINT;
				}else
				{
					if (temp->level == level)
					{
						x->baseAddr = resetLevel();
					}else
					{
						x->baseAddr = rootLevel();
						//x->baseAddr = changeLevel();
						
					}
					x->level = temp->level;
					return IndexedSelector(x) | END_PARSE_PRINT;
				}
			}else
			{
				x->type = type;
				//load(x);
				//printf("level var %d, level func %d\n",temp->level,level);
				if (temp->level == level)
				{
					x->baseAddr = resetLevel();
				}else
				{
					x->baseAddr = rootLevel();
					//x->baseAddr = changeLevel();
					
				}
				x->level = temp->level;
				return TRUE | END_PARSE_PRINT;
			}
		}
		
		//return IndexedSelector() | END_PARSE_PRINT ;
	}else{
		return END_PARSE_PRINT ;
	}
}

/* Indexed selector
 * used to index an array
 * production rule: IndexedSelector = "[" Expression "]"
 */
int IndexedSelector(cg_item *x){
	PRINT_PARSE
	if (expect(sym_leftblock,1))
	{
		var_node* temp = lookup_var(proc,def_name);
		if ((temp == NULL) || ((temp->type != a_int) && (temp->type != a_bool)) || (temp->value == 0 ))
		{
			ERR_HANDLER(13);
		}
		print("Lookup into array needed in procedure %s at level %d\n",proc,level);
		cg_item *y = newItem();
		if (Expression(y) && expect(sym_rightblock,1))
		{
			if (y->type != t_int)
			{
				ERR_HANDLER(19);
			}
			if ((y->c < 0) || (y->c >= temp->value))
			{
				ERR_HANDLER(28);
			}
			x->isArray = TRUE;
			if (temp->type == a_int)
			{
				x->type = t_int;
			}else
			{
				x->type = t_bool;
			}
			if (x->mode == var)
			{
				x->varIndex = TRUE;
			}
			/*
			y is the index in the array
			z is the size of the array
			x is the address of the array
			*/
			
			//get size of array
			cg_item *z = newItem();
			z->mode = cons;
			z->c = temp->value;
			
			//load address of x
			cg_item *x2 = newItem();
			x2->mode = cons;
			x2->c = x->a;
			x2->baseAddr = x->baseAddr;
			
			//size of int/boolean
			cg_item *z2 = newItem();
			z2->mode = cons;
			z2->c = 4;
			
			cg_item *y2 = malloc(sizeof (struct cg_item));
			memcpy(y2,y,sizeof(struct cg_item));
			//y2->isArray = FALSE;
			
			//check bounds
			if (y2->isArray == FALSE)
			{
				load(y2);
			}else
			{print("alloc new regs\n");
			printItem(y2);
				y2->r = getReg();
				Put(F1_ADD,y2->r,0,y->r);
			}
			op2(F2_CHKI,y2,z);
			closeItem(y2,TRUE);
			
			
			if (y->isArray)
			{
				Put(F1_LDW,y->r,y->r,0);
				y->isArray = FALSE;
			}else{
				load(y);
				//y->isArray = TRUE;
			}
			
			
			
			load(x2);
			
			//Put(F1_ADD,x2->r,0,x2->r);
			
			
			//index * size
			op2(F2_MULI,y,z2);
			closeItem(z2,TRUE);
			
			//get index's offset
			Put(F1_ADD,x2->r,x2->r,y->r);
			
			//save into register the value at that index
			load(x);
			
			
			Put(F1_ADD,x->r,x2->r,x->baseAddr);
			//Put(F1_LDW,x->r,x->r,0);
		
			closeItem(x2,TRUE);
			closeItem(y,TRUE);
			closeItem(z,TRUE);
			x->mode = reg;
			return TRUE | END_PARSE_PRINT;
		}
		return END_PARSE_PRINT ;
	}else{
		return END_PARSE_PRINT;
	}
}

/* a constant
 * can be a number or a defined constant
 * production rule: Constant = Numeral | ConstantName
 */
int Constant(cg_item *x, int *goback)
{
	PRINT_PARSE	
	if (Numeral(x))
	{
		x->type = t_int;
		x->c = val;
		type = c_int;
		x->mode = cons;
		return TRUE | END_PARSE_PRINT; 
	}else if (ConstantName(x,0)) 
	{
		var_node* temp = lookup_var(proc,const_name);
		
		if ((temp == NULL) || ((temp->type != c_bool) && (temp->type != c_int)))
		{
			if (*goback == FALSE)
			{
				ERR_HANDLER(14);
			}
			print("%s not a constant goback = %d\n", const_name, *goback);
			sprintf(def_name,"%s",const_name);
			
			return END_PARSE_PRINT;
		}else
		{
			x->mode = cons;
			x->c = temp->value;
			x->a = temp->disp;
			if (temp->type == c_int)
			{
				type = c_int;
				x->type = t_int;
			}else
			{
				type = c_bool;
				x->type = t_bool;
			}
			
			val = temp->value;
			print("Constant %s defined as %d\n",const_name,val);
			return TRUE | END_PARSE_PRINT;
		}
	}
	*goback = FALSE;
	return END_PARSE_PRINT;
}

/* A number is expected
 * production rule: Numeral = Digit {Digit}
 */
int Numeral(cg_item *x){
	PRINT_PARSE
	val = token->val;
	
	if (expect(sym_number,0))
	{
		x->isInt = TRUE;
		x->mode = cons;
		return TRUE | END_PARSE_PRINT;
	}
	return END_PARSE_PRINT ;
}

/* A name is expected
 * production rule: Name = Letter {Letter | Digit}
 */
int Name(cg_item *x,int priority){
	PRINT_PARSE
	 
	if (expect(sym_id,priority)){
		if (x != NULL)
		{
			x->isInt = FALSE;
			x->mode = var;
		}
		return TRUE | END_PARSE_PRINT;
	}else{
		return	END_PARSE_PRINT;
	}
}

int ConstantName(cg_item *x, int priority){
	PRINT_PARSE
	sprintf(const_name,"%s",token->name);
	return (Name(x,priority)) | END_PARSE_PRINT ;
}
int VariableName(cg_item *x, int priority){
	PRINT_PARSE
	sprintf(def_name,"%s",token->name);
	return Name(x,priority) | END_PARSE_PRINT ;
}
int ProcedureName(cg_item *x, int priority){
	PRINT_PARSE
	sprintf(def_name,"%s",token->name);
	return Name(x,priority) | END_PARSE_PRINT ;
}

