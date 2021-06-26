#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define opcodeFileName "opcode.txt"
#define sourceFileName "source.txt"
#define intermediateFileName "intermediate.txt"
#define LLSO "LLSO.txt"
#define objcodeFileName "objcode.txt"
int mapSize;

FILE *input,*output;
struct opcUnit{
	char opc[10];
	char value[3];
	struct opcUnit* next;
}**opTable;

void setopTable();
void freeOptable();
void freeOpUnit(struct opcUnit*);
int hashcode(char*,int);
void push(char*,char*);

void pass1();
void pass2();

int main(void){
	setopTable();
    pass1();
    pass2();
    
    
    
    
    
    
	freeOptable();
	
    system("PAUSE");
    return 0;
}

int hashcode(char* string, int Size){
	int i;
	int hash = 0;
	int len = strlen(string)-1;
	for(i = 0; i < strlen(string); i++){
		hash += string[i]*31^(len-i);
	}
	hash = abs(hash)%Size;
	return hash;
}

void setopTable(){
	mapSize = 0; 
	char lineBuf[20];
	char chBuf;
	input = fopen(opcodeFileName,"r");
	do{
    	fscanf(input,"%[^\n]",lineBuf);
    	mapSize++;
	}while((chBuf = fgetc(input)) != EOF);
	fclose(input);
	opTable = (struct opcUnit**)malloc(mapSize*sizeof(struct opcUnit*));
	int i;
	for(i = 0;i < mapSize;i++){
		*(opTable + i) = NULL;
	}
	input = fopen(opcodeFileName,"r");
    
    char blankBuf[10];
    char opc[10];
    char value[3];
	do{
    	fscanf(input,"%[^ \t]",opc);
    	fscanf(input,"%[ \t]",blankBuf);
    	fscanf(input,"%[^\n]",value);
    	push(opc,value);
	}while((chBuf = fgetc(input)) != EOF);
	fclose(input);
}
void push(char* opc,char* value){
	int index = hashcode(opc,mapSize);
	struct opcUnit* point;
	if(*(opTable + index) == NULL){
		*(opTable + index) = (struct opcUnit*)malloc(sizeof(struct opcUnit));
		point = *(opTable + index);
	}
	else{
		point = *(opTable + index);
		while(point->next != NULL){
			point = point->next;
		}
		point->next = (struct opcUnit*)malloc(sizeof(struct opcUnit));
		point = point->next;
	}
	strcpy(point->opc,opc);
	strcpy(point->value,value);
	point->next = NULL;
}

void freeOptable(){
	struct opcUnit* point;
	int i;
	for(i = 0;i<mapSize;i++){
		if(*(opTable + i) == NULL)
			continue;
		point = *(opTable + i);
		freeOpUnit(point);
	}
	free(opTable);
}
void freeOpUnit(struct opcUnit* point){
	if(point->next != NULL){
		freeOpUnit(point->next);
	}
	free(point);
}



void pass1(){
	
	
	
	
}

void pass2(){
	
	
	
}
