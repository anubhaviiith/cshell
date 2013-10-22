//Name       :  Anubhav Srivastava
//@IIIT Hyderabad
//Virtual C-Shell
//********************************************************************************************************************************************//
//********************************************************Welcome to my terminal**************************************************************//
//********************************************************************************************************************************************//

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<errno.h>
#include<string.h>
#include<fcntl.h>
#include<limits.h>
#include<signal.h>

int red=0,bread=0,dread=0;
int copy;
char curr[100],arr[100];
int count=0,ptr=0,pr=0;
typedef struct node
{
	char name[1000];
	int pid;
}node;
node proc[100];
typedef struct history 			//structure to store array of strings
{
	char name[1000];
}history;
history hist[1000];
char *pars[1000];
char home[100];
void prompt()				//  prompt function 
{
	char *cwd;
	char curr[100];
	char finalpath[1000];
	int i=0,j=0;
	getcwd(curr,100);
	//printf("%s",curr);
	if(strcmp(curr,home)==0)
	{
		gethostname(arr,100);
		printf("<%s@%s:~> ",getlogin(),arr);
	}
	else if (strstr(curr,home))
	{
		char shoty[1000];
		gethostname(arr,100);
		for(i=0;curr[i]==home[i];++i)
		{
		}
		for(j=0;curr[j];++j)
			shoty[j]=curr[i++];
		printf("<%s@%s:~%s> ",getlogin(),arr,shoty);
	}
	else
	{
		printf("<%s@%s:%s> ",getlogin(),arr,curr);
	}
	return;

}
void signalso(int i,int num)
{
	int x,y;
	kill(i,num);
	if(num==9)
	{
		for(x=0;x<pr;++x)
		{
			if(i==proc[x].pid)
			{
				if(x==pr-1)
					pr--;
				else
				{
					for(y=x;y<pr-1;++y)
					{
						strcpy(proc[y].name,proc[y+1].name);
						proc[y].pid=proc[y+1].pid;
					}
					pr--;
				}
			}
		}

	}
	return ;
}
void handler(int a)
{
	if(a==SIGTSTP)
	{
		kill(copy,SIGTSTP);

	}
	return;
}

void pipeme(char *input)
{
	int status;
	int pipeptr=0,i,countr=0,j;
	char *pipearr[100],*arg[100],*pop[100],*newarg[100];
	int oldPipe[2],newPipe[2];
	char *token;
	int pipefd[2];
	int stdin_copy, stdout_copy;	// creating copy of stdin and stdout to restore later on
	stdin_copy=dup(STDIN_FILENO);	
	stdout_copy=dup(STDOUT_FILENO);
	//printf("%d\n",bread);
	token=strtok(input,"|");
	while(token!=NULL)
	{
		token[strlen(token)]='\0';
		pipearr[pipeptr++]=token;
		token=strtok(NULL,"|");
	}
	pipearr[pipeptr]='\0';
	int pipesCount=pipeptr;
	//printf("%d\n",pipeptr);
	//printf("%s\n",pipearr[1]);
	for(i=0;i<pipeptr;++i)
	{
		int inpu=0,outpu=0;
		countr=0;  
		int k=0;
		// CHECKING IF THE ELEMENT HAS A < OR > TERM 
		while(pipearr[i][k]!='\0')
		{
			if(pipearr[i][k]=='<')
				inpu=1;
			else if(pipearr[i][k]=='>')
				outpu=1;
			k++;
		}
		if(inpu==1)
		{
			int newcountr=0;
			countr=0;
			token=strtok(pipearr[i],"<");				//pars array 
			while(token!=NULL)
			{
				token[strlen(token)]='\0';
				arg[countr++]=token;
				token=strtok(NULL,"<");
			}
			arg[countr]='\0';
			token=strtok(arg[0]," ");
			while(token!=NULL)
			{
				token[strlen(token)]='\0';
				newarg[newcountr++]=token;
				token=strtok(NULL," ");
			}
			newarg[newcountr]='\0';
			arg[1]=strtok(arg[1]," ");

		}
		else if(outpu==1)
		{
			int newcountr=0;
			countr=0;						//pars array
			token=strtok(pipearr[i],">");
			while(token!=NULL)
			{
				token[strlen(token)]='\0';
				arg[countr++]=token;
				token=strtok(NULL,">");
			}
			arg[countr++]='\0';
			
			token=strtok(arg[0]," ");	//parsing string with respect to " "
			while(token!=NULL)
			{
				token[strlen(token)]='\0';
				newarg[newcountr++]=token;
				token=strtok(NULL," ");
			}
			newarg[newcountr]='\0';
			arg[1]=strtok(arg[1]," ");

		}
		else
		{
			countr=0;
			token=strtok(pipearr[i]," ");  //parsing string
			while(token!=NULL)
			{
				token[strlen(token)]='\0';
				arg[countr++]=token;
				token=strtok(NULL," ");
			}
			arg[countr]='\0';
		}

		if(i<pipesCount-1)
			pipe(newPipe);			
		
		pid_t pid=fork();                       //fork a new process

		if(i>0 && i<=pipesCount-1)
		{
			dup2(oldPipe[0],0);
			close(oldPipe[1]);
			close(oldPipe[0]);

		}
		if(pid == 0) 
		{	
			if(i>=0 && i<pipesCount-1)
			{
				dup2(newPipe[1], 1);
				close(newPipe[0]);
				close(newPipe[1]);

			}
			//conditions for executing string 
			if(inpu==0 && outpu==0)			//if no redirection
			{
				execvp(arg[0], arg);
			}
			else if(inpu==1)			//if < redirection with piping
			{
				FILE *in=fopen(arg[1],"r");
				dup2(fileno(in),0);
				execvp(newarg[0],newarg);
				fclose(in);
				fflush(stdout);
			}
			else if(outpu==1)
			{
				FILE *out=fopen(arg[1],"w+");
				dup2(fileno(out),1);
				execvp(newarg[0],newarg);
				fclose(out);
				//fflush(stdout);
			}

		}	
		else 
		{	
			waitpid(pid,&status,0);
			if(i<pipesCount-1)
			{
				oldPipe[0]=newPipe[0];
				oldPipe[1]=newPipe[1];
			}
		}
	}

	close(oldPipe[0]);
	close(newPipe[0]);
	close(oldPipe[1]);
	close(newPipe[1]);

	// restore stdin, stdout
	dup2(stdin_copy, 0);
	dup2(stdout_copy, 1);
	close(stdin_copy);
	close(stdout_copy);
}
int main()
{
	signal(SIGCHLD,SIG_IGN);
	signal(SIGINT,SIG_IGN);
	signal(SIGQUIT,SIG_IGN);
	char s[100],c;
	char *kok[100];
	char *token,*myhome;
	int len,pid,i,flag=1,status;
	myhome=getenv("PWD");
	pid_t main_pid=getpid(); 				//default value of main_pid is set
	printf("\n***********************************************************************\n");
	printf("*****************  Welcome to Anubhav's Terminal **********************\n");
	printf("***********************************************************************\n\n");
	getcwd(home,100);

	while(1)
	{
		red=bread=dread=ptr=0;
		char scopy[100];
		int k,j;
		k=waitpid(-1,&status,WNOHANG);
		if(k>0)						// delete processes from queue which have exited
		{
			printf("Process with PID=%d exiting\n",k);
			for(i=0;i<pr;++i)
			{
				if(proc[i].pid==k)
				{
					if(i==pr-1)
						pr--;
					else
					{
						for(j=i;j<pr-1;++j)
						{
							proc[j].pid=proc[j+1].pid;
							strcpy(proc[j].name,proc[j+1].name);
							++j;
						}
						pr--;
					}

				}
			}
		}
		prompt();
		signal(SIGTSTP,handler);
		scanf("%[^\n]",s);
		strcpy(scopy,s);
		//printf("%s",s);
		scanf("%c",&c);
		if(strcmp(hist[count].name,s)!=0) 		//storing history
			strcpy(hist[count++].name,s);
		char *new1=strtok(s," ");
		while(new1!=NULL)				//storing the various arguments in array pars
		{
			pars[ptr++]=new1;
			new1=strtok(NULL," ");
		}
		pars[ptr]=new1;
		//++ptr;
		//write(1,pars[ptr-1],strlen(pars[ptr-1]));
		//write(1,ptr,sizeof(int));
		//printf("%s",pars[0]);
		for(i=0;i<ptr;++i)			//redirection
		{
			if(strcmp(pars[i],">")==0)
			{
				red=1;
			}
			else if(strcmp(pars[i],"<")==0)
			{
				bread=1;
				int index=i;
			}
			else if(strcmp(pars[i],"|")==0)
			{
				dread=1;
			}	
		}
		//if dread==1 matlab pipe so khush ho jao aur function call karo
		if(dread==1)
			pipeme(scopy);
		else if(red==1 && bread==0)
		{
			int pttr=0;
			char *parsr[100],*right[100];
			int i,in,out,ind=0,rptr=0;	
			char *x,*token;
			//printf("%s\n",scopy);
			token=strtok(scopy,">");

			while(token!=NULL)
			{
				token[strlen(token)]='\0';
				right[rptr++]=token;
				token=strtok(NULL,">");

			}
			right[rptr]='\0';

			token=strtok(right[0]," ");
			while(token!=NULL)
			{
				token[strlen(token)]='\0';
				parsr[pttr++]=token;
				token=strtok(NULL," ");
			}
			parsr[pttr]='\0';

			pid_t pid;
			pid=fork();
			if (pid == -1)
			{
				perror("Error forking...");
			}
			else if (pid==0) 
			{
				right[1]=strtok(right[1]," ");
				FILE* out=fopen(right[1], "w+");
				dup2(fileno(out),1);
				fclose(out);

				execvp(parsr[0],parsr);
			}
			else 
			{
				wait(NULL);
				fflush(stdout);
			}
		}
		else if (bread==1 && red==0)
		{
			int pttl=0;
			char *parsl[100],*left[100];
			int i,in,out,ind=0,lptr=0;
			char *x,*token;
			//printf("%s\n",scopy);
			token=strtok(scopy,"<");

			while(token!=NULL)
			{
				token[strlen(token)]='\0';
				left[lptr++]=token;
				token=strtok(NULL,"<");

			}
			left[lptr]='\0';

			/*
			   for(i=0;i<lptr;++i)
			   printf("%s\n",left[i]);
			 */

			token=strtok(left[0]," ");
			while(token!=NULL)
			{
				token[strlen(token)]='\0';
				parsl[pttl++]=token;
				token=strtok(NULL," ");
			}
			parsl[pttl]='\0';

			/*
			   for(i=0;i<pttl;++i)
			   printf("%s\n",parsl[i]);
			 */

			pid_t pid;
			pid=fork();
			if(pid == -1)
			{
				perror("Error forking...");
			}
			else if (pid==0) 
			{
				left[1]=strtok(left[1]," ");
				int in=open(left[1], O_RDONLY);
				dup2(in,0);
				close(in);
				//printf("%d %s\n",pttl,parsl[0]);
				//	write(1,parsl[0],strlen(parsl[0]));
				execvp(parsl[0],parsl);
			}
			else 
			{
				wait(NULL);
				fflush(stdout);
			}

		}
		else if(bread==1 && red==1)
		{
			//printf("here");
			int pttl=0,pttr=0;
			char *parsl[100],*left[100],*parsr[100],*right[100];
			int i,in,out,ind=0,lptr=0,rptr=0;
			char *x,*token;
			char scopyy[100];
			strcpy(scopyy,s);
			//printf("%s\n",scopy);
			token=strtok(scopy,"<");

			while(token!=NULL)
			{
				token[strlen(token)]='\0';
				left[lptr++]=token;
				token=strtok(NULL,"<");

			}
			left[lptr]='\0';
			token=strtok(left[1],">");
			while(token!=NULL)
			{
				token[strlen(token)]='\0';
				right[rptr++]=token;
				token=strtok(NULL,">");

			}
			right[rptr]='\0';

			
		/*	   for(i=0;i<lptr;++i)
				   printf("%s\n",left[i]);
			   printf("\n\n\n");
			   for(i=0;i<rptr;++i)
			printf("%s\n",right[1]);
			   printf("\n");*/
			right[1]=strtok(right[1]," ");
			token=strtok(left[0]," ");
			while(token!=NULL)
			{
				token[strlen(token)]='\0';
				parsl[pttl++]=token;
				token=strtok(NULL," ");
			}
			parsl[pttl]='\0';

			
			//   for(i=0;i<pttl;++i)
			  //	 printf("%s\n",parsl[i]);
			 

			pid_t pid;
			pid=fork();
			if(pid==-1)
			{
				perror("Error forking...");
			}
			else if (pid==0) 
			{
				left[1]=strtok(left[1]," ");
				//printf("%s\n",left[1]);
				int in=open(left[1], O_RDONLY);
				//printf("here\n");
				//right[1]=strtok(right[1]," ");
				//printf("%s\n",right[1]);
				FILE *out=fopen(right[1],"w+");
				dup2(in,0);
				dup2(fileno(out),1);
				close(in);
				close(fileno(out));
				//printf("%d %s\n",pttl,parsl[0]);
				//	write(1,parsl[0],strlen(parsl[0]));
				execvp(parsl[0],parsl);
			}
			else 
			{
				wait(NULL);
				fflush(stdout);
			}


		}
		else
		{
			if(strcmp(pars[0],"cd")==0)
			{
				char *cwd=NULL;
				//char ert[100];
				//printf("%s",cwd);
				//char *cwd=NULL;
				if(pars[1]==NULL )
					chdir(home);
				else if(strcmp(pars[1],"~/")==0 || strcmp(pars[1],"~")==0)
					chdir(home);
				else
				{
					chdir(pars[1]);
				}

			}
			else if(strcmp(s,"quit")==0)
			{
				printf("\n*******************************************************************\n");
				printf("*************************** Thank You *****************************\n");
				printf("*******************************************************************\n\n");
				return 0;
			}
			else if(strcmp(s,"pinfo")==0)
			{
				//printf("pid -- %d\n",main_pid);
				if(ptr==1)
				{	char val[100],waah[100];
					char *rt=NULL;
					size_t ty=0;
					FILE *fp;
					sprintf(waah,"%d",main_pid);
					strcpy(val,"/proc/");
					strcat(val,waah);
					strcat(val,"/status");
					fp=fopen(val,"r");
					//fp=fopen("/proc/%d/status",main_pid);
					getline(&rt,&ty,fp);
					printf("%s",rt);
					getline(&rt,&ty,fp);
					printf("%s",rt);
					getline(&rt,&ty,fp);
					getline(&rt,&ty,fp);
					printf("%s",rt);
					while(strncmp(rt,"VmSize",6)!=0)
					{
						getline(&rt,&ty,fp);
					}
					printf("%s",rt);
				}
				else
				{	size_t ty=0;
					FILE *fp;
					char val[100],waah[100];
					char *rt=NULL;
					//	sprintf(waah,"%d",pars[1]);
					strcpy(val,"/proc/");
					strcat(val,pars[1]);
					strcat(val,"/status");
					fp=fopen(val,"r");
					if(fp)
					{	
						getline(&rt,&ty,fp);
						getline(&rt,&ty,fp);
						printf("%s",rt);
						getline(&rt,&ty,fp);
						getline(&rt,&ty,fp);
						printf("%s",rt);
						int ctr=0;
						while(strncmp(rt,"VmSize",6)!=0)
						{
							getline(&rt,&ty,fp);
							++ctr;
							if(ctr>150)
							{
								int flag=1;
								break;
							}

						}
						if(flag!=1)
							printf("%s",rt);
					}
					else
						printf("File does not exist\n");
				}
			}
			else if(strcmp(s,"history")==0)
			{
				for(i=0;i<count;i++)
					printf("%d -> %s\n",i+1,hist[i].name);
			}
			else if(ptr>1 && strcmp(pars[1],"&")==0)
			{
				pid_t p=fork();
				//main_pid=getpid();
				strcpy(proc[pr].name,s);
				proc[pr].pid=p;
				copy=p;
				pr++;
				if(p<0)
				{
					printf("Error occured");
					exit(-1);
				}
				else if(p==0)
				{
					//strcat(pars[0],"&");
					//printf("%s",pars[0]);
					k=execvp(pars[0],pars);
					if(k<0)
						printf("Invalid Command\n");

				}

			}
			else if(strcmp(s,"jobs")==0)				//current jobs 
			{
				if(pr==0)
					printf("There are no jobs\n");
				else
				{
					for(i=0;i<pr;++i)
						printf("%d. %s %d\n",i+1,proc[i].name,proc[i].pid);
				}
			}
			else if(strcmp(s,"overkill")==0) 			//kill processes
			{
				int q;
				for(q=0;q<pr;++q)
					kill(proc[q].pid,9);
				pr=0;
				printf("All processes killed\n");
			}
			else if(strcmp(pars[0],"kjob")==0)
			{
				int x,t;
				if(ptr!=3)
					printf("Insuficient Arguments\n");
				else
				{
					x=atoi(pars[2]);
					t=atoi(pars[1]);
					if(t>pr)
						printf("Invalid Job Id.\n");
					else
						signalso(proc[t-1].pid,x);
				}
			}
			else if(strcmp(pars[0],"fg")==0)
			{
				int e;
				e=atoi(pars[1]);
				if(e>pr)
					printf("Invalid Job Id.\n");
				else
				{
					kill(proc[e-1].pid,SIGCONT);
					//printf("%d %d\n",e,proc[e-1].pid);
					wait(proc[e-1].pid,SIGCONT);
					k=proc[e-1].pid;

					for(i=0;i<pr;++i)
					{
						if(proc[i].pid==k)
						{
							if(i==pr-1)
								pr--;
							else
							{
								for(j=i;j<pr-1;++j)
								{
									proc[j].pid=proc[j+1].pid;
									strcpy(proc[j].name,proc[j+1].name);
									++j;
								}
								pr--;
							}

						}
					}

				}
			}
			else
			{
				pid_t p=fork();
				copy=p;
				if(p<0)
				{
					printf("Error occured");
					exit(-1);
				}
				else if(p==0)
				{
					//write(1,pars[ptr-1],strlen(pars[ptr-1]));
					k=execvp(pars[0],pars);
					if(pars[0]='\0')
						continue;
					else if(k<0)
						printf("Invalid Command\n");
				}
				else
				{
					waitpid(p,&status,WUNTRACED);
					if(WIFSTOPPED(status))
					{
						strcpy(proc[pr].name,pars[0]);
						proc[pr].pid=p;
						++pr;
					}
				}
			}
		}
	}	
	return 0;
}
