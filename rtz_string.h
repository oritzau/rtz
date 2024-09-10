#ifdef RTZ_STR_IMPLEMENTATION
#define rtz_strnew strnew
#define rtz_strfree strfree
#define rtz_strfrom strfrom
#define rtz_strreverse strreverse
#define rtz_strcopy strcopy
#define rtz_strpushch strpushch
#define rtz_strpushcstring strpushcstring
#define rtz_strcontains strcontains
#define rtz_strinto strinto
#define rtz_strsplit strsplit
#define rtz_strprint strprint
#endif
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct String String;

struct String
{
	size_t len;
	size_t capacity;
	char *inner;
};

#define DEFAULT_CAPACITY 8

String *rtz_strnew(void);
void rtz_strfree(String *str);
String *rtz_strfrom(char *cstring);
String *rtz_strcopy(String *str);
String *rtz_strreverse(String *str);
int rtz_strpushch(String *str, char ch);
int rtz_strpushcstring(String *str, char *cstring);
int rtz_strcontains(String *str, char *pattern);
char *rtz_strinto(String *str);
String **rtz_strsplit(String *str, char *pattern, size_t *result_size);
void rtz_strprint(String *str);

String *rtz_strnew(void)
{
	String *str = malloc(sizeof(*str));
	if (!str) return 0;
	str->len = 0;
	str->capacity = DEFAULT_CAPACITY;
	str->inner = malloc(sizeof(char) * DEFAULT_CAPACITY);
	if (!str->inner)
	{
		free(str);
		return 0;
	}
	return str;
}

String *rtz_strfrom(char *cstring)
{
	String *str = rtz_strnew();
	if (!str) return 0;
	for (size_t i = 0; cstring[i]; i++)
		if (!rtz_strpushch(str, cstring[i]))
		{
			rtz_strfree(str);
			return 0;
		}
	return str;
}

String *rtz_strreverse(String *str)
{
	String *s = rtz_strnew();
	if (!s) return 0;

	for (size_t i = s->len; i > 0; i--)
		if (!rtz_strpushch(s, str->inner[i - 1]))
		{
			rtz_strfree(s);
			return 0;
		}
	return s;
}

String *rtz_strcopy(String *str)
{
	String *s = rtz_strnew();
	for (size_t i = 0; i < str->len; i++)
		if (!rtz_strpushch(s, str->inner[i]))
		{
			rtz_strfree(s);
			return 0;
		}
	return s;
}

int rtz_strpushchar(String *str, char ch)
{
	if (!str) return 0;
	if (str->len == str->capacity)
	{
		str->capacity *= 2;
		str->inner = realloc(str->inner, str->capacity);
		if (!str->inner)
		{
			fprintf(stderr, "ctring_pushchar: out of memory\n");
			rtz_strfree(str);
			return 0;
		}
	}
	str->inner[str->len] = ch;
	str->len++;
	return 1;
}
int rtz_strpushcstring(String *str, char *cstring)
{
	if (!str) return 0;
	for (size_t i = 0; cstring[i]; i++)
	{
		if (!rtz_strpushch(str, cstring[i]))
			return 0;
	}
	return 1;
}

int rtz_strcontains(String *str, char *pattern)
{
	size_t pattern_len, i, j;

	pattern_len = strlen(pattern);
	for (i = 0; i < str->len; i++)
	{
		j = 0;
		while (i + j < str->len && str->inner[i + j] == pattern[j])
		{
			if (j == pattern_len - 1) return 1;
			j++;
		}
	}
	return 0;
}

char *rtz_strinto(String *str)
{
	if (!str) return 0;
	rtz_strpushch(str, 0);
	char *cstring = str->inner;
	free(str);
	return cstring;
}

void rtz_strfree(String *str)
{
	if (!str) return;
	free(str->inner);
	free(str);
}

String **rtz_strsplit(String *str, char *pattern, size_t *result_size)
{
	size_t i, j, k, offset, pattern_len;
	String **result;

	if (!str) return 0;
	if (!pattern || !*pattern) return 0;
	pattern_len = strlen(pattern);
	*result_size = 0;
	result = malloc(str->len * sizeof(String *));
	for (i = 0; i < str->len; i++)
		result[i] = rtz_strnew();
	i = j = offset = 0;
	while (i < str->len)
	{
		while (str->inner[i] == pattern[j])
		{
			if (j == pattern_len - 1)
			{
				for (k = offset; k < i - offset - pattern_len - 1; k++)
					rtz_strpushch(result[*result_size], str->inner[k]);
				offset = i + 1;
				(*result_size)++;
				break;
			}
			i++;
			j++;
		}
		j = 0;
		i++;
	}
	for (k = offset; k < i - offset - pattern_len - 1; k++)
		rtz_strpushch(result[*result_size], str->inner[k]);
	(*result_size)++;

	for (i = *result_size; i < str->len; i++)
		rtz_strfree(result[i]);
	return result;
}
