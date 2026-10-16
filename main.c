#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

typedef struct {
    char **items;
    size_t count;
    size_t cap;
} StrList;

static void list_init(StrList *l) {
    l->items = NULL;
    l->count = 0;
    l->cap = 0;
}

static void list_push(StrList *l, const char *s) {
    if (l->count == l->cap) {
        l->cap = l->cap ? l->cap * 2 : 8;
        char **tmp = realloc(l->items, l->cap * sizeof(char*));
        if (!tmp) { perror("realloc"); exit(1); }
        l->items = tmp;
    }
    l->items[l->count] = strdup(s);   // POSIX; или malloc+memcpy
    if (!l->items[l->count]) { perror("strdup"); exit(1); }
    l->count++;
}

static void list_free(StrList *l) {
    for (size_t i = 0; i < l->count; i++) free(l->items[i]);
    free(l->items);
    l->items = NULL;
    l->count = l->cap = 0;
}

int symb(char a){
    if (a == '+') return 1;
    if (a == '-') return 1;
    if (a == '*') return 1;
    if (a == '/') return 1;
    if (a == '(') return 1;
    if (a == ')') return 1;
    return 0;
}

char** parser(char* a, int n){
    char* pars[128];
   
    return pars;
}

int main(void){
    char example[512];
    scanf("%511s", example);
    printf("Solve \"%s\" is always easy!", example);
}