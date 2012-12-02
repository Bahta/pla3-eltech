#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <fstream>
#include <string>
enum code {encode, decode};

	void help(){
		std::cout << '\n';	
		std::cout << "$ crypt [options] <key> <source> [<dest>]" <<'\n';
		std::cout << '\n';
		std::cout << "options:" <<'\n';
		std::cout << "    -a, --alphabet=<alphabet>" <<'\n';
		std::cout << "    -t, --type=<type>" <<'\n';
		std::cout << "    -h, --help" <<'\n';
	}

	//caesar cipher: encode array by key and alphabet
	int encoding(std::string &array, std::string &alphabet, const int &key){
		int length = array.size();
		int cardinality = alphabet.size();
		for (int i = 0; i < length; ++i){
			for (int j = 0; j < cardinality; ++j){
				//optimisation - dispose alphabet by frequency of use
				if (array[i] == alphabet[j]) {
					array[i] = alphabet[(j+key)%cardinality];
					break; 
				}
			}
		}
	}

	//caesar cipher: decode array by key and alphabet
	void decoding(std::string &array, std::string &alphabet, const int &key){
		int length = array.size();
		int cardinality = alphabet.size();
		for (int i = 0; i < length; ++i){
			for (int j = 0; j < cardinality; ++j){
				//optimisation - dispose alphabet by frequency of use
				if (array[i] == alphabet[j]) {
					array[i] = alphabet[(j-key+cardinality)%cardinality];
					break;
				}
			}
		}
	}

	//mode("type") recognition by knowledge of his position in argv
	void type_input(const int &inputTypeAddr, const int &typeLongRequest_offset, code &mode, char **argv){
		//not equal,only contains
		if (strcmp(&argv[inputTypeAddr][typeLongRequest_offset], "encode")==0) {
			mode = encode;
		}else {
			if (strcmp(&argv[inputTypeAddr][typeLongRequest_offset], "decode")==0) {
				mode = decode;
			}else {
				printf("%s", "incorrect mode");
			}
		}
	}

	//alphabet recognition by his position in argv or default value
	void alphabet_input(char **argv, const bool &isInputAlphabet, const int &inputAlphabetAddr, const int &alphabetLongRequest_offset, std::string &alphabet){
		if (isInputAlphabet) {
			std::string argvS = argv[inputAlphabetAddr];
			int cardinality = argvS.size() - alphabetLongRequest_offset; //+-1 ?
			alphabet = argvS.substr(alphabetLongRequest_offset, cardinality);
		}else {
			//default alphabet
			alphabet="";
			//fills alphabet Aa...Zz0..9
			for (int i = 0; i < 26; ++i) {
				alphabet.insert(2*i,1,65+i);
				alphabet.insert(2*i+1,1,97+i);
			}for (int i=0; i<10; ++i){
				alphabet.insert(52+i,1,48+i);
			}
		}	
	}

void read_parameters(const int &argc, char **argv, int &lastParameterPosition, code &mode, std::string &alphabet, int &readError) {
	bool isInputAlphabet = false;
	bool isInputType = false;
	int inputAlphabetAddr;
	int alphabetLongRequest_offset;
	int inputTypeAddr;
	int typeLongRequest_offset;
	//default, if no parameters
	lastParameterPosition = 1;
	//parameters analyzing;
	for (int i = 1; i < argc; ++i) {
		//alphabet
		if (strncmp(argv[i], "-a", 2) == 0) {
			++i;
			//if nothing after "-a" or at once "-", what means new parameter --> incorrect input,error
			if (!((i==argc)||(argv[i][0]=='-'))) {
				isInputAlphabet = true;
				inputAlphabetAddr = i;
				alphabetLongRequest_offset = 0;
				//+1 to last parameter position, "crypt -a abc 5 in.txt" for example
				lastParameterPosition = i+1; 
				continue;
			}else {
				//look up
				readError = 1;
				break;
			}
		}						
		//type
		if (strncmp(argv[i], "-t",2)==0) {
			++i;
			//if nothing after "-t" or at once "-", what means new parameter --> incorrect input,error
			if (!((i==argc)||(argv[i][0]=='-'))) {
				isInputType = true;
				inputTypeAddr = i;
				typeLongRequest_offset = 0;
				lastParameterPosition = i+1;
				continue;
			}else {
				//look up
				readError = 2;
				break;
			}
		}
		
		//@needIdea strange thing - i can use it in middle of request
		if (strncmp(argv[i], "-h", 2)==0) {
			help();	
			break;
		}

		//full names
		//long input alphabet
		if (strncmp(argv[i],"--alphabet",10)==0) { 
			isInputAlphabet = true;
			inputAlphabetAddr = i;
			alphabetLongRequest_offset = 11;
			++i;
			lastParameterPosition = i;
			continue;
		}	
									
		//long input type
		if (strncmp(argv[i],"--type",6)==0) {
			inputTypeAddr = i;
			typeLongRequest_offset = 7;
			++i;
			lastParameterPosition = i;
			isInputType = true;
			continue; 
		}
		
		//long input help		
		if (argv[i] == "--help") {
			help();
			break;
		}

		//if parameters not found
		//@needIdea does it work? 
		if (argv[i][0]=='-'){
			readError = 6;
			break;
		}

	}	
	

	if (isInputType) { type_input(inputTypeAddr, typeLongRequest_offset, mode, argv);}
	alphabet_input(argv, isInputAlphabet, inputAlphabetAddr, alphabetLongRequest_offset, alphabet);	
}				


	void read_key_source_dest(const int &position, const int &argc, char **argv, int &key, int &source_addr, int &dest_addr, int &readError) {
			//from last parameter location
			for (int j=position; j<argc; ++j) {
				key = atoi(&argv[j][0]);
				++j;
				//source name  +  dest name (if exists, else dest=source)
				source_addr=j;
				++j;
				if (j < argc) {
					dest_addr=j;
				}else {
					dest_addr=source_addr;
				}				
			}
	}


int main(int argc,char **argv) {
	//too short input
	if (argc<3) {
		std::cout <<"incorrect input" <<'\n' <<"use help: -h / --help";
		return 1;
	}
	//mode: encoding/decoding
	code mode = encode;
	//key for caesar cipher
	int key;
	//place, where finish parameters and starts key&source...
	int lastParameterPosition;
	//name of source file addr in argv
	int source_addr;
	//name of dest file addr in argv
	int dest_addr;
	//errors collector
	int readError=0;

	//text
	std::string array;
	//alphabet
	std::string alphabet;



	read_parameters(argc, argv, lastParameterPosition, mode, alphabet, readError);
	read_key_source_dest(lastParameterPosition,argc,argv,key,source_addr,dest_addr, readError);

	//checking key for errors
	if (key<0) { readError = 4;} else 
		if (key==0) readError = 5;
	//readError processing 	
	if (readError != 0) {
		std::cout <<"incorrect parameters" <<'\n' <<"use help: -h / --help";
		//make case if you want
		//1 - alphabet,
		//2 - type,
		//4 -k<0,
		//5 - k=0 --> not digits,
		//6 - parameters not found
		return 1;
	}

	std::ifstream source(argv[source_addr]);
	std::ofstream dest(argv[dest_addr]);	
	//up to the eof
	getline(source, array, '\1');

	std::cout << array <<'\n';

	if (mode == encode) {
		encoding(array,alphabet, key);
	}else {
		decoding(array,alphabet, key);
	}

		std::cout << array <<'\n';

	dest << array <<'\n';

	return 0;
}	