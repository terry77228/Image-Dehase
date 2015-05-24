#include <cstdio>
#include <cstdlib>
#include "Dehaze.h"

int main(int argc, char ** argv){

	
	
	do{
		if (argc < 3){
			std::cout << "Usage: Image_Dehaze Input_name Output_name" << endl;
			break;
		}
		ImageDehazer dehazer;
		if (!dehazer.LoadImage(argv[1])){
			std::cout << "Load image failed" << endl;
			break;
		}

		if(!dehazer.Dehaze(3,0.1,0.95)){
			std::cout << "Dehaze failed" << endl;
			break;
		}
		if (!dehazer.WriteImage(argv[2])){
			std::cout << "Write image failed" << endl;
		}
	} while (false);

	return 0;
}