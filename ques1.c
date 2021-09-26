#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

// np is no. of processes

int main(int argc,char ** argv){

	int rank,size;
	int n;
	int my_sum=0,recv_sum,send_sum=0;


	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);

	srand(time(0)*(rank+1));
	
	if(rank==0){ // process 0 inputs array size and broafcasts it to every other process
		
		scanf("%d",&n);

		MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
	}

	else{
		MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
	}

	MPI_Barrier(MPI_COMM_WORLD); // synchronize broadcast

	//all processes have array size now

	int chunk_size= n/size;

	int extra = n%size;


	if(rank!=0){ // flow for process 1 to np-1
		int arr[chunk_size];
		
		for(int i=0;i<chunk_size;i++){ //populate array and calculate sum
			arr[i]=rand()%10;
			my_sum+=arr[i];
		}

		if(rank>1){ // process i will recieve sum from process i-1, for i = 2 to np-1
			
			MPI_Recv(&recv_sum,1,MPI_INT,rank-1,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			send_sum=my_sum+recv_sum;

		}
		else{ // process 1 wil not recieve a sum from any process
			send_sum=my_sum;
		}

		//process i will send sum to process i+1, for i= 1 to np-1

		MPI_Send(&send_sum,1,MPI_INT,(rank+1)%size,2,MPI_COMM_WORLD);
	}

	else{
		int my_sum=0,recv_sum;
		int arr[chunk_size+extra];
		

		for(int i=0;i<chunk_size+extra;i++){
			arr[i]=rand()%10;
			my_sum+=arr[i];
		}

		//process 0 will recieve sum from process np-1 and calculate final sum
		
		MPI_Recv(&recv_sum,1,MPI_INT,size-1,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		
		my_sum+=recv_sum;

		printf("sum :%d\n",my_sum);
		
	}
	MPI_Finalize();
	return 0;
}