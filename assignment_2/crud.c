#include <stdio.h>
#include <string.h>

#define MAX_NAME 100

struct User
{
    int id;
    char name[MAX_NAME];
    int age;
};

void createUser();
void readUsers();
void updateUser();
void deleteUser();
int idExists(int id);

int main()
{
    int choice;

    do
    {
        printf("\n===== USER MENU =====\n");
        printf("1. Create User\n");
        printf("2. Read Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 1)
        {
            createUser();
        }
        else if (choice == 2)
        {
            readUsers();
        }
        else if (choice == 3)
        {
            updateUser();
        }
        else if (choice == 4)
        {
            deleteUser();
        }
        else if (choice == 5)
        {
            printf("Goodbye!\n");
        }
        else
        {
            printf("Invalid choice. Please try again.\n");
        }

    } while (choice != 5);

    return 0;
}

// checks if a user with this id is already in the file
// returns 1 if found, 0 if not found
int idExists(int id)
{
    FILE *fp;
    struct User u;

    fp = fopen("users.txt", "r");
    if (fp == NULL)
    {
        return 0;   // file doesn't exist, so the id can't exist
    }

    while (fscanf(fp, "%d|%99[^|]|%d\n", &u.id, u.name, &u.age) == 3)
    {
        if (u.id == id)
        {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

// CREATE: adding a new user at the end of the file
void createUser()
{
    FILE *fp;
    struct User u;

    // "a" mode creates the file if it doesn't exist
    fp = fopen("users.txt", "a");
    if (fp == NULL)
    {
        printf("Error: could not open file.\n");
        return;
    }

    printf("Enter ID: ");
    scanf("%d", &u.id);

    // ids must be unique
    if (idExists(u.id) == 1)
    {
        printf("A user with this ID already exists.\n");
        fclose(fp);
        return;
    }

    printf("Enter Name: ");
    scanf(" %[^\n]", u.name);   // reads the full name including spaces

    printf("Enter Age: ");
    scanf("%d", &u.age);

    fprintf(fp, "%d|%s|%d\n", u.id, u.name, u.age);

    fclose(fp);
    printf("User added successfully.\n");
}

// READ: show every user in the file
void readUsers()
{
    FILE *fp;
    struct User u;
    int count = 0;

    fp = fopen("users.txt", "r");
    if (fp == NULL)
    {
        printf("No users found.\n");
        return;
    }

    printf("\n--- All Users ---\n");

    while (fscanf(fp, "%d|%99[^|]|%d\n", &u.id, u.name, &u.age) == 3)
    {
        printf("ID: %d\n", u.id);
        printf("Name: %s\n", u.name);
        printf("Age: %d\n\n", u.age);
        count++;
    }

    if (count == 0)
    {
        printf("No users found.\n");
    }

    fclose(fp);
}

// UPDATE: change the name and age of one user
void updateUser()
{
    FILE *fp;
    FILE *temp;
    struct User u;
    int id;
    int found = 0;

    printf("Enter the ID of the user to update: ");
    scanf("%d", &id);

    fp = fopen("users.txt", "r");
    if (fp == NULL)
    {
        printf("Error: no users file found.\n");
        return;
    }

    temp = fopen("temp.txt", "w");
    if (temp == NULL)
    {
        printf("Error: could not create temporary file.\n");
        fclose(fp);
        return;
    }

    // copy every user to temp.txt, but change the one we want
    while (fscanf(fp, "%d|%99[^|]|%d\n", &u.id, u.name, &u.age) == 3)
    {
        if (u.id == id)
        {
            printf("Enter new Name: ");
            scanf(" %[^\n]", u.name);

            printf("Enter new Age: ");
            scanf("%d", &u.age);

            found = 1;
        }

        fprintf(temp, "%d|%s|%d\n", u.id, u.name, u.age);
    }

    fclose(fp);
    fclose(temp);

    if (found == 0)
    {
        remove("temp.txt");
        printf("User not found.\n");
    }
    else
    {
        remove("users.txt");
        rename("temp.txt", "users.txt");
        printf("User updated successfully.\n");
    }
}

// DELETE: remove one user from the file
void deleteUser()
{
    FILE *fp;
    FILE *temp;
    struct User u;
    int id;
    int found = 0;

    printf("Enter the ID of the user to delete: ");
    scanf("%d", &id);

    fp = fopen("users.txt", "r");
    if (fp == NULL)
    {
        printf("Error: no users file found.\n");
        return;
    }

    temp = fopen("temp.txt", "w");
    if (temp == NULL)
    {
        printf("Error: could not create temporary file.\n");
        fclose(fp);
        return;
    }

    // copy every user to temp.txt except the one we want to delete
    while (fscanf(fp, "%d|%99[^|]|%d\n", &u.id, u.name, &u.age) == 3)
    {
        if (u.id == id)
        {
            found = 1;   // skip this user (don't write it)
        }
        else
        {
            fprintf(temp, "%d|%s|%d\n", u.id, u.name, u.age);
        }
    }

    fclose(fp);
    fclose(temp);

    if (found == 0)
    {
        remove("temp.txt");
        printf("User not found.\n");
    }
    else
    {
        remove("users.txt");
        rename("temp.txt", "users.txt");
        printf("User deleted successfully.\n");
    }
}