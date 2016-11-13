#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int comparator(const void *i1, const void *i2)
{
    return *(int *)i1 - *(int *)i2;
}
int reverse(const void *i1, const void *i2)
{
    return *(int *)i2 - *(int *)i1;
}
int main(int argc, char *argv[]) {
    if(argc < 4) {
        printf("-------------------------------------------------------\n");
        printf("gen_testcase: generate an array for natural merge sort\n");
        printf("Usage : ");
        printf("./gen_testcase [mode] [number] [range] [filename]\n");
        printf("[mode] characteristic of array: (1)random (2)sorted (3)nearly-sorted (4)reverse\n");
        printf("[number] the total of data to sort\n");
        printf("[range] the maximun number of data, or noise amplitude of nearly-sorted mode\n");
        printf("(optinal) [filename] file name for test case,\nif not type, program will output result to testcase.txt\n");
        printf("-------------------------------------------------------\n");
        return -1;
    }
    int mode = atoi(argv[1]);
    int number = atoi(argv[2]);
    int range = atoi(argv[3]);
    int *array = NULL;
    int divisor;
    int temp,i;
    FILE *fp;
    if(argc == 5)
        fp = fopen(argv[4],"w");
    else
        fp = fopen("testcase.txt","w");
    if(fp == NULL) {
        printf("Can't open a file\n");
        return -1;
    }
    fprintf(fp,"%d\n",number);
    switch(mode)
    {
        case 1:
            srand(time(NULL));
            divisor = RAND_MAX/(range+1);
            for(i=0; i<number; i++) {
                do {
                    temp = rand() / divisor;
                }while(temp > range);
                fprintf(fp, "%d ", temp);
            }
            break;
        case 2:
            array = (int *)malloc(number * sizeof(int));
            if(array == NULL) {
                printf("Error: Can't allocate memory!\n");
                return -1;
            }
            divisor = RAND_MAX/(range+1);
            for(i=0; i<number; i++) {
                do {
                    array[i] = rand() / divisor;
                } while (array[i] > range);
            }
            qsort(array, number, sizeof(int), comparator);
            for(i=0; i<number; i++) {
                fprintf(fp, "%d ", array[i]);
            }
            break;
        case 3:
            for(i=0; i<number; i++) {
                fprintf(fp, "%d ", i+rand()%range);
            }
            break;
        case 4:
            array = (int *)malloc(number * sizeof(int));
            if(array == NULL) {
                printf("Error: Can't allocate memory!\n");
                return -1;
            }
            divisor = RAND_MAX/(range+1);
            for(i=0; i<number; i++) {
                do {
                    array[i] = rand() / divisor;
                } while (array[i] > range);
            }
            qsort(array, number, sizeof(int), reverse);
            for(i=0; i<number; i++){
                fprintf(fp, "%d ",array[i]);
            }
            break;
        default:
            printf("Unknown mode, program terminated\n");
    }
    free(array);
    return 0;
}
