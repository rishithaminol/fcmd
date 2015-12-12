/* fcmd.c - main C source file of fcmd binary */

/* Copyright (C) 2015 	Rishitha Minol

   fcmd is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fcmd is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>             /* struct dirent *, DIR * */
#include <string.h>
#include <errno.h>              /* int errno */
#include <regex.h>
#include <malloc.h>             /* malloc_usable_size() */
#include <assert.h>             /*	assert() */

#define REGEX_MSG_BUFF 256      /* 'char regex_msg[REGEX_MSG_BUFF]' */

/*Program name*/
char *prog_name;

struct list {
	char *		content;
	struct list *	next;
};

struct list *list_allocate(void);
struct list *list_append(struct list *_list, const char *str);
int list_count(struct list *_list);
void list_clean(struct list *_list);
void list_free(struct list *_list);
char *list_element(struct list *_list, int pos);
const char **list_to_array(struct list *_list);
struct list *array_to_list(char **array);
void swap_str(const char **x, const char **y);
const char **list_sort(struct list *_list);
struct list *tokenize(char *string, const char *delim);
DIR *fcmd_opendir(const char *directory);
struct list *match_file_list(const char *dir_name, regex_t *regex);
int is_dir(struct dirent *dir);
regex_t compile_regex(const char *str);
void usage(void);

int main(int argc, char *argv[])
{
	int i, n;
	char *path, *dir_name;
	struct list *path_list;
	struct list *matched_list;
	regex_t regex;

	prog_name = *argv;

	if (argc < 2) {
		fprintf(stderr, "Too few arguments\n");
		usage();
		exit(EXIT_FAILURE);
	}

	path = getenv("PATH");
	assert(path != NULL);
	path_list = tokenize(path, ":");

	regex = compile_regex(argv[1]);

	n = list_count(path_list);
	for (i = 1; i <= n; i++) {
		dir_name = list_element(path_list, i);

		if ((matched_list = match_file_list(dir_name, &regex)) == NULL)
			continue;

		int abc, def;

		def = list_count(matched_list);
		for (abc = 1; abc <= def; abc++)
			printf("%s/%s\n", dir_name, list_element(matched_list, abc));
		list_clean(matched_list);
	}
	regfree(&regex);
	list_clean(path_list);

	return 0;
}

struct list *list_allocate(void)
{
	return (struct list *)malloc(sizeof(struct list));
}

/*
 * Take old list, duplicate str to the end
 * of the old list and return new list
 */
struct list *list_append(struct list *_list, const char *str)
{
	struct list *list_head = _list;
	struct list *tmp = list_allocate();

	tmp->content = strdup(str);
	tmp->next = NULL;

	if (_list == NULL) {
		list_head = tmp;

		return list_head;
	} else {
		while (_list->next != NULL)
			_list = _list->next;
	}

	_list->next = tmp;

	return list_head;
}

/*
 * return the number of elements in a 'struct list *'
 */
int list_count(struct list *_list)
{
	assert(_list != NULL);

	if (_list->next == NULL)
		return 1;

	return 1 + list_count(_list->next);
}

/*
 * Free list pointers and their pointed objects
 */
void list_clean(struct list *_list)
{
	assert(_list != NULL /*strcmp(_list->content, "")*/);

	if (_list->next != NULL)
		list_clean(_list->next);

	free(_list->content);
	free(_list);
	_list = NULL;
}

/*
 * Free list pointers
 */
void list_free(struct list *_list)
{
	if (_list->next != NULL)
		list_free(_list->next);

	free(_list);
	_list = NULL;
}

/*
 * return pointer to '_list->content' element in 'pos' position
 */
char *list_element(struct list *_list, int pos)
{
	int i;
	int list_len = list_count(_list);

	if (list_len < pos)
		return NULL;

	for (i = 0; i < (pos - 1); i++)
		_list = _list->next;

	return _list->content;
}

/*
 * points list elements into array elements
 */
const char **list_to_array(struct list *_list)
{
	int i, x;

	x = list_count(_list);
	const char **array = (const char **)malloc(sizeof(char *) * x);

	for (i = 0; i < x; i++) {
		array[i] = _list->content;
		_list = _list->next;
	}

	return array;
}

/*
 * points array elements into list elements
 */
struct list *array_to_list(char **array)
{
	int i, elements;
	struct list *tmp, *list_return;

	elements = (malloc_usable_size(array) / sizeof(char *));

	tmp = list_allocate();
	list_return = tmp;

	for (i = 0; i < elements; i++) {
		tmp->content = *(array + i);

		if (i + 1 == elements)
			break;

		tmp->next = list_allocate();
		tmp = tmp->next;
	}
	tmp->next = NULL;

	return list_return;
}

/*
 * swaps given strings
 */
void swap_str(const char **x, const char **y)
{
	const char *t;

	t = *x;
	*x = *y;
	*y = t;
}

/*
 * Sorts given 'struct list *'
 *		- Insertion algorithm
 */
const char **list_sort(struct list *_list)
{
	const char **array = list_to_array(_list);
	int x = list_count(_list);
	int c, d;

	for (c = 1; c <= x - 1; c++)
		for (d = c; d > 0 && strcmp(array[d], array[d - 1]) < 0; d--)
			swap_str(&array[d], &array[d - 1]);

	return array;
}

/*
 * breaks into tokens using given delim and
 * store them in a 'struct list *'.
 * All broken tokens are pointed to list elements
 */
struct list *tokenize(char *string, const char *delim)
{
	char *token, *str, *str_free;
	struct list *str_list = NULL;

	str = strdup(string);
	str_free = str;

	while ((token = strsep(&str, delim)) != NULL) {
		str_list = list_append(str_list, token);

		if (str == NULL)
			break;
	}

	free(str_free);

	return str_list;
}

/*
 * Open named 'directory'
 *		- error report
 */
DIR *fcmd_opendir(const char *directory)
{
	int errnum;
	DIR *pdir;

	pdir = opendir(directory);
	errnum = errno;         /* errno --> $? in shell */

	if (pdir == NULL) {
		fprintf(stderr, "%s: %s\n", prog_name, strerror(errnum));
		exit(errnum);
	}

	return pdir;
}

/*
 * is_dir - checks wether dir is a directory
 */
int is_dir(struct dirent *dir)
{
	return dir->d_type == DT_DIR;
}

/*
 * return file lists matched with the regex
 * if no one match this return NULL
 */
struct list *match_file_list(const char *dir_name, regex_t *regex)
{
	int regex_return;
	struct list *matched_list = NULL;
	struct dirent *dir;
	DIR *pdir = fcmd_opendir(dir_name);

	while ((dir = readdir(pdir)) != NULL) {
		regex_return = regexec(regex, dir->d_name, 0, NULL, 0);

		if (regex_return == 0 && !is_dir(dir))
			matched_list = list_append(matched_list, dir->d_name);
	}

	closedir(pdir);

	return matched_list;
}

/*
 * compile and return the regex_t(regular expression) for the given string
 */
regex_t compile_regex(const char *str)
{
	int ret;
	char regex_msg[REGEX_MSG_BUFF];
	regex_t regex;

	ret = regcomp(&regex, str, REG_EXTENDED);

	if (ret) {
		regerror(ret, &regex, regex_msg, sizeof(regex_msg));
		fprintf(stderr, "%s: %s\n", prog_name, regex_msg);
		exit(ret);
	}

	return regex;
}

void usage(void)
{
	printf("Usage: fcmd [OPTIONS] command\n\n" 
		"For bug reporting\n"
		"<lucidminol@gmail.com>\n");
}
