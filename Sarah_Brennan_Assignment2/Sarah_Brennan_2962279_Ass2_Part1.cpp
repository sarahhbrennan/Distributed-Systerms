// Sarah_Brennan_2962279_Ass2_Part1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

/*
	Part 1 (40%)

	Using the Prime Number program from the first assignment, add functionality that:

	a.	Distinctly synchronizes the node’s activities.  
	b.	Allows the master node to supply each of the slave nodes with an equal share of the initial array values.  
	c.	Include some notion of Performance Management (e.g. timestamps of task completion) and run the original program versus the newer version.  
	d.	Increase the relevant thresholds to gauge response, which you then document.  

*/

#include <iostream>
#include <mpi.h>
#include <chrono>

using namespace std;

// get time when program started, referenced from https://www.pluralsight.com/blog/software-development/how-to-measure-execution-time-intervals-in-c--
// c.Include some notion of Performance Management(e.g.timestamps of task completion) and run the original program versus the newer version.
auto startTimer = chrono::high_resolution_clock::now();

/**
 * prints out an array to console in a single line
 * @primeArray -> pointed to array
 * @primeArraySize -> size of array
 */
void printArray(int* primeArray, int primeArraySize) {
	for (unsigned int i = 0; i < primeArraySize; i++) {
		if (i == (primeArraySize - 1))
			cout << primeArray[i] << endl;
		else
			cout << primeArray[i] << ", ";
	}
}

/**
* returns true if number is prime (assumes 1 is not prime)
* @number -> number to check if prime
*/
bool checkPrime(int number) {
	bool prime = false;
	for (unsigned int i = 2; i <= number; i++) {
		prime = true;
		for (int j = 2; j < i; j++) {
			if (i % j == 0) {
				prime = false;
				break;
			}
		}
	}
	return prime;
}

/**
* returns amount of primes in array
* @primeArray -> reference to array
* @primeArray -> size of array
* @return number of primes
*/
int countPrime(int primeArray[], int primeArraySize) {
	int countPrimes = 0;
	for (unsigned int i = 0; i < primeArraySize; i++) {
		if (checkPrime(primeArray[i]))
			countPrimes++;
	}
	return countPrimes;
}

/**
* returns sum of primes in array
* @primeArray -> reference to array
* @primeArray -> size of array
* @return sum of array
*/
int sumPrimes(int primeArray[], int primeArraySize) {
	int sumPrimes = 0;
	for (unsigned int i = 0; i < primeArraySize; i++) {
		if (checkPrime(primeArray[i]))
			sumPrimes += primeArray[i];
	}
	return sumPrimes;
}

/**
* Generates array of numbers, determines size of participants and broadcasts to nodes,
* calculates sum of primes for each node and then prints the total sum
* @worldSize -> size of world
*/
void coordinator(int worldSize) {

	int* primeArray = NULL;
	int arraySize;

	// get size of array from user
	cout << "Please enter the array size evenly divisible by world size: ";
	cin >> arraySize;
	// print my name and student number to console
	cout << "Student Name: Sarah Brennan" << endl;
	cout << "Student Number: 2962279" << endl;

	// set array to size given by user
	primeArray = new int[arraySize];

	for (unsigned int i = 0; i < arraySize; i++)
		primeArray[i] = (rand() % 50) + 1;

	// print out array of numbers
	printArray(primeArray, arraySize);

	// get time when program started
	// c.Include some notion of Performance Management(e.g.timestamps of task completion) and run the original program versus the newer version.
	startTimer = chrono::high_resolution_clock::now();

	// determine size of each partition and broadcast to all nodes
	// this is done by dividing size of array by world size
	int partitionSize = arraySize / worldSize;

	// broadcast the partiton size to each node so memory allocated correctly
	MPI_Bcast(&partitionSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

	// send out the partition to each node
	// b.	Allows the master node to supply each of the slave nodes with an equal share of the initial array values.  	
	for (unsigned int i = 1; i < worldSize; i++) {
		MPI_Send(primeArray + partitionSize * i, partitionSize, MPI_INT, i, 0, MPI_COMM_WORLD);
	}

	// calculate sum of primes for each node
	int sum = sumPrimes(primeArray, partitionSize);

	// use reduce to gather the overall sum
	int totalSum = 0;
	MPI_Reduce(&sum, &totalSum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	cout << "Total sum of prime numbers in array is " << totalSum << endl;

	// get time program ended
	auto endTimer = chrono::high_resolution_clock::now();
	chrono::duration<double> elapsed = endTimer - startTimer;
	// prints out how long it took the program to run from splitting array into partitions
	//and getting total sum
	cout << "Elapsed time: " << elapsed.count() << endl;
}

/**
* gets the sum of primes within each node other than rank 0
* @worldRank -> gets node value
*/
void participant(int worldRank) {

	// get partition size and allocate memory
	int partitionSize = 0;
	MPI_Bcast(&partitionSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

	int* partitionArr = new int[partitionSize];

	// receive the partition
	MPI_Recv(partitionArr, partitionSize, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	// calculate sum of primes for each node
	int sum = sumPrimes(partitionArr, partitionSize);

	// use reduce to gather the overall sum
	int totalSum = 0;
	MPI_Reduce(&sum, &totalSum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	// free up memory once used
	delete partitionArr;
}

int main(int argc, char** argv) {

	// initialise MPI
	MPI_Init(NULL, NULL);

	// establish how many processes are in MPI instance
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	// find out rank
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	// if rank 0 then coordinator, otherwise participant
	if (world_rank == 0)
		coordinator(world_size);
	else
		participant(world_rank);

	// a.	Distinctly synchronizes the node’s activities. 
	MPI_Barrier(MPI_COMM_WORLD);

	// finalise MPI to shut down
	MPI_Finalize();
	return 0;
}