
//BT20CSE105 Rugved Radkar
// Heap Management

#include <iostream>

using namespace std;

int SIZE = 1000;

void check_tag(string);

// Data structure for the free list.
struct freeList
{
    struct freeList* next;
    struct freeList* prev;
    int size;
    int start;
};

// Data structure for allocated list.
struct alloc_list
{
    struct alloc_list* next;
    struct alloc_list* prev;
    int size;
    int start;
    string tag;
};

freeList* Free;
alloc_list* alloc;

//Function to allocate a block of memory.
void allocate()
{
    int n;
    string tag;
    cout << "Enter the size of block you want to allocate: ";
    cin >> n;
    cout << "Enter a name for allocated block: ";
    cin >> tag;
    check_tag(tag); // If tag already exists, it will be replaced.
    freeList* ptr = Free;
    while (ptr && (ptr->size < n))
        ptr = ptr->next;

    if (ptr == NULL)
        cout << "\nMemory cannot be allocated. (consider deallocating existing allocated memory)\n\n";

    else
    {
        if (ptr->size - n > 0)
        {
            freeList* temp = new freeList;
            temp->start = ptr->start + n;
            temp->next = temp->prev = NULL;
            if (ptr == Free)
            {
                temp->next = Free->next;
                Free = temp;
            }
            else
            {
                (ptr->prev)->next = temp;
                temp->next = ptr->next;
                temp->prev = ptr->prev;
                if(ptr->next)
                    ptr->next->prev = temp;
            }
            temp->size = ptr->size - n;
        }

        else
        {
            if (ptr->prev)
            {
                ptr->prev->next = ptr->next;
                if(ptr->next)
                    ptr->next->prev = ptr->prev;
            }
            else
            {
                Free = ptr->next;
                if(Free)
                Free->prev = NULL;
            }

        }

        alloc_list* alloc_t = new alloc_list;
        alloc_t->next = alloc_t->prev = NULL;
        alloc_t->size = n;
        alloc_t->tag = tag;
        alloc_t->start = ptr->start;
        free(ptr);

        if (alloc == NULL)
            alloc = alloc_t;
        else
        {
            alloc_list* Temp = alloc;
            while (Temp->next && Temp->start < alloc_t->start)
            {
                Temp = Temp->next;
            }
            //When node is to be allocated at the first position.
            if (Temp == alloc && Temp->start > alloc_t->start)
            {
                alloc_t->next = Temp;
                Temp->prev = alloc_t;
                alloc = alloc_t;
            }
            //When node is to be allocated at the last position.
            else if (Temp->next == NULL && Temp->start < alloc_t->start)
            {
                Temp->next = alloc_t;
                alloc_t->prev = Temp;
                alloc_t->next = NULL;
            }
            //When Node is to be inserted in between.
            else
            {
                (Temp->prev)->next = alloc_t;
                alloc_t->prev = Temp->prev;
                alloc_t->next = Temp;
                Temp->prev = alloc_t;
            }
        }
        cout << "\nAllocated from index: " << alloc_t->start << " to " << alloc_t->start + alloc_t->size <<  endl;
        cout << "\n\n";
    }
}

//Function to merge adjacent free blocks.
void merge(freeList* free_ptr)
{
    if (free_ptr->prev)
    {
        if (free_ptr->prev->start + free_ptr->prev->size == free_ptr->start)
        {
            free_ptr->prev->size = free_ptr->prev->size + free_ptr->size;
            free_ptr->prev->next = free_ptr->next;
            if(free_ptr->next)
                free_ptr->next->prev = free_ptr->prev;
            freeList* temp = free_ptr;
            free_ptr = free_ptr->prev;
            free(temp);
        }
    }

    if (free_ptr->next)
    {

        if (free_ptr->start + free_ptr->size == free_ptr->next->start)
        {
            free_ptr->size += free_ptr->next->size;
            free_ptr->next = free_ptr->next->next;
            if(free_ptr->next)
                free_ptr->next->prev = free_ptr;
        }
    }
}

//Function to deallocate memory.
void deallocate(string tag)
{
    alloc_list* ptr = alloc;
    //Finding node with given tag
    while (ptr && ptr->tag != tag)
    {
        ptr = ptr->next;
    }
    //If tag not found:
    if (ptr == NULL)
    {
        cout << "Not found\n";
    }
    else
    {
        //Updating free list if tag is found.
        freeList* free_ptr = new freeList;
        free_ptr->next = free_ptr->prev = NULL;
        free_ptr->size = ptr->size;
        free_ptr->start = ptr->start;
        //Taking care of boundary cases:
        if (ptr == alloc)
            alloc = ptr->next;
        else
        {
            ptr->prev->next = ptr->next;
            if (ptr->next)
                ptr->next->prev = ptr->prev;
        }

        freeList* temp = Free;
        if (Free == NULL)
        {
            Free = free_ptr;
        }
        else
        {
            while (temp && temp->next && temp->start < free_ptr->start)
            {
                temp = temp->next;
            }
            //When free list node is to be inserted at the end.
            if (temp && temp->next == NULL && temp->start < free_ptr->start)
            {
                temp->next = free_ptr;
                free_ptr->prev = temp;
            }
            else
            {
                //When free list node is to be inserted at the start.
                if (temp && temp == Free)
                {
                    free_ptr->next = temp;
                    temp->prev = free_ptr;
                    Free = free_ptr;
                }
                //When free list node is to be inserted in between.
                else if (temp)
                {
                    free_ptr->prev = temp->prev;
                    temp->prev->next = free_ptr;
                    temp->prev = free_ptr;
                    free_ptr->next = temp;
                }

            }
        }

        merge(free_ptr); // merge function called to link adjacent contiguous blocks.
    }
    cout << "\n\n";
}

void check_tag(string Tag)
{
    int flag = 1;
    alloc_list* ptr = alloc;
    while (ptr && flag)
    {
        if (ptr->tag == Tag)
        {
            cout << "\nMemory already exists with the given tag, overwriting the existing memory.\n";
            deallocate(Tag);
            flag = 0;
        }
        ptr = ptr->next;
    }
}

//Function to print the allocated list.
void print_alloc()
{
    alloc_list* ptr = alloc;
    while(ptr)
    {
        cout << ptr->start << " to " << ptr->start + ptr->size << endl;
        ptr = ptr->next;
    }
}

//Function to print the free list.
void print_free()
{
    freeList* ptr = Free;
    while (ptr)
    {
        cout << ptr->start << " to " << ptr->start + ptr->size << endl;
        ptr = ptr->next;
    }
}

int main()
{
    int choice;

    Free = new freeList;
    Free->next = Free->prev = NULL;
    Free->size = SIZE;
    Free->start = 0;

    alloc = NULL;
    cout << "Heap implementation with a memory of " << SIZE <<" units available.\n\n";
    cout << "Enter the size in integer format only.\nAny blocks initialized with an existing tag will be overwritten.\n\n";
    char ch = 'Y';
    while (ch == 'Y' || ch == 'y')
    {
        cout << "\t\t*******MENU*******\n1. Allocate Memory.\n2. Deallocate Memory.\n3. Display current status of the heap.";
        cout << "\n\nSelect one of the above: ";
        cin >> choice;
        cout << endl;
        switch (choice)
        {
            case 1:
            {
                allocate();
                cout << "Current condition:\n";
                cout << "Allocated list\n";
                print_alloc();
                cout << endl;
                cout << "Free list\n";
                print_free();
                cout << endl;
                break;
            }
            case 2:
            {
                string tag;
                cout << "Enter the tag of the block you wish to deallocate: ";
                cin >> tag;
                deallocate(tag);
                cout << "Current condition:\n";
                cout << "Allocated list\n";
                print_alloc();
                cout << endl;
                cout << "Free list\n";
                print_free();
                cout << endl;
                break;
            }

            case 3:
            {
                cout << "Allocated list\n";
                print_alloc();
                cout << endl;
                cout << "Free list\n";
                print_free();
                cout << endl;
                break;
            }
            default:cout << "Please enter a number only between 1-3.\n";
        }

        cout << "\nIf you want to continue, enter 'Y' (anything else to exit the program): ";
        cin >> ch;
        cout << "\n\n";
    }
    cout << "\nThe program has been terminated.\n";
    return 0;
}



