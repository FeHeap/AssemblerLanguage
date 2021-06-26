#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
using namespace std;
ifstream read;

Typedef struct node node;
struct node{
	char ASCII;
	node* left_0;
	node* right_1;
};


Typedef struct tableBucket ASCIIbucket;
struct tableBucket{
	char ASCII;
	int frequence;
};



class HuffmanTree{
	
	private:
	node* root;
	ASCIIbucket charTable[128];
	
	
	
	public:
	HuffmanTree(){
		for(int i = 0;i < 128;i++){
			charTable[i].ASCII = i;
			charTable[i].frequence = 0;
		}
	}
	
	
	
};




int main(){
	string fileName;
	
	cout << "It's an application of Huffman Coding in compression." <<endl;
	cout << "Input the file name:";
	(cin >> fileName).get();
	read.open(fileName,ios::in);
	if(read.is_open()){
		
	}
	else{
		cout << "Fail to open the file!" << endl;
		read.close();
		exit(0);
	}
	
	
	system("PAUSE");
	return 0;
}

