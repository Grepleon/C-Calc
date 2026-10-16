#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

typedef struct {
    char **items;
    size_t count;
    size_t cap;
} StrList;

int power_max = 999;
int count_errors = 0;
char* errors = "";
int global_index = 0;

int power_symbol(char* a){
    if (a[0] == '+') return 1;
    if (a[0] == '-') return 1;
    if (a[0] == '*') return 2;
    if (a[0] == '/') return 2;
    if (a[0] == ')') return power_max;
    if (a[0] == '(') return power_max;
    count_errors++;
    return -1;
}

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

double solve(StrList pars, int i, int last_power, int level){
    char *endptr;
    printf("\ni: %i", i);
    char* num = pars.items[i];
    if (i > pars.count - 1){
        printf("%i > ERROR!", level);
        return 0;
    }
    if (i == pars.count - 1 || last_power >= power_symbol(pars.items[i + 1])){
        global_index += 2;
        double val = strtod(num, &endptr);
        printf(" [%i> %f, %i]", level, val, i);
        return val;
    }
    char* d = pars.items[i + 1];
    printf(" {%i> %s, %s}", level, num, d);
    
    double val = strtod(num, &endptr);
    double new_val = val;
    int power = power_symbol(d);
    i+=2;
    global_index += 2;
    level += 1;
    if (d[0] == '+'){
        new_val += solve(pars, i, power_symbol(d), level);
    } 
    if (d[0] == '-'){
        new_val -= solve(pars, i, power_symbol(d), level);
    } 
    if (d[0] == '*'){
        new_val *= solve(pars, i, power_symbol(d), level);
    } 
    if (d[0] == '/'){
        new_val /= solve(pars, i, power_symbol(d), level);
    } 

    printf("\ni: %i (%i> %f)", i-2, level-1, new_val);

    return new_val;
}

double base_solve(StrList pars){
    int i=0;
    int last_power = -1;
    double val = 0;
    int gi;
    int at=0;
    double add = solve(pars, i, last_power, 0);

    printf("\n FINAL: %f, %f <%i>", add, val, global_index);
    //global_index++;
    i = global_index;
    return val;
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
    double ans = base_solve(pars);
    printf("\nAnswer: %f", ans);
}