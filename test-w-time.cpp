
// Lambda
#include "lambda-commented.h"

// Required
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

// Timing
#include <time.h>
#include <sys/time.h>

// Boost Lambda
#include <boost/lambda/lambda.hpp>

using namespace std;

class Plus1 {
	public:
	int operator()(int &v);
};

int Plus1::operator()(int &v) {
	return v + 1;
}

struct plus_1 : public unary_function<int, int> {
	int operator()(int v){ return v + 1; }
};

int main(void) {

	/* Timing */
	struct timeval tv_initial, tv_finish;
	struct timezone tz;
	int i = 0;
	
	/* Initial Vector of 1,000,000 integers */
	vector<int> v;
	for(i = 0; i < 1000000; i++) {
		v.push_back(i);
	}

	//printf("=== Lambda Library \\w transform() ===\n");
	gettimeofday(&tv_initial, &tz);
	transform(v.begin(), v.end(), v.begin(), _1 + 1);
	gettimeofday(&tv_finish, &tz);
	printf("%d,", ((int)(tv_finish.tv_sec - tv_initial.tv_sec) * 1000000) + (int)(tv_finish.tv_usec - tv_initial.tv_usec));

	//printf("=== Plus1 class with Functor ===\n");
	gettimeofday(&tv_initial, &tz);
	transform(v.begin(), v.end(), v.begin(), Mult10());
	gettimeofday(&tv_finish, &tz);
	printf("%d,", ((int)(tv_finish.tv_sec - tv_initial.tv_sec) * 1000000) + (int)(tv_finish.tv_usec - tv_initial.tv_usec));

	//printf("=== plus_1 unary_function ===\n");
	gettimeofday(&tv_initial, &tz);
	transform(v.begin(), v.end(), v.begin(), plus_1());
	gettimeofday(&tv_finish, &tz);
	printf("%d,", ((int)(tv_finish.tv_sec - tv_initial.tv_sec) * 1000000) + (int)(tv_finish.tv_usec - tv_initial.tv_usec));

	//printf("=== <functional> ===\n");
	gettimeofday(&tv_initial, &tz);
	transform(v.begin(), v.end(), v.begin(), bind2nd(std::plus<int>(), 1));
	gettimeofday(&tv_finish, &tz);
	printf("%d,", ((int)(tv_finish.tv_sec - tv_initial.tv_sec) * 1000000) + (int)(tv_finish.tv_usec - tv_initial.tv_usec));
	
	//printf("=== boost::lambda ===\n");
	gettimeofday(&tv_initial, &tz);
	transform(v.begin(), v.end(), v.begin(), boost::lambda::_1 + 1);
	gettimeofday(&tv_finish, &tz);
	printf("%d\n", ((int)(tv_finish.tv_sec - tv_initial.tv_sec) * 1000000) + (int)(tv_finish.tv_usec - tv_initial.tv_usec));

	return 0;
}
