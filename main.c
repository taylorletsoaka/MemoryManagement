// main.c

/*
 *  This is a simple program for your custom malloc. Only the most basic cases are tested,
 *  you are encouraged to add more test cases to ensure your functions work correctly.
 *  Having everything in this file working does not indicate that you have fully
 *  completed the assignment. You need to implement everything specified in the brief.
 *
 *  A result of 'done' does NOT indicate that the function works entirely correctly, only that
 *  the operation did not create fatal errors.
 */

#include "allocator.h"
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
int main(int argc, char **argv){

    int *data;
    bool done;
    double *ddata;
    printf("\n\n");
    ddata = (double*) custom_malloc(sizeof(double));
    *ddata = 12345.6789;
    printf("%s\n", (*ddata) == 12345.6789 ? "done" : "failed!");

    printf("Array malloc....\n");
    data = (int*) custom_malloc(1028*sizeof(int));
    for(int i = 0; i < 1028; i++){
      data[i] = i;
    }
    done = true;
    for(int i = 0; i < 1028; i++){
      done &= (data[i]==i);
    }
    printf("%s\n", done ? "done" : "failed!");

    printf("Basic free......\n");
    custom_free(ddata);
    printf("'done'\n");

    printf("Array free......\n");
    custom_free(data);
    printf("'done'\n");

    printf("Basic realloc...\n");
    ddata = (double*) custom_malloc(1000*sizeof(double));//
    double* old = ddata;
    ddata = (double*) custom_realloc(ddata,sizeof(double));
  //  print_list();
    done = ((old < ddata) && ((*ddata) == 12345.6789));

    custom_free(ddata);

    printf("%s\n",done?"'done'":"failed!");

    return 0;
}
