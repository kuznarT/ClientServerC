#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include<string.h>
#include <unistd.h>
#include <ctype.h>

char liczby[10];
int i = 0;
char wiadomosc[100];
int wybor = 0;
int sprawdz = 1;

void clearScreen() {
	const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
	write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
}

int wysylanie_liczb(int sockfd) {
	char sendline[10];
	bzero(sendline, 10);
	printf("Wprowadz liczbe (x aby zakonczyc): \n");
	do {
		fgets(sendline, 10, stdin);
		if(atoi(sendline) != 0){
			break;
		}else if(!strcmp(sendline, "x\n")){
			break;
		}
		printf("Wprowadz poprawna liczbe:\n");
	} while (1);

	if (write(sockfd, sendline, 10) < 0) {
		perror(write);
	}
	if (strcmp(sendline, "x\n") == 0) {
		return 1;
	}
	return 0;

}

void menu() {
	printf("Jaka operacje chcesz wykonac? \n");
	printf("a. Dowawanie\n");
	printf("b. Odejmowanie\n");
	printf("c. Sortowanie rosnąco\n");
	printf("d. Sortowanie malejaco\n");
	printf("e. Wprowadz liczby\n");
	printf("f. Wyswietl wprowadzone liczby\n");
	printf("exit - aby zakonczyc\n");

}

int wybor_opcji(int sockfd) {
	char opcja[10];
	menu();
	printf("Wprowadz litere odpowiadajaca dzialaniu: \n");
	fgets(opcja, 10, stdin);
	if (strcmp(opcja, "exit\n") == 0) {
		exit(0);
	}
	write(sockfd, opcja, 10);
	if (strcmp(opcja, "e\n") == 0) {
		return 0;
	} else if (strcmp(opcja, "f\n") == 0) {
		return 2;
	} else if (strcmp(opcja, "a\n") == 0) {
		return 2;
	} else if (strcmp(opcja, "b\n") == 0) {
		return 2;
	} else if (strcmp(opcja, "c\n") == 0) {
		return 2;
	} else if (strcmp(opcja, "d\n") == 0) {
		return 2;
	} else {
		return 1;
	}

}

int odbierz_liczby(int sockfd) {
	int recv_out;
	if ((recv_out = recv(sockfd, wiadomosc, sizeof(wiadomosc), NULL)) < 0) {
		perror("read");

	} else {
		printf("%s\n", wiadomosc);
		printf("Enter aby przejsc dalej");
		while (getchar() != '\n')
			;
		clearScreen();
	}
	return 1;
}

int main(int argc, char **argv) {
	int sockfd, n;
	int port[6];
	struct sockaddr_in servaddr;

	puts("Wpisz numer portu: ");
	fgets(port, 10, stdin);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof servaddr);

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(port));

	inet_pton(AF_INET, "127.0.0.1", &(servaddr.sin_addr));

	connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	while (1) {
		if (wybor == 0) {
			wybor = wysylanie_liczb(sockfd);
		} else if (wybor == 1) {
			wybor = wybor_opcji(sockfd);
		} else if (wybor == 2) {
			wybor = odbierz_liczby(sockfd);
		}
	}

}
