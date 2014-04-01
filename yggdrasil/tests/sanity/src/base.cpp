#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#include <ctype.h>
#include <algorithm>
#include <utility>
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <chrono>
#include <memory>
#include <thread>

using namespace std;

int result = 0;

void works(const char* str){
	printf("Works: %s\n", str);
}

void doesnt(const char* str){
	printf("Doesn\'t work: %s\n", str);
	result = 1;
}

void report(bool b, const char* str){
	b ? works(str) : doesnt(str);
}

int main(){
	works("stdio.h");
	
	int* p = (int*)malloc(10*sizeof(int));
	report(p, "stdlib.h");
	free(p);
	
	char str[] = "string";
	report(strlen(str) == 6, "string.h");
	
	report((int)sqrt(105) == 10, "math.h");
	
	intmax_t big = 10; big++;
	report(sizeof(intmax_t) >= sizeof(long long), "inttypes.h");
	
	report(atoi("12") == 12, "ctype.h");
	
	int v[4] = {3, 4, 1, 2};
	sort(v, v+4);
	report(v[0] < v[1] && v[1] < v[2] && v[2] < v[3], "algorithm");
	
	pair<int, char> pr = {1, 'c'};
	report(pr.first == 1 && pr.second == 'c', "utility");
	
	cout << "Works: iostream" << endl;
	
	string s = "asdf";
	report(s.size() == 4, "string");
	
	vector<int> vi = {4, 6, 1, 3};
	vector<int> vii = {1, 3, 4, 6};
	sort(vi.begin(), vi.end());
	report(vi == vii, "vector");
	
	mutex mut;
	{
		unique_lock<mutex> l(mut, defer_lock);
		report(l.try_lock(), "mutex");
	}
	
	auto c = chrono::system_clock::now();
	report(chrono::seconds(10).count() == 10 && chrono::system_clock::now() >= c, "chrono");
	
	{
		auto x = make_shared<string>("test");
		report(x->size() == 4, "memory");
	}
	
	thread t([](){ printf("Works: thread\n"); });
	t.join();
	
	
	printf("== Finished ==\n");
	return result;
}

