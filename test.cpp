#include "lambda-commented.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

using namespace std;

class Mult10 {
	public:
	int operator()(int &v);
};

int Mult10::operator()(int &v) {
	return v * 10;
}

int main(void) {

	/* Initial Vector */
	vector<int> v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);

	/*
	 * Using the Lambda Library to multiply by 10
	 */
	printf("=== Lambda Library \\w transform() ===\n");
	transform(v.begin(), v.end(), v.begin(), _1 * 10);
	for_each(v.begin(), v.end(), cout << _1 << "\n");

	/* 
	 * Using the Lambda library to multiply by 10
	 * with the assignment operator.
	 */
	printf("=== Lambda Library \\w operator=() ===\n");
	for_each(v.begin(), v.end(), _1 = _1 * 10);
	for_each(v.begin(), v.end(), cout << _1 << "\n");

	printf("=== Mult10 class with Functor ===\n");
	Mult10 f;
	transform(v.begin(), v.end(), v.begin(), f);
	for_each(v.begin(), v.end(), cout << _1 << "\n");

	printf("=== std::multiplies ===\n");
	transform(v.begin(), v.end(), v.begin(), bind2nd(std::multiplies<int>(), 10));
	for_each(v.begin(), v.end(), cout << _1 << "\n");

	return 0;
}
