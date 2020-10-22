// RUN: cconv-standalone -addcr -alltypes %s -- | FileCheck -match-full-lines -check-prefixes="CHECK_ALL","CHECK" %s
// RUN: cconv-standalone -addcr %s -- | FileCheck -match-full-lines -check-prefixes="CHECK_NOALL","CHECK" %s
// RUN: cconv-standalone -addcr %s -- | %clang -c -fcheckedc-extension -x c -o /dev/null -
// RUN: cconv-standalone -alltypes -output-postfix=checked %s
// RUN: cconv-standalone -alltypes %S/partial_checked.checked.c -- | count 0
// RUN: rm %S/partial_checked.checked.c

void test0(_Ptr<int *> a) { }
// CHECK: void test0(_Ptr<_Ptr<int>> a) _Checked { }

void test1(_Ptr<int **> a) { }
// CHECK: void test1(_Ptr<_Ptr<_Ptr<int>>> a) _Checked { }

void test2(_Ptr<int **> a) { 
// CHECK: void test2(_Ptr<_Ptr<int *>> a) { 
  **a = 1;
}

void test3(_Ptr<int *> a) {
// CHECK: void test3(_Ptr<int *> a) {
  *a = 1;
}

void test4() {
  _Ptr<int *> a;
  // CHECK: _Ptr<_Ptr<int>> a = ((void *)0);

  _Ptr<int *> b = 0;
  // CHECK: _Ptr<int *> b = 0;
  *b = 1;

  _Ptr<_Ptr<int *>> c;
  // CHECK: _Ptr<_Ptr<_Ptr<int>>> c = ((void *)0);

  int *d _Checked[4];
  // CHECK: _Ptr<int> d _Checked[4] = {((void *)0)};

  _Ptr<int *> e _Checked[4];
  // CHECK: _Ptr<_Ptr<int>> e _Checked[4] = {((void *)0)};

  _Ptr<int *> f _Checked[4] = {0};
  // CHECK: _Ptr<int *> f _Checked[4] = {0};
  (*f[0]) = 1;

  _Ptr<int **> g, h, i _Checked[1];
  // CHECK: _Ptr<_Ptr<_Ptr<int>>> g = ((void *)0);
  // CHECK: _Ptr<_Ptr<int *>> h = ((void *)0);
  // CHECK: _Ptr<_Ptr<_Ptr<int>>> i _Checked[1] = {((void *)0)};
  **h = 1;
}

void test5(_Ptr<int *> a, _Ptr<int *> b, _Ptr<_Ptr<int>> c, int **d) {
// CHECK: void test5(_Ptr<_Ptr<int>> a, _Ptr<int *> b, _Ptr<_Ptr<int>> c, _Ptr<_Ptr<int>> d) {
  *b  = 1;
}

struct s0  {
  _Ptr<int *> a,b;
  _Ptr<_Ptr<int *>> c;
  _Ptr<_Ptr<int>> d;
  int * e _Checked[1];
// CHECK:  _Ptr<_Ptr<int>> a;
// CHECK:  _Ptr<_Ptr<int>> b;
// CHECK:  _Ptr<_Ptr<_Ptr<int>>> c;
// CHECK:  _Ptr<_Ptr<int>> d;
// CHECK:  _Ptr<int> e _Checked[1];
};
