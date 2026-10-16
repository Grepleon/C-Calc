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

StrList parser(const char* a){
    StrList out;
    list_init(&out);

    char buf[256];
    size_t k=0;

    for (int i=0; a[i] != '\0'; i++){
        char l=a[i];
        if (l == ' '){
            if (k > 0){
                buf[k] = '\0';
                list_push(&out, buf);
                k = 0;
            } 
        } else if (symb(l)){
            if (k > 0){
                buf[k] = '\0';
                list_push(&out, buf);
                k = 0;
            }
            char _temp[2] = {l, '\0'};
            list_push(&out, _temp);
        }
        else {
            if (k < sizeof(buf)){
                buf[k] = l;
                k++;
            }
        }
    }

    if (k > 0) {
        buf[k] = '\0';
        list_push(&out, buf);
    }

    return out;
}

int main(void){
    char example[512];
    if (fgets(example, sizeof(example), stdin) == NULL) {
        // ошибка или EOF
        return 1;
    }
    example[strcspn(example, "\n")] = '\0';   // убрать '\n' с конца
    printf("Solve \"%s\" is always easy!\n", example);
    StrList pars = parser(example);
    for (int i=0; i<pars.count; i++){
        printf("%s, ", pars.items[i]);
    }
    printf("\nAll items: %zu", pars.count);
}