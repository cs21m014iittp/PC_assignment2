#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

/*
	np is no. of processes
	inputs to be given for various distribution techniques:
		1. block distribution --> block
		2. cyclic distribution --> cyclic
		3. block-cyclic distribution --> block-cyclic
*/ 

int main(int argc,char ** argv){

	int rank,size;
	int n;
	int my_sum=0;
	int mode;
	int input[2];

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);

	// process 0 will take size of array and distribution type as input and broadcast to all the other processes
	if(rank==0){
		
		scanf("%d *[^\n]",&n);

		char temp_mode[15];
		scanf("%s",temp_mode);

		puts(temp_mode);

		if(strcmp(temp_mode,"block")==0)
			mode=0;
		else if (strcmp(temp_mode,"cyclic")==0)
			mode=1;
		else
			mode=2;

		input[0]=n;
		input[1]=mode;
		MPI_Bcast(&input,2,MPI_INT,0,MPI_COMM_WORLD);
	}

	else{
		MPI_Bcast(&input,2,MPI_INT,0,MPI_COMM_WORLD);
	}
	
	MPI_Barrier(MPI_COMM_WORLD);

	// once every process has recieved the array size and distribution type they can now do their computations
	
	n=input[0];
	mode=input[1];

	if(mode==0){	// block distribution

		// setting block size to ceil(n/np) for load balancing
		// block size can be set to a number b such that n < np*b

		int block_size= ((n%size)==0)?(n/size):(n/size+1); 
		
		int arr_size=n-rank*block_size;


		if(arr_size>=block_size)
			arr_size=block_size;

		if(arr_size<0) //sometimes some processes will have empty arrays
			arr_size=0;

		int arr[arr_size]; 
		

		for(int i=0;i<arr_size;i++){ //populating each array
			arr[i]= block_size*rank+i;
			my_sum+=arr[i];
		}

		printf("array of process %d:  ",rank);

		for(int k=0;k<arr_size;k++){
			printf("%d ",arr[k]);
		}

		printf("\n");
		
		if(rank<size-1){
			MPI_Send(&my_sum,1,MPI_INT,size-1,0,MPI_COMM_WORLD); //send sum to process np-1
		}	

		else{ 
			//if  process np-1 then collect sum from all the other processes
			for(int i=0;i<size-1;i++){
				int recv_sum;
				MPI_Recv(&recv_sum,1,MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
				my_sum+=recv_sum;

			}
				
			printf("final sum :%d\n",my_sum);

		}
	}

	else if(mode==1){ // cyclic distribution 

		int chunk_size= n/size; //every process will have array with atleast this many elements
		int extra = n%size; 
		int arr_size;
		if(extra&&rank<extra){ // processes 0 to extra-1 will have 1 extra element if there are any extra elements
			arr_size=chunk_size+1;
		}

		else{
			arr_size=chunk_size;
		}
		
		int arr[arr_size]; // processes no. extra to np-1 will not have any extra elements
		
		for(int i=0;i<arr_size;i++){
			arr[i]= rank+i*size;
			my_sum+=arr[i];
		}
		
			
		printf("array of process %d:  ",rank);

		for(int k=0;k<arr_size;k++){
			printf("%d ",arr[k]);
		}

		printf("\n");

		if(rank<size-1){ 	
			MPI_Send(&my_sum,1,MPI_INT,size-1,0,MPI_COMM_WORLD); //send sum to nth process
		}	

		else{
		// collect sum from all the other processes
			for(int i=0;i<size-1;i++){
				int recv_sum;
				MPI_Recv(&recv_sum,1,MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
				my_sum+=recv_sum;

			}
				
			printf("final sum :%d\n",my_sum);

		}
	}

	else{  // block-cyclic diatribution

		int block_size =3; //since no block size is mentioned or to be taken as input
		int offset= rank*block_size; //starting value in the array of a process
		int step = size*block_size;  //distance between the values of subsequent blocks in a process

		int n_blocks = n/step; //minimum no. of blocks per process
		int extra = n%step;	   //no. of elements which cannot be divided uniformly among the processes

		int extra_elements = extra-rank*block_size;
		int arr_size;

		if(extra_elements>=block_size){
			arr_size = (n_blocks+1)*block_size;
		}

		else if(extra_elements<block_size&&extra_elements>=0){
			arr_size= n_blocks*block_size+extra_elements;
		}
		
		else
			arr_size= n_blocks*block_size;

		int arr[arr_size];

		int value = offset,count=1;
		int i=0;
		while(i<arr_size){
			int j=0;
			//etbf is elements to be filled in an iteration
			int etbf = (((arr_size-i)/block_size)>0)?block_size:(arr_size-i)%block_size;
			while(j<etbf){
				arr[i]=value;
				my_sum+=arr[i];
				value++;
				j++;
				i++;	
			}
			value=offset+count*step;
			count++;

		}

		printf("array of process %d:  ",rank);

		for(int k=0;k<arr_size;k++){
			printf("%d ",arr[k]);
		}

		printf("\n");

		if(rank<size-1){
			MPI_Send(&my_sum,1,MPI_INT,size-1,2,MPI_COMM_WORLD); //send sum to process np-1
		}


		else{ 
			//if  process np-1 then collect sum from all the other processes
			for(int i=0;i<size-1;i++){
				int recv_sum;
				MPI_Recv(&recv_sum,1,MPI_INT,MPI_ANY_SOURCE,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
				my_sum+=recv_sum;

			}
				
			printf("final sum :%d\n",my_sum);

		}
	}
	
	MPI_Finalize();
	return 0;
}