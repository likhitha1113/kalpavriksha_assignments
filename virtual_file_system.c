#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define MAX_NAME 50
#define MAX_INPUT 8192

typedef struct FreeBlock 
{
    int index;
    struct FreeBlock *next;
    struct FreeBlock *prev;
} FreeBlock;

typedef struct FileNode 
{
    char name[MAX_NAME + 1];
    int isDirectory;
    int size;
    int blockCount;
    int *blockPointers;
    struct FileNode *parent;
    struct FileNode *child;
    struct FileNode *next;
} FileNode;

char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];
FreeBlock *freeHead = NULL;
FreeBlock *freeTail = NULL;
int freeCount = 0;
FileNode *root = NULL;
FileNode *cwd = NULL;

void *safeMalloc(size_t size) 
{
    void *ptr = malloc(size);
    if (ptr == NULL) 
    {
        printf("Error: memory allocation failed.\n");
        exit(1);
    }
    return ptr;
}

void addFreeBlockAtTail(int index) 
{
    FreeBlock *node = (FreeBlock *)safeMalloc(sizeof(FreeBlock));
    node->index = index;
    node->next = NULL;
    node->prev = freeTail;
    if (freeTail == NULL) 
    {
        freeHead = node;
    } 
    else 
    {
        freeTail->next = node;
    }
    freeTail = node;
    freeCount++;
}

int removeFreeBlockFromHead() 
{
    if (freeHead == NULL) 
    {
        return -1;
    }
    FreeBlock *node = freeHead;
    int index = node->index;
    freeHead = node->next;

    if (freeHead != NULL) 
    {
        freeHead->prev = NULL;
    } 
    else 
    {
        freeTail = NULL;
    }

    free(node);
    freeCount--;
    return index;
}

void initDisk() 
{
    int i;
    memset(virtualDisk, 0, sizeof(virtualDisk));
    for (i = 0; i < NUM_BLOCKS; i++) 
    {
        addFreeBlockAtTail(i);
    }
}

FileNode *createNode(const char *name, int isDirectory) 
{
    FileNode *node = (FileNode *)safeMalloc(sizeof(FileNode));
    strcpy(node->name, name);
    node->isDirectory = isDirectory;
    node->size = 0;
    node->blockCount = 0;
    node->blockPointers = NULL;
    node->parent = NULL;
    node->child = NULL;
    node->next = NULL;
    return node;
}

void insertNode(FileNode *dir, FileNode *node) 
{
    node->parent = dir;
    if (dir->child == NULL) 
    {
        node->next = node;
        dir->child = node;
    } 
    else 
    {
        FileNode *temp = dir->child;
        while (temp->next != dir->child) 
        {
            temp = temp->next;
        }
        temp->next = node;
        node->next = dir->child;
    }
}

void removeNode(FileNode *dir, FileNode *node) 
{
    if (dir->child == node && node->next == node) 
    {
        dir->child = NULL;
    } 
    else 
    {
        FileNode *prev = dir->child;
        while (prev->next != node) 
        {
            prev = prev->next;
        }
        prev->next = node->next;
        if (dir->child == node) 
        {
            dir->child = node->next;
        }
    }
    node->next = NULL;
    node->parent = NULL;
}

FileNode *findInCwd(const char *name) 
{
    if (cwd->child == NULL) 
    {
        return NULL;
    }
    FileNode *temp = cwd->child;
    do 
    {
        if (strcmp(temp->name, name) == 0) 
        {
            return temp;
        }
        temp = temp->next;
    } 
    while (temp != cwd->child);

    return NULL;
}

int isValidName(const char *name) 
{
    int len = (int)strlen(name);
    if (len == 0) 
    {
        return 0;
    }
    if (len > MAX_NAME) 
    {
        printf("Error: name too long (max %d characters).\n", MAX_NAME);
        return 0;
    }
    if (strchr(name, '/') != NULL) 
    {
        printf("Error: name cannot contain '/'.\n");
        return 0;
    }
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
    {
        printf("Error: invalid name '%s'.\n", name);
        return 0;
    }
    return 1;
}

char *getPath(FileNode *node) {
    int total = 0;
    FileNode *temp = node;
    while (temp->parent != NULL) 
    {
        total += (int)strlen(temp->name) + 1;
        temp = temp->parent;
    }
    if (total == 0) 
    {
        char *rootPath = (char *)safeMalloc(2);
        strcpy(rootPath, "/");
        return rootPath;
    }
    char *path = (char *)safeMalloc(total + 1);
    path[total] = '\0';
    int pos = total;
    temp = node;
    while (temp->parent != NULL) 
    {
        int len = (int)strlen(temp->name);
        pos = pos - len;
        memcpy(path + pos, temp->name, len);
        pos--;
        path[pos] = '/';
        temp = temp->parent;
    }
    return path;
}

void makeEntry(const char *name, int isDirectory) 
{
    if (!isValidName(name)) 
    {
        return;
    }
    if (findInCwd(name) != NULL) 
    {
        printf("Error: '%s' already exists.\n", name);
        return;
    }
    FileNode *node = createNode(name, isDirectory);
    insertNode(cwd, node);
    if (isDirectory) 
    {
        printf("Directory '%s' created successfully.\n", name);
    } else 
    {
        printf("File '%s' created successfully.\n", name);
    }
}

void releaseFileBlocks(FileNode *file) 
{
    int i;
    for (i = 0; i < file->blockCount; i++) 
    {
        addFreeBlockAtTail(file->blockPointers[i]);
    }
    free(file->blockPointers);
    file->blockPointers = NULL;
    file->blockCount = 0;
    file->size = 0;
}

void writeFile(const char *name, const char *content) {
    FileNode *file = findInCwd(name);
    if (file == NULL) 
    {
        printf("Error: file '%s' not found.\n", name);
        return;
    }
    if (file->isDirectory) 
    {
        printf("Error: '%s' is a directory.\n", name);
        return;
    }
    int size = (int)strlen(content);
    int needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (needed > freeCount + file->blockCount) 
    {
        printf("Error: disk full. Write failed.\n");
        return;
    }
    int *newPointers = NULL;
    if (needed > 0) 
    {
        newPointers = (int *)safeMalloc(needed * sizeof(int));
    }
    releaseFileBlocks(file);
    int i;
    for (i = 0; i < needed; i++) 
    {
        int index = removeFreeBlockFromHead();
        int len = size - i * BLOCK_SIZE;
        if (len > BLOCK_SIZE) 
        {
            len = BLOCK_SIZE;
        }
        memcpy(virtualDisk[index], content + i * BLOCK_SIZE, len);
        newPointers[i] = index;
    }
    file->blockPointers = newPointers;
    file->blockCount = needed;
    file->size = size;
    printf("Data written successfully (size=%d bytes).\n", size);
}

void readFile(const char *name) {
    FileNode *file = findInCwd(name);
    if (file == NULL) 
    {
        printf("Error: file '%s' not found.\n", name);
        return;
    }
    if (file->isDirectory) 
    {
        printf("Error: '%s' is a directory.\n", name);
        return;
    }
    if (file->size == 0) 
    {
        printf("(empty file)\n");
        return;
    }
    int remaining = file->size;
    int i;
    for (i = 0; i < file->blockCount; i++) 
    {
        int len = remaining;
        if (len > BLOCK_SIZE) 
        {
            len = BLOCK_SIZE;
        }
        fwrite(virtualDisk[file->blockPointers[i]], 1, len, stdout);
        remaining = remaining - len;
    }
    printf("\n");
}

void deleteFile(const char *name) 
{
    FileNode *file = findInCwd(name);
    if (file == NULL) 
    {
        printf("Error: file '%s' not found.\n", name);
        return;
    }
    if (file->isDirectory) 
    {
        printf("Error: '%s' is a directory. Use rmdir.\n", name);
        return;
    }
    releaseFileBlocks(file);
    removeNode(cwd, file);
    free(file);
    printf("File deleted successfully.\n");
}

void removeDirectory(const char *name) 
{
    FileNode *dir = findInCwd(name);
    if (dir == NULL) 
    {
        printf("Error: directory '%s' not found.\n", name);
        return;
    }
    if (!dir->isDirectory) 
    {
        printf("Error: '%s' is not a directory.\n", name);
        return;
    }
    if (dir->child != NULL) 
    {
        printf("Error: directory '%s' is not empty.\n", name);
        return;
    }
    removeNode(cwd, dir);
    free(dir);
    printf("Directory removed successfully.\n");
}

void listDirectory() 
{
    if (cwd->child == NULL) 
    {
        printf("(empty)\n");
        return;
    }
    FileNode *temp = cwd->child;
    do {
        if (temp->isDirectory) 
        {
            printf("%s/\n", temp->name);
        } else 
        {
            printf("%s\n", temp->name);
        }
        temp = temp->next;
    } while (temp != cwd->child);
}

void changeDirectory(const char *name) 
{
    if (strcmp(name, "..") == 0) 
    {
        if (cwd->parent != NULL) 
        {
            cwd = cwd->parent;
        }
    } else if (strcmp(name, "/") == 0) 
    {
        cwd = root;
    } else 
    {
        FileNode *dir = findInCwd(name);
        if (dir == NULL) 
        {
            printf("Error: directory '%s' not found.\n", name);
            return;
        }
        if (!dir->isDirectory) 
        {
            printf("Error: '%s' is not a directory.\n", name);
            return;
        }
        cwd = dir;
    }
    char *path = getPath(cwd);
    printf("Moved to %s\n", path);
    free(path);
}

void printPwd() 
{
    char *path = getPath(cwd);
    printf("%s\n", path);
    free(path);
}

void showDiskUsage() 
{
    int used = NUM_BLOCKS - freeCount;
    double percent = (used * 100.0) / NUM_BLOCKS;
    printf("Total Blocks: %d\n", NUM_BLOCKS);
    printf("Used Blocks: %d\n", used);
    printf("Free Blocks: %d\n", freeCount);
    printf("Disk Usage: %.2f%%\n", percent);
}

void freeTree(FileNode *node) 
{
    if (node->child != NULL) 
    {
        FileNode *last = node->child;
        while (last->next != node->child) 
        {
            last = last->next;
        }
        last->next = NULL;
        FileNode *current = node->child;
        while (current != NULL) 
        {
            FileNode *nextNode = current->next;
            freeTree(current);
            current = nextNode;
        }
    }
    free(node->blockPointers);
    free(node);
}

void freeFreeList() 
{
    while (freeHead != NULL) 
    {
        FreeBlock *temp = freeHead;
        freeHead = freeHead->next;
        free(temp);
    }
    freeTail = NULL;
    freeCount = 0;
}

char *nextToken(char **p) 
{
    char *s = *p;
    while (*s == ' ' || *s == '\t') 
    {
        s++;
    }
    if (*s == '\0') 
    {
        *p = s;
        return NULL;
    }
    char *start = s;
    while (*s != '\0' && *s != ' ' && *s != '\t') 
    {
        s++;
    }
    if (*s != '\0') 
    {
        *s = '\0';
        s++;
    }
    *p = s;
    return start;
}

int main() 
{
    char line[MAX_INPUT];
    initDisk();
    root = createNode("/", 1);
    cwd = root;

    printf("Compact VFS - ready. Type 'exit' to quit.\n");

    while (1) 
    {
        char *prompt = getPath(cwd);
        printf("%s > ", prompt);
        free(prompt);
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) 
        {
            printf("\n");
            break;
        }

        int len = (int)strlen(line);
        if (len > 0 && line[len - 1] != '\n' && !feof(stdin)) 
        {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) 
            {
            }
            printf("Error: input too long.\n");
            continue;
        }
        while (len > 0 && isspace((unsigned char)line[len - 1])) 
        {
            line[len - 1] = '\0';
            len--;
        }

        if (len == 0) 
        {
            continue;
        }

        char *p = line;
        char *cmd = nextToken(&p);
        if (cmd == NULL) 
        {
            continue;
        }

        if (strcmp(cmd, "mkdir") == 0 || strcmp(cmd, "create") == 0 ||
            strcmp(cmd, "read") == 0 || strcmp(cmd, "delete") == 0 ||
            strcmp(cmd, "rmdir") == 0 || strcmp(cmd, "cd") == 0) 
            {
            char *arg = nextToken(&p);
            char *extra = nextToken(&p);
            if (arg == NULL || extra != NULL) 
            {
                printf("Usage: %s <name>\n", cmd);
                continue;
            }
            if (strcmp(cmd, "mkdir") == 0) 
            {
                makeEntry(arg, 1);
            } 
            else if (strcmp(cmd, "create") == 0) 
            {
                makeEntry(arg, 0);
            } 
            else if (strcmp(cmd, "read") == 0) 
            {
                readFile(arg);
            } 
            else if (strcmp(cmd, "delete") == 0) 
            {
                deleteFile(arg);
            } 
            else if (strcmp(cmd, "rmdir") == 0) 
            {
                removeDirectory(arg);
            } 
            else 
            {
                changeDirectory(arg);
            }
        } 
        else if (strcmp(cmd, "write") == 0) 
        {
            char *name = nextToken(&p);
            if (name == NULL) 
            {
                printf("Usage: write <name> \"content\"\n");
                continue;
            }
            while (*p == ' ' || *p == '\t') 
            {
                p++;
            }
            if (*p == '\0') 
            {
                printf("Usage: write <name> \"content\"\n");
                continue;
            }
            char *content = p;
            int clen = (int)strlen(content);

            if (content[0] == '"') 
            {
                if (clen < 2 || content[clen - 1] != '"') 
                {
                    printf("Error: content must be enclosed in double quotes.\n");
                    continue;
                }
                content[clen - 1] = '\0';
                content++;
            }
            writeFile(name, content);
        } 
        else if (strcmp(cmd, "ls") == 0 || strcmp(cmd, "pwd") == 0 || strcmp(cmd, "df") == 0 || strcmp(cmd, "exit") == 0) 
            {
            if (nextToken(&p) != NULL) 
            {
                printf("Usage: %s\n", cmd);
                continue;
            }
            if (strcmp(cmd, "ls") == 0) 
            {
                listDirectory();
            } 
            else if (strcmp(cmd, "pwd") == 0) 
            {
                printPwd();
            } 
            else if (strcmp(cmd, "df") == 0) 
            {
                showDiskUsage();
            } 
            else 
            {
                break;
            }
        } 
        else 
        {
            printf("Unknown command: %s\n", cmd);
        }
    }

    freeTree(root);
    freeFreeList();
    printf("Memory released. Exiting program...\n");
    return 0;
}