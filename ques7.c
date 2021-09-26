#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

/* 
	process 0 assigned with the task of checking if all the processes have invoked 
	the barrier call or not. Once every process has invoked the barrier call process 0 
	will send out signal for every process to continue out of the barrier ca;;
*/
void iittp_barrier(int rank,int size){

	// process 1 to np-1 will tell process 0 that they have invoked barrier and wait for the signal from process 0
	if(rank!=0){

		MPI_Send(&rank,1,MPI_INT,0,1,MPI_COMM_WORLD); // every process sends its rank to signal its arrival
		int signal;
		MPI_Bcast(&signal,1,MPI_INT,0,MPI_COMM_WORLD);
	}

	else{
		
		int pass_condition = ((size-1)*(size))/2; // sum of all the ranks from 1 to np-1
		int sum=0;
		while(1){
			int temp;
			MPI_Recv(&temp,1,MPI_INT,MPI_ANY_SOURCE,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			sum+=temp; //accumulate signal from all the processes

			if(sum==pass_condition){ // when all the processes have invoked barrier
				
				//printf("all processes have arrived, now releasing all\n");
				break;
			}
		}

		int signal=1;
		MPI_Bcast(&signal,1,MPI_INT,0,MPI_COMM_WORLD); // sending release signal
	}
		
}


int main(int argc,char ** argv){

	int rank,size;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);

	
	iittp_barrier(rank,size);

	//printf("process %d , out of barrier\n",rank);
	
	MPI_Finalize();
	return 0;
}




