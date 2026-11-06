#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FREE 0
#define ALLOCATED 1
#define FIRST_FIT 1
#define BEST_FIT 2
#define WORST_FIT 3

typedef struct
{
    int start;
    int size;
    int allocationID;
    int status;
} Block;

unsigned char *memoryPool = NULL;
Block *blocks = NULL;
int poolSize;
int blockCount = 0;
int capacity = 2;
int nextID = 1;
int currentStrategy;

int insertBlock(int index, Block newBlock)
{
    if(index < 0 || index > blockCount)
    {
        return 0;
    }
    if(blockCount >= capacity)
    {
        int newCapacity = capacity * 2;
        Block *temp = realloc(blocks, newCapacity * sizeof(Block));
        if(temp == NULL)
        {
            printf("Metadata expansion failed!\n");
            return 0;
        }
        blocks = temp;
        capacity = newCapacity;
    }

    for(int i = blockCount; i > index; i--)
    {
        blocks[i] = blocks[i - 1];
    }
    blocks[index] = newBlock;
    blockCount++;
    return 1;
}

void removeBlock(int index)
{
    for(int i = index; i < blockCount - 1; i++)
    {
        blocks[i] = blocks[i + 1];
    }
    blockCount--;
}

void mergeFreeBlocks()
{
    int i = 0;
    while(i < blockCount - 1)
    {
        if(blocks[i].status == FREE && blocks[i + 1].status == FREE)
        {
            blocks[i].size += blocks[i + 1].size;
            removeBlock(i + 1);
        }
        else
        {
            i++;
        }
    }
}

int findBlockIndex(int requestSize)
{
    int chosen = -1;
    if(currentStrategy == FIRST_FIT)
    {
        for(int i = 0; i < blockCount; i++)
        {
            if(blocks[i].status == FREE && blocks[i].size >= requestSize)
            {
                return i;
            }
        }
    }
    else if(currentStrategy == BEST_FIT)
    {
        int bestSize = 1000000000;
        for(int i = 0; i < blockCount; i++)
        {
            if(blocks[i].status == FREE && blocks[i].size >= requestSize && blocks[i].size < bestSize)
            {
                bestSize = blocks[i].size;
                chosen = i;
            }
        }
    }
    else if(currentStrategy == WORST_FIT)
    {
        int worstSize = 0;
        for(int i = 0; i < blockCount; i++)
        {
            if(blocks[i].status == FREE && blocks[i].size >= requestSize && blocks[i].size > worstSize)
            {
                worstSize = blocks[i].size;
                chosen = i;
            }
        }
    }
    return chosen;
}

void checkFragmentation(int requestSize)
{
    int totalFree = 0;
    int largestFree = 0;
    for(int i = 0; i < blockCount; i++)
    {
        if(blocks[i].status == FREE)
        {
            totalFree += blocks[i].size;
            if(blocks[i].size > largestFree)
                largestFree = blocks[i].size;
        }
    }
    if(totalFree >= requestSize && largestFree < requestSize)
    {
        printf("\nExternal Fragmentation Detected\n");
        printf("Free blocks: ");
        for(int i = 0; i < blockCount; i++)
        {
            if(blocks[i].status == FREE)
            {
                printf("%d ", blocks[i].size);
            }
        }
        printf("\nTotal free memory = %d bytes\n", totalFree);
    }
}

void allocateMemory()
{
    int req;
    printf("Requested size: ");
    scanf("%d", &req);
    if(req <= 0 || req > poolSize)
    {
        printf("Invalid request size!\n");
        return;
    }
    int idx = findBlockIndex(req);
    if(idx == -1)
    {
        printf("Allocation failed.\n");
        checkFragmentation(req);
        return;
    }
    Block *b = &blocks[idx];

    if(b->size > req)
    {
        Block freePart;
        freePart.start = b->start + req;
        freePart.size = b->size - req;
        freePart.status = FREE;
        freePart.allocationID = -1;
        if(!insertBlock(idx + 1, freePart))
        {
        printf("Metadata allocation failed.\n");
        return;
        }
    }
    blocks[idx].size = req;
    blocks[idx].status = ALLOCATED;
    blocks[idx].allocationID = nextID++;

    printf("Allocation successful.\n");
    printf("ID = %d Start = %d Size = %d\n",
           blocks[idx].allocationID,
           blocks[idx].start,
           blocks[idx].size);
}

void releaseMemoryBlock()
{
    int id;
    printf("Allocation ID: ");
    scanf("%d", &id);
    for(int i = 0; i < blockCount; i++)
    {
        if(blocks[i].allocationID == id && blocks[i].status == ALLOCATED)
        {
            blocks[i].status = FREE;
            blocks[i].allocationID = -1;
            mergeFreeBlocks();
            printf("Allocation released.\n");
            return;
        }
    }
    printf("Invalid Allocation ID!\n");
}

void resizeAllocation()
{
    int id;
    int newSize;
    printf("Allocation ID: ");
    scanf("%d", &id);
    printf("New Size: ");
    scanf("%d", &newSize);
    if(newSize <= 0 || newSize > poolSize)
    {
        printf("Invalid Size!\n");
        return;
    }
    int idx = -1;
    for(int i = 0; i < blockCount; i++)
    {
        if(blocks[i].allocationID == id && blocks[i].status == ALLOCATED)
        {
            idx = i;
            break;
        }
    }

    if(idx == -1)
    {
        printf("Allocation not found!\n");
        return;
    }
    int oldStart = blocks[idx].start;
    int oldSize = blocks[idx].size;

    if(newSize == oldSize)
    {
        printf("Size unchanged.\n");
        return;
    }

    // CASE 1: Shrink the allocation.
    if(newSize < oldSize)
    {
        Block freeBlock;
        freeBlock.start = oldStart + newSize;
        freeBlock.size = oldSize - newSize;
        freeBlock.status = FREE;
        freeBlock.allocationID = -1;
        if(!insertBlock(idx + 1, freeBlock))
        {
            printf("Resize failed: metadata allocation failed. "
                   "Original allocation is unchanged.\n");
            return;
        }
        blocks[idx].size = newSize;
        mergeFreeBlocks();
        printf("Allocation shrunk.\n");
        return;
    }

    // CASE 2: Expand into the immediately adjacent free block.
    int needed = newSize - oldSize;
    if(idx + 1 < blockCount && blocks[idx + 1].status == FREE && blocks[idx + 1].size >= needed)
    {
        blocks[idx].size = newSize;
        blocks[idx + 1].start += needed;
        blocks[idx + 1].size -= needed;
        if(blocks[idx + 1].size == 0)
            removeBlock(idx + 1);
        printf("Expanded in place.\n");
        return;
    }

    // CASE 3: Move the allocation to another suitable free block.
    int target = findBlockIndex(newSize);
    if(target == -1)
    {
        printf("Resize failed: no suitable free block.\n");
        printf("Original allocation and its data are unchanged.\n");
        return;
    }
     int newStart = blocks[target].start;
    int destinationSize = blocks[target].size;
    if(destinationSize > newSize)
    {
        Block remain;
        remain.start = newStart + newSize;
        remain.size = destinationSize - newSize;
        remain.status = FREE;
        remain.allocationID = -1;

        if(!insertBlock(target + 1, remain))
        {
            printf("Resize failed: metadata allocation failed. "
                   "Original allocation is unchanged.\n");
            return;
        }
    }

    memmove(memoryPool + newStart, memoryPool + oldStart, oldSize);
    blocks[target].start = newStart;
    blocks[target].size = newSize;
    blocks[target].status = ALLOCATED;
    blocks[target].allocationID = id;
    int oldIndex = -1;

    for(int i = 0; i < blockCount; i++)
    {
        if(blocks[i].start == oldStart && blocks[i].allocationID == id && blocks[i].status == ALLOCATED)
        {
            oldIndex = i;
            break;
        }
    }
    if(oldIndex != -1)
    {
        blocks[oldIndex].status = FREE;
        blocks[oldIndex].allocationID = -1;
    }
    mergeFreeBlocks();
    printf("Allocation moved and resized.\n");
    printf("Old start: %d, New start: %d, New size: %d\n", oldStart, newStart, newSize);
}

void displayMap()
{
    printf("\nMemory Map (%d bytes)\n\n", poolSize);
    for(int i = 0; i < blockCount; i++)
    {
        if(blocks[i].status == FREE)
        {
            printf("[Free : %d bytes, Start : %d]\n", blocks[i].size, blocks[i].start);
        }
        else
        {
            printf("[Allocated #%d : %d bytes, Start : %d]\n",
                   blocks[i].allocationID,
                   blocks[i].size,
                   blocks[i].start);
        }
    }
}

void displayStatistics()
{
    int allocated = 0;
    int freeMem = 0;
    int largestFree = 0;
    int allocatedBlocks = 0;
    int freeBlocks = 0;

    for(int i = 0; i < blockCount; i++)
    {
        if(blocks[i].status == FREE)
        {
            freeMem += blocks[i].size;
            freeBlocks++;
            if(blocks[i].size > largestFree)
            {
                largestFree = blocks[i].size;
            }
        }
        else
        {
            allocated += blocks[i].size;
            allocatedBlocks++;
        }
    }
    printf("\nMemory Statistics\n");
    printf("Total Memory      : %d bytes\n", poolSize);
    printf("Allocated Memory  : %d bytes\n", allocated);
    printf("Free Memory       : %d bytes\n", freeMem);
    printf("Allocated Blocks  : %d\n", allocatedBlocks);
    printf("Free Blocks       : %d\n", freeBlocks);
    printf("Largest Free Block: %d bytes\n", largestFree);
}

void changeStrategy()
{
    printf("\n1. First Fit\n");
    printf("2. Best Fit\n");
    printf("3. Worst Fit\n");
    printf("Choice: ");
    scanf("%d",&currentStrategy);

    if(currentStrategy < 1 || currentStrategy > 3)
    {
        currentStrategy = FIRST_FIT;
        printf("Invalid choice. First Fit selected.\n");
    }
}

int main()
{
    printf("Enter memory pool size: ");
    scanf("%d",&poolSize);
    if(poolSize < 1 || poolSize > 1000000)
    {
        printf("Invalid pool size!\n");
        return 1;
    }

    printf("\nSelect Allocation Strategy\n");
    printf("1. First Fit\n");
    printf("2. Best Fit\n");
    printf("3. Worst Fit\n");
    if (scanf("%d", &currentStrategy) != 1 || currentStrategy < FIRST_FIT || currentStrategy > WORST_FIT)
    {
        printf("Invalid allocation strategy!\n");
        return 1;
    }
    memoryPool = (unsigned char*)malloc(poolSize);
    if(memoryPool == NULL)
    {
        printf("Memory allocation failed!\n");
        return 1;
    }
    blocks = (Block*)malloc(capacity * sizeof(Block));
    if(blocks == NULL)
    {
        free(memoryPool);
        return 1;
    }
    blockCount = 1;
    blocks[0].start = 0;
    blocks[0].size = poolSize;
    blocks[0].allocationID = -1;
    blocks[0].status = FREE;

    int choice;
    do
    {
        printf("\n===== SMART MEMORY MANAGER =====\n");
        printf("1. Allocate Memory\n");
        printf("2. Release Memory\n");
        printf("3. Resize Allocation\n");
        printf("4. Display Memory Map\n");
        printf("5. Display Statistics\n");
        printf("6. Change Strategy\n");
        printf("7. Exit\n");
        printf("Choice: ");

        scanf("%d",&choice);
        switch(choice)
        {
            case 1:
                allocateMemory();
                break;

            case 2:
                releaseMemoryBlock();
                break;

            case 3:
                resizeAllocation();
                break;

            case 4:
                displayMap();
                break;

            case 5:
                displayStatistics();
                break;

            case 6:
                changeStrategy();
                break;

            case 7:
                printf("Exiting...\n");
                break;

            default:
                printf("Invalid choice!\n");
        }

    } while(choice != 7);

    free(memoryPool);
    free(blocks);
    return 0;
}
