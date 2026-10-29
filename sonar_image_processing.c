#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int **allocateMatrix(int n);
void generateMatrix(int **matrix, int n);
void displayMatrix(int **matrix, int n);
void rotateMatrix(int **matrix, int n);
void smoothMatrix(int **matrix, int n);
void freeMatrix(int **matrix, int n);

int main(void)
{
    int n;
    int **matrix;

    printf("Enter matrix size (2-10): ");
    if (scanf("%d", &n) != 1 || n < 2 || n > 10) 
    {
        printf("Invalid matrix size.\n");
        return 1;
    }

    srand((unsigned int)time(NULL));

    matrix = allocateMatrix(n);
    if (matrix == NULL) 
    {
        printf("Memory allocation failed.\n");
        return 1;
    }

    generateMatrix(matrix, n);
    printf("Original Randomly Generated Matrix:\n");
    displayMatrix(matrix, n);

    rotateMatrix(matrix, n);
    printf("\nMatrix after 90 Clockwise Rotation:\n");
    displayMatrix(matrix, n);

    smoothMatrix(matrix, n);
    printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
    displayMatrix(matrix, n);

    freeMatrix(matrix, n);
    return 0;
}

int **allocateMatrix(int n)
{
    int **m = (int **)calloc(n, sizeof(int *));   /* NULL-filled rows */
    if (!m) return NULL;

    for (int i = 0; i < n; i++) 
    {
        *(m + i) = (int *)malloc(n * sizeof(int));
        if (!*(m + i)) 
        { 
            freeMatrix(m, n); 
            return NULL; 
        }
    }
    return m;
}

void generateMatrix(int **matrix, int n)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            *(*(matrix + i) + j) = rand() % 256;
}

void displayMatrix(int **matrix, int n)
{
    for (int i = 0; i < n; i++) 
    {
        for (int j = 0; j < n; j++)
            printf("%4d", *(*(matrix + i) + j));
        printf("\n");
    }
}

void rotateMatrix(int **matrix, int n)
{
    int tmp;
    //transpose 
    for (int i = 0; i < n; i++) 
    {
        for (int j = i + 1; j < n; j++) 
        {
            int *a = *(matrix + i) + j;     
            int *b = *(matrix + j) + i;     
            tmp = *a;
            *a  = *b;
            *b  = tmp;
        }
    }

    //reverse every row 
    for (int i = 0; i < n; i++) 
    {
        int *left  = *(matrix + i);
        int *right = *(matrix + i) + n - 1;
        while (left < right) 
        {
            tmp    = *left;
            *left  = *right;
            *right = tmp;
            left++;
            right--;
        }
    }
}

void smoothMatrix(int **matrix, int n)
{
    int *prevRow = (int *)malloc(n * sizeof(int));
    int *currRow = (int *)malloc(n * sizeof(int));   
    if (!prevRow || !currRow) 
    {
         free(prevRow); 
         free(currRow); 
         return; 
        }

    for (int i = 0; i < n; i++) 
    {
        for (int j = 0; j < n; j++)
            *(currRow + j) = *(*(matrix + i) + j);

        for (int j = 0; j < n; j++) 
        {
            int sum = 0, count = 0;
            for (int rowOffset = -1; rowOffset <= 1; rowOffset++) 
            {
                int nRow = i + rowOffset;
                if (nRow < 0 || nRow >= n) continue;          

                for (int colOffset = -1; colOffset <= 1; colOffset++) 
                {
                    int nCol = j + colOffset;
                    if (nCol < 0 || nCol >= n) continue;     

                    if (rowOffset == -1)
                        sum += *(prevRow + nCol);             
                    else if (rowOffset == 0)
                        sum += *(currRow + nCol);             
                    else
                        sum += *(*(matrix + nRow) + nCol);    
                    count++;
                }
            }
            *(*(matrix + i) + j) = sum / count;
        }

        for (int j = 0; j < n; j++)
            *(prevRow + j) = *(currRow + j);
    }
    free(prevRow);
    free(currRow);
}

void freeMatrix(int **matrix, int n)
{
    for (int i = 0; i < n; i++)
        free(*(matrix + i));     
    free(matrix);
}