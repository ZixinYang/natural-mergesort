#include <stddef.h>

typedef struct run_length_queue {
    int *storage;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t size;
    size_t mask;
} run_length_queue;

static size_t fix_capacity(size_t capacity);

static size_t max(size_t a, size_t b);

static const size_t MINIMUM_RUN_LENGTH_QUEUE_CAPACITY = 256; //minimum queue storage length
static const size_t BITS_PER_BYTE = 8;

static run_length_queue* run_length_queue_alloc(size_t capacity);

static void run_length_queue_enqueue(run_length_queue *queue, size_t run_size);

static void run_length_queue_add_to_last(run_length_queue *queue,
        size_t run_size);

static size_t run_length_queue_dequeue(run_length_queue *queue);

static size_t run_length_queue_size(run_length_queue *queue);

static void run_length_queue_free(run_length_queue *queue);

static void reverse_run(char *base, size_t num, size_t size, void *swap_buffer);

static run_length_queue*
build_run_length_queue(void *base,
                       size_t num,
                       size_t size,
                       int (*cmp)(const void*, const void*));

void merge(void *source,
           void *target,
           size_t size,
           size_t offset,
           size_t left_run_length,
           size_t right_run_length,
           int (*cmp)(const void*, const void*));

static size_t get_number_of_leading_zeros(size_t number);

static size_t get_number_of_merge_passes(size_t runs);

void stable_sort(void *base, size_t num, size_t size, int (*comparator)(const void*, const void*));
