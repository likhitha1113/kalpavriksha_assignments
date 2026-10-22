#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 200

typedef struct Employee Employee;
typedef struct Report Report;
struct Report 
{
    Employee *emp;
    Report *next;
};
struct Employee 
{
    int id;
    char name[50];
    char designation[50];
    Employee *manager;
    Report *reports;
};
Employee *employees[MAX];
int n = 0;
// Find employee using ID 
Employee *findEmployee(int id) 
{
    for (int i = 0; i < n; i++) 
    {
        if (employees[i]->id == id)
            return employees[i];
    }
    return NULL;
}
// Count direct reports 
int directCount(Employee *emp) 
{
    int count = 0;
    Report *temp = emp->reports;
    while (temp != NULL) 
    {
        count++;
        temp = temp->next;
    }
    return count;
}
//Add an employee to the manager's report list 
int addReport(Employee *manager, Employee *emp) 
{
    Report *node = malloc(sizeof(Report));
    if (node == NULL)
        return 0;
    node->emp = emp;
    node->next = NULL;
    if (manager->reports == NULL)
    {
        manager->reports = node;
    } 
    else 
    {
        Report *temp = manager->reports;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = node;
    }
    return 1;
}
//Remove an employee from the manager's report list 
void removeReport(Employee *manager, Employee *emp) 
{
    Report *temp = manager->reports;
    Report *prev = NULL;
    while (temp != NULL) 
    {
        if (temp->emp == emp) 
        {
            if (prev == NULL)
                manager->reports = temp->next;
            else
                prev->next = temp->next;
            free(temp);
            return;
        }
        prev = temp;
        temp = temp->next;
    }
}
// Count all employees under an employee using recursion 
int countEmployees(Employee *emp) 
{
    if (emp == NULL)
        return 0;
    int count = 0;
    Report *temp = emp->reports;
    while (temp != NULL) 
    {
        count += 1 + countEmployees(temp->emp);
        temp = temp->next;
    }
    return count;
}
//Display hierarchy using recursion 
void displayHierarchy(Employee *emp, int level) 
{
    if (emp == NULL)
        return;
    for (int i = 0; i < level; i++)
        printf("    ");
    printf("%d - %s - %s\n",
           emp->id, emp->name, emp->designation);
    Report *temp = emp->reports;
    while (temp != NULL) 
    {
        displayHierarchy(temp->emp, level + 1);
        temp = temp->next;
    }
}
// Add employee 
void addEmployee(void) 
{
    int id, managerID;
    char name[50], designation[50];
    if (n >= MAX) 
    {
        printf("Employee limit reached.\n");
        return;
    }
    printf("Enter Employee ID: ");
    if (scanf("%d", &id) != 1)
        return;
    if (findEmployee(id) != NULL) 
    {
        printf("Employee ID already exists.\n");
        return;
    }
    printf("Enter Name: ");
    scanf(" %49[^\n]", name);

    printf("Enter Designation: ");
    scanf(" %49[^\n]", designation);

    printf("Enter Reporting Manager ID (0 for CEO): ");
    if (scanf("%d", &managerID) != 1)
        return;
    Employee *manager = NULL;
    if (managerID == 0) 
    {
        if (n != 0) 
        {
            printf("Only the first employee can be the CEO.\n");
            return;
        }
    } 
    else 
    {
        manager = findEmployee(managerID);
        if (manager == NULL) 
        {
            printf("Reporting manager not found.\n");
            return;
        }
        if (managerID == id) 
        {
            printf("An employee cannot report to themselves.\n");
            return;
        }
    }
    Employee *emp = malloc(sizeof(Employee));
    if (emp == NULL) 
    {
        printf("Memory allocation failed.\n");
        return;
    }
    emp->id = id;
    strcpy(emp->name, name);
    strcpy(emp->designation, designation);
    emp->manager = manager;
    emp->reports = NULL;
    if (manager != NULL && !addReport(manager, emp)) 
    {
        free(emp);
        printf("Memory allocation failed.\n");
        return;
    }
    employees[n++] = emp;
    printf("Employee added successfully.\n");
}
//Remove employee 
void removeEmployee(void) 
{
    int id;
    printf("Enter Employee ID to remove: ");
    if (scanf("%d", &id) != 1)
        return;
    Employee *emp = findEmployee(id);
    if (emp == NULL) 
    {
        printf("Employee not found.\n");
        return;
    }
    //The CEO cannot be removed
    if (emp->manager == NULL) 
    {
        printf("The CEO cannot be removed.\n");
        return;
    }
    Employee *manager = emp->manager;
    Employee *target = NULL;
    Report *temp = manager->reports;
    //Find the sibling with the fewest direct reports
    while (temp != NULL) 
    {
        Employee *sibling = temp->emp;
        if (sibling != emp) 
        {
            if (target == NULL || directCount(sibling) < directCount(target)) 
                target = sibling;
        }
        temp = temp->next;
    }
    //If there are no siblings, use the reporting manager
    if (target == NULL)
        target = manager;
    /*
       Transfer the report list without copying employees.
       Update each transferred employee's manager pointer.
    */
    Report *first = emp->reports;
    if (first != NULL) 
    {
        Report *last = first;
        while (last != NULL) 
        {
            last->emp->manager = target;
            if (last->next == NULL)
                break;
            last = last->next;
        }
        if (target->reports == NULL) 
        {
            target->reports = first;
        } 
        else 
        {
            Report *tail = target->reports;
            while (tail->next != NULL)
                tail = tail->next;
            tail->next = first;
        }
        emp->reports = NULL;
    }
    removeReport(manager, emp);
    // Remove employee from the main array
    for (int i = 0; i < n; i++) 
    {
        if (employees[i] == emp) 
        {
            for (int j = i; j < n - 1; j++)
                employees[j] = employees[j + 1];
            n--;
            break;
        }
    }
    free(emp);
    printf("Employee removed successfully.\n");
}
//Display hierarchy of a selected employee 
void displayEmployee(void) 
{
    int id;
    printf("Enter Employee ID: ");
    if (scanf("%d", &id) != 1)
        return;
    Employee *emp = findEmployee(id);
    if (emp == NULL) 
    {
        printf("Employee not found.\n");
        return;
    }
    printf("\n EMPLOYEE HIERARCHY :\n");
    displayHierarchy(emp, 0);
    printf("Total Employees Under %d: %d\n",
           id, countEmployees(emp));
}
//Display the complete organization
void displayComplete(void) 
{
    if (n == 0) 
    {
        printf("No employees in the organization.\n");
        return;
    }
    printf("\n ORGANIZATION HIERARCHY :\n");
    for (int i = 0; i < n; i++) 
    {
        if (employees[i]->manager == NULL)
            displayHierarchy(employees[i], 0);
    }
    printf("Total Employees: %d\n", n);
}
void cleanup(void) 
{
    for (int i = 0; i < n; i++) 
    {
        Report *temp = employees[i]->reports;
        while (temp != NULL) 
        {
            Report *next = temp->next;
            free(temp);
            temp = next;
        }
        free(employees[i]);
    }
    n = 0;
}
int main(void) 
{
    int choice;
    while (1) 
    {
        printf("\n EMPLOYEE HIERARCHY MANAGEMENT :\n");
        printf("1. Add Employee\n");
        printf("2. Remove Employee\n");
        printf("3. Display Employee Hierarchy\n");
        printf("4. Display Complete Hierarchy\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) 
        {
            printf("Invalid input.\n");
            cleanup();
            return 1;
        }
        switch (choice) 
        {
            case 1:
                addEmployee();
                break;
            case 2:
                removeEmployee();
                break;
            case 3:
                displayEmployee();
                break;
            case 4:
                displayComplete();
                break;
            case 5:
                cleanup();
                printf("Program terminated.\n");
                return 0;
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
}
