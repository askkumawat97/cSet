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




int blocksize1 = 1024;
int blockcount = 10; //server storage capacity
int bindex = 0; //currently used index
int check = 0;
int i = -1;

struct cid {
	int flag;
	int clientid;
};
struct cid *cid1 = (struct cid*)malloc(sizeof(struct cid));
struct block {
	struct cid cidd;
	int clientno;
	int fileno;
        int blockno;
        int blocksize;
        char blockdata[blocksize1];
};
struct block *block1[blockcount];
int blockfree[blockcount];
while(++i < blockcount) {
	block1[i] = (struct block*)malloc(sizeof(struct block));
	blockfree[i] = 0;
}


while(1) {
	len=sizeof(cliaddr);
	//waiting for recieve
        n=recvfrom(sockfd,block1[bindex],sizeof(struct block),0,(struct sockaddr*)&cliaddr,&len);
	cid1->flag = block1[bindex]->cidd.flag;
	cid1->clientid = block1[bindex]->cidd.clientid;

	//client request for sending block
	if(block1[bindex]->cidd.flag == 1 && check == 0) {
		i = -1;
		while(++i < 10)
			if(blockfree[i] == 0) {
				bindex = i;
				cid1->flag = 11; //allow client to send message
				check = 1;
				blockfree[i] = 1;
				break;
			}
	}
	//client has sended block
	else if(block1[bindex]->cidd.flag == 3 && check == 1) {
		check = 0;
		blockfree[bindex] = 2;
	}
	//client request for recieving block
	else if(block1[bindex]->cidd.flag == 2 && check == 0) {
		i = -1;
		//check server storage for client is requested fro recieve
		while(++i < 10)
			if(block1[i]->clientno == cid1->clientid && blockfree[i] == 2) {
				bindex = i;
				cid1->flag = 12; //allow client to recieve message
				blockfree[i] = 3;
				break;
			}
	}
	
	//send acknowledge
	sendto(sockfd,cid1,sizeof(struct cid),0,(struct sockaddr*)&cliaddr,len);

	//if requested client is matched, message sends to that client
	if(cid1->flag == 12) {
	        sendto(sockfd,block1[bindex],sizeof(struct block),0,(struct sockaddr*)&cliaddr,len);
		blockfree[bindex] = 0;
	}

}

}
