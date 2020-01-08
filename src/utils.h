#include <vector>
#include <iostream>

using namespace std;

#define NOW chrono::steady_clock::now()
#define DUR(t1, t2) chrono::duration <double, micro> (t2 - t1).count()

void print_test_res(bool res, string test_name);
void gen_random_input(vector<int>& inputs, int min, int max);
ostream& operator<<(ostream& out, const vector<double>& vec);
void split_string(const string& s, vector<string>& v, const string& c);
unsigned int pop_count_asm(unsigned int x);
