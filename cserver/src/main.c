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

int wybor_opcji = 0;



typedef struct lista {
    int val;
    struct lista * next;
}lista;



int dodawanie(lista * head){
	lista * current = head;
	int suma=0;
	while (current != NULL) {
	    	suma += current->val;
	        current = current->next;
	    }
	return suma;
}

int odejmowanie(lista * head){
	lista * current = head;
	int roznica=current->val;
	current = current->next;
	while (current != NULL) {
		roznica -= current->val;
		current = current->next;
	    }
	return roznica;
}

void sortuj_rosnaco(lista *start)
{
    int swapped, i;
    lista *ptr1;
    lista *lptr = NULL;

    do
    {
        swapped = 0;
        ptr1 = start;

        while (ptr1->next != lptr)
        {
            if (ptr1->val > ptr1->next->val)
            {
                zamien(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    }
    while (swapped);
}

void sortuj_malejaco(lista *start)
{
    int swapped, i;
    lista *ptr1;
    lista *lptr = NULL;

    do
    {
        swapped = 0;
        ptr1 = start;

        while (ptr1->next != lptr)
        {
            if (ptr1->val < ptr1->next->val)
            {
                zamien(ptr1->next, ptr1);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    }
    while (swapped);
}

void zamien(lista *a, lista *b)
{
    int temp = a->val;
    a->val = b->val;
    b->val = temp;
}

void print_list(lista * head) {

	lista * current = head;

    while (current != NULL) {
    	printf("%d, ", current->val);
        current = current->next;
    }
    printf("\n");
}


void push(lista ** head, int val) {

	lista * current = (*head);
	if((*head) == NULL){
		(*head) = malloc(sizeof(lista));
		current = (*head);
		current->next = NULL;
		current->val = val;
	}else{
		while(current->next != NULL){
			current=current->next;
		}
		current->next = malloc(sizeof(lista));
		current->next->val = val;
		current->next->next = NULL;
	}

}

void wyslij_liste(lista * head,int socket){
	lista * current = head;
	char liczby[100];
	char liczba[20];
	strcpy(liczby,"Twoje liczby: ");
	while (current != NULL) {
	    sprintf(liczba,"%d, ",current->val);
	    strcat(liczby,liczba);
		current = current->next;
	    }
	write(socket,liczby,sizeof(liczby));
}

int wyslij_wartosc(int var,int socket){
	char liczby[100];
	char liczba[20];
	strcpy(liczby,"Wynik dzialania: ");
	sprintf(liczba,"%d, ",var);
	strcat(liczby,liczba);
	write(socket,liczby,sizeof(liczby));

	return 1;
}

int odbieranie_liczb(int comm_fd, lista** lista_liczb){
	int recv_out;
	char odebrana_liczba[10];
	if ((recv_out = recv(comm_fd, odebrana_liczba, 10, NULL)) != -1) {
		if (recv_out == 0) {
			puts("Klient rozlaczyl sie");
			kill(getpid(), SIGTERM);
		}
		if(strcmp(odebrana_liczba,"x\n") == 0){
			printf("Zakonczono dodawanie liczb\n");
			return 1;

		}else{
		push(lista_liczb, atoi(&odebrana_liczba));
		}
	}

	return 0;
}

int czekaj_na_klienta(int comm_fd){
	int recv_out;
	char odebrana_liczba[10];
	printf("Czekam na wybor dzialania...\n");
	if ((recv_out = recv(comm_fd, odebrana_liczba, 10, NULL)) >= 0) {
			if (recv_out == 0) {
				puts("Klient rozlaczyl sie");
				kill(getpid(), SIGTERM);
			}
			if(strcmp(odebrana_liczba,"a\n") == 0){
				printf("Wybrano dodawanie\n");
				return 2;
			}else if(strcmp(odebrana_liczba,"b\n") == 0){
				printf("Wybrano odejmowanie\n");
				return 3;
			}else if(strcmp(odebrana_liczba,"c\n") == 0){
				printf("Wybrano sortowanie rosnąco\n");
				return 4;
			}else if(strcmp(odebrana_liczba,"d\n") == 0){
				printf("Wybrano sortowanie malejaco\n");
				return 5;
			}else if(strcmp(odebrana_liczba,"e\n") == 0){
				printf("Wprowadzanie liczb\n");
				return 0;
			}else if(strcmp(odebrana_liczba,"f\n") == 0){
				printf("Wybrano wyswietlanie liczb\n");
				return 6;
			}else{
				printf("Wybrano nieprawidlowa opcje \n");
				return 1;
			}

		}
	return 1;
}


int main() {

	system("fuser -k 22000/tcp");
	char listen_port[10];
	int listen_fd, comm_fd;
	int pid;

	struct sockaddr_in servaddr;

	lista * lista_liczb = NULL;


	listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));

	puts("Wpisz port:");
	fgets(listen_port,10,stdin);

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
	servaddr.sin_port = htons(atoi(listen_port));

	bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	listen(listen_fd, 10);
	puts("Czekam na polaczenie");

	comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);

	puts("Nawiazalem polacznie!");
	pid = fork();
	while (1) {
		if (pid == 0) {
			close(listen_fd);

			if(wybor_opcji == 0){
			wybor_opcji = odbieranie_liczb(comm_fd,&lista_liczb);
			}else if(wybor_opcji == 1){
				wybor_opcji = czekaj_na_klienta(comm_fd);
			}else if(wybor_opcji == 2){
				wybor_opcji = wyslij_wartosc(dodawanie(lista_liczb),comm_fd);
			}else if(wybor_opcji == 3){
				wybor_opcji = wyslij_wartosc(odejmowanie(lista_liczb),comm_fd);
			}else if(wybor_opcji == 4){
				sortuj_rosnaco(lista_liczb);
				wyslij_liste(lista_liczb,comm_fd);
				wybor_opcji = 1;
			}else if(wybor_opcji == 5){
				sortuj_malejaco(lista_liczb);
				wyslij_liste(lista_liczb,comm_fd);
				wybor_opcji = 1;
			}else if(wybor_opcji == 6){
				wyslij_liste(lista_liczb,comm_fd);
				wybor_opcji = 1;
			}

		} else {
			if ((comm_fd = accept(listen_fd, (struct sockaddr *) NULL, NULL))
					!= -1) {
					pid = fork();
			} else {
				if (comm_fd == -1) {
					perror("accept");
				}
			}
			sleep(1);
		}

	}
}
