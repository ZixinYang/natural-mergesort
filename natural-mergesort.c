typedef struct run_length_queue {
    int*   storage;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t size;
    size_t mask;
} run_length_queue;

//取得2的倍數
static size_t fix_capacity(size_t capacity)
{
    size_t ret = 1;

    while (ret < capacity) 
    {
        ret <<= 1;
    }

    return ret;
}

static size_t max(size_t a, size_t b) 
{
    return a > b ? a : b;
}

static const size_t MINIMUM_RUN_LENGTH_QUEUE_CAPACITY = 256; //最小的queue storage length
static const size_t BITS_PER_BYTE = 8;

static run_length_queue* run_length_queue_alloc(size_t capacity) 
{
    run_length_queue* queue;

    //選擇較大的capacity
    capacity = max(capacity, MINIMUM_RUN_LENGTH_QUEUE_CAPACITY);
    capacity = fix_capacity(capacity); //變成2^n

    queue = malloc(sizeof(*queue)); //自身malloc空間

    if (!queue) //malloc queue失敗
    {
        return NULL;
    }

    //為queue的storage分配capacity的空間(單位為int)
    queue->storage = malloc(sizeof(int) * capacity);

    if (!queue->storage) //malloc queue storage失敗
    {
        free(queue);
        return NULL;
    }
    //初始化queue
    queue->capacity = capacity; //容量
    queue->mask = capacity - 1; //環狀queue
    queue->head = 0; //起始
    queue->tail = 0; //末
    queue->size = 0; //在queue總數

    return queue;
}

static void run_length_queue_enqueue(run_length_queue* queue, size_t run_size)
{
    queue->storage[queue->tail] = run_size;
    queue->tail = (queue->tail + 1) & queue->mask;
    queue->size++;
}

static void run_length_queue_add_to_last(run_length_queue* queue, 
                                         size_t run_size)
{
    queue->storage[(queue->tail - 1) & queue->mask] += run_size;
}

static size_t run_length_queue_dequeue(run_length_queue* queue)
{
    size_t run_length = queue->storage[queue->head];
    queue->head = (queue->head + 1) & queue->mask;
    queue->size--;
    return run_length;
}

static size_t run_length_queue_size(run_length_queue* queue) 
{
    return queue->size;
}

static void run_length_queue_free(run_length_queue* queue) 
{
    if (queue && queue->storage)
    {
        free(queue->storage);
    }
}

static void reverse_run(char* base, size_t num, size_t size, void* swap_buffer)
{
    size_t left = 0;
    size_t right = num - 1;

    while (left < right)
    {
        memcpy(swap_buffer, base + size * left, size);
        memcpy(base + size * left, base + size * right, size);
        memcpy(base + size * right, swap_buffer, size);

        ++left;
        --right;
    }
}

static run_length_queue* 
build_run_length_queue(void* base, 
                       size_t num,
                       size_t size,
                       int (*cmp)(const void*, const void*)) 
{
    run_length_queue* queue;
    size_t head;
    size_t left;
    size_t right;
    size_t last;
    size_t run_length;
    bool previous_was_descending;
    void* swap_buffer = malloc(size);
    queue = run_length_queue_alloc((num >> 1) + 1); //分配(num*/2+1)(會resize成2^n或256)的queue

    if (!queue) //alloc 失敗
    {
        return NULL;
    }
    //初始化
    left = 0;
    right = 1;
    last = num - 1;
    previous_was_descending = false;

    while (left < last)
    {
        head = left;

        /* Decide the direction of the next run. */
        if (cmp(((char*) base) + size * left++, 
                ((char*) base) + size * right++) <= 0)
        {
            /* The run is ascending. */
            while (left < last 
                    && cmp(((char*) base) + size * left, 
                           ((char*) base) + size * right) <= 0) 
            {
                ++left;
                ++right;
            }
            //計算長度
            run_length = left - head + 1;

            if (previous_was_descending)//如果前一個是遞減型(實際上已經變成了遞增) 檢查是否可以合併
            {                           //data 會呈現 遞減 遞增 遞減 遞增......... 只有前一個是遞減才有機會合併
                if (cmp(((char*) base) + (head - 1) * size, 
                        ((char*) base) + head * size) <= 0)
                {
                    run_length_queue_add_to_last(queue, run_length);
                }
                else
                {
                    run_length_queue_enqueue(queue, run_length);
                }
            }
            else
            {
                run_length_queue_enqueue(queue, run_length);
            }

            previous_was_descending = false;
        }
        else
        {
            /* Scan a strictly descending run. */
            while (left < last
                    && cmp(((char*) base) + size * left, 
                           ((char*) base) + size * right) > 0)
            {
                ++left;
                ++right;
            }

            run_length = left - head + 1;
            reverse_run(((char*) base) + head * size, 
                        run_length,
                        size, 
                        swap_buffer);

            if (previous_was_descending)
            {
                if (cmp(((char*) base) + size * (head - 1), 
                        ((char*) base) + size * head) <= 0) 
                {
                    run_length_queue_add_to_last(queue, run_length);
                }
                else
                {
                    run_length_queue_enqueue(queue, run_length);
                }
            }
            else
            {
                run_length_queue_enqueue(queue, run_length);
            }

            previous_was_descending = true;
        }

        ++left;
        ++right;
    }

    if (left == last) //處理最後一個元素
    {
        if (cmp(((char*) base) + size * (last - 1), 
                ((char*) base) + size * last) <= 0) 
        {
            run_length_queue_add_to_last(queue, 1);
        }
        else 
        {
            run_length_queue_enqueue(queue, 1);
        }
    }

    free(swap_buffer);
    return queue;
}

void merge(void* source,
                  void* target,
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

    while (left < left_bound && right < right_bound)
    {
        if (cmp(((char*) source) + size * right, 
                ((char*) source) + size * left) < 0) 
        {
            memcpy(((char*) target) + size * target_index, 
                   ((char*) source) + size * right,
                   size);
            ++right;
        } 
        else
        {
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

    while (mask && ((mask & number) == 0))
    {
        ++number_of_leading_zeros;
        mask >>= 1;
    }

    return number_of_leading_zeros;
}

static size_t get_number_of_merge_passes(size_t runs) 
{   //計算size-clz
    return sizeof(size_t) * BITS_PER_BYTE - 
           get_number_of_leading_zeros(runs - 1); //runs-1 ex:8次(00.0001000)變成一次 共有3層
}

/*
base :陣列指標
num :資料數量
size :單位大小
comparator:比較函數
*/
void stable_sort(void* base, size_t num, size_t size, int (*comparator)(const void*, const void*))
{
    size_t i;

    run_length_queue* queue;

    void* buffer;
    void* source;
    void* target;
    void* tmp;

    size_t offset;
    size_t merge_passes;
    size_t runs_remaining;
    size_t tail_run_length;
    size_t left_run_length;
    size_t right_run_length;

    //if wrong input
    if (!base || !comparator || num < 2 || size == 0)
    {
        return;
    }

    buffer = malloc(num * size);

    //無法分配buffer空間 >>> 改用qsort
    if (!buffer)
    {
        qsort(base, num, size, comparator);
        return;
    }

    //建立queue
    queue = build_run_length_queue(base, num, size, comparator);

    if (!queue) 
    {
        /* Cannot allocate the run length queue. Resort to qsort and possibly 
           fail in the same manner as qsort. */
        qsort(base, num, size, comparator);
        return;
    }

    //計算總共需要merge幾"層"
    merge_passes = get_number_of_merge_passes(run_length_queue_size(queue));

    if ((merge_passes & 1) == 1) //如果為奇數
    {
        source = buffer;
        target = base;
        memcpy(buffer, base, num * size); //先把資料複製進buffer buffer為source
    }// 資料的更新狀況 -> base -> buffer ->base -> buffer ->base
    else  // 如果為偶數 ->不用copy 直接以 base為source 最終target也為source
    {
        source = base;
        target = buffer;
    }// 資料的更新狀況 -> buffer ->base -> buffer ->base -> buffer ->base

    offset = 0;
    runs_remaining = run_length_queue_size(queue); //取得queue的size總數

    while (run_length_queue_size(queue) > 1) 
    {
        left_run_length  = run_length_queue_dequeue(queue); //取得左陣列大小
        right_run_length = run_length_queue_dequeue(queue); //取得右陣列大小
        /*merge*/
        merge(source,
              target,
              size,
              offset,
              left_run_length,
              right_run_length,
              comparator);

        run_length_queue_enqueue(queue, left_run_length + right_run_length); //放入queue中
        runs_remaining -= 2;
        offset += left_run_length + right_run_length;

        switch (runs_remaining)
        {
            case 1: //如果剩一個無人配對
                tail_run_length = run_length_queue_dequeue(queue); //dequeue它
                memcpy(((char*) target) + offset * size,
                       ((char*) source) + offset * size,
                       size * tail_run_length);
                run_length_queue_enqueue(queue, tail_run_length); //enqueue它
                /* FALL THROUGH! */ /*接著繼續進行!!*/

            case 0:
                runs_remaining = run_length_queue_size(queue); //重新取得queue大小
                offset = 0; //重設為0
                /*交換source 與 target*/
                tmp = source;
                source = target;
                target = tmp;
                break;
        }
    }
    //free掉queue buffer
    run_length_queue_free(queue);
    free(buffer);
}
