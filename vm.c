// Jesse Johnson, Tam Nguyen
// COP 3402, Fall 2021
// je548081
// Overview of program.
// For this assignment I will be implementing a virtual machine called the P-machine(PM/0). The PM/0 has
// 4 segments: text, data, heap stack, and each will be contained in an array called the Process Address Space (PAS).
// Printing requires a separate array and contains the data from the data segment and the stack segment

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "compiler.h"

#define DEBUG 0
#define DEBUG_WHILE 0
#define MAX_PAS_LENGTH 3000 // changed from 500
#define LINE INT_MAX

// This function will be helpful to find a variable in a different Activation Record some L levels down
int base(int L, int BP, int PAS[])
{
	int arb = BP;	// arb = activation record base
	while (L > 0)     //find base L levels down
	{
		arb = PAS[arb - 1];
		L--;
        //printf("BP: %d, arb: %d, L: %d\n", BP, arb, L + 1);
	}
	return arb;
}

void execute_program(instruction *code, int outputs)
{
    char buffer[1001];
    char *file_Line;

    int PAS[MAX_PAS_LENGTH] = {0};
    int stackPointer = MAX_PAS_LENGTH;
    int basePointer = INT_MIN;
    int globalPointer = INT_MIN;
    int dataPointer = INT_MIN;
    int FREE = INT_MIN;
    int lexi = INT_MIN;
    int instruction_Counter = 0;
    int programCounter = 0;
    int counter = 0;
    int flag = 1;
    int i = 0;
    int j;
    int x;

    // variables for printing data/stack segments in print_PAS array
    char print_Opname[4];

    int print_PAS[MAX_PAS_LENGTH] = {0};
    int print_IR[3];
    int print_Counter = -1;
    int tempSL = -1;
    int isRecursive = 0;
    int CAL_Counter = 0;
    int CALtoINC = 0;
    int print_Line;
    int print_PC;
    int cIndex = 0;

    // Put contents from lex into PAS array
    while(code[cIndex].opcode != -1)
    {
        PAS[instruction_Counter++] = code[cIndex].opcode;
        PAS[instruction_Counter++] = code[cIndex].l;
        PAS[instruction_Counter++] = code[cIndex++].m;
    }

    // Setting up initial variables
    basePointer = instruction_Counter;
    globalPointer = instruction_Counter;
    dataPointer = instruction_Counter - 1;
    FREE = globalPointer + 40;

    if(DEBUG)
    {
        printf("%d, %d, %d, %d, %d\n", print_PAS[0], print_PAS[1], print_PAS[2], print_PAS[3], print_PAS[4]);
        printf("instruction_Counter: %d\n", instruction_Counter);
        printf("dataPointer: %d\n", dataPointer);
        printf("FREE: %d\n", FREE);
    }

    if(outputs == 1)
    {
        printf("                                PC      BP       SP     DP      data\n");
        printf("Initial values:                 0       %-3d     %-3d     %-2d\n", basePointer, MAX_PAS_LENGTH, dataPointer);
    }

    while(flag)
    {
        // OPcode is stored in x
        x = PAS[programCounter];

        if(DEBUG_WHILE)
        {
            printf("x: %d\n", x);
        }

        // storing line number
        print_Line = (programCounter / 3);

        switch(x)
        {
            case 1:
                // LIT

                // Move to M location
                programCounter += 2;

                // Set up to print the ISA Pseudo Code
                strcpy(print_Opname, "LIT");
                print_IR[1] = 0;
                print_IR[2] = PAS[programCounter];

                if(DEBUG_WHILE)
                {
                    printf("globalPointer: %d\n", globalPointer);
                    printf("basepointer: %d\n", basePointer);
                }

                if (basePointer == globalPointer)
                {
                    dataPointer++;
                    PAS[dataPointer] = PAS[programCounter];
                }
                else
                {
                    stackPointer--;
                    PAS[stackPointer] = PAS[programCounter];
                }

                // Move up one in print array and place value there.
                print_Counter++;
                print_PAS[print_Counter] = PAS[programCounter];


                if(DEBUG_WHILE)
                {
                    printf("print_COunter: %d, PAS[programCOunter: %d", print_Counter, PAS[programCounter]);
                    printf("print_PAS[5] = %d", print_PAS[print_Counter]);
                }

                break;
            case 2:
                // OPR

                // Move to M location and place M in counter;
                programCounter += 2;
                counter = PAS[programCounter];

                switch(counter)
                {
                    case 0:
                        // RTN

                        strcpy(print_Opname, "RTN");
                        print_IR[1] = 0;
                        print_IR[2] = PAS[programCounter];


                        stackPointer = basePointer + 1;
                        basePointer = PAS[stackPointer - 3];
                        programCounter = PAS[stackPointer - 4];

                        // For printing array. Puts print_Counter to correct spot in Activation Record
                        for(i = print_Counter, j = 0; ; i--)
                        {
                            if(i < 0)
                            {
                                printf("Oops you didnt make a Call");
                                flag = 0;
                                break;
                            }

                            if(print_PAS[i] == LINE)
                            {
                                print_Counter = i - 1;
                                break;
                            }
                        }
                        // For printing. Now theres one less Activation Record.
                        CAL_Counter--;

                        break;
                    case 1:
                        // NEG

                        strcpy(print_Opname, "NEG");
                        print_IR[1] = 0;
                        print_IR[2] = PAS[programCounter];

                        if(basePointer == globalPointer)
                        {
                            PAS[dataPointer] = -1 * PAS[dataPointer];

                            print_PAS[print_Counter] = PAS[dataPointer];
                        }
                        else
                        {
                            PAS[stackPointer] = -1 * PAS[stackPointer];

                            print_PAS[print_Counter] = PAS[stackPointer];
                        }

                        break;
                    case 2:
                        // ADD

                        strcpy(print_Opname, "ADD");
                        print_IR[1] = 0;
                        print_IR[2] = PAS[programCounter];

                        print_Counter--;
                        
                        if(basePointer == globalPointer)
                        {
                            dataPointer--;
                            PAS[dataPointer] = PAS[dataPointer] + PAS[dataPointer + 1];

                            print_PAS[print_Counter] = PAS[dataPointer];
                        }
                        else
                        {
                            stackPointer++;
                            PAS[stackPointer] = PAS[stackPointer] + PAS[stackPointer - 1];

                            print_PAS[print_Counter] = PAS[stackPointer];
                        }

                        break;
                    case 3:
                        // SUB

                        strcpy(print_Opname, "SUB");
                        print_IR[1] = 0;
                        print_IR[2] = PAS[programCounter];

                        print_Counter--;

                        if(basePointer == globalPointer)
                        {
                            dataPointer--;
                            PAS[dataPointer] = PAS[dataPointer] - PAS[dataPointer + 1];

                            print_PAS[print_Counter] = PAS[dataPointer];
                        }
                        else
                        {
                            stackPointer++;
                            PAS[stackPointer] = PAS[stackPointer] - PAS[stackPointer - 1];

                            print_PAS[print_Counter] = PAS[stackPointer];
                        }

                        break;
                    case 4:
                        // MUL

                        strcpy(print_Opname, "MUL");
                        print_IR[1] = 0;
                        print_IR[2] = PAS[programCounter];

                        print_Counter--;
                        
                        if(basePointer == globalPointer)
                        {
                            dataPointer--;
                            PAS[dataPointer] = PAS[dataPointer] * PAS[dataPointer + 1];

                            print_PAS[print_Counter] = PAS[dataPointer];
                        }
                        else
                        {
                            stackPointer++;
                            PAS[stackPointer] = PAS[stackPointer] * PAS[stackPointer - 1];

                            print_PAS[print_Counter] = PAS[stackPointer];
                        }

                        break;
                    case 5:
                        // DIV

                        strcpy(print_Opname, "DIV");
                        print_IR[1] = 0;
                        print_IR[2] = PAS[programCounter];

                        print_Counter--;
                        
                        if(basePointer == globalPointer)
                        {
                            dataPointer--;
                            PAS[dataPointer] = PAS[dataPointer] / PAS[dataPointer + 1];

                            print_PAS[print_Counter] = PAS[dataPointer];
                        }
                        else
                        {
                            stackPointer++;
                            PAS[stackPointer] = PAS[stackPointer] / PAS[stackPointer - 1];

                            print_PAS[print_Counter] = PAS[stackPointer];
                        }

                        break;
                    case 6:
                        // ODD

                        strcpy(print_Opname, "ODD");
                        print_IR[1] = 0;
                        print_IR[2] = PAS[programCounter];
                        
                        if(basePointer == globalPointer)
                        {
                            PAS[dataPointer] = PAS[dataPointer] % 2;

                            print_PAS[print_Counter] = PAS[dataPointer];
                        }
                        else
                        {
                            PAS[stackPointer] = PAS[stackPointer] % 2;

                            print_PAS[print_Counter] = PAS[stackPointer];
                        }

                        break;
                    case 7:
                        // MOD

                        strcpy(print_Opname, "MOD");
                        print_IR[1] = 0;
                        print_IR[2] = PAS[programCounter];

                        print_Counter--;
                        
                        if(basePointer == globalPointer)
                        {
                            dataPointer--;
                            PAS[dataPointer] = PAS[dataPointer] % PAS[dataPointer + 1];

                            print_PAS[print_Counter] = PAS[dataPointer];
                        }
                        else
                        {
                            stackPointer++;
                            PAS[stackPointer] = PAS[stackPointer] % PAS[stackPointer - 1];

                            print_PAS[print_Counter] = PAS[stackPointer];
                        }

                        break;
                    case 8:
                        // EQL

                        strcpy(print_Opname, "EQL");
                        print_IR[1] = 0;
                        print_IR[2] = PAS[programCounter];

                        print_Counter--;
                        
                        if(basePointer == globalPointer)
                        {
                            dataPointer--;
                            PAS[dataPointer] = PAS[dataPointer] == PAS[dataPointer + 1];

                            print_PAS[print_Counter] = PAS[dataPointer];
                        }
                        else
                        {
                            stackPointer++;
                            PAS[stackPointer] = PAS[stackPointer] == PAS[stackPointer - 1];

                            print_PAS[print_Counter] = PAS[stackPointer];
                        }

                        break;
                    case 9:
                        // NEQ

                        strcpy(print_Opname, "NEQ");
                        print_IR[1] = 0;
                        print_IR[2] = PAS[programCounter];

                        print_Counter--;
                        
                        if(basePointer == globalPointer)
                        {
                            dataPointer--;
                            PAS[dataPointer] = PAS[dataPointer] != PAS[dataPointer + 1];

                            print_PAS[print_Counter] = PAS[dataPointer];
                        }
                        else
                        {
                            stackPointer++;
                            PAS[stackPointer] = PAS[stackPointer] != PAS[stackPointer - 1];

                            print_PAS[print_Counter] = PAS[stackPointer];
                        }
                        break;
                    case 10:
                        // LSS

                        strcpy(print_Opname, "LSS");
                        print_IR[1] = 0;
                        print_IR[2] = PAS[programCounter];

                        print_Counter--;
                        
                        if(basePointer == globalPointer)
                        {
                            dataPointer--;
                            PAS[dataPointer] = PAS[dataPointer] < PAS[dataPointer + 1];

                            print_PAS[print_Counter] = PAS[dataPointer];
                        }
                        else
                        {
                            stackPointer++;
                            PAS[stackPointer] = PAS[stackPointer] < PAS[stackPointer - 1];

                            print_PAS[print_Counter] = PAS[stackPointer];
                        }

                        break;
                    case 11:
                        // LEQ

                        strcpy(print_Opname, "LIT");
                        print_IR[1] = 0;
                        print_IR[2] = PAS[programCounter];

                        print_Counter--;
                        
                        if(basePointer == globalPointer)
                        {
                            dataPointer--;
                            PAS[dataPointer] = PAS[dataPointer] <= PAS[dataPointer + 1];

                            print_PAS[print_Counter] = PAS[dataPointer];
                        }
                        else
                        {
                            stackPointer++;
                            PAS[stackPointer] = PAS[stackPointer] <= PAS[stackPointer - 1];

                            print_PAS[print_Counter] = PAS[stackPointer];
                        }

                        break;
                    case 12:
                        // GTR

                        strcpy(print_Opname, "GTR");
                        print_IR[1] = 0;
                        print_IR[2] = PAS[programCounter];

                        print_Counter--;
                        
                        if(basePointer == globalPointer)
                        {
                            dataPointer--;
                            PAS[dataPointer] = PAS[dataPointer] > PAS[dataPointer + 1];

                            print_PAS[print_Counter] = PAS[dataPointer];
                        }
                        else
                        {
                            stackPointer++;
                            PAS[stackPointer] = PAS[stackPointer] > PAS[stackPointer - 1];

                            print_PAS[print_Counter] = PAS[stackPointer];
                        }

                        break;
                    case 13:
                        // GEQ

                        strcpy(print_Opname, "GEQ");
                        print_IR[1] = 0;
                        print_IR[2] = PAS[programCounter];

                        print_Counter--;
                        
                        if(basePointer == globalPointer)
                        {
                            dataPointer--;
                            PAS[dataPointer] = PAS[dataPointer] >= PAS[dataPointer + 1];

                            print_PAS[print_Counter] = PAS[dataPointer];
                        }
                        else
                        {
                            stackPointer++;
                            PAS[stackPointer] = PAS[stackPointer] >= PAS[stackPointer - 1];

                            print_PAS[print_Counter] = PAS[stackPointer];
                        }

                        break;
                }
                break;
            case 3:
                // LOD

                // Lexi is the lexicographical level
                lexi = PAS[programCounter + 1];
                programCounter += 2;

                strcpy(print_Opname, "LOD");
                print_IR[1] = lexi;
                print_IR[2] = PAS[programCounter];

                print_Counter++;

                if(basePointer == globalPointer)
                {
                    dataPointer++;
                    PAS[dataPointer] = PAS[globalPointer + PAS[programCounter]];

                    print_PAS[print_Counter] = PAS[dataPointer];
                }
                else
                {
                    if(base(lexi, basePointer, PAS) == globalPointer)
                    {
                        stackPointer--;
                        PAS[stackPointer] = PAS[globalPointer + PAS[programCounter]];

                        print_PAS[print_Counter] = PAS[stackPointer];
                    }
                    else
                    {
                        stackPointer--;
                        PAS[stackPointer] = PAS[base(lexi, basePointer, PAS) - PAS[programCounter]];

                        print_PAS[print_Counter] = PAS[stackPointer];
                    }
                }
                break;
            case 4:
               // STO

                lexi = PAS[programCounter + 1];
                programCounter = programCounter + 2;

                strcpy(print_Opname, "STO");
                print_IR[1] = lexi;
                print_IR[2] = PAS[programCounter];

                if(basePointer == globalPointer)
                {
                    PAS[globalPointer + PAS[programCounter]] = PAS[dataPointer];
                    dataPointer--;
                }
                else
                {
                    if(base(lexi, basePointer, PAS) == globalPointer)
                    {
                        PAS[globalPointer + PAS[programCounter]] = PAS[stackPointer];
                        stackPointer++;
                    }
                    else
                    {
                        PAS[base(lexi, basePointer, PAS) - PAS[programCounter]] = PAS[stackPointer];
                        stackPointer++;
                    }
                }

                if(DEBUG_WHILE)
                {
                    printf("i: %d\n", i);
                    printf("print_PAS[%d]: %d\n", i, print_PAS[print_Counter]);
                    printf("print_counter: %d\n", print_Counter);
                }

                //printf("lexi: %d, CAL_Counter: %d\n", lexi, CAL_Counter);
                // For storing in correct print_PAS location
                if(lexi == CAL_Counter) // If L == number of time CAL has been called
                {
                    i = PAS[programCounter];
                    print_PAS[i] = print_PAS[print_Counter];
                }
                else if(lexi == 0) // If STO in current Activation register
                {
                    for(i = print_Counter; i >= 0; i--)
                    {
                        if(print_PAS[i] == LINE)
                        {
                            i += PAS[programCounter] + 1;
                            print_PAS[i] = print_PAS[print_Counter];
                            break;
                        }
                    }
                }
                else if((lexi >= 0) && (lexi < CAL_Counter))
                {
                    for(i = print_Counter, j = 0; i >= 0; i--)
                    {
                        if(print_PAS[i] == LINE || isRecursive == 1)
                        {
                            //printf("i: %d, tempSL: %d, print_PAS[i + 2]: %d\n", i, tempSL, print_PAS[i + 2]);
                            // If last register has same static link as current (recurssive)
                            if(tempSL == print_PAS[i + 2] || isRecursive == 1)
                            {
                                j = 0;
                                if(i == 0)
                                {
                                    break;
                                }

                                isRecursive = 1;
                            }
                            if(tempSL == -1)
                                tempSL = print_PAS[i + 2];

                            j++;
                            if(j > lexi)
                            {
                                i++;
                                break;
                            }
                        }
                    }
                    //printf("i: %d, PAS[programCounter]: %d\n", i, PAS[programCounter]);
                    i += PAS[programCounter];
                    print_PAS[i] = print_PAS[print_Counter];
                }
                else
                {
                    printf("Oops you entered stack territory");
                    flag = 0;
                }

                print_Counter--;
                break;
            case 5:
                // CAL

                lexi = PAS[programCounter + 1];
                programCounter += 2;

                strcpy(print_Opname, "CAL");
                print_IR[1] = lexi;
                print_IR[2] = PAS[programCounter];

                PAS[stackPointer - 1] = 0;
                PAS[stackPointer - 2] = base(lexi, basePointer, PAS);
                PAS[stackPointer - 3] = basePointer;
                PAS[stackPointer - 4] = programCounter;

                basePointer = stackPointer - 1;
                programCounter = PAS[programCounter] - 1;

                // Places correct values in PAS but does not show them when printing
                print_PAS[print_Counter + 1] = LINE;
                print_PAS[print_Counter + 2] = PAS[stackPointer - 1];
                print_PAS[print_Counter + 3] = PAS[stackPointer - 2];
                print_PAS[print_Counter + 4] = PAS[stackPointer - 3];
                print_PAS[print_Counter + 5] = PAS[stackPointer - 4] + 1;
                CAL_Counter++;
                // Used to signal that a call has been made for INC to actually work in print_PAS
                CALtoINC ^= 1;

                break;
            case 6:
               // INC

                programCounter += 2;

                strcpy(print_Opname, "INC");
                print_IR[1] = 0;
                print_IR[2] = PAS[programCounter];

                if(basePointer == globalPointer)
                {
                    dataPointer = dataPointer + PAS[programCounter];
                }
                else
                {
                    stackPointer = stackPointer - PAS[programCounter];
                }

                // increments by M
                if(CALtoINC == 1)
                {
                    print_Counter++;
                    CALtoINC = CALtoINC ^ 1;
                }

                print_Counter += PAS[programCounter];
                break;
            case 7:
               // JMP

                programCounter += 2;

                strcpy(print_Opname, "JMP");
                print_IR[1] = 0;
                print_IR[2] = PAS[programCounter];

                programCounter = PAS[programCounter];

                programCounter--;

                break;
            case 8:
                // JPC

                programCounter += 2;

                strcpy(print_Opname, "JPC");
                print_IR[1] = 0;
                print_IR[2] = PAS[programCounter];

                if(basePointer == globalPointer)
                {
                    if(PAS[dataPointer] == 0)
                    {
                        // Added "- 1" for correction when programCounter++ is done at the bottom.
                        programCounter = PAS[programCounter] - 1;
                    }

                    dataPointer--;
                    print_Counter--;
                }
                else
                {
                    if(PAS[stackPointer] == 0)
                    {                        
                        // Added "- 1" for correction when programCounter++ is done at the bottom.
                        programCounter = PAS[programCounter] - 1;
                    }

                    stackPointer++;
                    print_Counter--;
                }
                break;
            case 9:
               // SYS

                programCounter += 2;
                counter = PAS[programCounter];

                strcpy(print_Opname, "SYS");
                print_IR[1] = 0;
                print_IR[2] = PAS[programCounter];

                switch(counter)
                {
                    case 1:
                        // Print top of stack

                        printf("Top of Stack Value: ");

                        if(basePointer == globalPointer)
                        {
                            printf("%d\n", PAS[dataPointer]);
                            dataPointer--;
                        }
                        else
                        {
                            printf("%d\n", PAS[stackPointer]);
                            stackPointer++;
                        }

                        print_Counter--;
                        break;
                    case 2:
                        // User inputs integer

                        printf("Please Enter an Integer: ");

                        if(basePointer == globalPointer)
                        {
                            dataPointer++;
                            scanf("%d", &PAS[dataPointer]);

                            print_Counter++;
                            print_PAS[print_Counter] = PAS[dataPointer];

                            if(DEBUG_WHILE)
                            {
                                printf("User input from data: %d\n", PAS[dataPointer]);
                            }
                        }
                        else
                        {
                            stackPointer--;
                            scanf("%d", &PAS[stackPointer]);

                            print_Counter++;
                            print_PAS[print_Counter] = PAS[stackPointer];

                            if(DEBUG_WHILE)
                            {
                                printf("User input from stack: %d\n", PAS[stackPointer]);
                            }

                        }

                        break;
                    case 3:
                        // End program

                        flag = 0;
                        break; 
                }
                break;
        }        

        programCounter++;

        if(outputs == 1)
        {
            printf("%2d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t", print_Line, print_Opname, print_IR[1], print_IR[2], programCounter, basePointer, stackPointer, dataPointer);

            for(i = 0; i <= print_Counter; i++)
            {
                if(print_PAS[i] < LINE)
                {
                    printf("%-3d ", print_PAS[i]);
                }
                else
                {
                    break;
                }
            }

            printf("\nstack: ");

            for( ; i <= print_Counter; i++)
            {
                if(print_PAS[i] == LINE)
                {
                    continue;
                }

                // ******** dont know biggest number that can be put in here. ex user input 50000000
                printf("%-3d ", print_PAS[i]);
            }

            printf("\n");
        }
    }

    return;
}