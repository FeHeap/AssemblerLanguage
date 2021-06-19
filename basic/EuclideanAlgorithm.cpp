#include <iostream>
#include <cstdlib>
#include <cstring>
using namespace std;

bool isPositiveInt(char*);
void swap(int&,int&);
int GCD(int,int);
long int LCM(int,int);

int main(void){

    long int num1,num2;
 	char input[30]; 
    cout << "Input two positive integers,and it will output their GCD and LCM." << endl;
    while(true)
	{
    	cout << "The first number:";
    	cin.getline(input,29);
    	if(isPositiveInt(input)){
			break;
		}
		else{
			cout << "Input error!" << endl;
		}
	}
	num1 = atoi(input);
	
	while(true)
	{
    	cout << "The second number:";
    	cin.getline(input,29);
    	if(isPositiveInt(input)){
			break;
		}
		else{
			cout << "Input error!" << endl;
		}
	}
	num2 = atoi(input);
	
	cout << "- - - - - - - - - - - - - - - -" << endl;
	cout << "The GCD of "<< num1 << " and " << num2 << " is " <<  GCD(num1,num2) << endl;
	cout << "The LCM of "<< num1 << " and " << num2 << " is " <<  LCM(num1,num2) << endl;
	cout << endl;
	
    system("PAUSE");
    return 0;
}

bool isPositiveInt(char* input){ //determine whether input is a positive integer
	if(strlen(input) > 10){
		return false;
	}
	for(int i = 0;i < strlen(input);i++){
		if(!(input[i]>='0' && input[i]<='9')){
			return false;
		}
	}
	char maxOfInteger[] = "2147483647";
	if(strlen(input) == strlen(maxOfInteger)){
		for(int i = 0;i<strlen(maxOfInteger);i++){
			if(input[i] > maxOfInteger[i]){
				return false;
			}
		}
	}
	
	return true;
}

void swap(int& A,int& B){
	
	A = A ^ B;
	B = A ^ B;
	A = A ^ B;
}

int GCD(int A,int B){ //count the GCD of A and B by Euclidean Algorithm
	if(A < B){
		swap(A,B);
	}
	while(A!=0 && B!=0){
		A = A % B;
		swap(A,B);
	}
	
	return A;
}

long int LCM(int A,int B){
	long int lcm = (long int)A;
	lcm /= GCD(A,B);
	lcm *= B;
	
	return lcm;
}



