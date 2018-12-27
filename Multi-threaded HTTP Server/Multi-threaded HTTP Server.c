#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include<pthread.h>
#include<semaphore.h>
#include <fcntl.h>

#define PORT_NO 8888
#define IP_ADDRESS "127.0.0.1"
#define SIN_FAMILY AF_INET
#define TYPE SOCK_STREAM
#define OFFSET_FILENAME 5
#define BAD_CONT "HTTP/1.0 403 Bad Content\nContent-Type:text/html\n\n"
#define BAD_NOT "<html><body><h1>BAD CONTENT</h1></body></html>"
#define path "/home/onur/Desktop/hw2"
#define BUSY "HTTP/1.0 404 Server Busy\nContent-Type:text/html\n\n"
#define BUSY_TEXT "<html><body><h1>SERVER IS BUSY</h1></body></html>"


/* this part is used from https://github.com/adilansari/myhttpd/blob/master/myhttpd.c */
#define OK_TEXT "HTTP/1.0 200 OK\nContent-Type:text/html\n\n"
#define OK_IMAGE "HTTP/1.0 200 OK\nContent-Type:image/gif\n\n"
#define NOTOK_404 "HTTP/1.0 404 Not Found\nContent-Type:text/html\n\n"
#define MESS_404 "<html><body><h1>FILE NOT FOUND</h1></body></html>"

/* end of part */


sem_t mutex;
int server_socket;
int sem_count = 0;

void * parseHTTP(char * sequence,char * appendage);
void * thread_Function(void * cls);
int main(void){
//necessary arrays are initialized.
sem_init(&mutex,0,1);
pthread_t threads[10];


//server socket has been initialized
server_socket = socket(SIN_FAMILY,TYPE,0);

printf("SOCKET HAS BEEN CREATED !\n");
//server address has been initialized
struct sockaddr_in server_address;
server_address.sin_family = SIN_FAMILY;
server_address.sin_port = htons(PORT_NO);
server_address.sin_addr.s_addr = inet_addr(IP_ADDRESS);

printf("SERVER ADDRESS HAS BEEN CREATED !\n");

bind(server_socket,(struct sockaddr *)&server_address,sizeof(server_address));

printf("BINDING HAS BEEN COMPLETED !\n");

listen(server_socket,10);

//necessary assignments for client handling
int client_socket;
struct sockaddr_in client;
int c = sizeof(struct sockaddr_in);
int threadi = 0;
int dummyReturn;

while(1){
client_socket = accept(server_socket,(struct sockaddr *)&client,(socklen_t *)&c);
printf("A NEW REQUEST HAS CAME\n");

pthread_create(&threads[threadi], NULL, &thread_Function, (void *)&client_socket);

threadi++;


if(threadi = 10){
threadi %=10;
}
}




close(server_socket);



return 0;


}


void * thread_Function(void * cls){



int client_socket = *((int *) cls);

char server_message[1024];
char server_reply[2048];
char server_text[1024];
char filename[64];
char appendage[16];



recv(client_socket,&server_message,sizeof(server_message),0);


if(sem_count>=10){

	strcpy(server_reply,BUSY);
	strcat(server_reply,BUSY_TEXT);
	send(client_socket,server_reply,strlen(server_reply),0);
	close(client_socket);
	pthread_exit(NULL);

}

sem_wait(&mutex);
sem_count++;
sem_post(&mutex);


//parsing the server message
parseHTTP((char *)server_message,(char *)appendage);
strcpy(filename,server_message);


if(strcmp(appendage,".html") == 0){//if appendage is equal to '.html'

strcat(filename,appendage);
FILE * html_data = fopen(filename,"r");


if(html_data == NULL){//if file can not be found.


	strcpy(server_reply,NOTOK_404);
	strcat(server_reply,MESS_404);	
	
}
else{//file exists

	fgets(server_text,1024,html_data);
	fclose(html_data);
	strcpy(server_reply,OK_TEXT);
	strcat(server_reply,server_text);

}


}

else if(strcmp(appendage , ".jpeg") == 0 || strcmp(appendage , ".gif") == 0||strcmp(appendage , ".ico") == 0){//if request is a image
//not working , I don't know why , I applied the code from the comment.

int BUF_SIZE = 10240;
int img_file_descriptor;
char path_real[128];
strcpy(path_real,path);
strcat(path_real,filename);
strcat(path_real,appendage);
img_file_descriptor = open(path_real, O_RDONLY);
strcpy(server_reply,OK_IMAGE);
send(client_socket,server_reply,strlen(server_reply),0);
sendfile(socket, img_file_descriptor , NULL, (unsigned long)BUF_SIZE);
close(client_socket);
sem_wait(&mutex);
sem_count--;
sem_post(&mutex);
pthread_exit(NULL);

}

else if(strstr(appendage ,".1") != NULL && strstr(filename,"HTTP") != NULL){//if no appendage


	FILE * html_data = fopen("index.html","r");
	fgets(server_text,1024,html_data);
	fclose(html_data);
	strcat(server_reply,OK_TEXT);
	strcat(server_reply,server_text);


}

else{


	strcpy(server_reply,BAD_CONT);
	strcat(server_reply,BAD_NOT);	
	send(client_socket,server_reply,strlen(server_reply),0);
	close(client_socket);
	sem_wait(&mutex);
	sem_count--;
	sem_post(&mutex);
	pthread_exit(NULL);


}

send(client_socket,server_reply,strlen(server_reply),0);
close(client_socket);

sem_wait(&mutex);
sem_count--;
sem_post(&mutex);

pthread_exit(NULL);

}



void * parseHTTP(char * sequence, char * appendage){//parsing url to a filename and a appendage
	char single_char_for_filename;
	char filename[64];
	char append[16];
	int sequence_index = OFFSET_FILENAME;
	int filename_index = 0;
	
	while(*(sequence+sequence_index) != '.'){
		single_char_for_filename = *(sequence+sequence_index);

		filename[filename_index] = single_char_for_filename;
		sequence_index++;
		filename_index++;
	}

	filename_index = 0;
	while(*(sequence+sequence_index) != ' '){
		single_char_for_filename = *(sequence+sequence_index);

		append[filename_index] = single_char_for_filename;
		sequence_index++;
		filename_index++;
	}
	strcpy(sequence,filename);
	strcpy(appendage,append);
	
return NULL;
}
