// RUN: cconv-standalone -base-dir=%S -alltypes -output-postfix=checked %s %S/arrofstructcallermulti2.c
//RUN: FileCheck -match-full-lines --input-file %S/arrofstructcallermulti1.checked.c %s
//RUN: rm %S/arrofstructcallermulti1.checked.c %S/arrofstructcallermulti2.checked.c
#define size_t int
#define NULL 0
extern _Itype_for_any(T) void *calloc(size_t nmemb, size_t size) : itype(_Array_ptr<T>) byte_count(nmemb * size);
extern _Itype_for_any(T) void free(void *pointer : itype(_Array_ptr<T>) byte_count(0));
extern _Itype_for_any(T) void *malloc(size_t size) : itype(_Array_ptr<T>) byte_count(size);
extern _Itype_for_any(T) void *realloc(void *pointer : itype(_Array_ptr<T>) byte_count(1), size_t size) : itype(_Array_ptr<T>) byte_count(size);
extern int printf(const char * restrict format : itype(restrict _Nt_array_ptr<const char>), ...);
extern _Unchecked char *strcpy(char * restrict dest, const char * restrict src : itype(restrict _Nt_array_ptr<const char>));

struct general { 
    int data; 
    struct general *next;
};
//CHECK:     struct general *next;


struct warr { 
    int data1[5];
    char name[];
};
//CHECK:     int data1 _Checked[5];
//CHECK-NEXT:     _Ptr<char> name;


struct fptrarr { 
    int *values; 
    char *name;
    int (*mapper)(int);
};
//CHECK:     _Ptr<int> values; 
//CHECK-NEXT:     _Ptr<char> name;
//CHECK-NEXT:     _Ptr<int (int )> mapper;


struct fptr { 
    int *value; 
    int (*func)(int*);
};  
//CHECK:     _Ptr<int> value; 
//CHECK-NEXT:     _Ptr<int (_Ptr<int> )> func;


struct arrfptr { 
    int args[5]; 
    int (*funcs[5]) (int);
};
//CHECK:     int args _Checked[5]; 
//CHECK-NEXT:     _Ptr<int (int )> funcs _Checked[5];


int add1(int x) { 
    return x+1;
} 

int sub1(int x) { 
    return x-1; 
} 

int fact(int n) { 
    if(n==0) { 
        return 1;
    } 
    return n*fact(n-1);
} 

int fib(int n) { 
    if(n==0) { return 0; } 
    if(n==1) { return 1; } 
    return fib(n-1) + fib(n-2);
} 

int zerohuh(int n) { 
    return !n;
}

int *mul2(int *x) { 
    *x *= 2; 
    return x;
}

//CHECK: _Ptr<int> mul2(_Ptr<int> x) { 

struct general ** sus(struct general *, struct general *);
//CHECK: struct general ** sus(struct general *, struct general *);

struct general ** foo() {
        struct general * x = malloc(sizeof(struct general));
        struct general * y = malloc(sizeof(struct general));
        
        struct general *curr = y;
        for(int i = 1; i < 5; i++, curr = curr->next) { 
            curr->data = i;
            curr->next = malloc(sizeof(struct general));
            curr->next->data = i+1;
        }
        struct general ** z = sus(x, y);
return z; }
//CHECK: struct general ** foo() {
//CHECK:         struct general * x = malloc(sizeof(struct general));
//CHECK:         struct general * y = malloc(sizeof(struct general));
//CHECK:         struct general *curr = y;
//CHECK:         struct general ** z = sus(x, y);

struct general ** bar() {
        struct general * x = malloc(sizeof(struct general));
        struct general * y = malloc(sizeof(struct general));
        
        struct general *curr = y;
        for(int i = 1; i < 5; i++, curr = curr->next) { 
            curr->data = i;
            curr->next = malloc(sizeof(struct general));
            curr->next->data = i+1;
        }
        struct general ** z = sus(x, y);
z += 2;
return z; }
//CHECK: struct general ** bar() {
//CHECK:         struct general * x = malloc(sizeof(struct general));
//CHECK:         struct general * y = malloc(sizeof(struct general));
//CHECK:         struct general *curr = y;
//CHECK:         struct general ** z = sus(x, y);
