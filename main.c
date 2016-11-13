#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include IMPL

int integer_cmp(const void *a,const void *b)
{
    return *(int *)a - *(int *)b;
}

int main()
{
    clock_t start,end;
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
    start = clock();
    stable_sort(input_array,number,size,integer_cmp);
    end = clock();
#ifndef OPT
    FILE *out_file = fopen("orig_ans.txt","w");
    for(int i=0;i<number;i++)
        fprintf(out_file, "%d ", input_array[i]);
    fclose(out_file);
    FILE *out_time_file = fopen("orig_runtime.txt","a");
    cpu_time = (double)(end-start)/CLOCKS_PER_SEC;
    fprintf(out_time_file,"%lf\n", cpu_time);
#else
    FILE *out_file = fopen("change_ans.txt","w");
    for(int i=0;i<number;i++)
        fprintf(out_file, "%d ", input_array[i]);
    fclose(out_file);
    FILE *out_time_file = fopen("opt_runtime.txt","a");
    cpu_time = (double)(end-start)/CLOCKS_PER_SEC;
    fprintf(out_time_file,"%lf\n", cpu_time);
#endif
    return 0;
}
