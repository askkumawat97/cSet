
/*"############################################################################
EXECUTION:
	gcc huffman3.c
	./a.out FILENAME.EXT com
		it creates compressed file with name FILENAME.EXT#
	./a.out FILENAME.EXT# decom
		it decompress the file with name FILENAME#.EXT
############################################################################"*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<sys/stat.h>
#include<fcntl.h> //open()
#include<unistd.h> //read(),write(),close()


struct stat st; //find _fileSize

//store the addresses of leafNodes of minheap for backtrace the tree to find _huffmanCode
struct huffmanLeafNodeQue {
	struct huffmanTree *treeLink;
	struct huffmanLeafNodeQue *link;
};

struct huffmanTree {
	char symbol; //any char ~ASCII(0-255)
	int freq; //no. of occurances of symbol
	char pos; //'0' for left child or '1' for right child
	struct huffmanTree *left;
	struct huffmanTree *right;
	struct huffmanTree *link; //nextlink in 0th level of tree and parentlink in remaining node of tree
				  //parentlink used for backtrace when fetching _huffmanCode from _minheap
};

struct huffmanCode { //used for decompressing, store _huffmanCode after fetched from compressed file
	char symbol;
	char *code;
	struct huffmanCode *link;
};


int calcASCII(char ch) {
	int get = (int)ch;
	if(get < 0)
		get = 256+get; // 256-get, get is -ve, so 256-(-get)
	return get;
}

//count the frequency of characters
void calculateFreq(int fdInput, int storeFreq[]) {
        //int fd = open("huffman_input2.pdf", O_RDONLY, 0);
	char getChar;
	size_t cin;
	while(1) {
		cin = read(fdInput, &getChar, 1);
		if(cin == 0)
			break;
		storeFreq[calcASCII(getChar)]++;
	}
}

void insertIntoHuffmanTree(struct huffmanTree *treeRootNode, struct huffmanTree *treeNewNode) {
	while(1) {
		if(treeRootNode->link == NULL) {
			treeRootNode->link = treeNewNode;
			break;
		}
		else if(treeNewNode->freq <= treeRootNode->link->freq) {
			treeNewNode->link = treeRootNode->link;
			treeRootNode->link = treeNewNode;
			break;
		}
		else
			treeRootNode = treeRootNode->link;
	}
}

//build a queue of occured characters, sorted by frequency ascendingly
void generateHuffmanTree(struct huffmanTree *treeRootNode, int storeFreq[]) {
	for(int index = 0; index < 256; index++) {
		if(storeFreq[index] != 0) {
			struct huffmanTree *treeNewNode = (struct huffmanTree*)malloc(sizeof(struct huffmanTree));
			treeNewNode->symbol = (char)index;
			treeNewNode->freq = storeFreq[index];
			treeNewNode->left = NULL;
			treeNewNode->right = NULL;
			treeNewNode->link = NULL;
			insertIntoHuffmanTree(treeRootNode, treeNewNode);
		}
	}
}

//building huffman _minheap
void generateHuffmanMinHeap(struct huffmanTree *treeRootNode) {
	while(1) {
		if(treeRootNode->link->link == NULL)
			break;

		struct huffmanTree *treeJoinNode = (struct huffmanTree*)malloc(sizeof(struct huffmanTree));
		treeJoinNode->freq = treeRootNode->link->freq+treeRootNode->link->link->freq;
		treeJoinNode->left = treeRootNode->link;
		treeJoinNode->right = treeRootNode->link->link;
		treeRootNode->link = treeRootNode->link->link->link;

		treeJoinNode->left->link = treeJoinNode;
		treeJoinNode->right->link = treeJoinNode;
		treeJoinNode->left->pos = '0';
		treeJoinNode->right->pos = '1';

		insertIntoHuffmanTree(treeRootNode, treeJoinNode);
	}
	treeRootNode = treeRootNode->link; //remove extra starting node
	treeRootNode->link = NULL; //root node has no parent link
}

//copy _huffmanTree leaf nodes into the _huffmanLeafNodeQue
void copyTreeIntoHuffmanLeafNodeQue(struct huffmanTree *treeNextNode, struct huffmanLeafNodeQue *queNextNode) {
	while(1) {
		if(treeNextNode->link == NULL)
			break;
		queNextNode->link = (struct huffmanLeafNodeQue*)malloc(sizeof(struct huffmanLeafNodeQue));
		queNextNode->link->treeLink = treeNextNode->link;
		treeNextNode = treeNextNode->link;
		queNextNode = queNextNode->link;
	}
}

void revString(char *str) {
	int length, i = 0;
	char *begin, *end, temp;
	length = strlen(str);

	begin  = str;
	end    = str;
	while(++i < length)
		end++;
	i = -1;
	while(++i < length/2) {
		temp = *end;
		*end = *begin;
		*begin = temp;
		begin++;
		end--;
	}
}

int power1(int base, int exp) {
	int pow = 1;
	while(exp-- > 0)
		pow *= base;
	return pow;
}

//fetch huffman code from _meanheap
void generateHuffmanCode(struct huffmanLeafNodeQue *queStartNode, char *storeCode[]) {
	struct huffmanLeafNodeQue *queNextNode = queStartNode;
	struct huffmanTree *treeNextNode;

	while(1) {
		if(queNextNode->link == NULL)
			break;
		queNextNode = queNextNode->link;
		treeNextNode = queNextNode->treeLink;
		char ch = treeNextNode->symbol;
		char binCode[16];
		int index = 0;
		while(1) {
			if(treeNextNode->link == NULL)
				break;
			binCode[index] = treeNextNode->pos;
			treeNextNode = treeNextNode->link;
			index++;
		}
		binCode[index] = '\0';
		revString(binCode);
		storeCode[calcASCII(ch)] = (char*)malloc(index);
		strcpy(storeCode[calcASCII(ch)], binCode);
	}
}

void printHuffmanTree(struct huffmanTree *treeRootNode) {
	printf("\nPRINTING HUFFMAN TREE_\n");
	while(treeRootNode != NULL) {
		printf("%d ", treeRootNode->freq);
		treeRootNode = treeRootNode->link;
	}
}

void printHuffmanLeafNodeQue(struct huffmanLeafNodeQue *queStartNode) {
	printf("\nPRINTING HUFFMAN LEAFNODEQUEUE_\n");
	while(queStartNode->link != NULL) {
		queStartNode = queStartNode->link;
		printf("%d ", queStartNode->treeLink->freq);
	}
}

void printHuffmanCode(int storeFreq[], char *storeCode[]) {
	printf("\n\nPRINTING HUFFMAN CODE_\n");
	for(int index = 0; index < 256; index++) {
		if(storeFreq[index] != 0) {
			printf("%c-%d-%s\n",index,storeFreq[index],storeCode[index]);
		}
	}
}



//## COMPRESSION METHODS ##//

//generate intermediate file/compressed file
void generateIntermediateFile(int fdInput, char *storeCode[], char *newFileName) {
	int fdInter = open(newFileName, O_WRONLY|O_CREAT, 0644);
	char storeBytes[256];
	char getChar;
	size_t cin;

	//header data
	write(fdInter, "#######\n", 8);
	for(int index = 0; index < 256; index++) {
		if(storeCode[index] != NULL) {
			char tempChar = (char)index;
			write(fdInter, &tempChar, 1);
			write(fdInter, storeCode[index], strlen(storeCode[index]));
			write(fdInter, "\n", 1);
		}
	}
	write(fdInter, "--1\n", 4); //-1 indicates the end of header section

	//compessing file data
	lseek(fdInput,0,SEEK_SET);
	storeBytes[0] = '\0';
	while(1) {
		cin = read(fdInput, &getChar, 1);
		if(cin == 0)
			break;

		strcat(storeBytes,storeCode[calcASCII(getChar)]);
		int length = strlen(storeBytes);
		if(length >= 8) {
			int i = 0;
			while(i < length-7) {
				int num = 0;
				for(int k = i; k < i+8; k++)
					if(storeBytes[k] == '1')
						num += power1(2,7-(k%8));
				char tempChar = (char)num;
				write(fdInter, &tempChar, 1);
				i+=8;
			}
			while(i < length) {
				storeBytes[i%8] = storeBytes[i];
				i++;
			}
			storeBytes[length%8] = '\0';
		}
	}
	//some bits remains in _storeBytes, store it in header as it is, at first line
	lseek(fdInter, 0, SEEK_SET);
	write(fdInter, storeBytes, strlen(storeBytes));
	close(fdInter);
}


//## DECOMPRESSION METHODS ##//

int fetchIntermediateFile(int fdInter1, struct huffmanCode *codeNextNode, char remainBits[]) {
	int fileSizeCount = 8;
	char getChar;
	int i = 0;
	char str[256];
	size_t cin;

	//fetch first line of intermediate file(remaining bits) into data structure _remainBits[]
	while(1) {
		cin = read(fdInter1, &getChar, 1);
		if(getChar == '\n')
			break;
		if(getChar != '#') {
			remainBits[i] = getChar;
			i++;
		}
	}
	remainBits[i] = '\0';

	//fetch intermediate file into data structure _struct huffmanCode
	while(1) {
		cin = read(fdInter1, &getChar, 1);
		char getChar1 = getChar;

		i = 0;
		while(1) {
			cin = read(fdInter1, &getChar, 1);
			if(getChar == '\n')
				break;
			str[i] = getChar;
			i++;
		}
		str[i] = '\0';

		fileSizeCount += strlen(str)+2; //one for symbol and one for '\n' of each line
		if(!strcmp(str, "-1")) {
			break;
		}

		struct huffmanCode *codeNewNode = (struct huffmanCode*)malloc(sizeof(struct huffmanCode));
		codeNewNode->symbol = getChar1;
		codeNewNode->code = (char*)malloc(strlen(str));
		strcpy(codeNewNode->code, str);
		codeNewNode->link = NULL;
		codeNextNode->link = codeNewNode;
		codeNextNode = codeNextNode->link;
	}
	return fileSizeCount;
}

void decompressingFile(int fdInter1, int fileSize1, struct huffmanCode *codeStartNode, char *remainBits, char *newFileName) {
	struct huffmanCode *codeNextNode;
	int fdReal = open(newFileName, O_WRONLY | O_CREAT, 0644);
	char breakFlag = '0';
	int base = 0;
	char storeBytes[256+8]; //extra 8 bytes used for _remainBits
	char getChar;

	while(1) {
		while(1) { //fill _storeBytes[] by reading char from compressed file
			short ind = 7;
			if(base >= 256) {
				storeBytes[base] = '\0';
				break;
			}
			if(--fileSize1 < 0) {
				breakFlag = '1';
				storeBytes[base] = '\0';
				strcat(storeBytes, remainBits);
				break;
			}
			read(fdInter1, &getChar, 1);
			short num = calcASCII(getChar);

			while(ind >= 0) {
				storeBytes[base+ind] = (char)((num%2)+48);
				num = num/2;
				ind--;
			}
			base += 8;
		}

		//_storeBytes[] range is [0, base)
		char *storeBytesStr;
		short base1 = 0;
		while(1) { //traverse _storeBytes[]
			char flag = '0';
			codeNextNode = codeStartNode->link;
			while(codeNextNode != NULL) { //traverse _huffmanCodeQue
				short length = strlen(codeNextNode->code);
				if(length > 256-base1) {
					codeNextNode = codeNextNode->link;
					continue;
				}

				char tempChar = storeBytes[base1+length];
				storeBytes[base1+length] = '\0';
				storeBytesStr = &storeBytes[base1];

				if(!strcmp(storeBytesStr, codeNextNode->code)) {
					write(fdReal, &codeNextNode->symbol, 1);
					storeBytes[base1+length] = tempChar;
					base1 = base1+length;
					flag = '1';
					break;
				}
				storeBytes[base1+length] = tempChar;
				codeNextNode = codeNextNode->link;
			}
			if(flag == '1')
				continue;
			break;
		}
		//calculating new base
		storeBytesStr = &storeBytes[base1];
		strcpy(storeBytes, storeBytesStr);
		base = strlen(storeBytes);

		if(breakFlag == '1')
			break;
	}
}

void printFetchedHuffmanCode(struct huffmanCode *codeStartNode) {
	printf("\nPRINTING HUFFMAN CODE FETCHED FROM FILE_\n");
	while(codeStartNode->link != NULL) {
		codeStartNode = codeStartNode->link;
		printf("%c %s\n", codeStartNode->symbol, codeStartNode->code);
	}
}





int main(int argc, char *argv[]) {
	if(argc != 3) {
		printf("give two arguments as \"filename\" \"com/decom\"\n");
		exit(0);
	}

	//## COMPRESSION ##//
	if(!strcmp(argv[2], "com")) {
		int fdInput = open(argv[1], O_RDONLY, 0);
		if(fdInput == -1) {
			printf("file cann't open\n");
			exit(0);
		}

		int storeFreq[256] = {0};
		char *storeCode[256] = {NULL}; //store string of 0 and 1 that represent _huffmanCode

		struct huffmanTree *treeRootNode = (struct huffmanTree*)malloc(sizeof(struct huffmanTree));
		struct huffmanTree *treeNextNode = treeRootNode;
		struct huffmanLeafNodeQue *queStartNode = (struct huffmanLeafNodeQue*)malloc(sizeof(struct huffmanLeafNodeQue));
		struct huffmanLeafNodeQue *queNextNode = queStartNode;

		//count the frequency of characters
		calculateFreq(fdInput, storeFreq);

		//build a queue of occured characters, sorted by frequency ascendingly
		generateHuffmanTree(treeRootNode, storeFreq);
		//printHuffmanTree(treeRootNode);

		//copy _huffmanTree leaf nodes into the _huffmanLeafNodeQue
		copyTreeIntoHuffmanLeafNodeQue(treeRootNode, queStartNode);
		//printHuffmanLeafNodeQue(queStartNode);

		//building huffman _minheap
		generateHuffmanMinHeap(treeRootNode);

		//fetch huffman code from _meanheap
		generateHuffmanCode(queStartNode, storeCode);
		//printHuffmanCode(storeFreq, storeCode);

		//generate intermediate file/compressed file
		char *newFileName = (char*)malloc(strlen(argv[1]));
		strcpy(newFileName, argv[1]);
		strcat(newFileName, "#");
		generateIntermediateFile(fdInput, storeCode, newFileName);
		close(fdInput);
	}

	//## DECOMPRESSION  ##//
	else if(!strcmp(argv[2], "decom")) {
		int fdInter1 = open(argv[1], O_RDONLY, 0);
		stat(argv[1], &st);
		int fileSize = st.st_size;
		struct huffmanCode *codeStartNode = (struct huffmanCode*)malloc(sizeof(struct huffmanCode));
		char remainBits[8];

		//fetch intermediate file in data structure _remainBits and _huffmnaCode
		int fileSizeCount = fetchIntermediateFile(fdInter1, codeStartNode, remainBits);
		//printFetchedHuffmanCode(codeStartNode);

		//decompress the file
		fileSize = fileSize-fileSizeCount;
		char *newFileName = (char*)malloc(strlen(argv[1]));
		strcpy(newFileName, argv[1]);
		for(int i = strlen(newFileName)-1; i > 0; i--) {
			newFileName[i] = newFileName[i-1];
			if(newFileName[i] == '.') {
				newFileName[i-1] = '#';
				break;
			}
		}
		decompressingFile(fdInter1, fileSize, codeStartNode, remainBits, newFileName);
	}
	else {
		printf("give two arguments as \"filename\" \"com/decom\"\n");
	}

}
