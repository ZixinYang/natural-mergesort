#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    FILE *fp = fopen("orig_runtime.txt", "r");
    FILE *output = fopen("output.txt", "w");
    if (!fp) {
        printf("ERROR opening input file orig.txt\n");
        exit(0);
    }
    int i = 0;
    double orig_sum = 0.0,  orig_runtime;
    for (i = 0; i < 100; i++) {
        if (feof(fp)) {
            printf("ERROR: You need 100 datum instead of %d\n", i);
            printf("run 'make run' longer to get enough information\n\n");
            exit(0);
        }
        fscanf(fp, "%lf\n", &orig_runtime);
        orig_sum += orig_runtime;
    }
    fclose(fp);
    
    fp = fopen("opt_runtime.txt", "r");
    if (!fp) {
        if (!fp) {
            printf("ERROR opening input file opt.txt\n");
            exit(0);
        }
    }
    double opt_sum = 0.0, opt_runtime;
    for (i = 0; i < 100; i++) {
        if (feof(fp)) {
            printf("ERROR: You need 100 datum instead of %d\n", i);
            printf("run 'make run' longer to get enough information\n\n");
            exit(0);
        }
        fscanf(fp, "%lf\n", &opt_runtime);
        opt_sum += opt_runtime;
    }
    fprintf(output," ,");
    fprintf(output, "%lf,%lf\n",orig_sum / 100.0, opt_sum / 100.0);
    fclose(output); 
    fclose(fp);
    return 0;
}

