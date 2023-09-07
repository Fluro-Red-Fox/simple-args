#ifndef ARG_C
#define ARG_C

//

//

// Arg_table structure.
// contains main function arguments.
struct Arg_table;

// unpack arguments into an Arg_table structure.
struct Arg_table *Arg_Unpack(int argc, char **argv);

// free an Arg_table structure.
void Arg_Free(struct Arg_table *table);

// print an Arg_table structure.
void Arg_Print(struct Arg_table *table);

// check if an argument is defined.
int Arg_Check(struct Arg_table *table, char *key);

// get the int value of an argument.
int Arg_GetInt(struct Arg_table *table, char *key);

// get the string value of an argument.
char *Arg_GetString(struct Arg_table *table, char *key);

// create a new copy of a string.
char *NewStringCopy(char *string)

//

//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// internal arg.c type
enum Arg_type
{
    ARG_FLAG,
    ARG_STRING,
    ARG_INT
};

// internal arg.c type
union Arg_value
{
    char *string;
    int value;
};

// internal arg.c type
struct Arg
{
    char *key;
    enum Arg_type type;
    union Arg_value value;
};

struct Arg_table
{
    int num_args;
    struct Arg *args;
};

char *NewStringCopy(char *string)
{
    char *new = malloc(strlen(string) + 1);
    strcpy(new, string);
    return new;
}

// internal arg.c function
enum Arg_type Arg_ScanType(char *string)
{
    int len = strlen(string);
    int has_sign = 0;
    int has_value = 0;
    for (int i = 0; i < len; i++)
    {
        if (string[i] == '=')
        {
            has_value = 1;
            break;
        }
    }
    if (!has_value)
        return ARG_FLAG;
    while (string[0] != '=')
    {
        string++;
        len--;
    }
    string++;
    len--;
    if (string[0] == '\0')
        return ARG_STRING;
    for (int i = 0; i < len; i++)
    {
        if (string[i] == '-')
        {
            if (has_sign)
                return ARG_STRING;
            has_sign++;
        }
        else if (!(string[i] >= '0' && string[i] <= '9'))
            return ARG_STRING;
    }
    return ARG_INT;
}

// internal arg.c function
void Arg_TerminateName(char *string)
{
    while (string[0] != '=')
        string++;
    *string = '\0';
}

// internal arg.c function
char *Arg_ExtractValue(char *string)
{
    while (string[0] != '=')
        string++;
    return string + 1;
}

struct Arg_table *Arg_Unpack(int argc, char **argv)
{
    char *string;
    struct Arg *arg;
    struct Arg_table *table = malloc(sizeof(struct Arg_table));
    table->num_args = argc;
    table->args = malloc(sizeof(struct Arg) * argc);

    arg = table->args;
    arg->key = NewStringCopy("_path");
    arg->type = ARG_STRING;
    arg->value.string = NewStringCopy(argv[0]);

    for (int i = 1; i < argc; i++)
    {
        arg = table->args + i;
        arg->type = Arg_ScanType(argv[i]);
        switch (arg->type)
        {
            case ARG_FLAG:
            arg->key = NewStringCopy(argv[i]);
            break;

            case ARG_STRING:
            string = NewStringCopy(argv[i]);
            Arg_TerminateName(string);
            arg->key = NewStringCopy(string);
            free(string);
            arg->value.string = NewStringCopy(Arg_ExtractValue(argv[i]));
            break;

            case ARG_INT:
            string = NewStringCopy(argv[i]);
            Arg_TerminateName(string);
            arg->key = NewStringCopy(string);
            free(string);
            sscanf(Arg_ExtractValue(argv[i]), "%d", &(arg->value.value));
            break;
        }
    }

    return table;
};

void Arg_Print(struct Arg_table *table)
{
    for (int i = 0; i < table->num_args; i++)
    {
        switch (table->args[i].type)
        {
            case ARG_FLAG:
            printf("- %s\n", table->args[i].key);
            break;

            case ARG_STRING:
            printf("%s = %s\n", table->args[i].key, table->args[i].value.string);
            break;

            case ARG_INT:
            printf("%s = %d\n", table->args[i].key, table->args[i].value.value);
            break;
        }
    }
}

int Arg_Check(struct Arg_table *table, char *key)
{
    for (int i = 0; i < table->num_args; i++)
        if (!strcmp(key, table->args[i].key))
            return 1;
    return 0;
}

int Arg_GetInt(struct Arg_table *table, char *key)
{
    for (int i = 0; i < table->num_args; i++)
        if (!strcmp(key, table->args[i].key))
            return table->args[i].value.value;
    return 0;
}

char *Arg_GetString(struct Arg_table *table, char *key)
{
    for (int i = 0; i < table->num_args; i++)
        if (!strcmp(key, table->args[i].key))
            return table->args[i].value.string;
    return NULL;
}

void Arg_Free(struct Arg_table *table)
{
    struct Arg *arg;
    for (int i = 0; i < table->num_args; i++)
    {
        arg = table->args + i;
        if (arg->type == ARG_STRING)
            free(arg->value.string);
    }
    free(table->args);
    free(table);
}

#endif