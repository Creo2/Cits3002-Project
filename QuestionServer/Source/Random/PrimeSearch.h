#pragma once

#define NUMBER_OF_PRIMES 168

class PrimeSearch {
private:
	static int prime_array[NUMBER_OF_PRIMES];

	int skip;
	int currentPosition;
	int maxElements;
	int *currentPrime;
	int searches;

public:
	PrimeSearch(int elements, int rand_A, int rand_B, int rand_C);
	int GetNext(bool restart = false);
	bool Done() { return (searches == *currentPrime); }
	void Restart() { currentPosition = 0; searches = 0; }

};