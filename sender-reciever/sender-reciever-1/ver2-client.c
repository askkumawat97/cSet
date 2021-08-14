#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<unistd.h>
#define PORT 5000

int main(int argc, char* argv[]) {
int sockfd, len, n;
sockfd = socket(AF_INET, SOCK_DGRAM, 0);
if(sockfd < 0) {
	perror("socker error");
	exit(0);
}
struct sockaddr_in servaddr;
servaddr.sin_family = AF_INET;
//servaddr.sin_addr.s_addr = INADDR_ANY;
if(inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) < 0) {
	perror("invalid address");
	return -1;
}
servaddr.sin_port = htons(PORT);
len = sizeof(servaddr);




int filenosend = -1; //current file id to send
int filecount = (argc-2)/2; //total no of files
int filefdr[filecount]; //file descrypters for reading
int filefdw[1024]; //file descrypters for writing
ssize_t cin, cout;
int blocksize1 = 1024; //send and recieve buffer size
int clientid = atoi(argv[1]); //this client id as integer
int clientno[filecount]; //reciever client no.
int blockcount[filecount]; //total blocks of file of factor blocksize1
int flagt;
int state = argc;

//initialization of blockcount and filefdw
int i = -1;
while(++i < filecount) blockcount[i] = 0;
i = -1;
while(++i < 1024) filefdw[i] = -1;

//message buffer
struct cid {
	int flag;
	int clientid; //sender client id
};
struct cid *cid1 = (struct cid*)malloc(sizeof(struct cid));
struct block {
	struct cid cidd;
	int clientno;
	int fileno; //differential between multiple files
	int blockno; //one file breaks into blocks
	int blocksize; //total no. of blocks
	char blockdata[blocksize1];
};
struct block *block1 = (struct block*)malloc(sizeof(struct block));

//store command line input into data structure
if((argc-2)%2 != 0) exit(0);
if(argc > 2) {
	int k = -1;
	while(++k < filecount) {
		int filesize = strlen(argv[k*2+2]);
		char *file = (char*)malloc(filesize);
		file = argv[k*2+2];
		filefdr[k] = open(file, O_RDONLY, 0);
		clientno[k] = atoi(argv[k*2+3]);
	}
}

//piriodically send or recieve block
flagt = 1;
while(1) {
	if(filecount == 0) state = 1, flagt = 2;
	if(state > 1) {
		flagt = (flagt%2)+1;
		//periodically change the files
		if(flagt == 1) {
			if(filecount > 0)
				filenosend = (filenosend+1)%filecount;
			block1->clientno = clientno[filenosend];
		}
	}

	//send block and wait for acknowledge
	cid1->flag = flagt;
	cid1->clientid = clientid;
        sendto(sockfd,cid1,sizeof(struct cid),0,(const struct sockaddr*)&servaddr,len);
	n=recvfrom(sockfd,cid1,sizeof(struct cid),0,(struct sockaddr*)&servaddr,&len);

	//send block
	if(cid1->flag == 11) {
		//set poiter at next block of file and read
		lseek(filefdr[filenosend], blockcount[filenosend]*1024, SEEK_SET);
                cin = read(filefdr[filenosend], block1->blockdata, 1024);
		//set buffer values and sends
		block1->cidd.flag = 3;
		block1->fileno = clientid*100 + filefdr[filenosend];
                block1->blockno = blockcount[filenosend]*1024;
                block1->blocksize = cin;
		printf("s%d,%ld\n", clientno[filenosend], cin);
                sendto(sockfd,block1,sizeof(struct block),0,(const struct sockaddr*)&servaddr,len);
		//blank recieve, intentionaly taken
	        n=recvfrom(sockfd,cid1,sizeof(struct cid),0,(struct sockaddr*)&servaddr,&len);
		blockcount[filenosend]++;
		//after one file is completely sends
		if(cin == 0) {
			int i;
			//replace next file id to completed file id to reduce lenght of open files
                        for(i = filenosend; i < filecount-1; i++) {
                                filefdr[i] = filefdr[i+1];
                                clientno[i] = clientno[i+1];
                                blockcount[i] = blockcount[i+1];
                        }
                        filecount--;
                }               
	}
	//recieve block
	if(cid1->flag == 12) {
	        n=recvfrom(sockfd,block1,sizeof(struct block),0,(struct sockaddr*)&servaddr,&len);
		if(filefdw[block1->fileno] == -1) {
			//create string for filename and open it
			char *filer = (char*)malloc(4);
			int a = block1->fileno;
			*(filer+3) = (char)(a%10)+48, a = a/10;
			*(filer+2) = (char)(a%10)+48, a = a/10;
			*(filer+1) = (char)(a%10)+48, a = a/10;
			*(filer+0) = (char)(a%10)+48;
			printf("file:%s\n", filer);
			filefdw[block1->fileno] = open(filer, O_WRONLY | O_CREAT, 0664);
		}
		//set file pointer and write into file
		if(block1->blocksize != 0) {
			lseek(filefdw[block1->fileno], block1->blockno, SEEK_SET);
			cout = write (filefdw[block1->fileno], block1->blockdata, block1->blocksize);
			printf("r%d\n", block1->fileno);
		}
	}
}

return 0;
}
