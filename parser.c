// Jesse Johnson
// COP 3402, Fall 2021
// je548081

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 1000
#define MAX_SYMBOL_COUNT 100
#define DEBUG 0

instruction *code;
int cIndex = 0;
symbol *table;
int tIndex = 0;
int token = 0;
int level;
int errorCalled = 0;

// Used for debugging
int debugBlock = 0;
int debugConst = 0;
int debugMult = 0;
int debugVar = 0;
int debugProcedure = 0;
int debugStatement = 0;
int debugExpression = 0;
int debugTerm = 0;
int debugFacot = 0;
int debugCondition = 0;
int debugMark = 0;

void parseAndCodeGenerate(lexeme *list);
void PROGRAM(lexeme *list);
void BLOCK(lexeme *list);
void CONST_DECLARATION(lexeme *list);
int MULTIPLEDECLARATIONCHECK(char identifier[]); // Self made
int VAR_DECLARATION(lexeme *list);
void PROCEDURE_DECLARATION(lexeme *list);
void STATEMENT(lexeme *list);
int FINDSYMBOL(char name[], int symbolKind); // Self Made
void EXPRESSION(lexeme *list);
void TERM(lexeme *list);
void FACTOR(lexeme *list);
void CONDITION(lexeme *list);
void MARK(lexeme *list); // Self made but should still have debug var

void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();

instruction *parse(lexeme *list, int printTable, int printCode)
{
	if(DEBUG)
	{
		printf("Im in parse\n");
		fflush(stdin);
	}

	code = NULL;

	table = malloc(MAX_SYMBOL_COUNT * sizeof(*table));
	code = malloc(MAX_CODE_LENGTH * sizeof(*code));

	parseAndCodeGenerate(list);

	code[cIndex].opcode = -1;

	// code == NULL when error is found
	if(errorCalled == 1)
		return NULL;

	if(printTable == 1)
		printsymboltable();

	if(printCode == 1)
		printassemblycode();

	return code;
}

void parseAndCodeGenerate(lexeme *list)
{
	PROGRAM(list);

	return;
}

void PROGRAM(lexeme *list)
{
	int i;

	if(DEBUG)
	{
		printf("Im in program\n");
		fflush(stdout);
	}

	emit(7, 0, 0); // VM JMP
	addToSymbolTable(3, "main", 0, 0, 0, 0);
	level = -1;

	if(DEBUG)
		printf("%d. ***Block: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugBlock, list[token].type, list[token - 1].type);

	BLOCK(list);
	if(DEBUG)
	{
		printf("%d. ***Block: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugBlock, list[token].type, list[token - 1].type);
		fflush(stdout);
	}
	if(errorCalled == 1)
		return;

	if(list[token].type != 30) // Lex periodsym
	{
		printparseerror(1);
		errorCalled = 1;
		return;
	}

	if(DEBUG)
		printf("list[token].type = 30\n");

	emit(9, 0, 3); // VM Halt

	for (i  = 0; i < cIndex; i++)
	{
		if(code[i].opcode == 5) // Code Call
		{
			code[i].m = table[code[i].m].addr;
		}
	}

	code[0].m = table[0].addr;

	return;
}

void BLOCK(lexeme *list)
{
	int procedure_index;
	int x;

	level++;
	procedure_index = tIndex - 1;

	if(DEBUG)
		printf("%d. ***Const_declaration: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugConst, list[token].type, list[token - 1].type);


	CONST_DECLARATION(list);
	if(DEBUG)
	{
		printf("%d. ***Const_Declation: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugConst, list[token].type, list[token - 1].type);
		fflush(stdout);
	}
	if(errorCalled == 1)
		return;

	if(DEBUG)
		printf("%d. ***var_declaration: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugVar, list[token].type, list[token - 1].type);

	x = VAR_DECLARATION(list);
	if(DEBUG)
	{
		printf("%d. ***var_declaration: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugVar, list[token].type, list[token - 1].type);
		fflush(stdout);
	}
	if(errorCalled == 1)
		return;

	if(DEBUG)
		printf("%d. ***Procedure_declaration: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugProcedure, list[token].type, list[token - 1].type);


	PROCEDURE_DECLARATION(list);
	if(DEBUG)
	{
		printf("%d. ***Procedure_declaration: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugProcedure, list[token].type, list[token - 1].type);
		fflush(stdout);
	}
	if(errorCalled == 1)
		return;

	table[procedure_index].addr = cIndex * 3; // Changed 3 -> 4 ??

	if(level == 0)
		emit(6, 0, x); // code INC
	else
		emit(6, 0, x + 4); // code INC // Changed 3 -> 4

	if(DEBUG)
		printf("%d. ***statement: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugStatement, list[token].type, list[token - 1].type);

	STATEMENT(list);
	if(DEBUG)
	{
		printf("%d. ***statement: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugStatement, list[token].type, list[token - 1].type);
		fflush(stdout);
	}
	if(errorCalled == 1)
		return;

	if(DEBUG)
		printf("%d. ***Mark: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugMark, list[token].type, list[token - 1].type);


	MARK(list);
	if(DEBUG)
	{
		printf("%d. ***Mark: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugMark, list[token].type, list[token - 1].type);
		fflush(stdout);
	}
	if(errorCalled == 1)
		return;

	level--;


	return;
}

void CONST_DECLARATION(lexeme *list)
{
	int symidx;
	char savedIdentName[12];

	if(list[token].type == 1) // Lex constsym
	{
		do
		{
			token++;

			if(list[token].type != 12) // Lex identsym
			{
				printparseerror(2);
				errorCalled = 1;
				return;
			}

			symidx = MULTIPLEDECLARATIONCHECK(list[token].name);

			if(errorCalled == 1)
				return;

			if(symidx != -1)
			{
				printparseerror(19);
				errorCalled = 1;
				return;
			}

			// saves token name
			strcpy(savedIdentName, list[token].name);

			token++;

			if(list[token].type != 14) // Lex assignsym
			{
				printparseerror(2);
				errorCalled = 1;
				return;
			}

			token++;

			if(list[token].type != 13) // Lex numbersym
			{
				printparseerror(2);
				errorCalled = 1;
				return;
			}

			addToSymbolTable(1, savedIdentName, list[token].value, level, 0, 0);

			token++;
		} while (list[token].type == 29); // Lex commasym

		if(list[token].type != 31) // Lex semicolonsym
		{
			if(list[token].type == 12) // Lex identsym
			{
				printparseerror(13);
				errorCalled = 1;
				return;
			}
			else
			{
				if(DEBUG)
				{
					printf("In first 14 error\n");
					fflush(stdout);
				}
				printparseerror(14);
				errorCalled = 1;
				return;
			}
		}

		token++;
	}

	return;
}

int MULTIPLEDECLARATIONCHECK(char identifier[])
{
	int i;

	if(DEBUG)
	{
		printf("Im in multiple declarattion check\n");
		fflush(stdout);
	}

	for (i = 0; i < tIndex; i++)
	{
		if((strcmp(table[i].name, identifier) == 0) && table[i].mark == 0 && table[i].level == level)
		{
			if(DEBUG)
			{
				printf("Multiple declaration return i: %d\n", i);
				fflush(stdout);
			}

			return i;
		}
	}

	return -1;
}

int VAR_DECLARATION(lexeme *list)
{
	int numVars = 0;
	int symidx;

	if(DEBUG)
	{
		printf("Im in var_declaration\n");
		fflush(stdout);
	}

	if(list[token].type == 2) // Lex varsim
	{
		do
		{
			numVars++;
			token++;

			if(list[token].type != 12) // Lex identsym
			{
				printparseerror(3);
				errorCalled = 1;
				return -1;
			}

			symidx = MULTIPLEDECLARATIONCHECK(list[token].name);

			if(symidx != -1)
			{
				if(DEBUG)
				{
					printf("identifier is not in table\n");
					fflush(stdout);
				}

				printparseerror(18);
				errorCalled = 1;
				return -1;
			}

			if(level == 0)
			{
				addToSymbolTable(2, list[token].name, 0, level, numVars - 1, 0);
			}
			else
			{
				addToSymbolTable(2, list[token].name, 0, level, numVars + 3, 0);
			}

			token++;

		} while (list[token].type == 29); // Lex commasym

		if(list[token].type != 31) // Lex semicolonsym
		{
			if(DEBUG)
			{
				printf("list[token].type: %d\n", list[token].type);
				fflush(stdout);
			}

			if(list[token].type == 12) // Lex identsym
			{
				printparseerror(13);
				errorCalled = 1;
				return -1;
			}
			else
			{
				if(DEBUG)
				{
					printf("In second 14 error\n");
					fflush(stdout);
				}
				printparseerror(14);
				errorCalled = 1;
				return -1;
			}
		}

		token++;
	}

	return numVars;
}

void PROCEDURE_DECLARATION(lexeme *list)
{
	int symidx;


	if(DEBUG)
	{
		printf("Im in procedure declaration\n");
		fflush(stdout);
	}

	while(list[token].type == 3) // Lex procsym
	{
		token++;

		if(list[token].type != 12) // Lex identsym
		{
			printparseerror(4);
			errorCalled = 1;
			return;
		}

		symidx = MULTIPLEDECLARATIONCHECK(list[token].name);
		if(errorCalled == 1)
			return;

		if(symidx != -1)
		{
			printparseerror(18);
			errorCalled = 1;
			return;
		}

		addToSymbolTable(3, list[token].name, 0, level, 0, 0);

		token++;

		if(list[token].type != 31) // Lex semicolonsym
		{
			printparseerror(4);
			errorCalled = 1;
			return;
		}

		token++;

		if(DEBUG)
			printf("%d. ***Block: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugBlock, list[token].type, list[token - 1].type);

		BLOCK(list);
		if(DEBUG)
		{
			printf("%d. ***Block: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugBlock, list[token].type, list[token - 1].type);
			fflush(stdout);
		}
		if(errorCalled == 1)
			return;

		if(list[token].type != 31) // Lex semicolonsym
		{
			if(DEBUG)
			{
				printf("In third 14 error. list[token].type: %d, list[token - 1].type: %d\n", list[token].type, list[token - 1].type);
				fflush(stdout);
			}
			printparseerror(14);
			errorCalled = 1;
			return;
		}

		token++;
		emit(2, 0, 0); // VM RTN
	}

	return;
}

void STATEMENT(lexeme *list)
{
	int tempTokenType;
	int symIdx;
	int jpcIdx;
	int jmpIdx;
	int loopIdx;
	int L;
	int M;


	if(DEBUG)
	{
		printf("Im in statement list[token].type: %d, list[token - 1].type: %d\n", list[token].type, list[token - 1].type);
		fflush(stdout);
	}

	if(list[token].type == 12) // Lex identsym
	{
		symIdx = FINDSYMBOL(list[token].name, 2);

		//printf("symIdx: %d\n", symIdx);
		fflush(stdout);

		if(symIdx == -1)
		{
			if(FINDSYMBOL(list[token].name, 1) != FINDSYMBOL(list[token].name, 3))
			{
				printparseerror(6); // was 18
				errorCalled = 1;
				return;
			}
			else
			{
				printparseerror(19);
				errorCalled = 1;
				return;
			}
		}

		token++;

		if(list[token].type != 14) // Lex assignsym
		{
			printparseerror(5);
			errorCalled = 1;
			return;
		}

		token++;

		if(DEBUG)
			printf("%d. ***expression: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugExpression, list[token].type, list[token - 1].type);

		EXPRESSION(list);
		if(errorCalled == 1)
			return;

		if(DEBUG)
			printf("%d. ***expression: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugExpression, list[token].type, list[token - 1].type);


		//printf("symIdx: %d, name: %s\n", symIdx, table[symIdx].name);

		fflush(stdout);

		//printf("level: %d, table[symIdx].level: %d, table[symIdx].name: %s\n", level, table[symIdx].level, table[symIdx].name);

		L = level - table[symIdx].level;
		M = table[symIdx].addr;
		emit(4, L, M); // VM STO
		return;
	}

	if(list[token].type == 4) // Now Lex dosym
	{
		do
		{
			token++;

			if(DEBUG)
				printf("%d. ***statement: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugStatement, list[token].type, list[token - 1].type);

			STATEMENT(list);
			if(DEBUG)
			{
				printf("%d. ***statement: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugStatement, list[token].type, list[token - 1].type);
				fflush(stdout);
			}
			if(errorCalled == 1)
				return;

		} while (list[token].type == 31); // Lex semicolonsym

		if(list[token].type != 5) // Now Lex odsym
		{
			tempTokenType = list[token].type;
			if((tempTokenType == 12) || (tempTokenType == 4) || (tempTokenType == 7) || (tempTokenType == 6) || (tempTokenType == 11)
				|| (tempTokenType == 10) || (tempTokenType == 9)) // identsym, dosym, whensym, whilesym, readsym, writesym, callsym
			{
				printparseerror(15);
				errorCalled = 1;
				return;
			}
			else
			{
				printparseerror(16);
				errorCalled = 1;
				return;
			}
		}

		token++;
		return;
	}

	if(list[token].type == 7) // Lex whensym
	{
		token++;

		if(DEBUG)
			printf("%d. ***condition: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugCondition, list[token].type, list[token - 1].type);


		CONDITION(list);

		if(DEBUG)
			printf("%d. ***condition: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugCondition, list[token].type, list[token - 1].type);

		if(errorCalled == 1)
			return;

		jpcIdx = cIndex;

		emit(8, 0, 0);

		if(list[token].type != 4) // Lex dosym
		{
			printparseerror(8);
			errorCalled = 1;
			return;
		}

		token++;

		if(DEBUG)
			printf("%d. ***statement: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugStatement, list[token].type, list[token - 1].type);


		STATEMENT(list);
		if(DEBUG)
		{
			printf("%d. ***statement: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugStatement, list[token].type, list[token - 1].type);
			fflush(stdout);
		}
		if(errorCalled == 1)
			return;

		if(list[token].type == 8) // Lex elsedosym
		{
			jmpIdx = cIndex;
			emit(7, 0, 0); // VM JMP
			code[jpcIdx].m = cIndex * 3; // Changed 3 -> 4 ??

			token++;

			if(DEBUG)
				printf("%d. ***statement: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugStatement, list[token].type, list[token - 1].type);


			STATEMENT(list);
			if(DEBUG)
			{
				printf("%d. ***statement: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugStatement, list[token].type, list[token - 1].type);
				fflush(stdout);
			}
			if(errorCalled == 1)
				return;

			code[jmpIdx].m = cIndex * 3; // Changed 3 -> 4 ??
		}
		else
		{
			code[jpcIdx].m = cIndex * 3; // Changed 3 -> 4 ??
		}

		return;
	}

	if(list[token].type == 6) // Lex whilesym
	{
		token++;
		loopIdx = cIndex;

		if(DEBUG)
			printf("%d. ***condition: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugCondition, list[token].type, list[token - 1].type);


		CONDITION(list);

		if(DEBUG)
			printf("%d. ***condition: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugCondition, list[token].type, list[token - 1].type);

		if(errorCalled == 1)
			return;

		if(list[token].type != 4) // Lex dosym // MAY NEED FIXING
		{
			printparseerror(9);
			errorCalled = 1;
			return;
		}

		token++;
		jpcIdx = cIndex;
		emit(8, 0, 0); // VM JPC

		if(DEBUG)
			printf("%d. ***statement: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugStatement, list[token].type, list[token - 1].type);

		STATEMENT(list);
		if(DEBUG)
		{
			printf("%d. ***statement: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugStatement, list[token].type, list[token - 1].type);
			fflush(stdout);
		}
		if(errorCalled == 1)
			return;

		emit(7, 0, loopIdx * 3); // VM JMP
		code[jpcIdx].m = cIndex * 3; // Changed 3 -> 4 ??
		return;
	}

	if(list[token].type == 11) // Lex readsym
	{
		token++;
		if(list[token].type != 12) // Lex identsym
		{
			printparseerror(6);
			errorCalled = 1;
			return;
		}

		symIdx = FINDSYMBOL(list[token].name, 2);

		if(symIdx == -1)
		{
			if(FINDSYMBOL(list[token].name, 1) != FINDSYMBOL(list[token].name, 3))
			{
				printparseerror(6);
				errorCalled = 1;
				return;
			}
			else
			{
				printparseerror(19);
				errorCalled = 1;
				return;
			}
		}

		token++;
		emit(9, 0, 2);
		L = level - table[symIdx].level;
		M = table[symIdx].addr;
		emit(4, L, M); // VM STO

		return;
	}

	if(list[token].type == 10) // Lex writesym
	{
		token++;

		if(DEBUG)
			printf("%d. ***expression: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugExpression, list[token].type, list[token - 1].type);

		EXPRESSION(list);

		if(DEBUG)
			printf("%d. ***expression: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugExpression, list[token].type, list[token - 1].type);

		if(errorCalled == 1)
			return;

		emit(9, 0, 1);

		return;
	}

	if(list[token].type == 9) // Lex callsym
	{
		token++;
		symIdx = FINDSYMBOL(list[token].name, 3);

		if(symIdx == -1)
		{
			if(FINDSYMBOL(list[token].name, 1) != FINDSYMBOL(list[token].name, 2))
			{
				printparseerror(7);
				errorCalled = 1;
				return;
			}
			else
			{
				printparseerror(19);
				errorCalled = 1;
				return;
			}
		}

		token++;

		L = level - table[symIdx].level;
		emit(5, L, symIdx);
	}


	return;
}

int FINDSYMBOL(char name[], int symbolKind)
{
	int highestLevel = -1;
	int i;

	if(DEBUG)
	{
		printf("Im in find symbol\n");
		fflush(stdout);
	}

	for (i = 0; i < tIndex; i++)
	{
		if((strcmp(table[i].name, name) == 0) && (table[i].kind == symbolKind) && (table[i].mark == 0))
		{
			//printf("table[i].name: %s, level: %d, highestLevel: %d", table[i].name, table[i].level, highestLevel);
			if(highestLevel == -1)
			{
				if(table[i].level > highestLevel)
					highestLevel = i;
			}
			else
			{
				if(table[i].level < highestLevel)
					highestLevel = i;

			}
		}
	}

	return highestLevel;
}

void EXPRESSION(lexeme *list)
{
	int tempTokenType;

	if(DEBUG)
	{
		printf("Im in expression\n");
		fflush(stdout);
	}

	if(list[token].type == 16) // Lex subsym
	{
		token++;

		if(DEBUG)
			printf("%d. ***term: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugTerm, list[token].type, list[token - 1].type);

		TERM(list);

		if(DEBUG)
			printf("%d. ***term: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugTerm, list[token].type, list[token - 1].type);

		if(errorCalled == 1)
			return;

		emit(2, 0, 1);

		while(list[token].type == 15 || list[token].type == 16) // Lex addsym, subsym
		{
			if(list[token].type == 15) // Lex addsym
			{
				token++;

				if(DEBUG)
					printf("%d. ***term: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugTerm, list[token].type, list[token - 1].type);

				TERM(list);

				if(DEBUG)
					printf("%d. ***term: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugTerm, list[token].type, list[token - 1].type);

				if(errorCalled == 1)
					return;
				emit(2, 0, 2); // VM ADD
			}
			else
			{
				token++;

				if(DEBUG)
					printf("%d. ***term: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugTerm, list[token].type, list[token - 1].type);

				TERM(list);

				if(DEBUG)
					printf("%d. ***term: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugTerm, list[token].type, list[token - 1].type);

				if(errorCalled == 1)
					return;
				emit(2, 0, 3); // VM SUB
			}
		}
	}
	else
	{
		if(list[token].type == 15) // Lex addsym
		{
			token++;
		}

		if(DEBUG)
			printf("%d. ***term: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugTerm, list[token].type, list[token - 1].type);

		TERM(list);

		if(DEBUG)
			printf("%d. ***term: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugTerm, list[token].type, list[token - 1].type);
		if(errorCalled == 1)
			return;

		while(list[token].type == 15 || list[token].type == 16) // Lex addsym, subsym
		{
			if(list[token].type == 15) // Lex addsym
			{
				token++;

				if(DEBUG)
					printf("%d. ***term: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugTerm, list[token].type, list[token - 1].type);

				TERM(list);

				if(DEBUG)
					printf("%d. ***term: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugTerm, list[token].type, list[token - 1].type);

				if(errorCalled == 1)
					return;
				emit(2, 0, 2); // VM ADD
			}
			else
			{
				token++;

				if(DEBUG)
					printf("%d. ***term: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugTerm, list[token].type, list[token - 1].type);

				TERM(list);

				if(DEBUG)
					printf("%d. ***term: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugTerm, list[token].type, list[token - 1].type);

				if(errorCalled == 1)
					return;
				emit(2, 0, 3); // VM SUB
			}
		}
	}

	tempTokenType = list[token].type;

	// Lex lparensym, identsym, numbersym, oddsym
	if(tempTokenType == 27 || tempTokenType == 12 || tempTokenType == 13 || tempTokenType == 26)
	{
		printparseerror(17);
		errorCalled = 1;
		return;
	}

	return;
}

void TERM(lexeme *list)
{
	if(DEBUG)
	{
		printf("Im in term\n");
		fflush(stdout);
	}

	if(DEBUG)
		printf("%d. ***factor: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugFacot, list[token].type, list[token - 1].type);

	FACTOR(list);

	if(DEBUG)
		printf("%d. ***factor: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugFacot, list[token].type, list[token - 1].type);

	if(errorCalled == 1)
		return;

	while(list[token].type >= 17 && list[token].type <= 19) // Lex multsym, divsym, modsym
	{
		if(list[token].type == 17)
		{
			token++;

			if(DEBUG)
				printf("%d. ***factor: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugFacot, list[token].type, list[token - 1].type);

			FACTOR(list);

			if(DEBUG)
				printf("%d. ***factor: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugFacot, list[token].type, list[token - 1].type);
			if(errorCalled == 1)
				return;
			emit(2, 0, 4);
		}
		else if(list[token].type == 18)
		{
			token++;

			if(DEBUG)
				printf("%d. ***factor: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugFacot, list[token].type, list[token - 1].type);

			FACTOR(list);

			if(DEBUG)
				printf("%d. ***factor: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugFacot, list[token].type, list[token - 1].type);
			if(errorCalled == 1)
				return;
			emit(2, 0, 5);
		}
		else
		{
			token++;
			if(DEBUG)
				printf("%d. ***factor: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugFacot, list[token].type, list[token - 1].type);

			FACTOR(list);
			if(DEBUG)
				printf("%d. ***factor: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugFacot, list[token].type, list[token - 1].type);
			if(errorCalled == 1)
				return;
			emit(2, 0, 7);
		}
	}

	return;
}

void FACTOR(lexeme *list)
{
	int L;
	int M;
	int symIdx_var;
	int symIdx_const;


	if(DEBUG)
	{
		printf("Im in factor\n");
		printf("list[token].type: %d\n", list[token].type);
		fflush(stdout);
	}

	if(list[token].type == 12) // Lex identsym
	{
		symIdx_var = FINDSYMBOL(list[token].name, 2);
		symIdx_const = FINDSYMBOL(list[token].name, 1);

		if(symIdx_var == -1 && symIdx_const == -1)
		{
			if(FINDSYMBOL(list[token].name, 3) != -1)
			{
				if(DEBUG)
				{
					printf("first name is proc error\n");
					fflush(stdout);
				}

				printparseerror(11);
				errorCalled = 1;
				return;
			}
			else
			{
				printparseerror(19);
				errorCalled = 1;
				return;
			}
		}

		if(symIdx_var == -1)
		{
			emit(1, 0, table[symIdx_const].val);
		}
		else if(symIdx_const == -1 || table[symIdx_var].level > table[symIdx_const].level)
		{
			L = level - table[symIdx_var].level;
			M = table[symIdx_var].addr;
			emit(3, L, M);
		}
		else
		{
			emit(1, 0, table[symIdx_const].val);
		}

		token++;
	}
	else if(list[token].type == 13) // Lex numbersym
	{
		emit(1, 0, list[token].value); // ??
		token++;
	}
	else if(list[token].type == 27) // Lex lparetnsym
	{
		token++;

		if(DEBUG)
			printf("%d. ***expression: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugExpression, list[token].type, list[token - 1].type);

		EXPRESSION(list);

		if(DEBUG)
			printf("%d. ***expression: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugExpression, list[token].type, list[token - 1].type);


		if(errorCalled == 1)
			return;

		if(DEBUG)
		{
			printf("After EXPRESSION list[token].type: %d\n", list[token].type);
			fflush(stdout);
		}

		if(list[token].type != 28) // Lex rparentsym
		{
			printparseerror(12);
			errorCalled = 1;
			return;
		}
		token++;
	}
	else
	{
		printparseerror(11);
		errorCalled = 1;
		return;
	}

	return;
}

void CONDITION(lexeme *list)
{

	if(DEBUG)
	{
		printf("Im in condition\n");
		fflush(stdout);
	}

	if(list[token].type == 26) // Lex oddsym
	{
		token++;

		if(DEBUG)
			printf("%d. ***expression: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugExpression, list[token].type, list[token - 1].type);

		EXPRESSION(list);

		if(DEBUG)
			printf("%d. ***expression: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugExpression, list[token].type, list[token - 1].type);


		if(errorCalled == 1)
			return;
		emit(2, 0, 6);
	}
	else
	{
		if(DEBUG)
			printf("%d. ***expression: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugExpression, list[token].type, list[token - 1].type);

		EXPRESSION(list);

		if(DEBUG)
			printf("%d. ***expression: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugExpression, list[token].type, list[token - 1].type);


		if(errorCalled == 1)
			return;

		if(list[token].type == 20) // Lex eqlsym
		{
			token++;
			if(DEBUG)
				printf("%d. ***expression: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugExpression, list[token].type, list[token - 1].type);

			EXPRESSION(list);
			if(DEBUG)
				printf("%d. ***expression: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugExpression, list[token].type, list[token - 1].type);

			if(errorCalled == 1)
				return;
			emit(2, 0, 8);
		}
		else if(list[token].type == 21) // Lex neqlsym
		{
			token++;
			if(DEBUG)
				printf("%d. ***expression: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugExpression, list[token].type, list[token - 1].type);

			EXPRESSION(list);
			if(DEBUG)
				printf("%d. ***expression: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugExpression, list[token].type, list[token - 1].type);

			if(errorCalled == 1)
				return;
			emit(2, 0, 9);
		}
		else if(list[token].type == 22) // Lex lsssym
		{
			token++;
			if(DEBUG)
				printf("%d. ***expression: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugExpression, list[token].type, list[token - 1].type);

			EXPRESSION(list);
			if(DEBUG)
				printf("%d. ***expression: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugExpression, list[token].type, list[token - 1].type);

			if(errorCalled == 1)
				return;
			emit(2, 0, 10);
		}
		else if(list[token].type == 23) // Lex leqsym
		{
			token++;
			if(DEBUG)
				printf("%d. ***expression: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugExpression, list[token].type, list[token - 1].type);

			EXPRESSION(list);
			if(DEBUG)
				printf("%d. ***expression: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugExpression, list[token].type, list[token - 1].type);

			if(errorCalled == 1)
				return;
			emit(2, 0, 11);
		}
		else if(list[token].type == 24) // Lex gtrsym
		{
			token++;
			if(DEBUG)
				printf("%d. ***expression: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugExpression, list[token].type, list[token - 1].type);

			EXPRESSION(list);
			if(DEBUG)
				printf("%d. ***expression: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugExpression, list[token].type, list[token - 1].type);

			if(errorCalled == 1)
				return;
			emit(2, 0, 12);
		}
		else if(list[token].type == 25) // Lex geqsym
		{
			token++;
			if(DEBUG)
				printf("%d. ***expression: list[token].type: %d, list[token - 1].type: %d\n{\n", ++debugExpression, list[token].type, list[token - 1].type);

			EXPRESSION(list);
			if(DEBUG)
				printf("%d. ***expression: list[token].type: %d, list[token - 1].type: %d\n}\n", --debugExpression, list[token].type, list[token - 1].type);

			if(errorCalled == 1)
				return;
			emit(2, 0, 13);
		}
		else
		{
			printparseerror(10);
			errorCalled = 1;
			return;
		}
	}

	return;
}

void MARK(lexeme *list)
{
	int i = tIndex - 1;

	if(DEBUG)
	{
		printf("Im in mark list[token].type: %d, list[token - 1].type: %d\n", list[token].type, list[token - 1].type);
		fflush(stdout);
	}

	while(i >= 0)
	{
		if(table[i].mark == 0 && table[i].level < level)
		{
			break;
		}

		if(table[i].mark == 0 && table[i].level == level)
		{
			table[i].mark = 1;
		}
		i--;
	}

	return;
}

void emit(int opname, int level, int mvalue)
{
	code[cIndex].opcode = opname;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
}

void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].val = v;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;
}

void printparseerror(int err_code)
{
	switch (err_code)
	{
		case 1:
			printf("Parser Error: Program must be closed by a period\n");
			break;
		case 2:
			printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
			break;
		case 3:
			printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
			break;
		case 4:
			printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
			break;
		case 5:
			printf("Parser Error: Variables must be assigned using :=\n");
			break;
		case 6:
			printf("Parser Error: Only variables may be assigned to or read\n");
			break;
		case 7:
			printf("Parser Error: call must be followed by a procedure identifier\n");
			break;
		case 8:
			printf("Parser Error: when must be followed by do\n");
			break;
		case 9:
			printf("Parser Error: while must be followed by do\n");
			break;
		case 10:
			printf("Parser Error: Relational operator missing from condition\n");
			break;
		case 11:
			printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
			break;
		case 12:
			printf("Parser Error: ( must be followed by )\n");
			break;
		case 13:
			printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
			break;
		case 14:
			printf("Parser Error: Symbol declarations should close with a semicolon\n");
			break;
		case 15:
			printf("Parser Error: Statements within do-od must be separated by a semicolon\n");
			break;
		case 16:
			printf("Parser Error: do must be followed by od\n");
			break;
		case 17:
			printf("Parser Error: Bad arithmetic\n");
			break;
		case 18:
			printf("Parser Error: Confliciting symbol declarations\n");
			break;
		case 19:
			printf("Parser Error: Undeclared identifier\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");
			break;
	}

	free(code);
	free(table);
}

void printsymboltable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark);

	free(table);
	table = NULL;
}

void printassemblycode()
{
	int i;
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < cIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].opcode);
		switch (code[i].opcode)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				switch (code[i].m)
				{
					case 0:
						printf("RTN\t");
						break;
					case 1:
						printf("NEG\t");
						break;
					case 2:
						printf("ADD\t");
						break;
					case 3:
						printf("SUB\t");
						break;
					case 4:
						printf("MUL\t");
						break;
					case 5:
						printf("DIV\t");
						break;
					case 6:
						printf("ODD\t");
						break;
					case 7:
						printf("MOD\t");
						break;
					case 8:
						printf("EQL\t");
						break;
					case 9:
						printf("NEQ\t");
						break;
					case 10:
						printf("LSS\t");
						break;
					case 11:
						printf("LEQ\t");
						break;
					case 12:
						printf("GTR\t");
						break;
					case 13:
						printf("GEQ\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				switch (code[i].m)
				{
					case 1:
						printf("WRT\t");
						break;
					case 2:
						printf("RED\t");
						break;
					case 3:
						printf("HAL\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	if (table != NULL)
		free(table);
}
