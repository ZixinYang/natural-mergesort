#include <stddef.h>
#define OPT 1
typedef struct run_length_queue {
    int *storage;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t size;
    size_t mask;
} run_length_queue;

size_t fix_capacity(size_t capacity);

size_t max(size_t a, size_t b);

static const size_t MINIMUM_RUN_LENGTH_QUEUE_CAPACITY = 256; //minimum queue storage length
static const size_t BITS_PER_BYTE = 8;

run_length_queue* run_length_queue_alloc(size_t capacity);

void run_length_queue_enqueue(run_length_queue *queue, size_t run_size);

void run_length_queue_add_to_last(run_length_queue *queue,
        size_t run_size);

size_t run_length_queue_dequeue(run_length_queue *queue);

size_t run_length_queue_size(run_length_queue *queue);

void run_length_queue_free(run_length_queue *queue);

void reverse_run(char *base, size_t num, size_t size, void *swap_buffer);

run_length_queue*
build_run_length_queue(void *base,
                       size_t num,
                       size_t size,
                       int (*cmp)(const void*, const void*));

void merge(void *source,
           void *target,
           size_t size,
           size_t offset,
           size_t left_run_length,
           size_t right_run_length);

size_t get_number_of_leading_zeros(size_t number);

size_t get_number_of_merge_passes(size_t runs);

void stable_sort(void *base, size_t num, size_t size, int (*comparator)(const void*, const void*));
