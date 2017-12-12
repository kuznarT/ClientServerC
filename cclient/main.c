#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include<string.h>
#include <unistd.h>

typedef struct lista {
    int val;
    struct lista * next;
} lista;

void print_list(lista * head) {
    lista * current = head;
    puts("lel");
    while(current->next != NULL) {
    	puts("lel2");
    	printf("%d, ", current->val);
        current = current->next;
    }
    printf("\n");
}
char liczby[10];
int i=0;
int main(int argc,char **argv)
{
    int sockfd,n;
    char sendline[10];
    char recvline[100];
    struct sockaddr_in servaddr;
    int stan;

    liczby[0] = "\0";

    lista * test_list = malloc(sizeof(lista));

    sockfd=socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof servaddr);

    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(22000);

    inet_pton(AF_INET,"127.0.0.1",&(servaddr.sin_addr));

    connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

    while(1)
    {
        bzero( sendline, 100);
        bzero( recvline, 100);
        /*switch(stan){
        case 1:

        break;
        }*/

        printf("Wprowadz liczbe: ");
        fgets(sendline,100,stdin); /*stdin = 0 , for standard input */
        write(sockfd,sendline,10);
        read(sockfd,liczby,100);
        sleep(1);
        i = 0;

        while(liczby[i] != '\0'){
        printf("dodales: %d\n",liczby[i++]);
        }

        //print_list(test_list);
    }

}
