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
using namespace std;
ifstream  Source;
fstream Intermediate;
ofstream ObjCode, Outcome;

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
		static char* valueGet(char* oneLine){
			
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
	char value[15];
	
	while(!Source.eof()){ //the first line
		Source.getline(instructionBuf,maxLineLen,'\n');
		if(pass1StringProcess::CommentOrBlank(instructionBuf)){
			continue;
		}
		pass1StringProcess::disOneLine(instructionBuf,var,op,value);
		if(!stricmp(op,"START")){
			locationCounter = pass1StringProcess::chars16ToInt(value);
			Intermediate << pass1StringProcess::IntTochars16_L(locationCounter) << "\t" << var << "\t" << op << "\t" << value << endl;
		}
		else{
			locationCounter = 0;
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
	remove(IntermediateFileName);
}
