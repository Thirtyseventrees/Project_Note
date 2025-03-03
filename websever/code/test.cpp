#include <stdio.h>
#include <iostream>

#include<sys/types.h>
#include<sys/socket.h>
#include <arpa/inet.h>


int main(){

    struct in_addr addr1, addr2;
    inet_aton("1.2.3.4", &addr1);
    inet_aton("10.194.71.60", &addr2);

    char* szValue1 = inet_ntoa(addr1);
    char* szValue2 = inet_ntoa(addr2);
    
    printf("address 1: %s\n", szValue1);
    printf("address 2: %s\n", szValue2);

    int s_fd = socket(PF_INET, SOCK_STREAM, 0);
    std::cout << s_fd << std::endl;

    return 0;
}