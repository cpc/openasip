
#define PTHREAD_DEFAULT_INITIALIZER {0, NULL,0}
#define PTHREAD_MUTEX_DESTROYER {-1, NULL,1}

typedef void* pthread_mutexattr_t;

typedef struct {
    int lock;      
    int count;      
    void* owner; 
} pthread_mutex_t;

typedef void* pthread_attr_t;

/**
 * Thread info structure, which is also an element of linked list.
 */
struct SThreadInfo {
    /* linked list pointers */ 
    struct SThreadInfo* prev;
    struct SThreadInfo* next;
    struct SThreadInfo* dead_next;
    
    /* current stackpointer value */
    void** sp;

    /* this pointer points to end of structure where stack is allocated 
       MUST BE LAST FIELD OF STRUCTURE */
    void** stack;
};

typedef struct SThreadInfo ThreadInfo;
typedef ThreadInfo* pthread_t;

/* defined just for header to understand types */
typedef int pthread_condattr_t;
typedef int pthread_cond_t;
typedef int pthread_once_t;
typedef int pthread_key_t;
