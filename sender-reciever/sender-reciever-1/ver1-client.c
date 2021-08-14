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


int filenorecv = 0;
int filenosend = -1;
int filecount = (argc-1)/2;
int filefdr[filecount];
int filefdw[1024];
int filesize;
char *file;
char *filer = (char*)malloc(16);
ssize_t cin, cout;
int blocksize1 = 1024;
int clientid = -1;
int clientno[filecount];
int i = -1, j = -1, flagt;
int state = argc;
int count = 0;
int blockcount[filecount];

while(++i < filecount) {clientno[i] = -1, blockcount[i] = 0;}
i = -1;
while(++i < 1024) {filefdw[i] = -1;}
*(filer+0) = 's';
*(filer+1) = 'r';
*(filer+2) = 'e';
*(filer+3) = 'c';
*(filer+4) = 'v';
*(filer+5) = 'f';
*(filer+6) = 'i';
*(filer+7) = 'l';
*(filer+8) = 'e';
*(filer+12) = '.';
*(filer+13) = 'm';
*(filer+14) = 'p';
*(filer+15) = '4';

struct cid {
	int flag;
	int clientid;
	int clientno;
};
struct cid *cid1 = (struct cid*)malloc(sizeof(struct cid));
struct block {
	struct cid cidd;
	int fileno;
	int blockno;
	int blocksize;
	char blockdata[blocksize1];
};
struct block *block1 = (struct block*)malloc(sizeof(struct block));

if((argc-1)%2 != 0) exit(0);
if(argc> 1) {
	int k = -1;
	while(++k < filecount) {
		filesize = strlen(argv[k*2+1]);
		file = (char*)malloc(filesize);
		file = argv[k*2+1];
		filefdr[k] = open(file, O_RDONLY, 0);
		clientno[k] = atoi(argv[k*2+2]);
	}
}

flagt = 2;
flagt = 1;
cid1->clientno = -3;
while(1) {
	if(filecount == 0) state = 1, flagt = 2;
	if(state > 1) {
		flagt = (flagt%2)+1;
		if(flagt == 1) {
			if(filecount > 0)
				filenosend = (filenosend+1)%filecount;
			cid1->clientno = clientno[filenosend];
		}
	}

	cid1->flag = flagt;
	cid1->clientid = clientid;
        sendto(sockfd,cid1,sizeof(struct cid),0,(const struct sockaddr*)&servaddr,len);
	n=recvfrom(sockfd,cid1,sizeof(struct cid),0,(struct sockaddr *)&servaddr,&len);
	clientid = cid1->clientid;

	if(cid1->flag == 11) {
		lseek(filefdr[filenosend], blockcount[filenosend]*1024, SEEK_SET);
                cin = read(filefdr[filenosend], block1->blockdata, 1024);
		block1->cidd.flag = 3;
		block1->fileno = clientid*100 + filefdr[filenosend];
                block1->blockno = blockcount[filenosend]*1024;
                block1->blocksize = cin;
		printf("s%d,%ld\n", clientno[filenosend], cin);
                sendto(sockfd,block1,sizeof(struct block),0,(const struct sockaddr*)&servaddr,len);
	        n=recvfrom(sockfd,cid1,sizeof(struct cid),0,(struct sockaddr *)&servaddr,&len);
		blockcount[filenosend]++;
		if(cin == 0) {
                        for(i = filenosend; i < filecount-1; i++) {
                                filefdr[i] = filefdr[i+1];
                                clientno[i] = clientno[i+1];
                                blockcount[i] = blockcount[i+1];
                        }
                        filecount--;
                }               
	}
	if(cid1->flag == 12) {
	        n=recvfrom(sockfd,block1,sizeof(struct block),0,(struct sockaddr *)&servaddr,&len);
		int filenorecv = block1->fileno;
		if(filefdw[filenorecv] == -1) {
			int a = filenorecv;
			*(filer+11) = (char)(a%10)+48;
			a = a/10;
			*(filer+10) = (char)(a%10)+48;
			a = a/10;
			*(filer+9) = (char)(a%10)+48;
			printf("%s\n", filer);
			filefdw[filenorecv] = open(filer, O_WRONLY | O_CREAT, 0664);
		}
		if(block1->blocksize != 0) {
			lseek(filefdw[filenorecv], block1->blockno, SEEK_SET);
			cout = write (filefdw[filenorecv], block1->blockdata, block1->blocksize);
	        //       printf("r%d\n", block1->fileno);
		}


	}

}


return 0;
}
