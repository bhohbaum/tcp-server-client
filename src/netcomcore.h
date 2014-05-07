/**
 * Network Communication Core
 *
 * @author Botho Hohbaum (bhohbaum@googlemail.com)
 * @package tcp-server-client
 * @copyright Copyright (c) Botho Hohbaum 04.05.2014
 * @license LGPL version 3
 * @link https://github.com/bhohbaum/tcp-server-client
 */

#ifndef _NETCOMCORE_H
#define _NETCOMCORE_H

#include <arpa/inet.h>

/******************************************************************************
 * Macros
 *****************************************************************************/
#define die(e) do { fprintf(stderr, e); exit(EXIT_FAILURE); } while (0);

#define MAXSOCK 1024

/******************************************************************************
 * Client Functions
 *****************************************************************************/
int client_start(int argc, char**argv);

/******************************************************************************
 * Server Functions
 *****************************************************************************/
void run(char * cmd, int connfd, struct sockaddr_in cliaddr);
int server_main_loop(int listenfd);
int server_start(int argc, char ** argv);

#endif
