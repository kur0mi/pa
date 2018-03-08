#include "trap.h"

// 2, 3, 5, 7			return 1
int is_prime(int n) {
	if(n < 2) return 0;

	int i;
	for(i = 2; i < n; i ++) {
		if(n % i == 0) {
			return 0;
		}
	}

	return 1;
}

int goldbach(int n) {
	int i;
	for(i = 2; i < n; i ++) {
		if(is_prime(i) && is_prime(n - i)) {
			return 0;
		}
	}

	return 1;
}

int main() {
	nemu_assert(goldbach(6) == 1);
	//nemu_assert(is_prime());
	//nemu_assert(1 && 1 == 1);
	//nemu_assert(is_prime(2) == 1);
	//nemu_assert(is_prime(3) == 1);
	//nemu_assert(is_prime(5) == 1);
	//nemu_assert(is_prime(7) == 1);

	return 0;
}
/*
int main() {
	int n;
	for(n = 4; n <= 30; n += 2) {
		nemu_assert(goldbach(n) == 1);
	}

	return 0;
}
*/

