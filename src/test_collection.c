/*
 * test_collection.c
 *
 *  Created on: Aug 11, 2019
 *      Author: jjustman
 */


#include <stdio.h>
#include <array.h>

int main(int argc, char **argv) {
    // Create a new array
    Array *ar;
    array_new(&ar);

    // Add a string to the array
    array_add(ar, "Hello World!\n");

    // Retreive the string and print it
    char *str;
    array_get_at(ar, 0, (void*) &str);
    printf("%s", str);

    return 0;
}
