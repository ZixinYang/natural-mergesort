typedef struct run_length_queue {
    int *storage;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t size;
    size_t mask;
} run_length_queue;

//get 2^n
static size_t fix_capacity(size_t capacity)
{
    size_t ret = 1;

    while (ret < capacity) {
        ret <<= 1;
    }

    return ret;
}

static size_t max(size_t a, size_t b)
{
    return a > b ? a : b;
}

static const size_t MINIMUM_RUN_LENGTH_QUEUE_CAPACITY = 256; //minimum queue storage length
static const size_t BITS_PER_BYTE = 8;

static run_length_queue* run_length_queue_alloc(size_t capacity)
{
    run_length_queue *queue;

    //choose larger capacity
    capacity = max(capacity, MINIMUM_RUN_LENGTH_QUEUE_CAPACITY);
    capacity = fix_capacity(capacity); //become 2^n

    queue = malloc(sizeof(*queue)); //allococate memory for self

    if (!queue) {  //if failed
        return NULL;
    }

    //allocate memory for queue->stirage (length:capacity type:int)
    queue->storage = malloc(sizeof(int) * capacity);

    if (!queue->storage) { //failed to malloc queue storage
        free(queue);
        return NULL;
    }
    //initialize queue
    queue->capacity = capacity;
    queue->mask = capacity - 1; //for circular queue
    queue->head = 0; //starting point
    queue->tail = 0; //end point
    queue->size = 0; //total in queue

    return queue;
}

static void run_length_queue_enqueue(run_length_queue *queue, size_t run_size)
{
    queue->storage[queue->tail] = run_size;
    queue->tail = (queue->tail + 1) & queue->mask;
    queue->size++;
}

static void run_length_queue_add_to_last(run_length_queue *queue, size_t run_size)
{
    queue->storage[(queue->tail - 1) & queue->mask] += run_size;
}

static size_t run_length_queue_dequeue(run_length_queue *queue)
{
    size_t run_length = queue->storage[queue->head];
    queue->head = (queue->head + 1) & queue->mask;
    queue->size--;
    return run_length;
}

static size_t run_length_queue_size(run_length_queue *queue)
{
    return queue->size;
}

static void run_length_queue_free(run_length_queue *queue)
{
    if (queue && queue->storage) {
        free(queue->storage);
    }
}

static void reverse_run(char *base, size_t num, size_t size, void *swap_buffer)
{
    size_t left = 0;
    size_t right = num - 1;

    while (left < right) {
        memcpy(swap_buffer, base + size * left, size);
        memcpy(base + size * left, base + size * right, size);
        memcpy(base + size * right, swap_buffer, size);

        ++left;
        --right;
    }
}

static run_length_queue*
build_run_length_queue(void *base,
                       size_t num,
                       size_t size,
                       int (*cmp)(const void*, const void*))
{
    run_length_queue *queue;
    size_t head;
    size_t left;
    size_t right;
    size_t last;
    size_t run_length;
    bool previous_was_descending;
    void *swap_buffer = malloc(size);
    queue = run_length_queue_alloc((num >> 1) + 1); //allocate (num/2+1) size (will resize to 2^n or 256)  queue

    if (!queue) { //if failed
        return NULL;
    }
    //initialize
    left = 0;
    right = 1;
    last = num - 1;
    previous_was_descending = false;

    while (left < last) {
        head = left;

        /* Decide the direction of the next run. */
        if (cmp(((char*) base) + size * left++,
                ((char*) base) + size * right++) <= 0) {
            /* The run is ascending. */
            while (left < last
                    && cmp(((char*) base) + size * left,
                           ((char*) base) + size * right) <= 0) 1{
                ++left;
                ++right;
            }
            //calculate length
            run_length = left - head + 1;

            //if previous is descending (acturally it became ascending) , check if this can comibne with it
            //data will become desc asc desc asc ... it may be combined only if previous is descending
            if (previous_was_descending) {
                if (cmp(((char*) base) + (head - 1) * size,
                        ((char*) base) + head * size) <= 0) {
                    run_length_queue_add_to_last(queue, run_length);
                } else {
                    run_length_queue_enqueue(queue, run_length);
                }
            } else {
                run_length_queue_enqueue(queue, run_length);
            }

            previous_was_descending = false;
        } else {
            /* Scan a strictly descending run. */
            while (left < last
                    && cmp(((char*) base) + size * left,
                           ((char*) base) + size * right) > 0) {
                ++left;
                ++right;
            }

            run_length = left - head + 1;
            reverse_run(((char*) base) + head * size,
                        run_length,
                        size,
                        swap_buffer);

            if (previous_was_descending) {
                if (cmp(((char*) base) + size * (head - 1),
                        ((char*) base) + size * head) <= 0) {
                    run_length_queue_add_to_last(queue, run_length);
                } else {
                    run_length_queue_enqueue(queue, run_length);
                }
            } else {
                run_length_queue_enqueue(queue, run_length);
            }

            previous_was_descending = true;
        }

        ++left;
        ++right;
    }

    if (left == last) { //tackle last element
        if (cmp(((char*) base) + size * (last - 1),
                ((char*) base) + size * last) <= 0) {
            run_length_queue_add_to_last(queue, 1);
        } else {
            run_length_queue_enqueue(queue, 1);
        }
    }

    free(swap_buffer);
    return queue;
}

void merge(void *source,
           void *target,
           size_t size,
           size_t offset,
           size_t left_run_length,
           size_t right_run_length,
           int (*cmp)(const void*, const void*))
{
    size_t left  = offset;
    size_t right = left + left_run_length;
    const size_t left_bound = right;
    const size_t right_bound = right + right_run_length;
    size_t target_index = offset;

    while (left < left_bound && right < right_bound) {
        if (cmp(((char*) source) + size * right,
                ((char*) source) + size * left) < 0) {
            memcpy(((char*) target) + size * target_index,
                   ((char*) source) + size * right,
                   size);
            ++right;
        } else {
            memcpy(((char*) target) + size * target_index,
                   ((char*) source) + size * left,
                   size);
            ++left;
        }

        ++target_index;
    }

    memcpy(((char*) target) + size * target_index,
           ((char*) source) + size * left,
           (left_bound - left) * size);

    memcpy(((char*) target) + size * target_index,
           ((char*) source) + size * right,
           (right_bound - right) * size);
}

static size_t get_number_of_leading_zeros(size_t number)
{
    size_t mask = 1;
    size_t number_of_leading_zeros = 0;

    mask <<= (sizeof number) * BITS_PER_BYTE - 1;

    while (mask && ((mask & number) == 0)) {
        ++number_of_leading_zeros;
        mask >>= 1;
    }

    return number_of_leading_zeros;
}

static size_t get_number_of_merge_passes(size_t runs)
{
    //calculate size-clz
    return sizeof(size_t) * BITS_PER_BYTE -
           get_number_of_leading_zeros(runs - 1); //runs-1 ex:8(0001000)to 1  3 levels
}

/*
base :array pointer
num :data number
size :size of each element
comparator:comparator function
*/
void stable_sort(void *base, size_t num, size_t size, int (*comparator)(const void*, const void*))
{
    size_t i;

    run_length_queue *queue;

    void *buffer;
    void *source;
    void *target;
    void *tmp;

    size_t offset;
    size_t merge_passes;
    size_t runs_remaining;
    size_t tail_run_length;
    size_t left_run_length;
    size_t right_run_length;

    //if wrong input
    if (!base || !comparator || num < 2 || size == 0) {
        return;
    }

    buffer = malloc(num * size);

    // if failed to allocate memory for buffer >>> use qsort
    if (!buffer) {
        qsort(base, num, size, comparator);
        return;
    }

    //build queue
    queue = build_run_length_queue(base, num, size, comparator);

    if (!queue) {
        /* Cannot allocate the run length queue. Resort to qsort and possibly
           fail in the same manner as qsort. */
        qsort(base, num, size, comparator);
        return;
    }

    // calcutlate the "level" of merge
    merge_passes = get_number_of_merge_passes(run_length_queue_size(queue));

    if ((merge_passes & 1) == 1) { //if odd
        source = buffer;
        target = base;
        memcpy(buffer, base, num * size); //copy data to buffer, buffer is source
    }// update state -> base -> buffer ->base -> buffer ->base
    else { //if even  -> don't copy, base is source, final target is base
        source = base;
        target = buffer;
    }// update state -> buffer ->base -> buffer ->base -> buffer ->base

    offset = 0; //資料邊移量
    runs_remaining = run_length_queue_size(queue); //get queue's size
    // runs_remaining:the number of block in queue which is not tackled yet
    while (run_length_queue_size(queue) > 1) {
        left_run_length  = run_length_queue_dequeue(queue); //get left array size
        right_run_length = run_length_queue_dequeue(queue); //get right array size

        merge(source,
              target,
              size,
              offset,
              left_run_length,
              right_run_length,
              comparator);

        run_length_queue_enqueue(queue, left_run_length + right_run_length); //put into queue
        runs_remaining -= 2;
        offset += left_run_length + right_run_length;

        switch (runs_remaining) {
            case 1: //if these remains one block which can't make a pair
                tail_run_length = run_length_queue_dequeue(queue); //pop
                memcpy(((char*) target) + offset * size,
                       ((char*) source) + offset * size,
                       size * tail_run_length);
                run_length_queue_enqueue(queue, tail_run_length); //push
            /* FALL THROUGH! */

            case 0:
                runs_remaining = run_length_queue_size(queue); //reget queue
                offset = 0; //initialize to zero
                /*change source and target*/
                tmp = source;
                source = target;
                target = tmp;
                break;
        }
    }
    //free queue buffer
    run_length_queue_free(queue);
    free(buffer);
}
