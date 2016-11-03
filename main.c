#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include IMPL

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}

int integer_cmp(const void *a,const void *b)
{
    return *(int *)a - *(int *)b;
}

int main()
{
    struct timespec start,end;
    int  number;
    size_t size;
    double cpu_time;
    int *input_array = NULL;    
    FILE *src_file = fopen("testcase.txt","r");
    if(src_file == NULL) {
        printf("Can't open the file\n");
        return -1;
    }
    fscanf(src_file, "%d", &number);
    size = sizeof(int);
    input_array =(int *)malloc(sizeof(int)*number);
    for(int i=0; i<number;i++)
        fscanf(src_file,"%d",input_array+i);
    fclose(src_file);
    if(input_array == NULL) {
        printf("Can't allocate memory\n");
        return -1;
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    stable_sort(input_array,number,size,integer_cmp);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
#ifndef OPT
    FILE *out_file = fopen("orig_ans.txt","w");
    for(int i=0;i<number;i++)
        fprintf(out_file, "%d ", input_array[i]);
    fclose(out_file);
#else
    FILE *out_file = fopen("change_ans.txt","w");
    for(int i=0;i<number;i++)
        fprintf(out_file, "%d ", input_array[i]);
    fclose(out_file);
#endif
    cpu_time = diff_in_second(start,end);
    printf("%lf\n",cpu_time);
    return 0;
}
