// RUN: cconv-standalone %s -- | FileCheck -match-full-lines %s

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void basic1() {
	char data[] = "abcdefghijklmnop";

	char *buffer = malloc(50);
	strcpy(buffer, data);

	free(buffer);
	free(buffer); // Double free
}

//CHECK: char data _Checked[17]: count(16) =  "abcdefghijklmnop";
//CHECK: char *buffer = malloc(50);
//CHECK-NEXT: strcpy(buffer, ((const char *)data));

char* basic2(int temp) {
	char data[] = "abcdefghijklmnop";
	char data2[] = "abcdefghijklmnopabcdefghijklmnopabcdefghijklmnopabcdefghijklmnop";

	if (temp) {
		char *buffer = malloc(8);
		strcpy(buffer, data2); // Overflow
		if (temp) {
			free(buffer);
		} else {
			puts(buffer);
		}
		return buffer; // Return after free
	} else {
		char *buffer = malloc(1024);
		strcpy(buffer, data); // Data not freed
		return 0;
	}
}
//CHECK: char * basic2(int temp) {
//CHECK: char *buffer = malloc(8);
//CHECK: char *buffer = malloc(1024);

char* basic3(int* data, int count) {
	while (count > 1) {
		int* temp = malloc(8);
		data = malloc(8);
		count--;
	}
}
//CHECK: _Ptr<char> basic3(int *data, int count) {
//CHECK: int* temp = malloc(8);

void sum_numbers(int count) {
    int n, i, sum = 0;

    printf("Enter number of elements: ");
	n = count;

    int *ptr = (int*) malloc(n * sizeof(int));

    if(ptr == NULL)
    {
        printf("Error! memory not allocated.");
        exit(0);
    }
    printf("Enter elements: ");

    for(i = 0; i < n; ++i)
    {
        scanf("%d", ptr + i);
        sum += *(ptr + i);
    }
    free(ptr);
}
//CHECK: int *ptr = (int*) malloc(n * sizeof(int));


void basic_calloc(int count) {
    int n, i, sum = 0;

    printf("Enter number of elements: ");
	n = count;
    int *ptr = (int*) calloc(n, sizeof(int));

    if(ptr == NULL)
    {
        printf("Error! memory not allocated.");
        exit(0);
    }

    printf("Enter elements: ");

    for(i = 0; i < n; ++i)
    {
        scanf("%d", ptr + i);
        sum += *(ptr + i);
    }

    printf("Sum = %d", sum);
    free(ptr);
}
//CHECK: int *ptr = (int*) calloc(n, sizeof(int));

void basic_realloc(int count) {
    int i , n1, n2;

    printf("Enter size: ");

	n1 = count;
    int *ptr = (int*) malloc(n1 * sizeof(int));

    printf("Addresses of previously allocated memory: ");

    for(i = 0; i < n1; ++i)
         printf("%u\n",ptr + i);
    printf("\nEnter the new size: ");
    scanf("%d", &n2);
    ptr = realloc(ptr, n2 * sizeof(int));
    printf("Addresses of newly allocated memory: ");

    for(i = 0; i < n2; ++i)

         printf("%u\n", ptr + i);

    free(ptr);
}
//CHECK: int *ptr = (int*) calloc(n, sizeof(int));

struct student
{
    char name[30];
    int roll;
    float perc;
};
//CHECK: char name _Checked[30];

void basic_struct(int count) {

    int n,i;

    printf("Enter total number of elements: ");
	count = n;
    struct student *pstd=(struct student*)malloc(n*sizeof(struct student));



    if(pstd==NULL)
    {
        printf("Insufficient Memory, Exiting... \n");
    }

    for(i=0; i<n; i++)
    {
        printf("\nEnter detail of student [%3d]:\n",i+1);
        printf("Enter name: ");
        scanf(" "); /*clear input buffer*/
        gets((pstd+i)->name);
        printf("Enter roll number: ");
        scanf("%d",&(pstd+i)->roll);
        printf("Enter percentage: ");
        scanf("%f",&(pstd+i)->perc);
    }

    printf("\nEntered details are:\n");

    for(i=0; i<n; i++)
    {
        printf("%30s \t %5d \t %.2f\n",(pstd+i)->name,(pstd+i)->roll,(pstd+i)->perc);
    }

}
//CHECK: struct student *pstd=(struct student*)malloc(n*sizeof(struct student));

struct student * new_student() {
		char name[] = "Bilbo Baggins";
		struct student *new_s = malloc(sizeof(struct student));
		strcpy(new_s->name, name);
		new_s->roll = 3;
		new_s->perc = 4.3f;

}
//CHECK: _Ptr<struct student> new_student(void) {
//CHECK-NEXT: char name _Checked[14]: count(13) =  "Bilbo Baggins";
//CHECK: struct student *new_s = malloc(sizeof(struct student));

int main() {
	basic1();
	basic2(1);
	basic2(2);
	sum_numbers(10);
	basic_calloc(100);
	basic_realloc(100);
	basic_struct(100);
}
