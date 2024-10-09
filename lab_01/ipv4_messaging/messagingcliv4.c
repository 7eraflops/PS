#include <arpa/inet.h> /* inet(3) functions */
#include <errno.h>
#include <netinet/in.h> /* sockaddr_in{} and other Internet defns */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> /* basic socket definitions */
#include <sys/types.h>  /* basic system data types */
#include <unistd.h>

#define MAXLINE 1024
#define SA struct sockaddr

int main(int argc, char **argv)
{
    int sockfd, n, err;
    struct sockaddr_in servaddr;
    char recvline[MAXLINE + 1];
    char sendline[MAXLINE];

    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <IPaddress> \n", argv[0]);
        return 1;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "socket error : %s\n", strerror(errno));
        return 1;
    }

    // sleep(3);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(13); /* daytime server */
    if ((err = inet_pton(AF_INET, argv[1], &servaddr.sin_addr)) == -1)
    {
        fprintf(stderr, "ERROR: inet_pton error for %s : %s \n", argv[1], strerror(errno));
        return 1;
    }
    else if (err == 0)
    {
        fprintf(stderr, "ERROR: Invalid address family \n");
        return 1;
    }

    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0)
    {
        fprintf(stderr, "connect error : %s \n", strerror(errno));
        return 1;
    }

    while (1)
    {
        n = read(sockfd, recvline, MAXLINE);
        if (n > 0)
        {
            recvline[n] = 0; /* null terminate */
            printf("Server: %s", recvline);
        }
        else if (n < 0)
        {
            fprintf(stderr, "read error: %s\n", strerror(errno));
            break;
        }
        printf("Enter response: ");
        fgets(sendline, MAXLINE, stdin);
        if (write(sockfd, sendline, strlen(sendline)) < 0)
        {
            fprintf(stderr, "write error: %s\n", strerror(errno));
            break;
        }
        if (strcmp(recvline, "exit\n") == 0)
        {
            printf("Connection closed.\n");
            break;
        }
    }
    close(sockfd);
}
