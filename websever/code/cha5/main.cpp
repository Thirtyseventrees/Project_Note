#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 1024

static bool stop = false;

void handle_sigint(int sig){
    stop = true;
}

void print_message(char* buf, int size){
    for(int i = 0; i < size; ++i)
        printf("%c", buf[i]);
}

int main(int argc, char* argv[]){
    const char* ip = argv[1];
    int port = atoi(argv[2]);
    int backlog = 5;
    if(argc > 3){
        backlog = atoi(argv[3]);
    }
    // Create a socket
    int sock = socket(PF_INET, SOCK_STREAM, 0);

    // Name the socket
    struct sockaddr_in address;
    /* Set N bytes of S to 0.  */
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_aton(ip, &address.sin_addr);
    address.sin_port = htons(port);
    int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
    assert(ret!=-1);

    // listen the socket
    ret = listen(sock, backlog);
    signal(SIGINT, handle_sigint);
    
    sleep(15);

    // accept the client
    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    int connfd = accept(sock, (struct sockaddr*)&client, &client_addrlength);
    printf("ip address: %s \nport: %d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

    // recv message
    int recv_size;
    char buffer[BUF_SIZE];
    memset(buffer, '\0', BUF_SIZE);
    recv_size = recv(connfd, buffer, BUF_SIZE - 1, 0);
    printf("%s", buffer);
    //print_message(buffer, recv_size);

    // while(!stop){
    //     sleep(1);
    // }

    close(connfd);
    close(sock);
    return 0;
}