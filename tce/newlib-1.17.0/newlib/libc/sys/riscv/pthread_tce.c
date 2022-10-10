/**
 * Very simple implementation for pthread api.
 */

#ifdef __RISCV__
/* These includes will make generating x86 test code harder */
#include <pthread.h>
#include <stdlib.h>
#endif

#include <malloc.h>

#define STACK_SIZE 4096

#define WAIT_LOCK(x) while(testandset(&x))
#define TRY_LOCK(x) if(!testandset(&x))
#define RELEASE_LOCK(x) releaselock(&x)

//#define WAIT_LOCK(x) 
//#define TRY_LOCK(x) 
//#define RELEASE_LOCK(x)

/* right now all needed functions are in isAtomic list */

/* Definitions below were moved to internals.h */
/* #if 0 */

/* #define PTHREAD_DEFAULT_INITIALIZER {0, NULL,0} */
/* #define PTHREAD_MUTEX_DESTROYER {-1, NULL,1} */

/* typedef void* pthread_mutexattr_t; */

/* typedef struct { */
/*     int lock;       */
/*     int count;       */
/*     void* owner;  */
/* } pthread_mutex_t; */
/* #endif */

/* typedef void* pthread_attr_t; */

/* /\** */
/*  * Thread info structure, which is also an element of linked list. */
/*  *\/ */
/* struct SThreadInfo { */
/*     /\* linked list pointers *\/  */
/*     struct SThreadInfo* prev; */
/*     struct SThreadInfo* next; */
/*     struct SThreadInfo* dead_next; */
    
/*     /\* current stackpointer value *\/ */
/*     void* sp; */

/*     /\* this pointer points to end of structure where stack is allocated  */
/*        MUST BE LAST FIELD OF STRUCTURE *\/ */
/*     void** stack; */
/* }; */

/* typedef struct SThreadInfo ThreadInfo; */
/* typedef ThreadInfo* pthread_t; */

static int thread_lock = 0;

/**
 * Atomic operation function which is guaranteed to run without
 * thread switch.
 */
int testandset(int* lockVar) __attribute__((noinline));
int testandset(int* lockVar) {
    int retVal = (*lockVar != 0);
    *lockVar = 1;
    return retVal;
}

/**
 * Release lock for debuggin
 */
void releaselock(int* lockVar) __attribute__((noinline));
void releaselock(int* lockVar) {
    *lockVar = 0;
}

/** 
 * Main thread is first staticlly allocated thread element, for schedule_thread and 
 * others functions to work right without any initialization check. sp of main program
 * is stored to this element.
 */
static ThreadInfo mainThread = {NULL,NULL,NULL};

/**
 * Checks if requested info is found from thread list.
 */
int isThreadInfoActive(ThreadInfo* infoToFind) __attribute__((noinline));
int isThreadInfoActive(ThreadInfo* infoToFind) {
//    WAIT_LOCK(thread_lock);
    ThreadInfo* currInfo = mainThread.next;

    while (currInfo != NULL) {
        if (currInfo == infoToFind) {
//            RELEASE_LOCK(thread_lock);
            return 1;
        }
        currInfo = currInfo->next;
    }
//    RELEASE_LOCK(thread_lock);
    return 0;
}

/**
 * Gloabal data for handling threads. 
 * 
 * Right now contains only first thread, last thread and current thread pointers.
 */
static struct ThreadingData {
    /* First element of linked list, this is used for storing main thread and 
       is never freed. */
    ThreadInfo* firstThread;
    ThreadInfo* lastThread;
    ThreadInfo* currentThread;
    ThreadInfo* deadThreads;
} threadInternals = {&mainThread, &mainThread, &mainThread, NULL};

/**
 * Remove thread from thread list. Will never be executed again..
 */
void removeThreadInfo(ThreadInfo* threadToKill) {
    WAIT_LOCK(thread_lock);
    threadToKill->prev->next = threadToKill->next;

    if (threadToKill->next != NULL) {
        // middle thread
        threadToKill->next->prev = threadToKill->prev;
    } else {
        // must be last thread
        threadInternals.lastThread = threadToKill->prev;
    }
        
    /* Add thread to be first of list for freeing dead threads.
       Thread that is currently running cannot free data because 
       thread_scheduler still needs to use next and prev pointers 
       of this element. */   
    threadToKill->dead_next = threadInternals.deadThreads;

    /* to make sure that we do not return to dead thread 
       threadToKill->next = threadInternals.firstThread;
    */

    threadInternals.deadThreads = threadToKill;

    RELEASE_LOCK(thread_lock);
}

/**
 * Removes active thread from thread list.
 */
void removeActiveThread(void) {
    removeThreadInfo(threadInternals.currentThread);
}

/**
 * Initializes new ThreadInfo structure.
 *
 * @param lastThread ThreadInfo object where to thread will be connected. 
 *                   If NULL, leave unconnected.
 * @param threadedFunction Pointer to start of function which is runned as thread.
 */
ThreadInfo* addThreadInfo(ThreadInfo* lastThread, 
                          void *(*__start_routine)( void * ), void *__arg) {

    WAIT_LOCK(thread_lock);
    
    /* Allocate space for structure and stack at the same time */ 
    ThreadInfo* newThread = malloc(sizeof(ThreadInfo) + STACK_SIZE*sizeof(void*));
    
    if (newThread != NULL) {
        /* set stack pointer to point first word after end of the struct where is 
           extra allocated is found */ 
        newThread->stack = (void**)(&newThread->stack + 1);
        newThread->dead_next = NULL;
            
        /* Set pointers of linked list */
        if (lastThread != NULL) {
            lastThread->next = newThread;
            newThread->prev = lastThread;
            newThread->next = NULL;
        } else {
            newThread->prev = NULL;
            newThread->next = NULL;        
        }

        /* Set threaded function to be first return address in stack */        
        newThread->sp = &newThread->stack[STACK_SIZE-5];

        newThread->stack[STACK_SIZE-5] = newThread->sp;

        newThread->stack[STACK_SIZE-4] = __start_routine;
        
        /* And if we are returned from first function, let's return to function that
           removes dead thread from thread table */ 
        newThread->stack[STACK_SIZE-3] = __arg;

        /* And if we are returned from first function, let's return to function that
           removes dead thread from thread table */ 
        newThread->stack[STACK_SIZE-2] = &removeActiveThread;
        
        /* Set parameter to be last created thread for killing function. */
        newThread->stack[STACK_SIZE-1] = newThread;

/*
        printf("New thread: %x initial sp and thread->stack: %x initial sp: %x\n"
               "[sp+0] %x\n"
               "[sp+1] %x\n"
               "[sp+2] %x\n"
               "[sp+3] %x\n"
               "[sp+4] %x\n",
               (int)newThread,
               (int)newThread->stack,
               (int)newThread->sp,
               (int)newThread->sp[0],
               (int)newThread->sp[1],
               (int)newThread->sp[2],
               (int)newThread->sp[3],
               (int)newThread->sp[4]);
*/

    }

    RELEASE_LOCK(thread_lock);
    return newThread;
}

/**
 * Frees data of all threads that can be removed.
 * 
 * NOTE: must called inside block locked with thread_lock
 */
inline void freeDeadThreads() {
    while (threadInternals.deadThreads != NULL && 
           threadInternals.deadThreads != threadInternals.currentThread) {
        ThreadInfo* current = threadInternals.deadThreads;
        threadInternals.deadThreads = current->dead_next;
        free(current);
    }
}

/* if noinline attribute is set seems to be that llvm cannot remove the function */ 
void* __tce_schedule_next_thread(void* sp) __attribute__((noinline));

/**
 * Select next thread. This should also be uninlined atomic function.
 */
void* __tce_schedule_next_thread(void* sp) {
    TRY_LOCK(thread_lock) {
        /* update sp of current thread to table */
        threadInternals.currentThread->sp = sp;
    
        /* select next thread */
        threadInternals.currentThread = threadInternals.currentThread->next;
        
        if (threadInternals.currentThread == NULL) {
            threadInternals.currentThread = threadInternals.firstThread;

            /* When going to main thread all dead threads should be 
               able to be freed */
            freeDeadThreads();
        }
        void* retVal = threadInternals.currentThread->sp;        
        RELEASE_LOCK(thread_lock);        
        return retVal;        
    }

    return sp;
}

#if 0
int pthread_mutex_init(pthread_mutex_t *mutex,
                       const pthread_mutexattr_t *attr) {

    mutex->lock = 0;
    mutex->count = 0;
    mutex->owner = NULL;
    return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex) {
    mutex->lock = 0;
    mutex->count = ~0;
    mutex->owner = NULL;
    return 0;
}

int canStealMutex(pthread_mutex_t* mutex) {
    return (mutex->count != 0 || 
            mutex->owner != threadInternals.currentThread);
}

int pthread_mutex_lock(pthread_mutex_t *mutex) {
    WAIT_LOCK(mutex->lock);
    while (!canStealMutex(mutex));    
    mutex->owner = threadInternals.currentThread;
    mutex->count++;
    RELEASE_LOCK(mutex->lock);
    return 0;
}

int pthread_mutex_trylock(pthread_mutex_t *mutex) {
    WAIT_LOCK(mutex->lock);
    if (canStealMutex(mutex)) {
        mutex->owner = threadInternals.currentThread;
        mutex->count++;
        RELEASE_LOCK(mutex->lock);
        return 0;
    }
        
    RELEASE_LOCK(mutex->lock);
    return 1;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex) {    
    while(!canStealMutex(mutex));
    mutex->owner = threadInternals.currentThread;
    mutex->count--;
    return 0;
}
#endif

/**
 * pthread_create parameters as they can be expected with following exceptions.
 *
 * Thread attributes are not supported (just ignored)
 * Thread arguments are ignored as well
 */
int	pthread_create(pthread_t *__pthread, 
                   _CONST pthread_attr_t  *__attr,
                   void *(*__start_routine)( void * ), 
                   void *__arg) {
    
    // create new thread table element with given function
    *__pthread = addThreadInfo(
        threadInternals.lastThread, __start_routine, __arg);
    
    threadInternals.lastThread = *__pthread;
    
    if (*__pthread == NULL) {
        return 1;
    }

    return 0;
}

/**
 * Waits until thread is not executed anymore.
 */
int	pthread_join(pthread_t __pthread, void **__value_ptr) {
    // would be better to put this thread to wait list
    // and remove it from wait list when joined __pthread is removed from waitlist
    while (isThreadInfoActive(__pthread));
}

/**
 * If called from main thread runs global exit. Otherwise removes caller
 * thread from thread list.
 */ 
void pthread_exit(void *__value_ptr) {
    // remove thread from table
    if (threadInternals.currentThread == &mainThread) {
        /* Maybe should wait that all threads are ready before exit */ 
        exit(0);
    } else {
        removeThreadInfo(threadInternals.currentThread);        
    }
}
