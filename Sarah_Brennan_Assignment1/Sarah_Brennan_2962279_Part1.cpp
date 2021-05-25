// Sarah_Brennan_2962279_Part1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

/*

Part 1 (50%)

Two hash methods required for completion of P1 of Assignment1.

You are tasked with creating a sequential ring of 4 nodes.

The initial node (rank 0) should use values 422 and 233 and pass the difference of the two hashes to the next node.

Like so:

HashInput1 = worldsWorstHash1(422) - worldsWorstHash2(233)
HashInput2 = worldsWorstHash1(HashInput1) - worldsWorstHash2(HashInput1) HashInput3 = worldsWorstHash1(HashInput2) - worldsWorstHash2(HashInput2)

Print the final two hash values worldsWorstHash1(HashInput3) and worldsWorstHash2(HashInput3) to the console.

Note: Any time a message is sent, output the message to the console.


Expected output:

Rank 0 sending: -108
Rank 1 sending: -238
Rank 2 sending: -140

Result: hash1=-66, hash2=58

*/

#include<iostream>
#include<cstdlib>
#include<mpi.h>

// initialize size and rank of world
int world_size;
int world_rank;

// create hash input 1, 2 and 3
int hashInput;

int worldsWorstworldsWorstHash1(int ip) {
    for (int l = 1; l < 100; l++) {
        ip = ip * l % 254;
    }
    return ip;
}

int worldsWorstworldsWorstHash2(int ip) {
    for (int l = 1; l < 50; l++) {
        ip = ((ip * l) + 2) % 254;
    }
    return ip;
}

int main(int argc, char** argv) {

	// initialise MPI
	MPI_Init(NULL, NULL);

	// establish how many processes are in MPI instance
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	// find out rank
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	// node zero should be given the job of setting initial values
	if (world_rank == 0) {
		// print my name and student number to console
		std::cout << "Student Name: Sarah Brennan" << std::endl;
		std::cout << "Student Number: 2962279" << std::endl;

		// place first rank values in hashIn
		hashInput = worldsWorstworldsWorstHash1(422) - worldsWorstworldsWorstHash2(233);
		// send result of hashInput1 to hashInput2
		MPI_Send(&hashInput, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		// print out number rank 0 sending
		std::cout << "Rank " << world_rank << " sending: " << hashInput << std::endl;
	}	
	//node 3 is then given results from node 2
	else if (world_rank == 3) {
		// received info from node 2
		MPI_Recv(&hashInput, world_rank, MPI_INT, (world_rank - 1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		// print the final two hash values worldsWorstHash1(HashInput3) and worldsWorstHash2(HashInput3) to the console.
		std::cout << "Result: hash1 = " << worldsWorstworldsWorstHash1(hashInput) << ", hash2 = " << worldsWorstworldsWorstHash2(hashInput) << std::endl;
	}
	//node 2 is then given results from node 1
	else {
		// received info from previous node sent
		MPI_Recv(&hashInput, world_rank, MPI_INT, (world_rank - 1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		// place next rank values in hashInput
		hashInput = worldsWorstworldsWorstHash1(hashInput) - worldsWorstworldsWorstHash2(hashInput);
		// send hashInput to next rank
		MPI_Send(&hashInput, world_rank, MPI_INT, (world_rank + 1), 0, MPI_COMM_WORLD);
		std::cout << "Rank " << world_rank << " sending: " << hashInput << std::endl;
	}
		
	// finalise MPI to shut down
	MPI_Finalize();

	return 0;
}