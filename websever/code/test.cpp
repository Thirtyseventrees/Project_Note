#include <stdio.h>
#include <iostream>

#include<sys/types.h>
#include<sys/socket.h>
#include <arpa/inet.h>
#include<netdb.h>
#include<unistd.h>
#include<assert.h>


int main(){

    pid_t id = fork();
    if(id == 0){
        printf("Child process\n");
    }
    else{
        printf("Father process\n");
    }

    return 0;
}