// Sarah_Brennan_2962279_Ass2_Part2.cpp

/*
	Part 2 (60%)

	You are to create an MPI program containing coordinator and participant nodes that will calculate the average of the medians from a subset of numbers.

	The program should contain a median() method that will find the median value of a subset, irrespective of the size of that subset. This should be done by the slave nodes.

	These values will then be gathered by the co-ordinator (either individually or in one operation).

	The coordinator and participant methods should do the following:
	•	Generate the array of numbers (coordinator only). For predictable results seed the random number generator with the value of 1 and limit their maximum value to 50.
	•	Determine the size of each partition (coordinator only). Broadcast this to all nodes.
	•	Scatter the partitions to each node.
	•	Calculate the median for this node.
	•	Use a reduce operation to gather the medians.
	•	Compute the overall average (coordinator only).

	Modify your code to work with any world size and accept a dataset size from the command line. You may assume that the dataset size will be evenly divisible by the world size.

	•	Perform a comparison of four nodes against a single node on datasets of different sizes.
	•	Do some rudimentary examination of the performance of the program, in it’s most basic form and every subsequent iteration of development.
	•	Also, while keeping things simple, there will be distinct marks for any work above and beyond the remit, but be reasonable!


*/

#include <iostream>
#include <mpi.h>
#include <chrono>
#include <algorithm> 

using namespace std;

// get time when program started, referenced from https://www.pluralsight.com/blog/software-development/how-to-measure-execution-time-intervals-in-c--
auto startTimer = chrono::high_resolution_clock::now();

/**
 * prints out an array to console in a single line
 * @medianArray -> pointed to array
 * @medianArraySize -> size of array
 */
void printArray(int* medianArray, int medianArraySize) {
	// sorted array when printing so easier to see which is median number in list
	sort(medianArray, medianArray + medianArraySize);
	for (unsigned int i = 0; i < medianArraySize; i++) {
		if (i == (medianArraySize - 1))
			cout << medianArray[i] << endl;
		else
			cout << medianArray[i] << ", ";
	}
}

/**
* returns median of array
* @medianArray -> reference to array
* @medianArraySize -> size of array
* @return median of array
*/
double median(int medianArray[], int medianArraySize) {
	// sort array first
	double med;
	sort(medianArray, medianArray + medianArraySize);
	if (medianArraySize % 2 != 0)
		return (double)medianArray[medianArraySize / 2];
	med = (double)(static_cast<__int64>(medianArray[(medianArraySize - 1) / 2]) + static_cast<__int64>(medianArray[medianArraySize / 2])) / 2.0;
	return med;
}

/**
* Generates array of numbers, determines size of participants and broadcasts to nodes,
* calculates median for each node and then prints the median
* @worldSize -> size of world
*/
void coordinator(int worldSize) {

	int* medianArray = NULL;
	int arraySize;

	// get size of array from user
	cout << "Please enter the array size evenly divisible by world size: ";
	cin >> arraySize;
	// print my name and student number to console
	cout << "Student Name: Sarah Brennan" << endl;
	cout << "Student Number: 2962279" << endl;

	// set array to size given by user
	medianArray = new int[arraySize];

	for (unsigned int i = 0; i < arraySize; i++)
		medianArray[i] = (rand() % 50) + 1;

	// get time when program started
	startTimer = chrono::high_resolution_clock::now();

	// determine size of each partition and broadcast to all nodes
	// this is done by dividing size of array by world size
	int partitionSize = arraySize / worldSize;

	// broadcast the partiton size to each node so memory allocated correctly
	MPI_Bcast(&partitionSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

	// send out the partition to each node
	for (unsigned int i = 1; i < worldSize; i++) {
		MPI_Send(medianArray + partitionSize * i, partitionSize, MPI_INT, i, 0, MPI_COMM_WORLD);
	}

	// calculate median for each node
	double med = median(medianArray, partitionSize);

	// number of elements in send buffer
	int count = 0;

	// print out array of numbers
	// printArray(medianArray, arraySize);

	// use reduce to gather the overall median
	MPI_Reduce(&med, &count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	cout << "The median of the numbers in array is " << med << endl;

	// get time program ended
	auto endTimer = chrono::high_resolution_clock::now();
	chrono::duration<double> elapsed = endTimer - startTimer;
	// prints out how long it took the program to run from splitting array into partitions
	//and getting median
	cout << "Elapsed time: " << elapsed.count() << endl;
}

/**
* gets the median of numbers within each node other than rank 0
* @worldRank -> gets node value
*/
void participant(int worldRank) {

	// get partition size and allocate memory
	int partitionSize = 0;
	MPI_Bcast(&partitionSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

	int* partitionArr = new int[partitionSize];

	// receive the partition
	MPI_Recv(partitionArr, partitionSize, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	// calculate median of primes for each node
	double med = median(partitionArr, partitionSize);

	// number of elements in send buffer
	int count = 0;

	// use reduce to gather the overall median
	MPI_Reduce(&med, &count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

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

	// finalise MPI to shut down
	MPI_Finalize();
	return 0;
}