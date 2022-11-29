#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <Eigen/Dense>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

using Eigen::Vector4d;

#define SERVER_IP "127.0.0.1"
#define PORT    8080
#define MAXLINE 40
#define ip_server "localhost"

#define charsforvalues 20
#define maxnumofvalues 20

void error(const char *msg){
	perror(msg);
	exit(1);
}

class MySocketServer{
    int serSockDes, readStatus;
    struct sockaddr_in serAddr, cliAddr;
    socklen_t cliAddrLen;
    char buff[1024] = {0};
    std::string msg_string = "Hello to you too!!!\n";
    const char * msg = msg_string.c_str();

	public:
    MySocketServer(){
        if ((serSockDes = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("socket creation error...\n");
            exit(-1);
        }

          //binding the port to ip and port
        serAddr.sin_family = AF_INET;
        serAddr.sin_port = htons(PORT);
        serAddr.sin_addr.s_addr = INADDR_ANY;

        if ((bind(serSockDes, (struct sockaddr*)&serAddr, sizeof(serAddr))) < 0) {
            perror("binding error...\n");
            close(serSockDes);
            exit(-1);
        }

        cliAddrLen = sizeof(cliAddr);
        readStatus = recvfrom(serSockDes, buff, 1024, 0, (struct sockaddr*)&cliAddr, &cliAddrLen);
        if (readStatus < 0) {
            perror("reading error...\n");
            close(serSockDes);
            exit(-1);
        }

        std::cout.write(buff, readStatus);
        std::cout << std::endl;

        if (sendto(serSockDes, msg, strlen(msg), 0, (struct sockaddr*)&cliAddr, cliAddrLen) < 0) {
            perror("sending error...\n");
            close(serSockDes);
            exit(-1);
        }
    }

	void send_data(Vector4d eta){
	    char msg[300];
	    sprintf(msg, "[%.6f,%.6f,%.6f,%.6f]", eta(0), eta(1), eta(2), eta(3));
 	    if (sendto(serSockDes, msg, strlen(msg), 0, (struct sockaddr*)&cliAddr, cliAddrLen) < 0) {
            perror("sending error...\n");
            close(serSockDes);
            exit(-1);
        }
        //std::cout.write(msg, strlen(msg));
        //std::cout << std::endl;
	}

    void close_connection(){
        close(serSockDes);
    }
};

class MySocketClient{
    int cliSockDes, readStatus;
    struct sockaddr_in serAddr;
    socklen_t serAddrLen;
    std::string msg_string = "Hello!!!\n";
    const char * msg = msg_string.c_str();
    char buff[1024] = {0};
	
	char temp_number[charsforvalues];
    int ii = 0;
    int num_recived_values = 0;
    double recived_values[maxnumofvalues];

    public:
    MySocketClient(){
    	for (int i=0; i<maxnumofvalues; i++){
    		recived_values[i] = 0.0;
    	}
        //create a socket
        if ((cliSockDes = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("socket creation error...\n");
            exit(-1);
        }

        //server socket address
        serAddr.sin_family = AF_INET;
        serAddr.sin_port = htons(PORT);
        serAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        int rc;
        int optval;
        optval = 512;
        rc = setsockopt(cliSockDes, SOL_SOCKET, SO_RCVBUF,(char *)&optval, sizeof(int)); // Setta al minimo il numero di elementi messi in coda!

        if (sendto(cliSockDes, msg, strlen(msg), 0, (struct sockaddr*)&serAddr, sizeof(serAddr)) < 0) {
            perror("sending error...\n");
            close(cliSockDes);
            exit(-1);
        }

        serAddrLen = sizeof(serAddr);
        readStatus = recvfrom(cliSockDes, buff, 1024, 0, (struct sockaddr*)&serAddr, &serAddrLen);
        if (readStatus < 0) {
            perror("reading error...\n");
            close(cliSockDes);
            exit(-1);
        }

        std::cout.write(buff, readStatus);
        std::cout << std::endl;
   	}

   	Vector4d reading(){
   	    readStatus = recvfrom(cliSockDes, buff, 1024, 0, (struct sockaddr*)&serAddr, &serAddrLen);
        if (readStatus < 0) {
            perror("reading error...\n");
            close(cliSockDes);
            exit(-1);
        }
        //std::cout.write(buff, readStatus);
        //std::cout << std::endl;
        ii=0;
        num_recived_values=0;
        for (int i=1; i<readStatus; i++){ // 1 because we don't care about parentesis
        	if (buff[i] != ',' && buff[i] != ']'){
        		temp_number[ii] = buff[i];
        		ii++;
        	}else{
        		for (int iii=ii; iii<charsforvalues; iii++){
        			temp_number[iii] = '0';
        		}
        		ii = 0;
        		recived_values[num_recived_values] = std::stod(temp_number);
        		std::cout << recived_values[num_recived_values] << std::endl;
        		num_recived_values++;
        	}
        }
        return Vector4d(recived_values[0], recived_values[1], recived_values[2], recived_values[3]);
   	}

   	void close_connection(){
    	close(cliSockDes);
    }
};
