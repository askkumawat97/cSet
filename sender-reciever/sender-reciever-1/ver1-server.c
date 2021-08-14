#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#define PORT 5000
struct stat st;

void main() {
int sockfd, len, n;
sockfd = socket(AF_INET, SOCK_DGRAM, 0);
if(sockfd < 0) {
	perror("socket error");
	exit(0);
}
struct sockaddr_in servaddr, cliaddr;
servaddr.sin_family = AF_INET;
servaddr.sin_addr.s_addr = INADDR_ANY;
servaddr.sin_port = htons(PORT);
if(bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
	perror("bind error");
	exit(0);
}

int filefd, filesize, blocksize1 = 1024;
int clientid = 0;
int i, j;
ssize_t cin;
char *file;

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

cid1->clientid = -2;
int cliavail;
int check = 0;
int checkcli = 0;
int serfree = 0;
while(1) {
	len=sizeof(cliaddr);
        cliavail = cid1->clientid;
        n=recvfrom(sockfd,block1,sizeof(struct block),0,(struct sockaddr *)&cliaddr,&len);
	cid1->flag = block1->cidd.flag;
	cid1->clientid = block1->cidd.clientid;
        cid1->clientno = block1->cidd.clientno;

	if(block1->cidd.clientid == -1) {
		cid1->clientid = clientid;
		printf("%d\n",clientid);
		clientid++;
	}
	else if(block1->cidd.flag == 1 && check == 0 && serfree == 0 && cliavail == cid1->clientno) {
		cid1->flag = 11;
		check = 1;
		serfree = 1;
		checkcli = block1->cidd.clientno;
	}
	if(block1->cidd.flag == 2 && check == 2 && checkcli == block1->cidd.clientid) {
		cid1->flag = 12;
		check = 3;
		serfree = 0;
	}
	if(block1->cidd.flag == 3) {
		check = 2;
		cid1->flag = 0;
	}
	
	sendto(sockfd,cid1,sizeof(struct cid),0,(struct sockaddr*)&cliaddr,len);

	if(check == 3) {
	        sendto(sockfd,block1,sizeof(struct block),0,(struct sockaddr*)&cliaddr,len);
		check = 0;
	}

}

}
