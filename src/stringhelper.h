/*
 * stringhelper.h
 *
 *  Created on: 06.05.2014
 *      Author: botho
 */

#ifndef STRINGHELPER_H_
#define STRINGHELPER_H_

/******************************************************************************
 * Type Defs
 *****************************************************************************/
typedef struct split_res {
	char ** arr;
	int num;
} t_split_res;

/******************************************************************************
 * General Functions
 *****************************************************************************/
t_split_res * split(char * str);



#endif /* STRINGHELPER_H_ */
