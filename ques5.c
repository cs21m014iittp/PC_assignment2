#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

// np is no. of processes
int main(int argc,char ** argv){

	int rank,size;
	int n;
	int my_sum=0;

	srand(time(0)*(rank+1));
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);

	// process 0 will take size of array as input and broadcast to all the other processes
	if(rank==0){
		
		scanf("%d",&n);

		for(int i=1;i<size;i++)
		MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
	}

	else{
		MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	// once every process has recieved the array size they can now do their computations
	int chunk_size= n/size;

	int extra = n%size;


	if(rank<size-1){ // flow for process 0 - np-2
		int arr[chunk_size]; // array of size only chunk size in each process
		
		for(int i=0;i<chunk_size;i++){
			arr[i]= rand()%50;
			my_sum+=arr[i]*arr[i];
		}

		
		MPI_Send(&my_sum,1,MPI_INT,size-1,2,MPI_COMM_WORLD); //send sum to process np-1
	}

	else{ // flow for process np-1
		int my_sum=0;
		int arr[chunk_size+extra];
		

		for(int i=0;i<chunk_size+extra;i++){
			arr[i]= rand()%50;
			my_sum+=arr[i]*arr[i];
		}

		// collect sum from all the other processes
		for(int i=0;i<size-1;i++){
			int recv_sum;
			MPI_Recv(&recv_sum,1,MPI_INT,MPI_ANY_SOURCE,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			my_sum+=recv_sum;

		}
			
		printf("final sum :%d\n",my_sum);
		
	}
	MPI_Finalize();
	return 0;
}