#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

/*

	writing such a program is not possible because:-

	1.  We can neither predict which processes will be recieving data from some other process
		nor can we guarantee that all the process will be receiving data from some other processes.
		therefore, some of the processes will the stuck waiting to recieve data when no other process 
		is sending them anything.

	2.  We cannot guarantee that within m iterations all the processes will have passed their data to some 
		other process. Therefore we cannot surely print the arrays of all the processes.

	3.  One particular process may recieve data one or more than one number of times. We cant say how many times
		a particular process should both recieve and send data, i.e , the no of sends and recieves are undertermined.

	4.	we cannot write a single generalized code containing a generalized number of sends and recieve for all 
		the processes because every process will be sending and recieving for different number of times.
*/

int main(int argc,char ** argv){

	int rank,size;
	int n,m,x;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);

	srand(time(0)*(rank+1));

	if(rank==0){
		
		scanf("%d",&n);
		scanf("%d",&m);
		MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
	}

	else{
		MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
	}

	MPI_Barrier(MPI_COMM_WORLD);


	int my_arr[n],recv_arr[n];

	//generate array of ith process
	for(int i=0;i<n;i++){
		my_arr[i]=rand()%100;

	}

	if(rank==0){
		if(m){
			while(1){
				 x = rand()%size;
				if(x!=0)
				break;	
			}

			MPI_Send(&my_arr,n,MPI_INT,x,2,MPI_COMM_WORLD);
			m--;
			MPI_Send(&m,1,MPI_INT,x,3,MPI_COMM_WORLD);
				
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
	
	else{
		MPI_Barrier(MPI_COMM_WORLD);
	}
	
	// now process 0 has initiated the process


	//first recieve both the number of iterations left and data
	MPI_Recv(&m,1,MPI_INT,MPI_ANY_SOURCE,3,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	MPI_Recv(&recv_arr,n,MPI_INT,MPI_ANY_SOURCE,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	//if no process is sending to the current process then it will be stuck here forever

	// if the current process recieved data and there are still iterations to happen then it will send data forward.
	if(m){
		while(1){
			x = rand()%size;
			if(x!=rank)
				break;	
		}

		
		MPI_Send(&my_arr,n,MPI_INT,x,2,MPI_COMM_WORLD); // send its own data
		m--;
		MPI_Send(&m,1,MPI_INT,x,3,MPI_COMM_WORLD); // send no. of iterations remaining
			
	}
	/*
		how many times do we run the above if block is undetermined and even different for different processes.
	*/

	// print what was recieved
	for(int i=0;i<n;i++){
		printf("%d  ",recv_arr[i]);
	}

	printf("\n");
	MPI_Finalize();
	return 0;
}