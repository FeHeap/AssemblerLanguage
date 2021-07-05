#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

template <class T>
void shuffle(T &Array){	//Knuth shuffle
	
	srand(time(NULL));
	int range = sizeof(Array)/sizeof(Array[0]); 
	for(int i = 1,buf; i < 100;i++){
		buf = rand()%range;
		swap(Array[buf],Array[range-i]);
	}
}


template <class T>
void swap(T *a, T *b){
	*a = *a ^ *b;
	*b = *a ^ *b;
	*a = *a ^ *b;
}


int main(){
	
	int Array[100];
	for(int i = 0; i < 100; i++){
		Array[i] = i;
	}
	
	shuffle(Array); //shuffle(only array!)

	for(int i = 0; i < 100; i++){
		cout << i << ":" << Array[i] << endl;
	}
	cout << endl;
	
	system("PAUSE");
	return 0;
}

