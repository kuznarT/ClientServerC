#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>



typedef struct lista {
    int val;
    struct lista * next;
} lista;

int dodawanie(lista * head){
	lista * current = head;
	int suma=0;
	while (current->next != NULL) {
	    	suma += current->val;
	        current = current->next;
	    }
	return suma;
}

void print_list(lista * head) {
    lista * current = head;

    while (current->next != NULL) {
    	printf("%d, ", current->val);
        current = current->next;
    }
    printf("\n");
}

int pop(lista ** head) {
    int retval = -1;
    lista * next_node = NULL;

    if (*head == NULL) {
        return -1;
    }

    next_node = (*head)->next;
    retval = (*head)->val;
    free(*head);
    *head = next_node;

    return retval;
}

void push(lista * head, int val) {

	lista * current = head;

	if(current->next == NULL){
    	current->val = val;
    	current->next = malloc(sizeof(lista));
    	current->next->next = NULL;

    }else{
	while (current->next != NULL) {
        current = current->next;
    }
    current->next = malloc(sizeof(lista));
    current->val = val;
    current->next->next = NULL;
    }
}

void wyslij(lista * head,int socket){
	lista * current = head;
	char liczby[11];
	int i=0;
	while (current->next != NULL) {
	    liczby[i++]= current->val;
		current = current->next;
	    }
	liczby[i]='\0';
	write(socket,liczby,sizeof(liczby));
}


int main() {

	system("fuser -k 22000/tcp");
	char str[14] = "hello user nr ";
	char liczba[5];
	char* message;
	int listen_port;
	int listen_fd, comm_fd;
	int pid, count_users=0;
	char odebrana_liczba[10];
	int recv_out;
	struct sockaddr_in servaddr;

	lista * test_list = malloc(sizeof(lista));

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));

	/*puts("Wpisz port:");
	 scanf("%d",&listen_port);*/

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
	servaddr.sin_port = htons(22000);

	bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	listen(listen_fd, 10);
	puts("Czekam na polaczenie");

	comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);

	puts("Nawiazalem polacznie!");
	message = malloc(100);
	sprintf(liczba,"%d",count_users);
	strcat(message,str);
	strcat(message,liczba);
	pid = fork();
	fcntl(listen_fd, F_SETFL, O_NONBLOCK);
	int i=1;
	while (1) {
		if (pid == 0) {
			close(listen_fd);
			/*if (send(comm_fd, message, 100, 0) == -1) {
				perror("send");
			}*/
			fcntl(comm_fd,F_SETFL,O_NONBLOCK);
			if((recv_out=recv(comm_fd,odebrana_liczba,10,NULL))!= -1){
				if(recv_out == 0){
					puts("Klient rozlaczyl sie");
					kill(getpid(),SIGTERM);
				}
				push(test_list,atoi(&odebrana_liczba));
			}
			sleep(2);
			print_list(test_list);
			printf("\nSuma: %d\n",dodawanie(test_list));
			wyslij(test_list,comm_fd);
		} else {
			if ((comm_fd = accept(listen_fd, (struct sockaddr *) NULL, NULL))
					!= -1) {
				message = malloc(100);
				sprintf(liczba,"%d",count_users);
					strcat(message,str);
					strcat(message,liczba);
					pid = fork();
			} else {
				if (comm_fd == -1) {
					//perror("accept");
				}
			}
			//close(comm_fd);  // parent doesn't need this
			//fcntl(listen_fd,F_SETFL,O_NONBLOCK);
			sleep(1);
		}

	}
}
