#include <iostream>
#include <cstdlib>
#include <string>
using namespace std;


class checkerboardunit{
	private:
		string state;
	public:
		checkerboardunit(){
			state = "  "; 
		}
		void setState(bool piece){
			if(piece){
				this->state = "¡´";
			}
			else{
				this->state = "¡³";
			}
		}
		string getState(){
			return state;
		}
		
};

class checkerboard{
	private:
		checkerboardunit board[8][8];
		bool turn = true;
		/*true :¡´
		  false:¡³*/
		short pieces = 4;
		
	public: 
		
		checkerboard(){
			board[3][3].setState(true);
			board[3][4].setState(false);
			board[4][3].setState(false);
			board[4][4].setState(true);
		}
		
		view(){
			for(int i = 0;i < 8;i++){
				cout << "-------------------------" << endl;
				for(int j = 0;j < 8;j++){
					cout << "|" << board[i][j].getState();
				}
				cout << "|" << endl;
			}
			cout << "-------------------------" << endl;
		}

};



int main(){
	
	
	checkerboard game;
	
	game.view();
	
	
	system("PAUSE");
	return 0;
}
