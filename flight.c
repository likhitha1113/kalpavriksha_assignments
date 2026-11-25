#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main() 
{
    int n;
    scanf("%d\n", &n);
    char source[105][101];
    char dest[105][101];
    char line[250];
    for(int i = 0; i < n; i++) 
    {
        fgets(line, sizeof(line), stdin);
        char *brk = strstr(line, "->");
        *brk = '\0';
        strcpy(source[i], line);
        strcpy(dest[i], brk + 2);
    }
    int start = -1;
    for(int i = 0; i < n; i++) 
    {
        int found = 0;
        for(int j = 0; j < n; j++)
        {
            if(strcmp(source[i],dest[j])== 0) 
            {
                found = 1;
                break;
            }
        }
        if(found == 0) 
        {
            start = i;
            break;
        }
    }
    int curr_index = start;
    for(int i = 0; i < n; i++)
    {
        printf("%s->%s", source[curr_index], dest[curr_index]);
        if(i <n - 1) 
        {
            printf(",");
        }
        for(int j = 0; j < n; j++) 
        {
            if(strcmp(dest[curr_index], source[j]) == 0) 
            {
                // i thought of moving to next source and
                // upate curr_index to the index of the next source and this process continues
            }
        }
    }
      return 0;
}