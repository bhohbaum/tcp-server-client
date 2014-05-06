/*
 * stringhelper.c
 *
 *  Created on: 06.05.2014
 *      Author: botho
 */

#include <string.h>

#include "stringhelper.h"

/******************************************************************************
 * split a string into an array of strings.
 * the returned array has to be freed by the caller.
 *****************************************************************************/
t_split_res * split(char * str) {
	char buf[1024];
	char ** res;
	char * p;
	char * tmpbuf;
	int n_spaces;
	int i;
	t_split_res * s_res;

	strcpy(buf, str);
	res = NULL;
	p = (char *) (long) strtok(buf, " ");
	n_spaces = 0;
	i = 0;
	s_res = (t_split_res *) malloc(sizeof(t_split_res));

	printf("Splitting String: '%s'\n", str);

	/* split string and append tokens to 'res' */
	while (p) {
		res = realloc(res, sizeof(char *) * ++n_spaces);
		if (res == NULL) {
			printf("Insufficient memory.\n");
			exit(-1); /* memory allocation failed */
		}
		res[n_spaces - 1] = p;
		p = (char *) (long) strtok(NULL, " ");
	}
	/* realloc one extra element for the last NULL */
	res = realloc(res, sizeof(char *) * (n_spaces + 1));
	res[n_spaces] = 0;
	for (i = 0; i < (n_spaces); i++) {
		printf("res[%d] = %s\n", i, res[i]);
		tmpbuf = (char *) malloc((strlen(res[i]) + 1) * sizeof(char *));
		strcpy(tmpbuf, res[i]);
		res[i] = tmpbuf;
	}
	s_res->arr = res;
	s_res->num = n_spaces + 1;
	return(s_res);
}

