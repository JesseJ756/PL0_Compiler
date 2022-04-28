// Jesse Johnson
// COP 3402, Fall 2021
// je548081

/* 
	This is the lex.c file for the UCF Fall 2021 Systems Software Project.
	For HW2, you must implement the function lexeme *lexanalyzer(char *input).
	You may add as many constants, global variables, and support functions
	as you desire.
	
	If you choose to alter the printing functions or delete list or lex_index, 
	you MUST make a note of that in you readme file, otherwise you will lose 
	5 points.
*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"

#define MAX_NUMBER_TOKENS 1000 // changed from 500
#define MAX_IDENT_LEN 11
#define MAX_NUMBER_LEN 5
#define MAX_CODE_LENGTH 1000 // changed from 500

lexeme *list;
int lex_index = 0; // added "= 0"

void printlexerror(int type);
void printtokens();


// Adds the identifier to lexeme list.
void identifierToList(char buffer[])
{
	strcpy(list[lex_index].name, buffer);
	list[lex_index].type = identsym;
	lex_index++;

	return;
}

// Adds the one of the reserved words to lexeme list.
void reserveWordToList(char buffer[])
{
	printf("buffer: %s, ", buffer);

	list[lex_index].type = *buffer;
	lex_index++;

	return;
}

// Checks to see if buffer is a reserved then adds it the list
int isReserveWord(char buffer[])
{
	int returnNum = 0;

	if(strcmp(buffer, "const") == 0)
	{
		list[lex_index].type = constsym;
		returnNum = 1;
	}
	else if(strcmp(buffer, "var") == 0)
	{
		list[lex_index].type = varsym;
		returnNum = 1;
	}
	else if(strcmp(buffer, "procedure") == 0)
	{
		list[lex_index].type = procsym;
		returnNum = 1;
	}
	else if(strcmp(buffer, "call") == 0)
	{
		list[lex_index].type = callsym;
		returnNum = 1;
	}
	else if(strcmp(buffer, "when") == 0) // In parser needs to account that "do" isnt do(begin) but do(then)
	{
		list[lex_index].type = whensym;
		returnNum = 1;
	}
	else if(strcmp(buffer, "elsedo") == 0) // DELETED then
	{
		list[lex_index].type = elsedosym;
		returnNum = 1;
	}
	else if(strcmp(buffer, "while") == 0) // Like buffer == "when", change parser to look for do after while
	{
		list[lex_index].type = whilesym;
		returnNum = 1;
	}
	else if(strcmp(buffer, "do") == 0)
	{
		list[lex_index].type = dosym;
		returnNum = 1;
	}
	else if(strcmp(buffer, "od") == 0)
	{
		list[lex_index].type = odsym;
		returnNum = 1;
	}
	else if(strcmp(buffer, "read") == 0)
	{
		list[lex_index].type = readsym;
		returnNum = 1;
	}
	else if(strcmp(buffer, "write") == 0)
	{
		list[lex_index].type = writesym;
		returnNum = 1;
	}
	else if(strcmp(buffer, "odd") == 0)
	{
		list[lex_index].type = oddsym;
		returnNum = 1;
	}
	else
	{
		returnNum = 0;
	}

	return returnNum;
}

// Adds a digit with a length of less then 5 to the list.
void digitToList(char buffer[])
{
	int i;
	sscanf(buffer, "%d", &i);

	list[lex_index].value = i;
	list[lex_index].type = numbersym;
	lex_index++;

	return;
}

// Loops though input from file, grabs tokens, then puts them into the list.
lexeme *lexanalyzer(char *input, int tokens)
{
	int i = 0;
	int stringIndex = 0;
	int isIdentifier = 0;
	int bufferLength = 0;
	char buffer[MAX_CODE_LENGTH] = {0};
	list = malloc(MAX_NUMBER_TOKENS * sizeof(*list));

	while(input[stringIndex] != '\0')
	{
		// If input is a space, tab, or newline.
		if(iscntrl(input[stringIndex]) || (isspace(input[stringIndex])))
		{
			stringIndex++;
			continue;
		}
		else if(isdigit(input[stringIndex]))
		{
			// Keep looping until a non-number is reached.
			while(isdigit(input[stringIndex]))
			{
				buffer[i++] = input[stringIndex++];
				bufferLength++;
			}
			buffer[i] = '\0';

			// Number constraint
			if(bufferLength > 5)
			{
				printlexerror(3);
				return NULL;
			}
			else if(isalpha(input[stringIndex])) // Identifier cant have a number at the beginning
			{
				printlexerror(2);
				return NULL;
			}

			digitToList(buffer);

		}
		else if(isalpha(input[stringIndex])) // New token starts with a letter
		{
			while(isalpha(input[stringIndex]) || isdigit(input[stringIndex]))
			{
				buffer[i++] = input[stringIndex++];
				bufferLength++;
			}
			buffer[i] = '\0';

			if(bufferLength > 11) // Identifier constraint
			{
				printlexerror(4);
				return NULL;
			}
			else if(isReserveWord(buffer)) // Checks and adds it to the list
			{
				lex_index++;
			}
			else
			{
				identifierToList(buffer);
			}
		}
		else
		{
			// Checks for special symbols
			if(input[stringIndex] == '=') // deleted && other == '='
			{
				list[lex_index].type = eqlsym; // DELETED lex_index++ after this line
			}
			else if((input[stringIndex] == '!') && (input[stringIndex + 1] == '='))
			{
				list[lex_index].type = neqsym;
				stringIndex++;
			}
			else if(input[stringIndex] == '<')
			{
				if(input[stringIndex + 1] == '=')
				{
					list[lex_index].type = leqsym;
					stringIndex++;
				}
				else
				{
					list[lex_index].type = lsssym;
				}
			}
			else if(input[stringIndex] == '>')
			{
				if(input[stringIndex + 1] == '=')
				{
					list[lex_index].type = geqsym;
					stringIndex++;
				}
				else
				{
					list[lex_index].type = gtrsym;
				}
			}
			else if(input[stringIndex] == '%')
			{
				list[lex_index].type = modsym;
			}
			else if(input[stringIndex] == '*')
			{
				list[lex_index].type = multsym;
			}
			else if(input[stringIndex] == '/')
			{
				if(input[stringIndex + 1] == '/')
				{
					// Skip all input after // until new line is reached.
					while(input[stringIndex++] != '\n');
					continue;
				}
				else
				{
					list[lex_index].type = divsym;
				}
			}
			else if(input[stringIndex] == '+')
			{
				list[lex_index].type = addsym;
			}
			else if(input[stringIndex] == '-')
			{
				list[lex_index].type = subsym;
			}
			else if(input[stringIndex] == '(')
			{
				list[lex_index].type = lparensym;
			}
			else if(input[stringIndex] == ')')
			{
				list[lex_index].type = rparensym;
			}
			else if(input[stringIndex] == ',')
			{
				list[lex_index].type = commasym;
			}
			else if(input[stringIndex] == '.')
			{
				list[lex_index].type = periodsym;
			}
			else if(input[stringIndex] == ';')
			{
				list[lex_index].type = semicolonsym;
			}
			else if((input[stringIndex] == ':') && (input[stringIndex + 1] == '='))
			{
				list[lex_index].type = assignsym;
				stringIndex++;
			}
			else
			{
				printf("input[stringIndex]: %c\n", input[stringIndex]);
				printlexerror(1);
				return NULL;
			}

			stringIndex++;
			lex_index++;
		}
		
		// Reset buffer array
		bufferLength = 0;
		i = 0;
		buffer[0] = '\0';
	}

	if(tokens == 1)
		printtokens();

	return list;
}

void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < lex_index; i++)
	{
		switch (list[i].type)
		{
			case oddsym:
				printf("%11s\t%d", "odd", oddsym);
				break;
			case eqlsym:
				printf("%11s\t%d", "=", eqlsym);
				break;
			case neqsym:
				printf("%11s\t%d", "!=", neqsym);
				break;
			case lsssym:
				printf("%11s\t%d", "<", lsssym);
				break;
			case leqsym:
				printf("%11s\t%d", "<=", leqsym);
				break;
			case gtrsym:
				printf("%11s\t%d", ">", gtrsym);
				break;
			case geqsym:
				printf("%11s\t%d", ">=", geqsym);
				break;
			case modsym:
				printf("%11s\t%d", "%", modsym);
				break;
			case multsym:
				printf("%11s\t%d", "*", multsym);
				break;
			case divsym:
				printf("%11s\t%d", "/", divsym);
				break;
			case addsym:
				printf("%11s\t%d", "+", addsym);
				break;
			case subsym:
				printf("%11s\t%d", "-", subsym);
				break;
			case lparensym:
				printf("%11s\t%d", "(", lparensym);
				break;
			case rparensym:
				printf("%11s\t%d", ")", rparensym);
				break;
			case commasym:
				printf("%11s\t%d", ",", commasym);
				break;
			case periodsym:
				printf("%11s\t%d", ".", periodsym);
				break;
			case semicolonsym:
				printf("%11s\t%d", ";", semicolonsym);
				break;
			case assignsym:
				printf("%11s\t%d", ":=", assignsym);
				break;
			case dosym:
				printf("%11s\t%d", "do", dosym);
				break;
			case odsym:
				printf("%11s\t%d", "od", odsym);
				break;
			case whensym:
				printf("%11s\t%d", "when", whensym);
				break;
			case elsedosym: // DELETE then case
				printf("%11s\t%d", "elsedo", elsedosym);
				break;
			case whilesym:
				printf("%11s\t%d", "while", whilesym);
				break;
			case callsym: // DELETE do after while case
				printf("%11s\t%d", "call", callsym);
				break;
			case writesym:
				printf("%11s\t%d", "write", writesym);
				break;
			case readsym:
				printf("%11s\t%d", "read", readsym);
				break;
			case constsym:
				printf("%11s\t%d", "const", constsym);
				break;
			case varsym:
				printf("%11s\t%d", "var", varsym);
				break;
			case procsym:
				printf("%11s\t%d", "procedure", procsym);
				break;
			case identsym:
				printf("%11s\t%d", list[i].name, identsym);
				break;
			case numbersym:
				printf("%11d\t%d", list[i].value, numbersym);
				break;
		}
		printf("\n");
	}
	printf("\n");
	printf("Token List:\n");
	for (i = 0; i < lex_index; i++)
	{
		if (list[i].type == numbersym)
			printf("%d %d ", numbersym, list[i].value);
		else if (list[i].type == identsym)
			printf("%d %s ", identsym, list[i].name);
		else
			printf("%d ", list[i].type);
	}
	printf("\n");
	list[lex_index++].type = -1;
}

void printlexerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Excessive Number Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Excessive Identifier Length\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");
	
	free(list);
	return;
}