#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size < 2) {
        MPI_Finalize();
        return 0;
    }

    if (rank == 0) {
        cout << "=== Умова 1 ===" << endl;
    }

    double t1_received = 0.0;

    if (rank == 0) {
        vector<double> src_data(size - 1);
        for (int i = 0; i < size - 1; ++i) {
            src_data[i] = 10.5 + i * 2.5;
        }
        for (int i = 1; i < size; ++i) {
            MPI_Send(&src_data[i - 1], 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
        }
    }
    else {
        MPI_Recv(&t1_received, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    for (int p = 1; p < size; ++p) {
        MPI_Barrier(MPI_COMM_WORLD);
        if (p == rank) {
            cout << "Process " << rank << " received number: " << t1_received << endl;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        cout << "\n=== Умова 2 ===" << endl;
    }
    MPI_Barrier(MPI_COMM_WORLD);

    double my_val = rank * 1.5 + 2.0;
    double t2_master_val = 0.0;
    vector<double> collected_vals(size, 0.0);

    if (rank == 0) {
        for (int i = 1; i < size; ++i) {
            MPI_Send(&my_val, 1, MPI_DOUBLE, i, 2, MPI_COMM_WORLD);
        }
        collected_vals[0] = my_val;
        for (int i = 1; i < size; ++i) {
            MPI_Recv(&collected_vals[i], 1, MPI_DOUBLE, i, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
    else {
        MPI_Send(&my_val, 1, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD);
        MPI_Recv(&t2_master_val, 1, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    if (rank == 0) {
        cout << "Master process (rank 0) received values in ascending order:" << endl;
        for (int i = 1; i < size; ++i) {
            cout << "From process " << i << ": " << collected_vals[i] << endl;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    for (int p = 1; p < size; ++p) {
        MPI_Barrier(MPI_COMM_WORLD);
        if (p == rank) {
            cout << "Process " << rank << " received master value: " << t2_master_val << " (own value: " << my_val << ")" << endl;
        }
    }

    MPI_Finalize();
    return 0;
}