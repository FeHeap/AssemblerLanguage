#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#define numOfEngAlph 26 //the number of the english alphabet
#define maxVarLen 247
#define maxLineLen 500
#define OPCodeFileName "opcode.txt"
#define SourceFileName "source(comment&blank).txt"
#define IntermediateFileName "Intermediate.txt"
#define OutcomeFileName "outcome.txt"
#define ObjectCodeFileName "objectcode.txt" 
using namespace std;
ifstream  Source;
fstream Intermediate;
ofstream ObjCode, Outcome;

int startAddress,programLength;
char programName[40];

class opcProcess{
	
	private:
		
		struct TableUnit{
			char assemblyInst[8];
			int code;
			struct TableUnit *next;
		}*table;
		
		int disOpcData(char* &opcData){
			int cut;
			for(cut = 0; opcData[cut] != ' '; cut++);
			opcData[cut] = '\0';
			return cut+1;
		}
		
		int chars16ToInt(char* &charArray,int i){
			int num = 0;
			for(; charArray[i] != '\0';i++){
				num *= 16;
				num += (charArray[i] >= '0'&&charArray[i]<='9')?(charArray[i] - '0'):(10 + charArray[i] - 'A');
			}
			return num;
		}
		
		void freeLink(TableUnit* point){
			if(point->next != NULL){
				freeLink(point->next);
			}
			delete point;
		}
		
	public:
		
		opcProcess(){
			table = new struct TableUnit[numOfEngAlph];
			for(int i  = 0; i < numOfEngAlph; i++){
				table[i].code = -1;
				table[i].next = NULL;
			}
		}
		
		~opcProcess(){
			for(int i = 0; i < numOfEngAlph; i++){
				if(table[i].next != NULL)
					freeLink(table[i].next);
			}
			delete [] table;
		}
		
		void put(char* Code){
			int codeStart = disOpcData(Code);
			if(table[Code[0] - 'A'].code == -1){
				strcpy(table[Code[0] - 'A'].assemblyInst,Code);
				table[Code[0] - 'A'].code = chars16ToInt(Code,codeStart);
			}
			else{
				struct TableUnit* point = &table[Code[0] - 'A'];
				while(point->next != NULL){
					point = point -> next;
				}
				point -> next = new struct TableUnit;
				point = point -> next;
				strcpy(point -> assemblyInst,Code);
				point -> code = chars16ToInt(Code,codeStart);
				point -> next = NULL;
			}	
		}
		
		int codeValue(char *instruction){
			struct TableUnit* point = &table[instruction[0] - 'A'];
			while(point != NULL){
				if(!stricmp(point -> assemblyInst,instruction)){
					return point -> code;
				}
				else{
					point = point -> next;
				}
			}
			return -1;
		}
		void viewOpCode(){
			cout << "opcode Table:\n" << endl;
			for(int i = 0; i < numOfEngAlph;i++){
				if(table[i].code != -1){
					struct TableUnit* point = &table[i];
					while(point != NULL){
						printf("%s\t%02X\n",point->assemblyInst,point->code);
						point = point->next;
					}
				}	
			}
		}	
}opcTable;

class symbolProcess{
	
	private:
		
		struct TableUnit{
			char symbol[maxVarLen+1];
			int line;
			struct TableUnit *next;
		}*table;
		
		int chars16ToInt(char* &charArray,int i = 0){
			int num = 0;
			for(; charArray[i] != '\0';i++){
				num *= 16;
				num += (charArray[i] >= '0'&&charArray[i]<='9')?(charArray[i] - '0'):(10 + charArray[i] - 'A');
			}
			return num;
		}
		
		void freeLink(TableUnit* point){
			if(point->next != NULL){
				freeLink(point->next);
			}
			delete point;
		}
		
	public:
		
		symbolProcess(){
			table = new struct TableUnit[numOfEngAlph];
			for(int i  = 0; i < numOfEngAlph; i++){
				table[i].line = -1;
				table[i].next = NULL;
			}
		}
		
		~symbolProcess(){
			for(int i = 0; i < numOfEngAlph; i++){
				if(table[i].next != NULL)
					freeLink(table[i].next);
			}
			delete [] table;
		}
		
		bool put(char* symbol,int line){ //duplicate varible declare return true, else return false
			if(table[(symbol[0] - 'A')%numOfEngAlph].line == -1){
				strcpy(table[(symbol[0] - 'A')%numOfEngAlph].symbol,symbol);
				table[(symbol[0] - 'A')%numOfEngAlph].line = line;
				return false;
			}
			else{
				struct TableUnit* point = &table[(symbol[0] - 'A')%numOfEngAlph];
				while(point->next != NULL){
					if(!strcmp(point->symbol,symbol)){
						return true;
					}
					point = point -> next;
				}
				if(!strcmp(point->symbol,symbol)){
					return true;
				}
				point -> next = new struct TableUnit;
				point = point -> next;
				strcpy(point -> symbol,symbol);
				point -> line = line;
				point -> next = NULL;
				return false;
			}	
		}
		
		int codeValue(char *symbol){ //when find the symbol retrun the position it is, else return -1
			struct TableUnit* point = &table[(symbol[0] - 'A')%numOfEngAlph];
			while(point != NULL){
				if(!stricmp(point -> symbol,symbol)){
					return point -> line;
				}
				else{
					point = point -> next;
				}
			}
			return -1;
		}
		
		void viewSymbols(){
			cout << "Symbol Table:\n" << endl;
			for(int i = 0; i < numOfEngAlph;i++){
				if(table[i].line != -1){
					struct TableUnit* point = &table[i];
					while(point != NULL){
						printf("%s\t%02X\n",point->symbol,point->line);
						point = point->next;
					}
				}	
			}
		}
}symbolTable;

class FSMofInst{
	private:
		struct state{
			char state;
			char contain[maxVarLen+1];
			int top;
		}*point,var_state,op_state,val_state,b1_state,b2_state,E_state;
		
		bool byteFlag;
		
		void set(char C){
			point->contain[point->top++] = C;
			point->contain[point->top] = '\0';
		}
	
	public:
		FSMofInst(){

			var_state.state = 'r';
			
			b1_state.state = 'S';
			
			op_state.state = 'o';
			
			b2_state.state = 'X';
			
			val_state.state = 'l';
			
			E_state.state = 'E';
		}
		void process(char* str){
			int i = 0;
			while(point->state != 'E'){
				switch (point->state){
					case 'r':
						if(str[i] == '\t'||str[i] == ' '){
							point = &b1_state;
						}
						else if(str[i] == '\0'||str[i] == ';'){
							point = &E_state;
						}
						else{
							set(str[i++]);
						}
						break;
					case 'o':
						if(str[i] == '\t'||str[i] == ' '){
							point = &b2_state;
						}
						else if(str[i] == '\0'||str[i] == ';'){
							point = &E_state;
						}
						else{
							set(str[i++]);
						}
						break;
					case 'l':
						if((str[i] == '\t'||str[i] == ' '||str[i] == '\0'||str[i] == ';')&&!byteFlag){
							point = &E_state;
						}
						else if(str[i] == '\''){
							set(str[i++]);
							byteFlag = !byteFlag;
						}
						else{
							set(str[i++]);
						}
						break;
					case 'S':
						if(!(str[i] == '\t'||str[i] == ' ')){
							point = &op_state;
						}
						else{
							i++;
						}
						break;
					case 'X':
						if(str[i] == '\t'||str[i] == ' '){
							i++;
						}
						else if(str[i] == '\0'||str[i] == ';'){
							point = &E_state;
						}
						else{
							point = &val_state;
						}
						break;
						
					default:
						cout << "Error point using!! " << point->state <<endl;
				}
			}
		}
		
		void reset(){
			byteFlag = false;
			point = &var_state;
			var_state.contain[0] = '\0';
			var_state.top = 0;
			op_state.contain[0] = '\0';
			op_state.top = 0;
			val_state.contain[0] = '\0';
			val_state.top = 0;
		}
		void setVar(char* str){
			strcpy(str,var_state.contain);
		}
		void setOp(char* str){
			strcpy(str,op_state.contain);
		}
		void setVal(char* str){
			strcpy(str,val_state.contain);
		}		
}FSM;

class pass1StringProcess{
	public:
		static bool CommentOrBlank(char* oneLine){
			for(int i = 0; i <= strlen(oneLine); i++){
				if(oneLine[i] == ' '||oneLine[i] == '\t'){
					continue;
				}
				if(oneLine[i] == ';'||oneLine[i] == '\0'){
					return true;
				}
				break;
			}
			return false;
		}
		static char* varGet(char* oneLine){
			
		}
		static char* opGet(char* oneLine){
			
		}
		
		static void disOneLine(char* oneLine,char* var,char* op,char* value){
			FSM.reset();
			FSM.process(oneLine);
			FSM.setVar(var);
			FSM.setOp(op);
			FSM.setVal(value);
		}
		static int chars16ToInt(char* charArray){
			int num = 0;
			for(int i = 0; charArray[i] != '\0';i++){
				num *= 16;
				num += (charArray[i] >= '0'&&charArray[i]<='9')?(charArray[i] - '0'):(10 + charArray[i] - 'A');
			}
			return num;
		}
		static char* IntTochars16_L(int num){
			char charArray[5];
			charArray[4] = '\0'; 
			for(int i = 3; i >= 0; i--){
				charArray[i] = (num%16 < 10)?'0'+num%16:'A'-10+num%16;
				num /= 16;
			}
			return charArray;
		}
		static int chars10ToInt(char* charArray){
			int num = 0;
			for(int i = 0; charArray[i] != '\0';i++){
				num *= 10;
				num += charArray[i] - '0';
			}
			return num;
		}
		static bool countAndCheck(int &locationCounter,char* op,char* value){
			if(opcTable.codeValue(op) == -1){
				if(!stricmp("BYTE",op) && strlen(value) != 0){
					if(value[0] == 'X'||value[0] == 'x'){
						locationCounter += 1;
					}
					else if(value[0] == 'C'||value[0] == 'c'){
						locationCounter += 3;
					}
				}
				else if(!stricmp("WORD",op) && strlen(value) != 0){
					locationCounter += 3;
				}
				else if(!stricmp("RESB",op) && strlen(value) != 0){
					locationCounter += 1*chars10ToInt(value);
				}
				else if(!stricmp("RESW",op) && strlen(value) != 0){
					locationCounter += 3*chars10ToInt(value);
				}
				else{
					return true;
				}
			}
			else{
				locationCounter += 3;
			}
			return false;
		}
	
};

class pass2StringProcess{
	public:
		static char* valueGet(char* oneLine){
			char value[maxLineLen+1];
			FSM.reset();
			FSM.process(oneLine);
			FSM.setVal(value);
			return value;
		}
		static char* opGet(char* oneLine){
			char op[8];
			FSM.reset();
			FSM.process(oneLine);
			FSM.setOp(op);
			return op;
		}
		
		
		static char* opLocation(char* oneLine){
			char location[5];
			for(int i = 0;i < 4;i++){
				location[i] = oneLine[i];
			}
			location[4] = '\0';
			return location;
		}
		static void count(int &locationCounter,char* op,char* value){
			if(opcTable.codeValue(op) == -1){
				if(!stricmp("BYTE",op)){
					if(value[0] == 'X'||value[0] == 'x'){
						locationCounter += 1;
					}
					else if(value[0] == 'C'||value[0] == 'c'){
						locationCounter += 3;
					}
				}
				else if(!stricmp("WORD",op)){
					locationCounter += 3;
				}
				else if(!stricmp("RESB",op)){
					locationCounter += 1*pass1StringProcess::chars10ToInt(value);
				}
				else if(!stricmp("RESW",op)){
					locationCounter += 3*pass1StringProcess::chars10ToInt(value);
				}
			}
			else{
				locationCounter += 3;
			}
		}
		static char* IntTochars16_2(int num){
			char charArray[3];
			charArray[2] = '\0'; 
			for(int i = 1; i >= 0; i--){
				charArray[i] = (num%16 < 10)?'0'+num%16:'A'-10+num%16;
				num /= 16;
			}
			return charArray;
		}
		static char* IntTochars16_6(int num){
			char charArray[7];
			charArray[6] = '\0'; 
			for(int i = 5; i >= 0; i--){
				charArray[i] = (num%16 < 10)?'0'+num%16:'A'-10+num%16;
				num /= 16;
			}
			return charArray;
		}
};

class objectCodeProcess{
	private:
		struct instructions{
			char* instruction;
			int location;
		}*pocket;
		
		int top;
		int basicSize;
		
	public:
		objectCodeProcess(){
			top = 0;
			basicSize = 10;
			pocket = new instructions[basicSize];
		}
		~objectCodeProcess(){
			for(int i = 0;i<top;i++){
				delete [] pocket[i].instruction;
			}
			delete [] pocket;
		}
		
		static void	objDispoay(char* instruction){
			char var[maxVarLen+1];
			char op[8];
			char value[maxVarLen+1];
			pass1StringProcess::disOneLine(instruction,var,op,value);
			if(opcTable.codeValue(op) != -1){
				cout << pass2StringProcess::IntTochars16_2(opcTable.codeValue(op));
				
				bool Xflag = false;
				if(value[strlen(value)-2] == ',' && (value[strlen(value)-1] == 'X'||value[strlen(value)-1] == 'x')){
					value[strlen(value)-2] = '\0';
					Xflag = true;
				}
				int val = symbolTable.codeValue(value);
				if(val == -1&&strlen(value) != 0){
					cout << "The varible being used does not exist!" << endl;
					Intermediate.close();
					Outcome.close();
					remove(IntermediateFileName);
					remove(OutcomeFileName);
					exit(0);
				}
				else if(val == -1&&strlen(value) == 0){
					val = 0;
				}
				if(Xflag){
					val += 0x8000;
				}
				printf("%04X",val);
			}
			else{
				if(!stricmp("BYTE",op)){
					if(value[0] == 'X'||value[0] == 'x'){
						printf("%c%c",value[2],value[3]);
					}
					else if(value[0] == 'C'||value[0] == 'c'){
						printf("%02X%02X%02X",value[2],value[3],value[4]);
					}
				}
				else if(!stricmp("WORD",op)){
					printf("%06X",pass1StringProcess::chars10ToInt(value));
				}
				else if(!stricmp("RESB",op)){
					//pass
				}
				else if(!stricmp("RESW",op)){
					//pass
				}
			
			}
		}
		static void	fobjDispoay(char* instruction,ofstream &fout){
			char var[maxVarLen+1];
			char op[8];
			char value[maxVarLen+1];
			pass1StringProcess::disOneLine(instruction,var,op,value);
			if(opcTable.codeValue(op) != -1){
				fout << pass2StringProcess::IntTochars16_2(opcTable.codeValue(op));
				
				bool Xflag = false;
				if(value[strlen(value)-2] == ',' && (value[strlen(value)-1] == 'X'||value[strlen(value)-1] == 'x')){
					value[strlen(value)-2] = '\0';
					Xflag = true;
				}
				int val = symbolTable.codeValue(value);
				if(val == -1&&strlen(value) != 0){
					fout << "The varible being used does not exist!" << endl;
					Intermediate.close();
					Outcome.close();
					remove(IntermediateFileName);
					remove(OutcomeFileName);
					exit(0);
				}
				else if(val == -1&&strlen(value) == 0){
					val = 0;
				}
				if(Xflag){
					val += 0x8000;
				}
				fout << pass1StringProcess::IntTochars16_L(val);
			}
			else{
				if(!stricmp("BYTE",op)){
					if(value[0] == 'X'||value[0] == 'x'){
						fout << value[2] << value[3];
					}
					else if(value[0] == 'C'||value[0] == 'c'){
						char buf0[] = {value[2],'\0'};
						char buf1[] = {value[3],'\0'};
						char buf2[] = {value[4],'\0'};
						fout << pass2StringProcess::IntTochars16_2(value[2]) << pass2StringProcess::IntTochars16_2(value[3]) << pass2StringProcess::IntTochars16_2(value[4]);
					}
				}
				else if(!stricmp("WORD",op)){
					fout << pass2StringProcess::IntTochars16_6(pass1StringProcess::chars10ToInt(value));
				}
				else if(!stricmp("RESB",op)){
					//pass
				}
				else if(!stricmp("RESW",op)){
					//pass
				}
			
			}
		} 
		static char* getInstruction(char* oneLine){
			char lineBuf[maxLineLen+1];
			int j = 0;
			for(int i = 5;oneLine[i] != '\0';i++,j++){
				lineBuf[j] = oneLine[i];
			}
			lineBuf[j] = '\0';
			return lineBuf;
		}
		void push(char* instruction,int location){
			if(top == basicSize){
				struct instructions* temp = pocket;
				basicSize *= 2;
				pocket = new instructions[basicSize];
				for(int i = 0;i < top;i++){
					(pocket+i)->instruction = (temp+i)->instruction;
					(pocket+i)->location = (temp+i)->location;
				}
				delete [] temp;
			}
			pocket[top].instruction = instruction;
			pocket[top].location = location;
			
			top++;
		}
		
		void outputToFile(){
			ObjCode.open(ObjectCodeFileName,ios::out);
			
			ObjCode << "H" << programName << "\t" << pass2StringProcess::IntTochars16_6(startAddress) ;
			ObjCode << " " << pass2StringProcess::IntTochars16_6(programLength) << endl;
			int sum = 0;
			int start = pocket[0].location;
			int startIndex = 0;
			int len = 0;
			char instructionBuf[maxLineLen+1];
			int i, j;
			for(i = 0;i < top;i++){
				sum = pocket[i].location - start;
				if(sum > 27){
					ObjCode << "T" << pass2StringProcess::IntTochars16_6(start);
					ObjCode << " " << pass2StringProcess::IntTochars16_2(len) << " ";
					for(j = startIndex;j<i;j++){
						fobjDispoay(pocket[j].instruction,ObjCode);
						ObjCode << " ";
					}
					ObjCode << endl;
					start = pocket[i].location;
					startIndex = i;
					len = 0;
				}
				strcpy(instructionBuf,pocket[i].instruction);
				pass2StringProcess::count(len,pass2StringProcess::opGet(instructionBuf),pass2StringProcess::valueGet(instructionBuf));
			}
			
			ObjCode << "T" << pass2StringProcess::IntTochars16_6(start);
			ObjCode << " " << pass2StringProcess::IntTochars16_2(len) << " ";
			for(j = startIndex;j<i;j++){
				fobjDispoay(pocket[j].instruction,ObjCode);
				ObjCode << " ";
			}
			ObjCode << endl;
			ObjCode << "E" << pass2StringProcess::IntTochars16_6(startAddress) << endl;
			ObjCode.close();
		}
		
		void output(){

			printf("H%s\t%06X %06X\n",programName,startAddress,programLength);
			
			int sum = 0;
			int start = pocket[0].location;
			int startIndex = 0;
			int len = 0;
			char instructionBuf[maxLineLen+1];
			int i, j;
			for(i = 0;i < top;i++){
				sum = pocket[i].location - start;
				if(sum > 27){
					printf("T%06X %02X ",start,len);
					for(j = startIndex;j<i;j++){
						objDispoay(pocket[j].instruction);
						printf(" ");
					}
					printf("\n");
					start = pocket[i].location;
					startIndex = i;
					len = 0;
				}
				strcpy(instructionBuf,pocket[i].instruction);
				pass2StringProcess::count(len,pass2StringProcess::opGet(instructionBuf),pass2StringProcess::valueGet(instructionBuf));
			}
			
			printf("T%06X %02X ",start,len);
			for(j = startIndex;j<i;j++){
				objDispoay(pocket[j].instruction);
				printf(" ");
			}
			printf("\n");
			printf("E%06X\n",startAddress);
		}
}objTable;

void setOpCode();
void pass1();
void showIntermediate();
void pass2();

int main(void){

	setOpCode();

	pass1();
	
	symbolTable.viewSymbols();
	cout << "\n\n" << endl;
	showIntermediate();
	
	pass2();

    system("PAUSE");
    return 0;
}

void setOpCode(){
	ifstream OpCode;
    OpCode.open(OPCodeFileName,ios::in);
    if(OpCode.is_open()){
    	char charArrayBuf[15];
    	while(!OpCode.eof()){
			OpCode.getline(charArrayBuf,15,'\n');
			opcTable.put(charArrayBuf);
		}
    	OpCode.close();
	}
	else{
		cout << "Fail to open the file OpCode!" << endl;
		exit(0);
	}
}

void pass1(){
	int locationCounter;
	char instructionBuf[maxLineLen+1];
	Source.open(SourceFileName,ios::in);
	Intermediate.open(IntermediateFileName,ios::out);
	
	if(!Source.is_open()){ //check if the file open
		cout << "Fail to open the file Source!" << endl;
		Intermediate.close();
		remove(IntermediateFileName);
		exit(0);
	}
	char var[maxVarLen+1];
	char op[8];
	char value[maxVarLen+1];
	
	while(!Source.eof()){ //the first line
		Source.getline(instructionBuf,maxLineLen,'\n');
		if(pass1StringProcess::CommentOrBlank(instructionBuf)){
			continue;
		}
		pass1StringProcess::disOneLine(instructionBuf,var,op,value);
		if(!stricmp(op,"START")){
			strcpy(programName,var);
			startAddress = locationCounter = pass1StringProcess::chars16ToInt(value);
			Intermediate << pass1StringProcess::IntTochars16_L(locationCounter) << "\t" << var << "\t" << op << "\t" << value << endl;
		}
		else{
			programName[0] = '\0';
			startAddress = locationCounter = 0;
			Source.close();
			Source.open(SourceFileName,ios::in);
		}
		break;
	}
	
	while(!Source.eof()){
		Source.getline(instructionBuf,maxLineLen,'\n');
		if(pass1StringProcess::CommentOrBlank(instructionBuf)){
			continue;
		}
		pass1StringProcess::disOneLine(instructionBuf,var,op,value);
		if(!stricmp(op,"END"))
			break;
		Intermediate << pass1StringProcess::IntTochars16_L(locationCounter) << "\t" << var << "\t" << op << "\t" << value << endl;
		if(var[0] != '\0'){
			if(!((var[0] >= 'A'&&var[0]<='Z')||var[0] == '_'||var[0] == '?'||var[0] =='$')||strlen(var) > maxVarLen){
				cout << "valid varible declaration!" << endl;
				Source.close();
				Intermediate.close();
				remove(IntermediateFileName);
				exit(0);
			}else if(var[0] =='@'){
				cout << "dangerous varible declaration!" << endl;
				Source.close();
				Intermediate.close();
				remove(IntermediateFileName);
				exit(0);
			}
			else if(symbolTable.codeValue(var) == -1){
				symbolTable.put(var,locationCounter);
			}
			else{
				cout << "duplicate varible declaration!" << endl;
				Source.close();
				Intermediate.close();
				remove(IntermediateFileName);
				exit(0);
			}
		}
		if(pass1StringProcess::countAndCheck(locationCounter, op, value)){
			cout << "valid instruction!" << endl;
			Source.close();
			Intermediate.close();
			remove(IntermediateFileName);
			exit(0);	
		}
		
	}
	
	Intermediate << "\t" << var << "\t" << op << "\t" << value << endl; //END line
	programLength = locationCounter - startAddress;
	
	Source.close();
	Intermediate.close();
}
void showIntermediate(){
	cout << "Intermediate file:\n" << endl;
	Intermediate.open(IntermediateFileName,ios::in);
	while(!Intermediate.eof()){
		char charArrayBuf[maxLineLen+1];
		Intermediate.getline(charArrayBuf,maxLineLen,'\n');
		cout << charArrayBuf << endl;
	}
	Intermediate.close();
}

void pass2(){
	Intermediate.open(IntermediateFileName,ios::in);
	Outcome.open(OutcomeFileName,ios::out);
	
	char* opBuf;
	while(!Intermediate.eof()){
		char charArrayBuf[maxLineLen+1];
		Intermediate.getline(charArrayBuf,maxLineLen,'\n');
		if(strlen(pass2StringProcess::valueGet(objectCodeProcess::getInstruction(charArrayBuf))) < 8){
			Outcome << charArrayBuf << "\t\t";
		}
		else{
			Outcome << charArrayBuf << "\t";
		}
		char* instructionBuf = new char[maxLineLen+1];
		strcpy(instructionBuf,objectCodeProcess::getInstruction(charArrayBuf));
		opBuf = pass2StringProcess::opGet(instructionBuf);
		if(stricmp("RESW",opBuf) && stricmp("RESB",opBuf) && stricmp("START",opBuf) && stricmp("END",opBuf)){
			objTable.push(instructionBuf,pass1StringProcess::chars16ToInt(pass2StringProcess::opLocation(charArrayBuf)));
		}
		objectCodeProcess::fobjDispoay(instructionBuf,Outcome);
		Outcome << endl;
	}
	Outcome.close();
	Intermediate.close();
	remove(IntermediateFileName);
	objTable.outputToFile();
	objTable.output();
}
