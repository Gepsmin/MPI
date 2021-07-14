/*
 * Student Name: Onur Sefa Özçıbık
 * Student Number: 2017400222
 * Compile Status: Compiling
 * Program Status: Working
 * To Compile: mpic++ -o cmpe300_mpi_2017400222 ./cmpe300_mpi_2017400222.cpp
 * To Run: mpirun --oversubscribe -np <P> cmpe300_mpi_2017400222 <inputfile>
 */

#include <iostream>
#include <mpi.h>
#include <fstream>
#include <string>
#include <limits>


// max double value to find the overall minimum values later on
double maximum = std::numeric_limits<double>::max();
using namespace std;

// Manhattan Distance algorithm which will be used to find the nearest hit and
// nearest miss instances
double manDist(double arr1[], double arr2[], int feature){
    double total= 0;

    for(int i=0; i<feature; i++){
        total += abs(arr1[i]-arr2[i]);
    }

    return total;
}

// Helper function to find the location of the maximum value in an array
int findMax(double arr[], int feature){
    double high = arr[0];
    int index = 0;

    for(int i=0; i<feature; i++){
        if(arr[i]>high){
            high = arr[i];
            index = i;
        }
    }

    return index;
}

// Helper function of Merge Sort algorithm
// It takes two sorted partition of an array and gathers this two part in a sorted fashion
// then it write backs to the original array interval
void merge(int arr[], int left, int middle, int right){
    int left_head = left;
    int right_head = middle+1;

    // Merging the two parts of an array in a temporary array in a sorted way
    double temp[right-left+1];
    for(int i =0; i <right-left+1; i++){
        if(left_head<= middle && right_head<=right){
            if(arr[left_head] < arr[right_head]){
                temp[i] = arr[left_head];
                left_head ++;
            }else{
                temp[i] = arr[right_head];
                right_head ++;
            }
        }else if(left_head <= middle){
            temp[i] = arr[left_head];
            left_head ++;
        }else{
            temp[i] = arr[right_head];
            right_head ++;
        }
    }

    // Copies data from the temporary array to the original one
    for(int i =0; i<right-left+1; i++){
        arr[left+i] = temp[i];
    }

}

// Merge Sort algorithm to handle sort operation
// It takes an array and it divides into two part then sorts them recursively
void merge_sort(int arr[], int left, int right){
    if(left >= right) return;

    int middle = (left+right)/2;

    // Left part will be sorted
    merge_sort(arr, left, middle);
    // Right part will be sorted
    merge_sort(arr, middle+1, right);
    // Sorted left and right parts will be merged into one sorted array
    merge(arr, left, middle, right);
}

// The main function
// If it is used by the Master Process:
//      It receives the arguments, reads files and sends the needed information to the slave
//      process. Then waits slaves to receive best attributes. After that it sorts the given indexes and
//      prints the found answer.
// If it is used by Slave Processes:
//      It receives instances from master, modifies the weight array with respect to the relief algorithm.
//      Then finds the best attributes, and prints the attribute indexes in a sorted way.
int main(int argc, char *argv[])
{
    // File stream to read the input file
    ifstream file(argv[1]);
    // Initialization of MPI
    MPI_Init(NULL, NULL);
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Reading the first 5 input
    int P;
    int N;
    int A;
    int M;
    int T;
    file >> P >> N >> A >> M >> T;

    // To send the first parameters to the slaves
    int will_be_send[5] = {P, N, A, M, T};
    int will_be_received[5];

    // Data allocation will be maintained via these arras and variables
    int multiplier = N/(size-1);
    double arr[N][A+1];
    int result_size = T*(P-1);
    int result[result_size];

    // If Master Process
    if(rank==0){
        // Reads and stores the data from input file
        for(int i=0; i <N; i++){
            for(int j=0; j <=A; j++){
                file >> arr[i][j] ;
            }
        }

        // Sends all the slaves the first 5 input parameters
        for(int i=0; i< size-1; i++){
            MPI_Send(&will_be_send,
                     5,
                     MPI_INT,
                     i+1,
                     2,
                     MPI_COMM_WORLD);
        }

        // Sends the necessary parts of the information to the aimed Slave one by one
        for(int i=0; i <size-1; i++){
            MPI_Send(&arr[i*multiplier][0],
                     multiplier*(A+1),
                     MPI_DOUBLE,
                     i+1,
                     0,
                     MPI_COMM_WORLD);
        }


    }else{
    // If Slave Process

        // Receives the first 5 parameters which are P, N, A, M, and T
        MPI_Recv( &will_be_received,
                  5,
                  MPI_INT,
                  0,
                  2,
                  MPI_COMM_WORLD,
                  MPI_STATUS_IGNORE);

        P = will_be_received[0];
        N = will_be_received[1];
        A = will_be_received[2];
        M = will_be_received[3];
        T = will_be_received[4];

        // Receives the appropriate part of the data which send by the master
        MPI_Recv( &arr,
                 multiplier*(A+1),
                 MPI_DOUBLE,
                 0,
                 0,
                 MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);


        // Creates the weight array and initializes with the zeros.
        // Creates the max and min arrays for finding the maximum and minimum values of the attributes
        double W[A];
        double maxes[A];
        double mines[A];
        int talented[T];
        for(int i=0; i<A; i++){
            W[i] = 0;
            maxes[i] = 0;
            mines[i] = maximum;
        }

        // Searches the given instances and finds the maximum and minimum values of the attributes
        for(int i=0; i<multiplier; i++){
            for(int j=0; j<A; j++){
                if(arr[i][j]> maxes[j]){
                    maxes[j]= arr[i][j];
                }
                if(arr[i][j]< mines[j]){
                    mines[j]= arr[i][j];
                }
            }
        }

        // To update the weights there will be M iterations
        for(int i =0; i< M; i++){
            double nearestHit = maximum;
            double nearestMiss = maximum;
            double* hitTarget;
            double* missTarget;
            // All instances will be traversed and the nearest hit and nearest miss instances will be found
            // using the manhattan distance algorithm
            for(int j =0; j<multiplier; j++){
                if(j==i){
                    continue;
                }
                double distance = manDist(arr[i], arr[j], A);

                bool Hit = arr[i][A]==arr[j][A];
                if(Hit){
                    if(distance < nearestHit){
                        nearestHit = distance;
                        hitTarget = arr[j];
                    }
                }else{
                    if(distance < nearestMiss){
                        nearestMiss = distance;
                        missTarget = arr[j];
                    }
                }
            }


            // Weight array will be updated for all attributes with respect to the nearest hit, nearest miss instances
            // and the maximum, minimum values of the attributes
            for(int j=0; j<A; j++){
                double divider =  (maxes[j] - mines[j]) * M;
                double diff1 = abs(hitTarget[j]-arr[i][j]);
                double diff2 = abs(missTarget[j]-arr[i][j]);
                W[j] = W[j] - (diff1/divider) + (diff2/divider);
            }
        }

        // T many attribute will be chosen from the weight array
        for(int i=0; i<T; i++){
            talented[i] = findMax(W, A);
            W[talented[i]] = -1*maximum;
        }

        // Attribute indexes will be sorted to be printed later on
        merge_sort(talented, 0, T-1);

        // Creating the output string regarding to the calculated results
        string s = "Slave P" + to_string(rank) + " : ";
        for(int i=0; i<T; i++){
            s = s + to_string(talented[i]) + " ";
        }
        // Printing the result to the console
        cout << s << endl;

        // Sending the found indexes to the master process to unite the all results
        MPI_Send(&talented,
                 T,
                 MPI_INT,
                 0,
                 1,
                 MPI_COMM_WORLD);

    }

    // If master process
    if(rank==0){
        // Gathers the all found results in single result array
        for(int i=0; i <size-1; i++){
            MPI_Recv(&result[i*T],
                     T,
                     MPI_INT,
                     i+1,
                     1,
                     MPI_COMM_WORLD,
                     MPI_STATUSES_IGNORE);
        }

        // Sorts the final result array and prints the indexes in order in such a way that multiple occurrences of
        // indexes will be printed only one times.
        merge_sort(result, 0, result_size-1);
        string s = "Master P0 : ";
        int last_value = -1;
        for(int i=0; i<result_size; i++){
            int value = result[i];
            if(value != last_value){
                last_value = value;
                s = s + to_string(result[i]) + " ";
            }
        }
        cout << s << endl;
    }

    // Finalizes the MPI for all processes
    MPI_Finalize();
}