#include "mpi.h"
#include <iostream>
using namespace std;

#define SIZE 5

int main(int argc, char* argv[])
{
    int myRank,    /* rank of a process */
        size,      /* number of processes in MPI_COMM_WORLD */
        root;      /* process ID for the broadcasting machine */

    int *bufferPtr;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (myRank == 0) // master node
    {
        cout << "Please give the rank number for the process broadcasting its own data: ";
        cin >> root;
        cout << endl;

        if (root < 0 || root >= size)
        {
            cout << "Error: Invalid rank number ID (" << root << ") : aborting !...\n";
            MPI_Abort(MPI_COMM_WORLD, 99);
        }
    }

    MPI_Bcast(&root, 1, MPI_INT, 0, MPI_COMM_WORLD);

    bufferPtr = new int[SIZE];

    for (int i = 0; i < SIZE; i++)
        bufferPtr[i] = myRank * i;

    printf("%d's buffer BEFORE Broadcast is: %d %d %d %d %d\n",
           myRank, bufferPtr[0], bufferPtr[1], bufferPtr[2], bufferPtr[3], bufferPtr[4]);

    if (myRank == root)
        printf("%d BROADCASTS an ARRAY with the size of %d\n", myRank, SIZE);

    MPI_Bcast(bufferPtr, SIZE, MPI_INT, root, MPI_COMM_WORLD);

    printf("%d's buffer AFTER Broadcast is: %d %d %d %d %d\n",
           myRank, bufferPtr[0], bufferPtr[1], bufferPtr[2], bufferPtr[3], bufferPtr[4]);

    delete[] bufferPtr;
    MPI_Finalize();

    return 0;
}
