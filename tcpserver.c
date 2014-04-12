#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/wait.h>
#include <netdb.h>


/******************************************************************************
 * Macros
 *****************************************************************************/
#define die(e) do { fprintf(stderr, e); exit(EXIT_FAILURE); } while (0);

#define MAXSOCK 1024

/******************************************************************************
 * Type Defs
 *****************************************************************************/
typedef struct split_res {
  char ** arr;
  int num;
} t_split_res;

/******************************************************************************
 * split a string into an array of strings. 
 * the returned array has to be freed by the caller.
 *****************************************************************************/
t_split_res * split(char * str) {
  char buf[1024];
  char ** res;
  char *  p;
  char *  tmpbuf;
  int n_spaces;
  int i, k;
  t_split_res * s_res;

  strcpy(buf, str);
  res = NULL;
  p = (char *)(long)strtok(buf, " ");
  n_spaces = 0;
  i = 0;
  s_res = (t_split_res *)malloc(sizeof(t_split_res));

  printf("Splitting String: '%s'\n", str);

  /* split string and append tokens to 'res' */
  while (p) {
    res = realloc(res, sizeof(char *) * ++n_spaces);
    if (res == NULL) {
      printf("Insufficient memory.\n");
      exit (-1); /* memory allocation failed */
    }
    res[n_spaces - 1] = p;
    p = (char *)(long)strtok(NULL, " ");
  }
  /* realloc one extra element for the last NULL */
  res = realloc(res, sizeof(char *) * (n_spaces + 1));
  res[n_spaces] = 0;
  for (i = 0; i < (n_spaces); i++) {
    printf("res[%d] = %s\n", i, res[i]);
    tmpbuf = (char *)malloc((strlen(res[i]) + 1) * sizeof(char *));
    strcpy(tmpbuf, res[i]);
    res[i] = tmpbuf;
  }
  s_res->arr = res;
  s_res->num = n_spaces + 1 ;
  return s_res;
}

void run(char * cmd, int connfd, struct sockaddr_in cliaddr) {
  int rlink[2];
  int wlink[2];
  pid_t pid;
  char foo[1024];
  int status;
  socklen_t clilen;
  char mesg[1000];
  int n;
  int selret;
  int nfds;
  fd_set rfds;
  fd_set wfds;
  fd_set efds;
  struct timeval tv;
  int killret;
  int waitret;

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
  if(pid == 0) {
    dup2(rlink[1], STDOUT_FILENO);
    close(rlink[0]);
    dup2(wlink[0], STDIN_FILENO);
    close(wlink[1]);
    system(cmd);
    close(rlink[1]);
    close(wlink[0]);
    exit(0);
  } else {
    close(rlink[1]);
    close(wlink[0]);
    n = 1024;
    selret = 0;
    while (waitret >= 0) {
      tv.tv_sec = 1;
      tv.tv_usec = 0;
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
      if (FD_ISSET(rlink[0], &rfds)) {
        n = read(rlink[0], foo, sizeof(foo));
        sendto(connfd, foo, n, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
      } else if (FD_ISSET(connfd, &rfds)) {
        n = recvfrom(connfd, mesg, 1000, 0, (struct sockaddr *)&cliaddr, &clilen);
        if (n < 0) goto end;
        mesg[n] = 0;
        printf("Received input from client: %s\n", mesg);
        write(wlink[1], mesg, n);
      } else if (FD_ISSET(connfd, &efds)) {
        goto end;
      }
      waitret = waitpid(pid, &status, WNOHANG);
    }
end:
    close(rlink[0]);
    close(wlink[1]);
    exit(0);
  }
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

  for(;;)
  {
    clilen = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

    if ((childpid = fork()) == 0) {
      close (listenfd);
      printf("-------------------------------------------------------\n");
      printf("Received a new connection.\n");
      n = recvfrom(connfd, mesg, 1000, 0, (struct sockaddr *)&cliaddr, &clilen);
      mesg[n] = 0;
      printf("Command:\n");
      printf("%s\n", mesg);
      printf("-------------------------------------------------------\n");
      if (strcmp(mesg, "quit\n") == 0) {
        printf("Client sent quit command, terminating connection.\n");
        close(connfd);
        exit(0);
      } else {
        printf("Executing %s\n", mesg);
        run(mesg, connfd, cliaddr);
      }
    } else {
      while (waitpid(-1, &status, WNOHANG) != 0);
    }
    close(connfd);
  }
  return 0;
}

/******************************************************************************
 * MAIN
 *****************************************************************************/
int main(int argc, char ** argv)
{
  struct addrinfo hints;
  struct addrinfo * res;
  struct addrinfo  *res0;
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
    errx(1, "%s", gai_strerror(error));
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
      close(s[nsock]);
      continue;
    }
    server_main_loop(s[nsock]);
    nsock++;
  }
  if (nsock == 0) {
    err(1, cause);
    /*NOTREACHED*/
  }
  freeaddrinfo(res0);
}



