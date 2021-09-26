#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int main(int argc,char ** argv){

	int rank,size;
	int n;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);

	srand(time(0)*(rank+1));
	// process 0 inputs array size and broafcasts it to every other process
	if(rank==0){ 
		
		scanf("%d",&n);

		MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
	}

	else{
		MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
	}

	MPI_Barrier(MPI_COMM_WORLD); // broadcast synchronization

	int my_arr[n],recv_arr[n];

	//generate array of ith process
	for(int i=0;i<n;i++){
		my_arr[i]=rand()%10;

	}

	
	
	//send array to next process
	MPI_Send(&my_arr,n,MPI_INT,(rank+1)%size,2,MPI_COMM_WORLD);
	
	
	/* uncomment to check the contents of array of a process before send happens
	printf("array of: %d before sending\n",rank);
	for(int i=0;i<n;i++){
		printf("%d  ",my_arr[i]);
	}
	printf("\n");

	MPI_Barrier(MPI_COMM_WORLD);
	*/

	//recieve from previous process in the chain
	if(rank>0){
		MPI_Recv(&recv_arr,n,MPI_INT,rank-1,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	}
		
	else{
		MPI_Recv(&recv_arr,n,MPI_INT,size-1,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			
	}
	
	// print what was recieved
	for(int i=0;i<n;i++){
		printf("%d  ",recv_arr[i]);
	}

	printf("\n");
	MPI_Finalize();
	return 0;
}