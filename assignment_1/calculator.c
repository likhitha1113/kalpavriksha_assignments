#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main() {
    char s[1000];
    int stack[1000];
    int top = -1;
    int curr_num = 0;
    int result = 0;
    char op = '+';
    int next_num = 1;

    if (fgets(s, 1000, stdin) == NULL) {
    printf("Error: Invalid expression.\n");
    return 0;
    }
    s[strcspn(s, "\n")] = '\0';

    int len = strlen(s);

    for (int i = 0; i < len; i++) {
        char ch = s[i];

        if (isspace(ch)) {
            continue;
        }
        else if (isdigit(ch)) {
            curr_num = curr_num * 10 + (ch - '0');
            next_num = 0;
        }
        else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
            if (next_num == 1) {
                printf("Error: Invalid expression.\n");
                return 0;
            }

            if (op == '+') {
                top++;
                stack[top] = curr_num;
            }
            else if (op == '-') {
                top++;
                stack[top] = -curr_num;
            }
            else if (op == '*') {
                stack[top] = stack[top] * curr_num;
            }
            else if (op == '/') {
                if (curr_num == 0) {
                    printf("Error: Division by zero.\n");
                    return 0;
                }
                stack[top] = stack[top] / curr_num;
            }

            op = ch;
            curr_num = 0;
            next_num = 1;
        }
        else {
            printf("Error: Invalid expression.\n");
            return 0;
        }
    }

    // expression ended with an operator or was empty
    if (next_num == 1) {
        printf("Error: Invalid expression.\n");
        return 0;
    }

    // process the last number
    if (op == '+') {
        top++;
        stack[top] = curr_num;
    }
    else if (op == '-') {
        top++;
        stack[top] = -curr_num;
    }
    else if (op == '*') {
        stack[top] = stack[top] * curr_num;
    }
    else if (op == '/') {
        if (curr_num == 0) {
            printf("Error: Division by zero.\n");
            return 0;
        }
        stack[top] = stack[top] / curr_num;
    }

    // add everything in the stack
    while (top >= 0) {
        result = result + stack[top];
        top--;
    }

    printf("%d\n", result);
    return 0;
}