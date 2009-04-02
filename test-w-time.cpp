#include "lambda-commented.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

#include <time.h>
#include <sys/time.h>

using namespace std;

class Mult10 {
	public:
	int operator()(int &v);
};

int Mult10::operator()(int &v) {
	return v + 1;
}

int main(void) {

	/* Timing */
	struct timeval tv_initial, tv_finish;
	struct timezone tz;
	int i = 0;
	
	/* Initial Vector */
	vector<int> v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);

	/*
	 * Using the Lambda Library to multiply by 10
	 */
	printf("=== Lambda Library \\w transform() ===\n");
	gettimeofday(&tv_initial, &tz);
	for(i = 0; i < 1000000; i++) {
		transform(v.begin(), v.end(), v.begin(), _1 + 1);
	}
	gettimeofday(&tv_finish, &tz);
	for_each(v.begin(), v.end(), cout << _1 << "\n");
	printf("Took %d microseconds\n", ((int)(tv_finish.tv_sec - tv_initial.tv_sec) * 1000000) + (int)(tv_finish.tv_usec - tv_initial.tv_usec));

	printf("=== Mult10 class with Functor ===\n");
	gettimeofday(&tv_initial, &tz);
	Mult10 f;
	for(i = 0; i < 1000000; i++) {
		transform(v.begin(), v.end(), v.begin(), f);
	}
	gettimeofday(&tv_finish, &tz);
	for_each(v.begin(), v.end(), cout << _1 << "\n");
	printf("Took %d microseconds\n", ((int)(tv_finish.tv_sec - tv_initial.tv_sec) * 1000000) + (int)(tv_finish.tv_usec - tv_initial.tv_usec));

	printf("=== std::plus ===\n");
	gettimeofday(&tv_initial, &tz);
	for(i = 0; i < 1000000; i++) {
		transform(v.begin(), v.end(), v.begin(), bind2nd(std::plus<int>(), 1));
	}
	gettimeofday(&tv_finish, &tz);
	for_each(v.begin(), v.end(), cout << _1 << "\n");
	printf("Took %d microseconds\n", ((int)(tv_finish.tv_sec - tv_initial.tv_sec) * 1000000) + (int)(tv_finish.tv_usec - tv_initial.tv_usec));

	return 0;
}