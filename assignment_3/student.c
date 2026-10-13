
#include <stdio.h>

struct Student_Details
{
    int roll_no;
    char name[50];
    int s1, s2, s3;
};

int total(int a, int b, int c)
{
    int sum;
    sum = a + b + c;
    return sum;
}

float average(int total_marks)
{
    float avg;
    avg = total_marks / 3.0;
    return avg;
}

char grade(float avg_marks)
{
    if(avg_marks >= 85)
        return 'A';
    else if(avg_marks >= 70)
        return 'B';
    else if(avg_marks >= 50)
        return 'C';
    else if(avg_marks >= 35)
        return 'D';
    else
        return 'F';
}


void stars(char grade)
{
    int i;

    switch(grade)
    {
        case 'A':
            for(i = 0; i < 5; i++)
                printf("*");
            break;

        case 'B':
            for(i = 0; i < 4; i++)
                printf("*");
            break;

        case 'C':
            for(i = 0; i < 3; i++)
                printf("*");
            break;

        case 'D':
            for(i = 0; i < 2; i++)
                printf("*");
            break;
    }
}

void display_roll(struct Student_Details students[], int n)
{
    if(n == 0)
        return;

    display_roll(students, n - 1);
    printf("%d ", students[n - 1].roll_no);
}

int main()
{
    struct Student_Details students[100];
    int n, i;
    int total_marks;
    float avg_marks;
    char g;

    printf("Enter number of students: ");
    scanf("%d", &n);

    for(i = 0; i < n; i++)
    {
        printf("\nEnter details of student %d:\n", i + 1);
        scanf("%d %s %d %d %d",
              &students[i].roll_no,
              students[i].name,
              &students[i].s1,
              &students[i].s2,
              &students[i].s3);
    }

    printf("\n");

    for(i = 0; i < n; i++)
    {
        total_marks = total(students[i].s1, students[i].s2, students[i].s3);
        avg_marks = average(total_marks);
        g = grade(avg_marks);

        printf("Roll: %d\n", students[i].roll_no);
        printf("Name: %s\n", students[i].name);
        printf("Total: %d\n", total_marks);
        printf("Average: %.2f\n", avg_marks);
        printf("Grade: %c\n", g);

        if(avg_marks < 35)
        {
            continue;
        }

        printf("Performance: ");
        stars(g);
        printf("\n");
    }

    printf("List of Roll Numbers: ");
    display_roll(students, n);
    printf("\n");

    return 0;
}
