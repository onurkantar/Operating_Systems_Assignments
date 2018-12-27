//necessary includes
#include <aio.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <aio.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <time.h>

//global source and destination initalition
char source[2048];
char destination[2048];

struct threadInfo{//struct for sending data to thread

int i;
int file_size;
int number_of_threads;

};

void * multithread(void * info){//thread function


struct aiocb aio_r,aio_w;//struct for multithread read and write


struct threadInfo ti ;//struct for getting struct from parameter to process info
ti = *((struct threadInfo *)info);
int offset;
int buffer_size;
if(ti.i == ti.number_of_threads-1){//deciding offset and buffer size for last thread(important for odd bytes)

offset = (ti.file_size/ti.number_of_threads)*(ti.number_of_threads-1);
buffer_size = ti.file_size - offset;

}



else{

offset = ti.i*(ti.file_size/ti.number_of_threads);
buffer_size = ti.file_size/ti.number_of_threads;

}





//defining buffer for reading and writing
char buffer[buffer_size];
int ret,err;
printf("thread i : %d offset : %d buffer size : %d\n\n",ti.i,offset,buffer_size);
memset(&buffer, 0, sizeof(buffer));
memset(&aio_r, 0, sizeof(aio_r)); // memset() is used to fill a block of memory with a particular value.
int read = open(source,O_RDONLY);//opening the source file to read

//READING PART
aio_r.aio_fildes = read;
aio_r.aio_buf = buffer;
aio_r.aio_nbytes = sizeof(buffer);
aio_r.aio_offset = offset;
aio_read(&aio_r);

while (aio_error(&aio_r) == EINPROGRESS) { }
    err = aio_error(&aio_r);
    ret = aio_return(&aio_r);

//ERROR HANDLING
    if (err != 0) {
        printf ("Error at aio_error() : %s\n", strerror (err));
        close (read);
        exit(2);
    }

    if (ret != buffer_size) {
        printf("Error at aio_return()\n");
        close(read);
        exit(2);
    }

close(read);
//INFORMATIVE PART
fprintf(stdout, "read : %s from thread : %d\n\n", buffer,ti.i);


	
memset(&aio_w, 0, sizeof(aio_w));
//opening or creating(if the file does not exist in the path) for writing
int write = open(destination, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

//WRITING PART
aio_w.aio_fildes = write;
aio_w.aio_buf = buffer;
aio_w.aio_nbytes = sizeof(buffer);
aio_w.aio_offset = offset;
aio_write(&aio_w);

        while (aio_error(&aio_w) == EINPROGRESS) { }
        err = aio_error(&aio_w);
        ret = aio_return(&aio_w);
//ERROR HANDLING PART
        if (err != 0) {
	    printf("write error\n");
            printf ("Error at aio_error() : %s\n", strerror (err));
            close (write);
            exit(2);
        }

        if (ret != sizeof(buffer)) {
            printf("Error at aio_return()\n");
            close(write);
            exit(2);
        }


close(write);
//INFORMATIVE PART
fprintf(stdout, "write : %s from thread : %d\n\n", buffer,ti.i);

}

int main(int argc , char *args[]){//PASSIGN ARGUMENTS FROM TERMINAL

char currentworkingdirectory[2048];

if(argc > 4){//if wrong terminal command

printf("ARGUMENT ERROR\n");

return;
}
//deciding source and destination path
if(strcmp(args[1],"-") == 0){

	
getcwd(currentworkingdirectory, sizeof(currentworkingdirectory));

strcat(currentworkingdirectory,"/randomfile.txt");

strcpy(source,currentworkingdirectory);

}

else{

strcpy(source,args[1]);
strcat(source,"/randomfile.txt");
printf("source : %s\n",source);
}

if(strcmp(args[2],"-") == 0){

getcwd(currentworkingdirectory, sizeof(currentworkingdirectory));

strcat(currentworkingdirectory,"/randomfile_new.txt");

strcpy(destination,currentworkingdirectory);

}

else{

strcpy(destination,args[2]);
strcat(destination,"/randomfile_new.txt");
printf("destination : %s\n",destination);

}

char random_text[] = "ilkim cok guzel ve onu cok seviyorum<3<3<3<3";
//DECIDING RANDOM TEXT
int random;
char text_random[500];
srand(time(NULL));
random = rand()%strlen(random_text);
int j;
for(j = 0;j<500;j++){

text_random[j] = random_text[random];
random=rand()%strlen(random_text);

}

int total_size;

FILE * fp = fopen(source,"w");
fprintf(fp,text_random);

fseek(fp,0,2);

total_size = ftell(fp);//getting total size of file

printf("total size : %d\n",total_size);

int not;

not = atoi(args[3]);//DECIDING NUMBER OF THREADS FROM TERMINAL ARGUMENT

pthread_t threads[not];

double bytePerThread = (double) total_size / not;

printf("byte per thread : %d\n",(int)bytePerThread);

printf("number of threads : %d\n",not);


int i;

int dummy;

struct threadInfo * ti = (struct threadInfo *)malloc(sizeof(struct threadInfo)*not);//memory allocation for struct to express data

for(i = 0 ; i < not ; i++){//creating threads
	
	ti[i].i = i;
	ti[i].file_size = total_size;
	ti[i].number_of_threads = not;//PASSING DATA TO STRUCT THREAD INFO
	dummy = pthread_create(&threads[i] , NULL , &multithread , (void*)&ti[i]);

}

for(i = 0 ; i < not;i++){
	
pthread_join(threads[i], NULL);//MAKING THE MAIN PROCESS WAIT FOR THREADS

}

return 0;

}
