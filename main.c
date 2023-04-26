#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// declare structure to store block information (id, starting address, ending address, link to next block)
struct block {
    int id;
    int start;
    int end;
    struct block *next;
};

// declare linked list to connect allocation block
struct block *head = NULL;
//struct block *curr = NULL;

// declare global variables
int mem_size;
int cur_mem = 0;
int alg_choice;


/********************************************************************/
void create() {
	// declare local variables (if any)

	// prompt for size of physical memory and choice of hole-fitting algorithm (0=first-fit, 1=best-fit), initialize remaining memory
    printf("Enter size of physical memory: ");
    scanf("%d", &mem_size);
    printf("Enter hole-fitting algorithm (0=first fit, 1=best_fit): ");
    scanf("%d", &alg_choice);

	// initialize linked list with "dummy" block of size 0
    struct block *dummy = (struct block*)malloc(sizeof(struct block));
    dummy->id = -1;
    dummy->start = 0;
    dummy->end = 0;
    dummy->next = NULL;
    head = dummy;

	return;
}


/********************************************************************/
void print() {
	// declare local variables
    struct block *ptr = head;

    // print table containing block id, starting address, ending address
    printf("\nID\tStart\tEnd\n"
           "-------------------\n");
    while(ptr != NULL) {
        if(ptr->id == -1) {
            ptr = ptr->next;
            continue;
        }
        printf("%d\t\t%d\t%d\n", ptr->id, ptr->start, ptr->end);
        ptr = ptr->next;
    }

	return;
}

/********************************************************************/
void allocate() {
	// declare local variables
    int id;
    int block_size;
    struct block* best_block = NULL;
    int best_start;
    int best_end;

	// initialize best hole so far to size of physical memory
    int best_hole = mem_size;

	// prompt for block id & block size
    printf("Enter block id: ");
    scanf("%d", &id);
    printf("Enter block size: ");
    scanf("%d", &block_size);


	// check if size of block is larger than remaining unallocated space, if so, print message and return
    if((block_size + cur_mem) > mem_size) {
        printf("the block size you selected is larger than the remaining unallocated space\n");
        return;
    }

	// allocate space for new block and set id
    struct block *new_block = (struct block*)malloc(sizeof(struct block));
    new_block->id = id;

	// if only "dummy" block exists, insert block at end of linked list, set fields, return
    if(head->next == NULL && head->id == -1) {
        new_block->start = 0;
        new_block->end = block_size;
        new_block->next = NULL;
        head->next = new_block;
        cur_mem += block_size;
        print();
        return;
    }
	// else traverse list until either appropriate hole is found or the end of the list is reached
    struct block *ptr = head;
    while(ptr->next != NULL) {
        // if id already exists, reject request and return
        if(ptr->id == id) {
            printf("the id you entered already exists\n");
            return;
        }
        // set values for start and end of currently found hole
        int hole_start = ptr->end;
        int hole_end = ptr->next->start;
        // if hole is large enough for block
        if(hole_end - hole_start > block_size) {
            // if first-fit algorithm
            if(alg_choice == 0) {
                // set start & end fields of new block & add block into linked list
                new_block->start = ptr->end;
                new_block->end = new_block->start + block_size;
                new_block->next = ptr->next;
                ptr->next = new_block;
                // reduce remaining available memory and return
                cur_mem += block_size;
                print();
                return;
            }
            else { // else--best-fit algorithm
                // if hole is smaller than best so far
                //printf("\nid: %d\nstart: %d\nend: %d\n", ptr->id, ptr->start, ptr->end);
                if(hole_end - hole_start < best_hole) {
                    // set values of best start & best end & best hole size so far
                    best_start = ptr->end;
                    best_end = ptr->next->start;
                    best_hole = best_end - best_start;
                    // update best block & advance next block
                    best_block = ptr;
                    //printf("\nbest hole so far: %d\n", best_hole);
                }
            }
        }
        ptr = ptr->next;
    }

    if(((mem_size - ptr->end) < best_hole) && (ptr->end + block_size <= mem_size)) {
        best_block = ptr;
    }

    if(best_block != NULL){
        // set start & end fields of new block & add block into linked list
        new_block->start = best_block->end;
        new_block->end = new_block->start + block_size;
        new_block->next = best_block->next;
        best_block->next = new_block;
        // reduce remaining available memory and return
        cur_mem += block_size;
        print();
        return;
    }else {
        // set start & end fields of new block & add block into linked list
        new_block->start = ptr->end;
        new_block->end = new_block->start + block_size;
        new_block->next = NULL;
        ptr->next = new_block;
        // reduce remaining available memory and return
        cur_mem += block_size;
        print();
    }

	return;	
}
	

/********************************************************************/
void deallocate() {
	// declare local variables
    int id;
    struct block* ptr = head;
    struct block* temp;

	// prompt for block id
    printf("Enter block id: ");
    scanf("%d", &id);

	// until end of linked list is reached or block id is found
    while(ptr->next != NULL) {
        // if end of linked list reached, print block id not found
        if(ptr->next->next == NULL) {
            printf("block id not found\n");
            return;
        }
        // if block id is found
        if(ptr->next->id == id) {
            // set start & end fields of current block to eliminate hole
            ptr->next->start = 0;
            ptr->next->end = 0;
            temp = ptr->next->next;
            // deallocate memory from block
            free(ptr->next);
            ptr->next = temp;
            // reduce remaining available memory
            cur_mem -= (ptr->next->end - ptr->next->start + 1);
            print();
            return;
        }

        ptr = ptr->next;
    }

    print();
	return;
}


/********************************************************************/
void defrag() {
	// declare local variables
    struct block* ptr = head;
    int hole_size;

	// until end of list is reached
    while(ptr->next != NULL) {
        // if current block is a hole
        if(ptr->end != ptr->next->start) {
            // calculate current hole size
            hole_size = ptr->next->start - ptr->end;
            // adjust start & end fields of current block to eliminate hole
            ptr->next->start -= hole_size;
            ptr->next->end -= hole_size;
        }
        ptr = ptr->next;
    }
    print();
	return;
}


/********************************************************************/
void quit(struct block *node) {
	// if node is NULL, return
    if(node == NULL) {
        return;
    }
    else{
        // recursively call procedure on node->next
        quit(node->next);
        // deallocate memory from node
        free(node);
    }
	return;
}


/***************************************************************/
int main() {
    /* declare local vars */
    bool cont = true;
    enum state {ENTER = 1, ALLOCATE = 2, DEALLOCATE = 3, DEFRAG = 4, QUIT = 5};
    int selection;

    /* while user has not chosen to quit */
    while(cont){
        /* print menu of options */
        printf("\nMemory allocation\n"
               "-----------------\n"
               "1) Enter parameters\n"
               "2) Allocate memory for block\n"
               "3) Deallocate memory for block\n"
               "4) Defragment memory\n"
               "5) Quit program\n\n");

        /* prompt for menu selection */
        printf("Enter selection: ");
        scanf("%d", &selection);

        /* call appropriate procedure based on choice--use switch */
        switch (selection){
            case ENTER:
                create();
                break;

            case ALLOCATE:
                allocate();
                break;

            case DEALLOCATE:
                deallocate();
                break;

            case DEFRAG:
                defrag();
                break;

            case QUIT:
                printf("Quitting program...\n");
                cont = false;
                quit(head);
                break;

            default:
                printf("input not recognized\n\n");
                break;
        }
    }
    return 1; /* indicates success */
} /* end of procedure */
// test string: 1 1024 1 2 0 128 2 1 320 2 2 224 2 3 288 3 2 2 4 128 3 1 2 2 224 2 5 64 4 4