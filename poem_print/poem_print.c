/*
 * =====================================================================================
 *
 *       Filename:  poem_print.c
 *
 *    Description:  控制台随机打印一首诗
 *
 *        Version:  1.0
 *        Created:  2013年05月20日 22时02分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "poem_print.h"

int main(int argc, char *argv[])
{
	int length;
	int index;

	length = sizeof(poem) / sizeof(*poem);
	
	srand(time(0));
	index = rand() % length;

	printf("\033[32m%s\033[0m", poem[index].title);
	printf("\033[33m%s\033[0m", poem[index].date);
	printf("\033[37m%s\033[0m", poem[index].info);

	return EXIT_SUCCESS;
}

