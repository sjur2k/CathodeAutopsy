#include <stdio.h>
#include <cmath>
#include "grid.hpp"

int main(){
    int num = 100;
    Grid grid(num, 1);
    for (int i = 0; i < grid.height*grid.width; i++){
        for (int j = 0; j < int(10); j++){
            printf("*");
        }
        printf("\n");
    }
    return 0;
}