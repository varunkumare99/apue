#include<stdio.h>

int main(int argc, char *argv[])
{
    char arr[][3]={
        {'a','b','c'},
        {'d','e','f'},
        {'g','h','i'}
    };
    char *p = arr[1];
    printf("p->%c\n",*p);
    printf("arr->%c\n",arr[1][0]);

    *p = 'z';
    printf("p->%c\n",*p);
    printf("arr->%c\n",arr[1][0]);
    return 0;
}
