#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

static bool stop = false;

void handle_sigint(int sig){
    stop = true;
}

int main(int argc, char* argv[]){
    const char* ip = argv[1];
    int port = atoi(argv[2]);
    int backlog = 5;
    if(argc > 3){
        backlog = atoi(argv[3]);
    }
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    /* Set N bytes of S to 0.  */
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_aton(ip, &address.sin_addr);
    address.sin_port = htons(port);
    int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
    assert(ret!=-1);
    ret = listen(sock, backlog);
    signal(SIGINT, handle_sigint);
    
    sleep(20);

    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    int connfd = accept(sock, (struct sockaddr*)&client, &client_addrlength);
    printf("ip address: %s \nport: %d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

    // while(!stop){
    //     sleep(1);
    // }

    close(sock);
    return 0;
}