#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>


#define MAX_LENGTH 10000
int NUM_LABELS = 0;
int NUM_LINES  = 0;
int NUM_COMMENTS = 0;
int lcount=0;
int lineCount = 0;

FILE *output;


void CleanUp(FILE *in, FILE *out)
{
    fclose(in);
    fclose(out);
}

char * ReadLine(int num, char *line,FILE * in)
{
    char * linestring = fgets(line,MAX_LENGTH,in);
    if(linestring!=NULL)
    {
        CommentOrInstruction(num,linestring);
        return linestring;
    }else
    return NULL;
}

char * CountLabels(char *line, FILE *in)
{
    char *lineString = fgets(line,MAX_LENGTH,in);
    if(lineString!=NULL && lineString[0]!='\n'&&lineString[0]!='0'&&strlen(lineString)!=0)
    {
        char *LToken = strtok(line," \t\n");
        if(LToken[strlen(LToken)-1]==':')
        {
            NUM_LABELS++;
        }
        return LToken;
    }else
    return NULL;
}

// Fetch the strings ignoring the labels and the comments
// We will have 4 arguements for each instruction
char * GetInstruction(char * line ,FILE *in, char *Labels, char *LabelVals)
{
        char * opCode;
        char * arg1, * arg2, * arg3;
        char * lineString = fgets(line,MAX_LENGTH,in); // Fetch a line of assembly code...
        if(lineString != NULL && lineString[0]!='\n'&&lineString[0]!='0'&&strlen(lineString)!=0)
        {
            char * FirstToken = strtok(line,", \t\n");
            if(FirstToken[0]=='#') // Were dealing with a comment
            {
                return FirstToken; // This should move onto the next line of code...
            }else if(FirstToken[strlen(FirstToken)-1]==':')
            {
                FirstToken = strtok(NULL,", \t\n");
                int t =0;
                while(FirstToken!=NULL)
                {
                    switch(t)
                    {
                        case 0:{opCode = FirstToken;    break;}
                        case 1:{arg1 = FirstToken;      break;}
                        case 2:{arg2 = FirstToken;      break;}
                        case 3:{arg3 = FirstToken;      break;}
                        default:{ break;}
                    }
                    t++;
                    FirstToken = strtok(NULL,", \t\n");
                }
                ParseInstructions(opCode,arg1,arg2,arg3, Labels, LabelVals);

            }else
            {
                opCode = FirstToken;
                int t = 0;
                while(FirstToken!=NULL)
                {
                    switch(t)
                    {
                        case 0:{opCode = FirstToken;    break;}
                        case 1:{arg1 = FirstToken;      break;}
                        case 2:{arg2 = FirstToken;      break;}
                        case 3:{arg3 = FirstToken;      break;}
                        default:{break;}
                    }
                    t++;
                    FirstToken = strtok(NULL,", \t\n");
                }
                ParseInstructions(opCode,arg1,arg2,arg3,Labels,LabelVals);
            }
            return "0xDEAD";
        }else
        {
            return NULL;
        }
}


char * ParseImm(char * arg, char **Labels, char ** LabelVals)
{
    if(arg[0] == '-')
    {
        arg++;
        arg = atoi(arg)*(-1);
    }
    else if(isalpha(*arg)) // Checking for a placeholder or a symbol
    {
        int t = 0;
        for(t = 0;t<lcount;t++)
        {
            if(!strcmp(arg,Labels[t])){ arg = LabelVals[t]; break;}
        }
    }
    else if(isdigit(*arg))
    {
            arg = atoi(arg);
    }

    return arg;
}

void ParseInstructions(char *Op,char *arg1, char *arg2, char *arg3, char **Labels, char **LabelVals)
{


            unsigned short data = 0x0000;

            if(!strcmp(Op,"lw"))
            {
                arg3 = ParseImm(arg3,Labels,LabelVals);
                Op = 0b101;
                RRI(Op,arg1,arg2,arg3);

            }else if(!strcmp(Op,"add"))
            {
                data = 0b000;
                Op = 0b000;
                RRR(Op,arg1,arg2,arg3);

            }else if(!strcmp(Op,"addi"))
            {
                arg3 = ParseImm(arg3,Labels,LabelVals);
                Op = 0b001;
                RRI(Op,arg1,arg2,arg3);

            }else if(!strcmp(Op,"nand"))
            {
                Op = 0b010;
                RRR(Op,arg1,arg2,arg3);
            }else if(!strcmp(Op,"lui"))
            {
                Op = 0b011;
                RI(Op,arg1,arg2);
            }else if(!strcmp(Op,"sw"))
            {
                Op = 0b110;
                arg3 = ParseImm(arg3,Labels,LabelVals);
                RRI(Op,arg1,arg2,arg3);
            }else if(!strcmp(Op,"bne"))
            {
                arg3 = ParseImm(arg3,Labels,LabelVals);
                Op = 0b110;
                RRI(Op,arg1,arg2,arg3);
            }else if(!strcmp(Op, "jalr"))
            {
                Op = 111;
                arg3 = 0; // The immediate value in this case
                RRI(Op,arg1,arg2,arg3);
            }else if(!strcmp(Op,"nop"))
            {
                RRR(0,0,0,0);
            }else if(!strcmp(Op,"halt"))
            {
                Op = 0b111;
                arg1 = 0x000;
                arg2 = 0x000;
                arg3 = 113; // By Convention
                RRI(Op,arg1,arg2,arg3);
            }else if(!strcmp(Op,".fill"))
            {
                arg1 = ParseImm(arg1,Labels,LabelVals);
                Op = 0;
                RI(Op,0,arg1);

            }else
            {
                printf("Improper Instruction! -- Maybe add support for lines?\n");
            }
}


void RRR(char * op, char * arg1, char * arg2, char * arg3)
{

    unsigned short int data = 0x0000;
    data = op;
    data = (data<<3)^atoi(arg1);
    data = (data<<3)^atoi(arg2);
    data = (data<<7)^atoi(arg3);
    printf("RRR: \t0x%04x\n",data);
    OutputToFile(data);


}
void RRI(char * op, char * arg1, char * arg2, char * arg3)
{
    unsigned short int data = 0x0000;
    data = op;
    data = (data<<3)^atoi(arg1);
    data = (data<<3)^atoi(arg2);
    data = (data<<7)^(int)(arg3);
    printf("RRI: \t0x%04x\n",data);
    OutputToFile(data);


}
void  RI(char * op, char * arg1, char *arg2)
{
    unsigned short int data = 0;
    data = op;
    data = (data<<3)^atoi(arg1);
    data = (data<<10)^(int)(arg2);
    printf("RI: \t0x%04x\n",data);
    OutputToFile(data);
}

void OutputToFile(unsigned short int data)
{
     fprintf(output,"0x%04x\n",data);
}

char * TrimWhiteSpace(char *str)
{
    char *ending;
    while(isspace(*str)) str++; // Counting all of the intances of white space
    if(*str==0) return str;     // all white space
    ending = str+strlen(str)-1;
}

char *  BuildLabels(char * line, FILE *in, char *Labels[], char *LabelVals[])
{
    char *lineString = fgets(line,MAX_LENGTH,in);
    if(lineString!=NULL)
    {
        char * FirstToken = strtok(line," \t\n");
     if(FirstToken!=NULL)
    {

        if(FirstToken[0]!='#')// || FirstToken != NULL) // FirstToken != NULL)
        {
            if(FirstToken[strlen(FirstToken)-1]==':')
            {
                Labels[lcount] = malloc(strlen(FirstToken));
                strcpy(Labels[lcount],FirstToken);
                int t=0;
                while(Labels[lcount][t]!=':'){t++;}
                Labels[lcount][t] = 0;
                LabelVals[lcount] = lineCount;
                lcount++;
            }
            lineCount++;
        }
    }
        return FirstToken;
    }else
    {
        return NULL;
    }
}



void CommentOrInstruction(int num, char * line)
{
   char * FirstToken = strtok(line," \t\n",",");
   //If it's a comment, we don't care
   if(FirstToken == NULL || FirstToken[0] =='#')
   {
       NUM_COMMENTS++;
       return;
   }else if(FirstToken[strlen(FirstToken)-1]==':')
   {
       NUM_LABELS++;
   }
}

int main(int argc, char *args[])
{

    char *inputString, *outputString, *lineString[MAX_LENGTH+1];
    char *label,*opcode,*arg1,*arg2,*arg3; // All of the possible things that we can find
    FILE *input;
    int lines = 0; // Lines of actual Code
    //input = fopen("first.asm","a+");
    char response[1];
    char  InputFile[MAX_LENGTH];
    char OutputFile[MAX_LENGTH];
    printf("Please enter the asm file you wish to load: ");
    scanf("%s",InputFile);
    printf("\nPlease enter output filename: ");
    scanf("%s",OutputFile);
//    if(access(OutputFile,F_OK)!=-1)
//    {
//        printf("\nYou will be overwriting the file %s Y/N to continue",OutputFile);
//        scanf("%s", response);
//        if(strcmp(response,"y"))
//        {
//            exit(0);
//        }
////      remove(OutputFile);
//   }
    input = fopen(InputFile,"a+");
    output = fopen(OutputFile,"a+");
    if(input == NULL){  printf("Error opening file %s",input);}
    else{               printf("Opening input...\n");}

    if(output == NULL){ printf("Error in output file %s", output);}
    else{              printf("Opening output...\n");}
    printf("CountLabels\n");
    while(CountLabels(lineString,input)!=NULL) {}
    printf("Finished...\n");
    char *Labels[NUM_LABELS];       //This array is supposed to store the names of the Labels, eg start:,done:, etc
    char *LabelVals[NUM_LABELS];    //This array is supposed to store the line of code that it points to, so for start it would be 2, etc
    rewind(input);
    printf("Building Labels...\n");
    while(BuildLabels(lineString,input,Labels,LabelVals)!=NULL){}
    printf("Finished...\n");
    rewind(input);
    printf("Listing Instructions as Tokens \n");
    while(GetInstruction(lineString,input,Labels,LabelVals)!=NULL){}
    CleanUp(input, output);

    return 0;
}
