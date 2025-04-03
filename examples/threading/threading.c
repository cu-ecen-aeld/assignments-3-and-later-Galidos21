#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
//#define DEBUG_LOG(msg,...)
#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

// Thread function to handle waiting and mutex operations
void* threadfunc(void* thread_param) {
    struct thread_data* thread_func_args = (struct thread_data *) thread_param;

    // Wait before attempting to obtain the mutex
    usleep(thread_func_args->wait_to_obtain_ms * 1000);

    // Lock the mutex
    if (pthread_mutex_lock(thread_func_args->mutex) != 0) {
        ERROR_LOG("Failed to lock mutex");
        return NULL;
    }
    DEBUG_LOG("Mutex locked");

    // Wait while holding the mutex
    usleep(thread_func_args->wait_to_release_ms * 1000);

    // Unlock the mutex
    if (pthread_mutex_unlock(thread_func_args->mutex) != 0) {
        ERROR_LOG("Failed to unlock mutex");
        return NULL;
    }
    DEBUG_LOG("Mutex unlocked");

    // Free memory and exit
    free(thread_func_args);
    return NULL;
}

bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex, int wait_to_obtain_ms, int wait_to_release_ms) {
    // Allocate memory for thread data
    struct thread_data* thread_args = (struct thread_data*) malloc(sizeof(struct thread_data));
    if (!thread_args) {
        ERROR_LOG("Memory allocation failed");
        return false;
    }

    // Initialize thread data
    thread_args->mutex = mutex;
    thread_args->wait_to_obtain_ms = wait_to_obtain_ms;
    thread_args->wait_to_release_ms = wait_to_release_ms;

    // Create the thread
    if (pthread_create(thread, NULL, threadfunc, thread_args) != 0) {
        ERROR_LOG("Thread creation failed");
        free(thread_args);
        thread_args->thread_complete_success = false;
        return false;
    }else{
        thread_args->thread_complete_success = true;
    }

    return true;
}
