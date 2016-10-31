#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int compare(const void *a,const void *b)
{
    return *(int *) a - *(int *)b;
}

int main(int argc,char* argv[])
{
    if(argc < 3) {
        printf("Usage:/gen_testcase [number] [range]\n");
        return -1;
    }
    int number = atoi(argv[1]);
    int range = atoi (argv[2]);
    int i;
    int temp;
    int divisor;
    FILE *fp = fopen("testcase.txt","w");
    if(fp == NULL) {
        printf("Can't not open a file\n");
        return -1;
    }
    srand(time (NULL));
    fprintf(fp,"%d\n",number);

    divisor=RAND_MAX/(range+1);

    for(i=0; i<number; i++){
        do{
            temp=rand()/divisor;
        }while(temp>range);
        fprintf(fp,"%d ",temp);
    }

    return 0;
}
