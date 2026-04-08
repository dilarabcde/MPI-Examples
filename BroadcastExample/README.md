# Broadcast Example

This example demonstrates the use of `MPI_Bcast`.

Process 0 first asks for the rank of the process that will broadcast its own array. Then the selected process broadcasts its array to all other processes in `MPI_COMM_WORLD`.

