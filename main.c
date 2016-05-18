/*
 * main.c
 *
 * A simple program to illustrate the use of the GNU Readline library
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>



struct backProgram{
	int id;
	char* path;
	pid_t pid;
	int StatOrStop;
};

struct backProgram backGround[6];
int p_num=0;
int status; 
char cwd[1024];
pid_t hmmpid;

void p_child(char **commands){
	execvp(commands[0], commands);
}

void bg_child(char **commands){
	execvp(commands[1], commands);
}

void pwd(char **commands){
	//char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	
}
void cd(char **commands){
	char *directory =commands[1];
	if(chdir(directory)==-1){
		printf("No such directory\n");
	};
}


void bglist(){
	int i;
	for(i=0; i< p_num; i++){
		char HH;
		if(backGround[i].StatOrStop==0){
			HH='R';
		}else{
			HH='S';
		}
		printf("%i[%c]:\tpid:%i\tpath:%s\n", backGround[i].id,HH,backGround[i].pid,backGround[i].path);	
	}
	printf("Total Background jobs: %i\n", i);
}

void AddBG(pid_t childpid){
	backGround[p_num].id=p_num;
	backGround[p_num].pid=childpid;
	
	backGround[p_num].path=cwd;
	backGround[p_num].StatOrStop=0;
	//Runnig is 0
	//strcpy(backGround[p_num].path, cwd);
	p_num++;
}
//kill(pid, SIGTERM);
//pid_t tmp = waitpid(-1, &status, WNOHANG);


void terminate(){
	pid_t tmp = waitpid(-1, &status, WNOHANG);
	if(tmp >0){
		printf("********************************************\n");
		printf("Hey, Hey, %i has been terminated\n",hmmpid);
		printf("********************************************\n");
		//p_num--;
	}		
}
int main ( void )
{
	for (;;)
	{
		//pid_t pid;
		terminate();
		
		char 	*cmd = readline ("shell>");
		
		const char s[2]= " ";
		char* args[15];
		//char* argsBG[15];
		
		char *token=strtok(cmd,s);
		int a=0;		
		while(token!=NULL){
			if(a==0){
				args[a]=token;
				a++;
			}
			else if(a<15){			
				args[a]=token;
				//argsBG[a-1]=token;
				a++;
				
			}
			token=strtok(NULL,s);
		}
		pwd(args);
		
		if (strcmp(args[0],"pwd") == 0){
			
			fprintf(stdout, "Current working dir: %s\n", cwd);
		}else if(strcmp(args[0],"cd")==0){
			cd(args);
		}else if(strcmp(args[0],"bgkill")==0){
				int WhichKill=atoi(args[1]);
				//printf("%i",WhichKill);
				int WhichPid=backGround[WhichKill].pid;
				
				if(kill(WhichPid, SIGTERM)==0){
					printf("Kill program with pid  %i\n",WhichPid);
				}
				
		
		//}else if(strcmp(args[0],"bg")==0){
			//p_num++;
			//RunBack(argsBG);
		}else if(strcmp(args[0],"bglist")==0){
			
				bglist();
			
		}else if(strcmp(args[0],"stop")==0){
				int WhichKill=atoi(args[1]);
				if(backGround[WhichKill].StatOrStop==1){
					printf("Error, this program is stoped\n");
				}else{
					backGround[WhichKill].StatOrStop=1;
				int WhichPid=backGround[WhichKill].pid;
				
				if(kill(WhichPid, SIGSTOP)==0){
					printf("Stop program with pid  %i\n",WhichPid);
				}
				
				}
		}else if(strcmp(args[0],"start")==0){
				int WhichKill=atoi(args[1]);
				if(backGround[WhichKill].StatOrStop==0){
					printf("Error, this program is running\n");
				}else{
					backGround[WhichKill].StatOrStop=0;
				int WhichPid=backGround[WhichKill].pid;
				
				if(kill(WhichPid, SIGSTOP)==0){
					printf("Stop program with pid  %i\n",WhichPid);
				}
				
				}
		
		}else{
			
			
		
		//printf("%s",*args);
		hmmpid=fork();
		if(hmmpid>=0){
			
		
			if(hmmpid==0){
				//execvp(args[0], args);
				//sleep(3);
				p_child(args);
				if(strcmp(args[0],"bg")==0){
				
					bg_child(args);
				}
				//p_child(args);
				
			}else{
				if(strcmp(args[0],"bg")==0){
					//waitpid(hmmpid, &status, 0);
					AddBG(hmmpid);
				}else{
				wait(0);
				}
			}
			free (cmd);
		}else{
			printf("OUTPUT ERROR");		
		}
		}
		

		//printf ("Got: [%s]\n", cmd);
		
		//free (cmd);
		
	}	
}
