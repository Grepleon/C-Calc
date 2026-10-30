/*
 * C-Calc — простой калькулятор на языке C.
 *
 * Поддерживает: + - * /, скобки (), унарный минус и числа
 * с плавающей точкой. Читает одно выражение из stdin и
 * печатает ответ в stdout.
 *
 * Разбор построен на рекурсивном спуске:
 *   expression := term { ('+' | '-') term }
 *   term       := factor { ('*' | '/') factor }
 *   factor     := число | '(' expression ')' | '-' factor
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------
 * Лексер: разбивает входную строку на токены
 * ------------------------------------------------------------------ */

typedef enum {
    TOK_NUMBER, TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH,
    TOK_LPAREN, TOK_RPAREN, TOK_END, TOK_ERROR
} token_kind;

typedef struct {
    token_kind kind;
    double value;      /* значение, если kind == TOK_NUMBER */
    const char *error; /* сообщение, если kind == TOK_ERROR */
} token;

typedef struct {
    const char *src;   /* входная строка */
    size_t pos;        /* текущая позиция */
} lexer;

/* Пропустить пробельные символы */
static void skip_spaces(lexer *lx) {
    while (lx->src[lx->pos] != '\0' &&
           isspace((unsigned char)lx->src[lx->pos])) {
        lx->pos++;
    }
}

/* Взять следующий токен */
static token next_token(lexer *lx) {
    token tok;
    tok.kind = TOK_ERROR;
    tok.value = 0.0;
    tok.error = NULL;

    skip_spaces(lx);

    char c = lx->src[lx->pos];
    if (c == '\0') {
        tok.kind = TOK_END;
        return tok;
    }

    /* Число (целое или с плавающей точкой) */
    if (isdigit((unsigned char)c) || c == '.') {
        char *end = NULL;
        tok.kind = TOK_NUMBER;
        tok.value = strtod(lx->src + lx->pos, &end);
        if (end == lx->src + lx->pos) {
            tok.error = "некорректное число";
            return tok;
        }
        lx->pos = (size_t)(end - lx->src);
        return tok;
    }

    lx->pos++;
    switch (c) {
        case '+': tok.kind = TOK_PLUS;   break;
        case '-': tok.kind = TOK_MINUS;  break;
        case '*': tok.kind = TOK_STAR;   break;
        case '/': tok.kind = TOK_SLASH;  break;
        case '(': tok.kind = TOK_LPAREN; break;
        case ')': tok.kind = TOK_RPAREN; break;
        default:
            tok.error = "неизвестный символ";
            break;
    }
    return tok;
}

/* ------------------------------------------------------------------
 * Парсер: рекурсивный спуск
 * ------------------------------------------------------------------ */

typedef struct {
    lexer *lx;     /* лексер */
    token cur;     /* текущий токен */
    int failed;    /* флаг ошибки разбора */
} parser;

/* Перейти к следующему токену */
static void parser_advance(parser *p) {
    p->cur = next_token(p->lx);
    if (p->cur.kind == TOK_ERROR) {
        p->failed = 1;
    }
}

static double parse_expression(parser *p);

/* factor := число | '(' expression ')' | '-' factor */
static double parse_factor(parser *p) {
    if (p->failed) {
        return 0.0;
    }

    /* Унарный минус: -5, -(2+3), 2 * -3 */
    if (p->cur.kind == TOK_MINUS) {
        parser_advance(p);
        return -parse_factor(p);
    }

    if (p->cur.kind == TOK_NUMBER) {
        double v = p->cur.value;
        parser_advance(p);
        return v;
    }

    if (p->cur.kind == TOK_LPAREN) {
        parser_advance(p);
        double v = parse_expression(p);
        if (p->cur.kind == TOK_RPAREN) {
            parser_advance(p);
        } else if (!p->failed) {
            p->failed = 1;
            fprintf(stderr, "Ошибка: не хватает ')'\n");
        }
        return v;
    }

    p->failed = 1;
    fprintf(stderr, "Ошибка: неожиданный символ в выражении\n");
    return 0.0;
}

/* term := factor { ('*' | '/') factor } */
static double parse_term(parser *p) {
    double left = parse_factor(p);

    while (!p->failed) {
        if (p->cur.kind == TOK_STAR) {
            parser_advance(p);
            left *= parse_factor(p);
        } else if (p->cur.kind == TOK_SLASH) {
            parser_advance(p);
            double right = parse_factor(p);
            if (right == 0.0) {
                p->failed = 1;
                fprintf(stderr, "Ошибка: деление на ноль\n");
                return 0.0;
            }
            left /= right;
        } else {
            break;
        }
    }
    return left;
}

/* expression := term { ('+' | '-') term } */
static double parse_expression(parser *p) {
    double left = parse_term(p);

    while (!p->failed) {
        if (p->cur.kind == TOK_PLUS) {
            parser_advance(p);
            left += parse_term(p);
        } else if (p->cur.kind == TOK_MINUS) {
            parser_advance(p);
            left -= parse_term(p);
        } else {
            break;
        }
    }
    return left;
}

/* ------------------------------------------------------------------
 * Вывод результата: целое печатаем без дробной части,
 * остальное — с точностью 10 значащих цифр
 * ------------------------------------------------------------------ */
static void print_answer(double result) {
    if (result == (long long)result) {
        printf("Ответ: %lld\n", (long long)result);
    } else {
        printf("Ответ: %.10g\n", result);
    }
}

/* ------------------------------------------------------------------
 * Главная функция
 * ------------------------------------------------------------------ */
int main(void) {
    char input[512];

    printf("Введите выражение: ");
    fflush(stdout);

    /* Читаем одну строку из stdin (как в оригинальном C-Calc) */
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return 1;
    }
    input[strcspn(input, "\n")] = '\0'; /* убираем '\n' с конца */

    lexer lx;
    lx.src = input;
    lx.pos = 0;

    parser p;
    p.lx = &lx;
    p.failed = 0;

    parser_advance(&p);
    double result = parse_expression(&p);

    /* После выражения не должно остаться лишних символов */
    if (!p.failed && p.cur.kind != TOK_END) {
        p.failed = 1;
        fprintf(stderr, "Ошибка: лишние символы в конце выражения\n");
    }

    if (p.failed) {
        fprintf(stderr, "Не удалось вычислить выражение: \"%s\"\n", input);
        return 1;
    }

    print_answer(result);
    return 0;
}
