/**
 * Network Communication Core
 *
 * @author Botho Hohbaum (bhohbaum@googlemail.com)
 * @package tcp-server-client
 * @copyright Copyright (c) Botho Hohbaum 04.05.2014
 * @license LGPL version 3
 * @link https://github.com/bhohbaum/tcp-server-client
 */

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/wait.h>
#include <netdb.h>

#include "netcomcore.h"

int client_start(int argc, char**argv) {
	int sockfd, n;
	struct sockaddr_in servaddr;
	char sendline[32];
	char recvline[32];
	int selret;
	int nfds;
	fd_set rfds;
	fd_set wfds;
	struct timeval tv;
	int cont;

	if (argc != 3) {
		printf("usage:  %s <IP address>\n", argv[0]);
		exit(EXIT_FAILURE);
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
		selret = (selret > -1) ? selret : selret;
		if (FD_ISSET(sockfd, &rfds)) {
//			printf("selret = %d\n", selret);
			n = recvfrom(sockfd, recvline, sizeof(recvline), 0, NULL, NULL);
			recvline[n] = 0;
			fputs(recvline, stdout);
			cont = n > 0;
		} else if (FD_ISSET(STDIN_FILENO, &rfds)) {
			n = read(STDIN_FILENO, sendline, sizeof(sendline));
//			if (fgets(sendline, sizeof(sendline), stdin) != NULL) {
			if (n > 0) {
//				cont = sendto(sockfd, sendline, (strlen(sendline) - 1) * sizeof(char), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
				cont = sendto(sockfd, sendline, n, 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
			}
		}
//		printf("cont = %d\n", cont);
	} while (cont > 0);

	exit(EXIT_SUCCESS);

	while (fgets(sendline, sizeof(sendline), stdin) != NULL) {
		sendto(sockfd, sendline, (strlen(sendline) - 1) * sizeof(char), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
		for (;;) {
			n = recvfrom(sockfd, recvline, sizeof(recvline), 0, NULL, NULL);
			recvline[n] = 0;
			fputs(recvline, stdout);
			if (strcmp(recvline, "quit\n") == 0) {
				exit(EXIT_SUCCESS);
			}
		}
	}
	return EXIT_FAILURE;
}


void run(char * cmd, int connfd, struct sockaddr_in cliaddr) {
	int rlink[2];
	int wlink[2];
	pid_t pid;
	char foo[32];
	int status;
	socklen_t clilen;
	char mesg[32];
	int n;
	int selret;
	int nfds;
	fd_set rfds;
	fd_set wfds;
	fd_set efds;
	struct timeval tv;
	int waitret;
	int sleeptime;
	int retval = 0;

	while (retval == 0) {
		clilen = sizeof(cliaddr);
		if (pipe(rlink) == -1) {
			die("rpipe");
		}
		if (pipe(wlink) == -1) {
			die("wpipe");
		}
		if ((pid = fork()) == -1) {
			die("fork");
		}
		if (pid == 0) {
			dup2(rlink[1], STDOUT_FILENO);
			close(rlink[0]);
			dup2(wlink[0], STDIN_FILENO);
			close(wlink[1]);
			n = system(cmd);
			close(rlink[1]);
			close(wlink[0]);
			exit(n);
		} else {
			close(rlink[1]);
			close(wlink[0]);
			n = 1024;
			selret = 0;
			waitret = 1;
			sleeptime = 100000;
			while (waitret >= 0) {
//				sleeptime -= 100;
				tv.tv_sec = 0;
				tv.tv_usec = sleeptime;
				FD_ZERO(&rfds);
				FD_SET(rlink[0], &rfds);
				FD_SET(connfd, &rfds);
				FD_ZERO(&wfds);
				//      FD_SET(wlink[1], &wfds);
				//      FD_SET(connfd, &wfds);
				FD_ZERO(&efds);
				FD_SET(connfd, &efds);
				nfds = ((rlink[0] > wlink[1]) ? rlink[0] : (connfd > wlink[1]) ? connfd : wlink[1]) + 1;
				selret = select(nfds, &rfds, &wfds, NULL, &tv);
				selret = (selret > 0) ? 1 : 0;
				if (FD_ISSET(rlink[0], &rfds)) {
					printf("FD_ISSET(rlink[0], &rfds)\n");
					n = read(rlink[0], foo, sizeof(foo));
					if (n < 0) {
						retval = -1;
						goto end;
					}
					n = sendto(connfd, foo, n, 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
					if (n < 0) {
						retval = -1;
						goto end;
					}
				} else if (FD_ISSET(connfd, &rfds)) {
					printf("FD_ISSET(connfd, &rfds)\n");
					n = recvfrom(connfd, mesg, sizeof(mesg), 0, (struct sockaddr *) &cliaddr, &clilen);
					if (n < 0) {
						retval = -1;
						goto end;
					}
//					mesg[n] = 0;
//					printf("Received input from client: %s\n", mesg);
					n = write(wlink[1], mesg, n);
					if (n < 0) {
						retval = -1;
						goto end;
					}
				} else if (FD_ISSET(connfd, &efds)) {
					printf("excepion fd!!\n");
//					sleeptime += 100000;
					int error = 0;
					socklen_t len = sizeof(error);
					retval = getsockopt(connfd, SOL_SOCKET, SO_ERROR, &error, &len);
					printf("retval = %d, error = %d, len = %d, errno = %d, %s\n", retval, error, len, errno, strerror(errno));
					if (retval != 0) {
						goto end;
					}
				}
				waitret = waitpid(pid, &status, WNOHANG);
			}
			end: close(rlink[0]);
			close(wlink[1]);
			exit(0);
		}

/*		int error = 0;
		socklen_t len = sizeof(error);
		retval = getsockopt(connfd, SOL_SOCKET, SO_ERROR, &error, &len);
		printf("retval = %d, error = %d, len = %d, errno = %d, %s\n", retval, error, len, errno, strerror(errno));
*/	}
}

int server_main_loop(int listenfd) {
	struct sockaddr_in cliaddr;
	socklen_t clilen;
	int connfd;
	int n;
	int status;
	pid_t childpid;
	char mesg[1000];

	listen(listenfd, 1024);

	for (;;) {
		clilen = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);

		if ((childpid = fork()) == 0) {
			close(listenfd);
			printf("-------------------------------------------------------\n");
			printf("Received a new connection.\n");
			n = recvfrom(connfd, mesg, 1000, 0, (struct sockaddr *) &cliaddr, &clilen);
			mesg[n] = 0;
			printf("Command:\n");
			printf("%s\n", mesg);
			printf("-------------------------------------------------------\n");
			if (strcmp(mesg, "quit\n") == 0) {
				printf("Client sent quit command, terminating connection. close(connfd);\n");
				close(connfd);
				exit(0);
			} else {
				printf("Executing %s\n", mesg);
				run(mesg, connfd, cliaddr);
			}
		} else {
			while (waitpid(-1, &status, WNOHANG) != 0) {
				;
			}
		}
		printf("Terminating connection. close(connfd);\n");
		close(connfd);
	}
	return 0;
}

/******************************************************************************
 * MAIN
 *****************************************************************************/
int server_start(int argc, char ** argv) {
	struct addrinfo hints;
	struct addrinfo * res;
	struct addrinfo *res0;
	int error;
	int s[MAXSOCK];
	int nsock;
	const char * cause = NULL;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	error = getaddrinfo(argv[1], argv[2], &hints, &res0);
	if (error) {
		err(1, "%s", gai_strerror(error));
		/*NOTREACHED*/
	}
	nsock = 0;
	for (res = res0; res && nsock < MAXSOCK; res = res->ai_next) {
		s[nsock] = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (s[nsock] < 0) {
			cause = "socket";
			continue;
		}

		if (bind(s[nsock], res->ai_addr, res->ai_addrlen) < 0) {
			cause = "bind";
			printf("Terminating connection. close(s[nsock]);\n");
			close(s[nsock]);
			continue;
		}
		server_main_loop(s[nsock]);
		nsock++;
	}
	if (nsock == 0) {
		err(1, "%s", cause);
		/*NOTREACHED*/
	}
	freeaddrinfo(res0);
	return EXIT_SUCCESS;
}

