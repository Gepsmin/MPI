# MPI

The main purpose of the project is being able to handle multiple processes using MPI programming. The goal is finding best attributes which will be used making decisions afterwards. Because of the probability of having too many attributes and instances, the probable run time of the program may be unfeasible while some processers may be idle in the meantime. So, we should use Master-Slave process relationship to overcome the handicap. With the multi process structure, Master process can handle the I/O operations to pull the inputs from the file and distribute the data to the slave processes using MPI. And all processes continue to calculate the weights of the attributes simultaneously. The processes will produce outputs and send the results to the master process again. The master process will also give output according to the information it gathered.

### Program Execution

&nbsp;&nbsp;&nbsp;To compile:
```
mpic++ -o cmpe300_mpi_2017400222 ./ cmpe300_mpi_2017400222.cpp 
```

&nbsp;&nbsp;&nbsp;To run:
<P> is the Process count.  
<inputfile> is the path of the input file. 
```
mpirun –oversubscribe -np <P> cmpe300_mpi_2017400222 <inputfile>
```

&nbsp;&nbsp;&nbsp;Used libraries:  
iostream: to be able to do console operations   
fstream: to read the input file    
string: to convert the result array to one line string to be printed    
limits: to obtain the maximum double value    
open mpi (4.0.3)(mpi.h): to achieve message passing mechanism between processes    

&nbsp;&nbsp;&nbsp;Compiler:   
gcc (9.3.0): to compile the code   

### Program Structure

The MPI library allows a programmer to accomplish message passing system between processes created inside the same program. Without fork operations, the library creates a workspace with the given number of processes. It can also be used to pull the information about the ranks of the processes and the size of the workspace. It offers API to send and receive messages between processes via the MPI_Send and MPI_Recv functions. So, the centerpiece of this program is the MPI library.
In this project, I did not see any necessity to use functions from the MPI library other than the stated ones. After the initialization, main function reads the global parameters from the input file and creates necessary arrays to hold the data and modify the result data. Master starts to read the file and stores the extracted data in the 2D array. This 2D array is the main data storage structure and the Master distributes the information to the Slaves from this array via MPI_Send function. Also, the Master distributes the first 5 arguments from the input. As we can observe, the main goal of the Master process is reading the data and distributing it to the slaves. After Slaves come up with the result values, the Master gathers the all the information and prints the result to the user. 
The Slaves receive the allocated data fragments using MPI_Recv. Then, they work individually in the different parts of the data. A Slave picks an instance and finds the appropriate instances regarding to the classes and the distances of the other instances. With this way, we can calculate the value or in other words the weight of the attributes. After the stated number of iterations, the slave finds a result. Every slave prints their results to the console. Then, again using MPI, they send the information to the Master.

### Difficulties Encountered

Understanding how to use the 2D arrays in a message passing system was difficult in the first place. It requires some back knowledge about the storage mechanism of the arrays in the main memory. After understanding the usage of the buffer system and the possible utilities of the message passing system, the project become a job that requires only the implementation of the pseudocode. Yet, at the completion stage, I also encountered with another problem. Master should have waited the other processes before printing its result. I realized that I could implement the program in such a way that Master receives the results in the end of the code. So, while waiting the all the results, it also waits the printing operations of the slaves. Because the slaves send the messages after printing their results, I handled this problem also.

### Conclusion

The utilization of a solution may be achieved via multi processes. Yet, if we want to use processes that works on same problem with the same data at the same time, we need to set a communication line between the processes and must have a Master process to handle the main organization of the problem. To achieve this, message passing is handy tool. And the MPI library also eases the programmer burden greatly. Main goal becomes clearing out the needed communications between the processes.  