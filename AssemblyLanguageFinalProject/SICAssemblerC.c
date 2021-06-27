#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define opcodeFileName "opcode.txt"
#define sourceFileName "source.txt"
#define intermediateFileName "intermediate.txt"
#define LLSO "LLSO.txt"
#define objcodeFileName "objcode.txt"
#define symbolTableSize 100
int mapSize;


FILE *input,*output;
struct opcUnit{
	char opc[10];
	char value[3];
	struct opcUnit* next;
}**opTable;

struct symbolUnit{
	char symbol[20];
	char position[5];
	struct symbolUnit* next;
}**symbolTable;

void setopTable();
void freeOptable();
void freeOpUnit(struct opcUnit*);
int hashcode(char*,int);
void pushOP(char*,char*);
char* getOPvalue(char*);

void pass1();
void setsymbolTable();
int pushSYM();
void freeSymbolTable();
void freeSymbolUnit(struct symbolUnit*);
int sXtoD(char*);
int DXtoD(char*);
char* DtosX(int);
int ADD(char*,char*);
void clearBuf(char*,char*,char*);
void showSymbles();

void pass2();
void getAll(char*,char*,char*,char*,char*);
char* getSP(char*);
void opcfprintfProcess(char*,char*,char*);
struct link{
	int position;
	char varible_d[20];
	char opc[10];
	char varible_u[20];
	struct link* next;
}*front,*rear;
void addq(char*,char*,char*,char*);
void deleteq();
void deleteAllq();

/*main function*/
int main(void){
	
	
	setopTable();
    pass1();
    showSymbles();
    
    pass2();
    
	freeOptable();
	freeSymbolTable();
	deleteAllq();
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


//<------About opcode
void setopTable(){
	mapSize = 0; 
	char lineBuf[20];
	char chBuf;
	if((input = fopen(opcodeFileName,"r")) == NULL){
		printf("Fail to open opcode!");
		exit(0);
	}
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
    	pushOP(opc,value);
	}while((chBuf = fgetc(input)) != EOF);
	fclose(input);
}
void pushOP(char* opc,char* value){
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

void setsymbolTable(){
	
	symbolTable = (struct symbolUnit**)malloc(symbolTableSize*sizeof(struct symbolUnit*));
	int i;
	for(i = 0;i < symbolTableSize;i++){
		*(symbolTable + i) = NULL;
	}

}

int exist(char* opc){
	int index = hashcode(opc,mapSize);
	struct opcUnit* point = *(opTable + index);
	while(point != NULL){
		if(!stricmp(opc,point->opc)){
			return 1;
		}
		point = point->next;
	}
	return 0;
}

char* getOPvalue(char* opcode){
	int index = hashcode(opcode,mapSize);
	struct opcUnit* point = *(opTable + index);
	while(1){
		if(!stricmp(point->opc,opcode)){
			return point->value;
		}
		point = point->next;
	}
}
//------>About opcode

//<------About symbol
int pushSYM(char* symbol,char* position){
	int index = hashcode(symbol,symbolTableSize);
	struct symbolUnit* point;
	if(*(symbolTable + index) == NULL){
		*(symbolTable + index) = (struct symbolUnit*)malloc(sizeof(struct symbolUnit));
		point = *(symbolTable + index);
	}
	else{
		point = *(symbolTable + index);
		if(!strcmp(symbol,point->symbol)){
			return 1;
		}
		while(point->next != NULL){
			if(!strcmp(symbol,point->symbol)){
				return 1;
			}
			point = point->next;
		}
		point->next = (struct symbolUnit*)malloc(sizeof(struct symbolUnit));
		point = point->next;
	}
	strcpy(point->symbol,symbol);
	strcpy(point->position,position);
	point->next = NULL;
	return 0;
}

void freeSymbolTable(){
	struct symbolUnit* point;
	int i;
	for(i = 0;i<symbolTableSize;i++){
		if(*(symbolTable + i) == NULL)
			continue;
		point = *(symbolTable + i);
		freeSymbolUnit(point);
	}
	free(symbolTable);
}
void freeSymbolUnit(struct symbolUnit* point){
	if(point->next != NULL){
		freeSymbolUnit(point->next);
	}
	free(point);
}
void showSymbles(){
	struct symbolUnit* point;
	int i;
	printf("symble table:\n\n");
	for(i = 0;i<symbolTableSize;i++){
		point = *(symbolTable + i);

		while(point != NULL){
			printf("%s\t%s\n",point->symbol,point->position);
			point = point->next;
		}
	}
}
char* getSP(char* symbol){
	int index = hashcode(symbol,symbolTableSize);
	struct symbolUnit* point = *(symbolTable + index);
	while(point != NULL){
		if(!stricmp(point->symbol,symbol)){
			return point->position;
		}
		point = point->next;
	}
	
	printf("the varible doesn't exist'!\n");
	freeOptable();
	freeSymbolTable();
	deleteAllq();
	exit(0);
}

//------>About symbol

//<------About translation
int sXtoD(char* sX){
	int i;
	int num;
	for(num = 0,i = 0;i < strlen(sX);i++){
		num *= 16;
		num += (sX[i]>='0'&&sX[i]<='9')?sX[i]-'0':sX[i]+10-'A';
	}
	return num;
}
int sDtoD(char* sD){
	int i;
	int num;
	for(num = 0,i = 0;i < strlen(sD);i++){
		num *= 10;
		num += sD[i]-'0';
	}
	return num;
}

char* DtosX(int D){
	char sX[5];
	int i;	
	for(i = 0;i < 4;i++){
		sX[3 - i] = (D%16 < 10)?'0'+D%16:'A'-10+D%16;
		D /= 16;
	}
	sX[4] = '\0';
	return sX;
}
//------>About translation

void clearBuf(char* varible_d,char* opc,char* varible_u){
	varible_d[0] = '\0';
	opc[0] = '\0';
	varible_u[0] = '\0';
}

char BWtabke[][5] = {"BYTE","WORD","RESB","RESW"};

int ADD(char* opc,char* varible_u){
	if(exist(opc)){
		return 3;
	}
	else{
		int i;
		for(i = 0;i < 4;i++){
			if(!stricmp(BWtabke[i],opc))
				break;
		}
		switch (i){
			case 0:
				if(varible_u[0] == 'X'||varible_u[0] == 'x'){
					return 1;
				}else if(varible_u[0] == 'C'||varible_u[0] == 'c'){
					return 3;
				}
			case 1:
				return 3;
			case 2:
				return sDtoD(varible_u);
			case 3:
				return 3*sDtoD(varible_u);
			default:
				return -1;
		}
	}	
}

char program[30] = {};
int start;
int programEnd;
int programLength;

void pass1(){
	int LC;
	
	setsymbolTable();
	if((input = fopen(sourceFileName,"r")) == NULL){
		printf("Fail to open source!");
		exit(0);
	}
	output = fopen(intermediateFileName,"w");
	
	char Buf[50];
	char chBuf;
	char varible_d[20];
    char opc[10];
    char varible_u[20];
    do{
		
    	fscanf(input,"%[^ \t;\n]",varible_d);
    	fscanf(input,"%[ \t]",Buf);
    	fscanf(input,"%[^ \t;\n]",opc);
    	fscanf(input,"%[ \t]",Buf);
    	fscanf(input,"%[^ \t;\n]",varible_u);
    	fscanf(input,"%[^\n]",Buf);
	}while((chBuf = fgetc(input)) != EOF&&strlen(opc) == 0);
	
	if(!stricmp(opc,"START")){
		strcpy(program,varible_d);
		start = LC = sXtoD(varible_u);
		fprintf(output,"%04X\t%s\t%s\t%s\n",LC,varible_d,opc,varible_u);
	}
	else{
		LC = 0;
		fclose(input);
		input = fopen(sourceFileName,"r");
	}
	
	clearBuf(varible_d,opc,varible_u);
	
	do{
    	fscanf(input,"%[^ \t;\n]",varible_d);
    	fscanf(input,"%[ \t]",Buf);
    	fscanf(input,"%[^ \t;\n]",opc);
    	fscanf(input,"%[ \t]",Buf);
    	fscanf(input,"%[^ \t;\n]",varible_u);
    	fscanf(input,"%[^\n]",Buf);
		
    	if(!stricmp("END",opc)){
    		break;
		}
    	if(strlen(opc) != 0){
    		fprintf(output,"%04X\t%s\t%s\t%s\n",LC,varible_d,opc,varible_u);
		}
		else{
			continue;
		}
		
		if(strlen(varible_d) != 0){
			if(pushSYM(varible_d,DtosX(LC))){
				printf("The varible has already exist!\n");
				freeOptable();
				freeSymbolTable();
				exit(0);
			}
		}

		int add = ADD(opc,varible_u);
		if(add == -1){
			printf("invalid instruction!\n");
			freeOptable();
			freeSymbolTable();
			exit(0);
		}
		LC += add;
		clearBuf(varible_d,opc,varible_u);
	}while((chBuf = fgetc(input)) != EOF);
	fprintf(output,"\t\t%s\t%s\n",opc,varible_u);
	
	programEnd = LC;
	programLength = programEnd - start;
	
	fclose(input);
	fclose(output);
}

void getAll(char* instruction,char* location,char* varible_d,char* opc,char* varible_u){
	if(instruction[4] == '\t'&&instruction[5] == '\t'){
		int i = 0,j = 0;
		for(;instruction[i] != '\t'&&instruction[i] != ' ';i++,j++){
			location[j] = instruction[i];
		}
		location[j] = '\0';
		varible_d[0] = '\0';
		i = 6,j = 0;
		for(;instruction[i]!='\t'&&instruction[i]!=' ';i++,j++){
			opc[j] = instruction[i];
		}
		opc[j] = '\0';
		for(;instruction[i] =='\t'||instruction[i]==' ';i++);
		j = 0;
		for(;instruction[i]!='\t'&&instruction[i]!=' '&&instruction[i]!='\0';i++,j++){
			varible_u[j] = instruction[i];
		}
		varible_u[j] = '\0';
	}
	else{
		int i = 0,j = 0;
		for(;i < 4;i++,j++){
			location[j] = instruction[i];
		}
		location[j] = '\0';
		
		i = 5,j = 0;
		for(;instruction[i]!='\t'&&instruction[i]!=' ';i++,j++){
			varible_d[j] = instruction[i];
		}
		varible_d[j] = '\0';
		
		for(;instruction[i] =='\t'||instruction[i]==' ';i++);
		
		j = 0;
		for(;instruction[i]!='\t'&&instruction[i]!=' ';i++,j++){
			opc[j] = instruction[i];
		}
		opc[j] = '\0';
		
		for(;instruction[i] =='\t'||instruction[i]==' ';i++);
		
		j = 0;
		for(;instruction[i]!='\t'&&instruction[i]!=' '&&instruction[i]!='\0';i++,j++){
			varible_u[j] = instruction[i];
		}
		varible_u[j] = '\0';
	}
}

void addq(char* location,char* varible_d,char* opc,char* varible_u){
	if(front == NULL){
		front = rear = (struct link*)malloc(sizeof(struct link));
		rear->position = sXtoD(location);
		strcpy(rear->varible_d,varible_d);
		strcpy(rear->opc,opc);
		strcpy(rear->varible_u,varible_u);
		rear->next = NULL;
	}
	else{
		rear->next = (struct link*)malloc(sizeof(struct link));
		rear = rear->next;
		rear->position = sXtoD(location);
		strcpy(rear->varible_d,varible_d);
		strcpy(rear->opc,opc);
		strcpy(rear->varible_u,varible_u);
		rear->next = NULL;
	}
}

void deleteq(){
	struct link* temp = front;
	front = front->next;
	free(temp);
}
void deleteAllq(){
	if(front != NULL){
		deleteq();
	}
}

void opcfprintfProcess(char* varible_d,char* opc,char* varible_u){
	if(exist(opc)){
		fprintf(output,"%s",getOPvalue(opc));
		if(strlen(varible_u) == 0){
			fprintf(output,"0000");
		}
		else if(varible_u[strlen(varible_u)-2] == ','&&(varible_u[strlen(varible_u)-1] == 'X'||varible_u[strlen(varible_u)-1] == 'x')){
			char varible_u_buf[20];
			strcpy(varible_u_buf,varible_u);
			varible_u_buf[strlen(varible_u_buf)-2] = '\0';
			char positionBuf[20];
			strcpy(positionBuf,getSP(varible_u_buf));
			int i;
			for(i = 0;i < 8;i++){
				if(positionBuf[0] <= '8'){
						positionBuf[0] += 1;
				}
				else if(positionBuf[0] == '9'){
					positionBuf[0] = 'A';
				}
				else{
					positionBuf[0] += 1;
					}
			}
			
			fprintf(output,"%s",positionBuf);
		}
			else{
			fprintf(output,"%s",getSP(varible_u));
		}
	}
	else{
		int i;
		for(i = 0;i < 4;i++){
			if(!stricmp(BWtabke[i],opc))
				break;
		}
		switch (i){
			case 0:
					if(varible_u[0] == 'X'||varible_u[0] == 'x'){
					fprintf(output,"%c%c",varible_u[2],varible_u[3]);
				}else if(varible_u[0] == 'C'||varible_u[0] == 'c'){
					fprintf(output,"%02X%02X%02X",varible_u[2],varible_u[3],varible_u[4]);
				}
				break;
			case 1:
					fprintf(output,"%06X",sDtoD(varible_u));
				break;
			case 2:
			case 3:
			default:
				break;
		}
	}
	
}

void pass2(){
	input = fopen(intermediateFileName,"r");
	output = fopen(LLSO,"w");
	char location[5];
	char Buf[50];
	char chBuf;
	char varible_d[20];
    char opc[10];
    char varible_u[20];
    char instruction[100];
    
    fscanf(input,"%[^\n]",instruction);
   	fprintf(output,"%s\n",instruction);
   	instruction[0] = '\0';
   	chBuf = fgetc(input);
   	
   	front = NULL;
   	rear = NULL;
   	
    do{
    	fscanf(input,"%[^\n]",instruction);
    	
    	getAll( instruction, location, varible_d, opc, varible_u);
		if(instruction[0] == '\t')
    		break;
    	if(strlen(varible_u) < 8){
    		fprintf(output,"%s\t\t",instruction);
		}
    	else{
    		fprintf(output,"%s\t",instruction);
		}
		
		if(stricmp(opc,"RESW")&&stricmp(opc,"RESB")){
			addq(location,varible_d,opc,varible_u);
		}
    	opcfprintfProcess(varible_d,opc,varible_u);
    	
		fprintf(output,"\n");
    	instruction[0] = '\0';
	}while((chBuf = fgetc(input)) != EOF);
	
	fprintf(output,"%s\n",instruction);
	
	fclose(input);
	fclose(output);
	
	output = fopen(objcodeFileName,"w");
	fprintf(output,"H%s\t%06X %06X",program,start,programLength);
	int sum = 0;
	int len = 0;
	struct link* point = front;
	int ST = front->position;
	fprintf(output,"\nT%06X",point->position);
	for(;point->next != NULL;point = point->next){
		sum = point->position - ST;
		if(sum > 27){
			fprintf(output," %02X",len);
			while(front != point){
				fprintf(output," ");
				opcfprintfProcess(front->varible_d,front->opc,front->varible_u);
				deleteq();
			}
			
			ST = front->position;
			point = front;
			len = 0;
			fprintf(output,"\nT%06X",point->position);
		}
		len += ADD(point->opc,point->varible_u);
	}
	len += ADD(point->opc,point->varible_u);
	fprintf(output," %02X",len);
	while(front != NULL){
		fprintf(output," ");
		opcfprintfProcess(front->varible_d,front->opc,front->varible_u);
		deleteq();
	}
	fprintf(output,"\nE%06X",start);
	
	fclose(output);
}
