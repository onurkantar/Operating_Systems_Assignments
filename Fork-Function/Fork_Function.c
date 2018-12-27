#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(void){
	//---DEFINING THE PARENT AND CHILDS---
	pid_t p1,c1,c2,c3,c4;

	p1 = fork();
	//---ERROR HANDLING FOR PARENT---
	if(p1 == -1){

		printf("\nPARENT PROCESS CAN NOT BE INITIALIZED!!!\n");

		}
		//---MAIN PARENT PROCESS---
		//---TASK : CREATING CHILD 1---
	else if(p1 == 0){
		printf("\nP1 HAS BEEN CREATED , P1 PID : %d\n" , getpid());
		c1 = fork();
		//---ERROR HANDLING FOR CHILD 1---
		if(c1 == -1){
			printf("\nCHILD 1 CAN NOT BE INITIALIZED !!!\n");
		}
		//---MAIN CHILD 1 PROCESS--
		//---TASK : CREATING A FILE WHICH HAS A NAME "opSys.txt"
		else if(c1 == 0){
			printf("\nC1 HAS BEEN CREATED , C1 PID : %d , PARENT : %d\n" , getpid(), getppid());
			FILE * fp;
			fp = fopen("opSys.txt","w");
			fclose(fp);
			printf("\nOPSYS.TXT HAS BEEN CREATED\n");
			exit(0);//@@@SUCCESSFULLY TERMINATION
			}
		//---MAIN PARENT PROCESS---
		//---TASK : CREATING CHILD 2---
		else{
			sleep(1);//@@@SLEEP TIME FOR LETTING C1 FINISHES ITS TASK FIRST.

			c2 = fork();
			//---ERROR HANDLING FOR CHILD 2---
			if(c2 == -1){
				printf("\nCHILD 2 CAN NOT BE INITIALIZED!!!\n");
				}
			//---MAIN CHILD 2 PROCESS---
			//---TASK : CREATING CHILD 4 &
			//GETTING INPUT FROM USER AND WRITE IT TO THE FILE---
			else if(c2 == 0){

				printf("\nC2 HAS BEEN CREATED , C2 PID : %d , PARENT : %d\n" , getpid(),getppid());

				c4 = fork();
				//---ERROR HANDLING FOR CHILD 4---
				if(c4 == -1){
					printf("\nCHILD 4 CAN NOT BE INITIALIZED!!!\n");
					}
				//---MAIN CHILD 4 PROCESS---
				else if(c4 == 0){
					printf("\nC4 HAS BEEN CREATED , C4 PID : %d , PARENT : %d\n" , getpid(), getppid());

					printf("\nSAY ME PASSWORD...\n\n");//@@@CHILD 4'S TASK
					exit(0);

					}
				//---CHILD 2 PROCESSES---
				else{
					wait(NULL);//WAITING TILL OTHER CHILD PROCESSES ARE DONE
					int input = -1;
					scanf("%d" , &input);
					FILE * fp;
					fp = fopen("opSys.txt" , "w");
					fprintf(fp , "%d" , input);
					fclose(fp);
					exit(0);
					}
				//---END OF CHILD 2 & CHILD 4 PROCESSES
				}
			//---CHILD 3 MAIN PROCESS---
			//---TASK : COMPARING THE USER INPUT WITH ITS PARENT'S ID &
			//PRITING MATCHED IF THEY MATCH OR NOT MATCHED IF THEY NOT---
			else {
				wait(NULL);
				sleep(5);
				c3 = fork();
				//---ERROR HANDLING FOR CHILD 3---
				if(c3 == -1){
					printf("\nCHILD 3 CAN NOT BE INITIALIZED!!!\n");
					}
				//---CHILD 3 PROCESSES---
				else if(c3 == 0){
					int readPass = 0;
					printf("\nC3 HAS BEEN CREATED , C3 PID : %d , PARENT : %d\n",getpid() , getppid());


					FILE * fp;
					fp = fopen("opSys.txt" , "r");

					fscanf(fp , "%d" , &readPass);

					fclose(fp);
					printf("\nUser Input : %d\n" , readPass);
					printf("\nParent PID : %d\n",getppid());//@@@GETPPID() RETURNS THE PARENT'S ID
					//COMPARISON PART
					if(readPass == getppid()){
						printf("\nMATCHED\n\n");
						}

					else{
						printf("\nNOT MATCHED\n\n");
						}

					}

				else{
					wait(NULL);
					}

				}//@@@ELSE JUST BEFORE FORKING CHILD 3

			}//@@@ ELSE JUST BEFORE FORKING CHILD 2

		}//@@@IF(P1 == 0) CLOSING BRACKET

	else {
		wait(NULL);
		}

	wait(NULL);
	return 0;
}
