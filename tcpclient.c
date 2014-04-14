#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char**argv) {
	int sockfd, n;
	struct sockaddr_in servaddr, cliaddr;
	char sendline[10000];
	char recvline[10000];
	int selret;
	int nfds;
	fd_set rfds;
	fd_set wfds;
	struct timeval tv;
	int cont;

	if (argc != 3) {
		printf("usage:  %s <IP address>\n", argv[0]);
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(atoi(argv[2]));

	connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	cont = 1;
	do {
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_SET(sockfd, &rfds);
//    FD_SET(sockfd, &wfds);
		FD_SET(STDIN_FILENO, &rfds);
//    FD_SET(STDOUT_FILENO, &wfds);
		nfds = sockfd + 1;
		selret = select(nfds, &rfds, &wfds, NULL, &tv);
		if (FD_ISSET(sockfd, &rfds)) {
			printf("selret = %d\n", selret);
			n = recvfrom(sockfd, recvline, 10000, 0, NULL, NULL);
			recvline[n] = 0;
			fputs(recvline, stdout);
			cont = n > 0;
		} else if (FD_ISSET(STDIN_FILENO, &rfds)) {
			if (fgets(sendline, 10000, stdin) != NULL) {
				cont = sendto(sockfd, sendline, (strlen(sendline) - 1) * sizeof(char), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
			}
		}
	} while (cont > 0);

	exit(0);

	while (fgets(sendline, 10000, stdin) != NULL) {
		sendto(sockfd, sendline, (strlen(sendline) - 1) * sizeof(char), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
		for (;;) {
			n = recvfrom(sockfd, recvline, 10000, 0, NULL, NULL);
			recvline[n] = 0;
			fputs(recvline, stdout);
			if (strcmp(recvline, "quit\n") == 0) {
				exit(0);
			}
		}
	}
}

