#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>



void expand_array(char ***args,int *array_size,int *arg_buf,int option) {
		char **temp = NULL;
		int i,orig_size,j;
		/*Epektasi toy pinaka ean ta orismata einai parapano apo to arxiko megethos toy pinaka*/
	if (option == -1) {
		orig_size = *array_size;
		*array_size = (*array_size) * 2;
		temp = (char **) realloc(*args,(*array_size)*sizeof(char*));
		if (temp == NULL) {
			fprintf(stderr, "realloc option 1 failed\n");
			exit(-1);
		}
		*args = temp;
		for (i = orig_size; i < *array_size; i++) {
			(*args)[i] = calloc(*arg_buf,sizeof(char));
		}
	}
	/*Epektasi toy pinaka kata mhkos ean to mhkos ton orismaton einai megalo*/
	if (option != -1) {
		int orig_buf = *arg_buf;
		*arg_buf = 2 * (*arg_buf);
			(*args)[option] = realloc((*args)[option],(*arg_buf) * sizeof(char));
			if ((*args)[option] == NULL) {
				fprintf(stderr, "realloc option 2 failed\n");
				exit(-1);
			}
		for (j = orig_buf; j < *arg_buf; j++) {
			(*args)[option][j] = '\0';
		}
	}
}


int main(int argc,char *argv[]) {
	/*Elegxos gia sosta orismata*/
	if (argc != 5) {
		fprintf(stderr,"<c file> <args file> <input file> <output file>");
		exit(2);
	}
	char progname[strlen(argv[1]) - 1],check_for_comp[15],err_name[strlen(argv[1]) + 3];
	char exec_name[strlen(argv[1]) + 1];
	char **args = NULL,check_for_space = '\0';
	char check_EOF = '\0';
	int pid1,pid2,pid3,i,j,err_fd,args_fd,fd,res,status,comp_error = 0,comp_warning = 0;
	int pipe_fd[2],total = 0,compilation = 0,mem_access = 0,output = 0,array_size = 2;
	int arg_buf = array_size,enter_loop = 0;
	args = malloc(array_size*sizeof(char *));
	if (args == NULL) {
		fprintf(stderr, "out of memory\n");
		exit(-1);
	}
	for (i = 0; i < array_size; i++) {
		args[i] = malloc(array_size * sizeof(char));
		if(args[i] == NULL) {
			fprintf(stderr, "out of memory\n");
			exit(-1);
		}
	}
	/*Oi aparaithtes arxikopoihseis*/
	for (i = 0; i < array_size; i++) {
		for (j = 0; j < array_size; j++) {
			args[i][j] = '\0';
		}
	}
	for (i = 0; i < strlen(argv[1]) - 1; i++) {
		progname[i] = '\0';
	}
	for (i = 0; i < strlen(argv[1]) + 3; i++) {
		err_name[i] = '\0';
	}
	for (i = 0; i < strlen(argv[1]) + 1; i++) {
		exec_name[i] = '\0';
	}
	for (i = 0; i < 15; i++) {
		check_for_comp[i] = '\0';
	}
	for (i = 0; i < strlen(argv[1]) - 2; i++) {
		progname[i] = argv[1][i];
	}
	strcpy(err_name,progname);
	strcat(err_name,".err");
	exec_name[0] = '.';exec_name[1] = '/';
	strcat(exec_name,progname);
	err_fd = open(err_name,O_RDWR|O_CREAT|O_TRUNC,S_IRWXU);
	if (err_fd == -1) {
		fprintf(stderr,"open failed at %d",__LINE__ - 2);
		exit(-1);
	}
	/*Dhmiourgo thn diergasia p1*/
	pid1 = fork();
	if (pid1 < 0) {
		fprintf(stderr,"fork failed at %d",__LINE__ - 2);
	}
	if (pid1 == 0) {
		/*Anakateythinsi ejodoy lathon*/
		res = dup2(err_fd,STDERR_FILENO);
		if (res == -1) {
			fprintf(stderr,"dup2 failed at %d",__LINE__ - 2);
			exit(-1);
		}
		res = close(err_fd);
		if (res == -1) {
			fprintf(stderr,"close failed at %d",__LINE__ - 2);
			exit(-1);
		}
		execl("/usr/bin/gcc","gcc","-Wall",argv[1],"-o",progname,NULL);
		perror("execl");
		fprintf(stderr,"%d",__LINE__);
		exit(1);
	}
	/*Perimeno na teleiosei h diergasia p1*/
	waitpid(pid1,&status,0);
	res = lseek(err_fd,0,SEEK_SET);
	if (res == -1) {
		fprintf(stderr,"lseek failed at %d",__LINE__ - 2);
		exit(-1);
	}
	/*Pairno se ejades ta bytes gia na do an iparxei h lejh error*/
	while (read(err_fd,check_for_comp,6) != 0) {
		if (strcmp(check_for_comp,"error:") == 0) {
			comp_error = 1;
			break;
		}
		/*Elegxos gia to telos tou arxeiou*/
		res = read(err_fd,&check_EOF,1);
		if (res == -1) {
			fprintf(stderr,"read failed at %d",__LINE__ - 2);
			exit(-1);
		}
		if (res == 0) {
			break;
		}
		res = lseek(err_fd,(off_t) - 1,SEEK_CUR);
		if (res == -1) {
			fprintf(stderr,"lseek failed at %d",__LINE__ - 2);
			exit(-1);
		}
		res = lseek(err_fd,(off_t) - 5,SEEK_CUR);
		if (res == -1) {
			fprintf(stderr,"lseek failed at %d",__LINE__ - 2);
			exit(-1);
		}
	}
	/*Epanafora tou err_fd sthn arxh toy arxeioy*/
	res = lseek(err_fd,0,SEEK_SET);
	if (res == -1) {
		fprintf(stderr,"lseek failed at %d",__LINE__ - 2);
		exit(-1);
	}
	if (comp_error != 1) {
		/*Pairno se oktades ta bytes gia na do an iparxei h lejh warning*/
		while (read(err_fd,check_for_comp,8) != 0) {
			if (strcmp(check_for_comp,"warning:") == 0) {
				comp_warning = 1;
				break;
			}
			res = read(err_fd,&check_EOF,1);
			if (res == 0) {
				break;
			}
			res = lseek(err_fd,(off_t) - 1,SEEK_CUR);
			if (res == -1) {
				fprintf(stderr,"lseek failed at %d",__LINE__ - 2);
				exit(-1);
			}
			res = lseek(err_fd,(off_t) - 7,SEEK_CUR);
			if (res == -1) {
				fprintf(stderr,"lseek failed at %d",__LINE__ - 2);
				exit(-1);
			}
		}
		/*Dhmiourgo to pipe*/
		res = pipe(pipe_fd);
		if (res == -1) {
			fprintf(stderr,"pipe failed at %d",__LINE__ - 2);
			exit(-1);
		}
		/*Dhmiourgo thn diergasia p2*/
		pid2 = fork();
		if (pid2 < 0) {
			fprintf(stderr,"fork failed at %d",__LINE__ - 2);
		}
		if (pid2 == 0) {
			res = close(pipe_fd[0]);
			if (res == -1) {
				fprintf(stderr,"close failed at %d",__LINE__ - 2);
				exit(-1);
			}
			fd = open(argv[3],O_RDWR,S_IRWXU);
			if (fd == -1) {
				fprintf(stderr,"open failed at %d",__LINE__ - 2);
				exit(-1);
			}
			/*Anakateythinsi ths eisodou ths diergasias sto arxeio ejodou tou mathiti*/
			res = dup2(fd,STDIN_FILENO);
			if (res == -1) {
				fprintf(stderr,"dup2 failed at %d",__LINE__ - 2);
				exit(-1);
			}
			/*Anakateythinsi ths ejodou ths diergasias sto akro eggrafhs tou pipe*/
			res = dup2(pipe_fd[1],STDOUT_FILENO);
			if (res == -1) {
				fprintf(stderr,"dup2 failed at %d",__LINE__ - 2);
				exit(-1);
			}
			res = close(pipe_fd[1]);
			if (res == -1) {
				fprintf(stderr,"close failed at %d",__LINE__ - 2);
				exit(-1);
			}
			args_fd = open(argv[2],O_RDWR,S_IRWXU);
			if (args_fd == -1) {
				fprintf(stderr,"open failed at %d",__LINE__ - 2);
				exit(-1);
			}
			i = 0;
			j = 0;
			strcpy(args[i],progname);
			i++;
			while ((res = read(args_fd,&check_for_space,1)) != 0) {
				enter_loop = 1;
				if (res == -1) {
					fprintf(stderr,"read failed at %d",__LINE__ - 2);
					exit(-1);
				}
				if (check_for_space == ' ') {
					do {
						res = read(args_fd,&check_for_space,1);
						/*An exei na diavasei akomh pragmatika orismata apo to arxeio tote synexizei thn anagnosh*/
						if (res > 0 && check_for_space != ' ' && check_for_space != '\n') {
							i++;
							break;
						}
					} while(res > 0);
					/*Epektash pinaka an xreiastei*/
					if (i == array_size) {
						expand_array(&args,&array_size,&arg_buf,-1);
					}
					j = 0;
				}
				if (check_for_space >= '!' && check_for_space <= '~')     {
					args[i][j] = check_for_space;
					j++;
					/*Epektash kapoias seiras tou pinaka an xreiastei*/
					if (j == arg_buf) {
						expand_array(&args,&array_size,&arg_buf,i);
					}
				}
			}
			/*an den exei dextei orisma,h metavlhth i exei hdh ayjithei opote den thn janaayjano*/
			if (enter_loop == 0) {
				args[i] = NULL;
			}
			else {
				i++;
				args[i] = NULL;
			}
			res = close(args_fd);
			if (res == -1) {
				fprintf(stderr,"close failed at %d",__LINE__ - 2);
				exit(-1);
			}
			execv(exec_name,args);
			perror("execl");
			fprintf(stderr,"%d",__LINE__ - 2);
			exit(1);
		}
		/*Dhmiourgo thn diergasia p3 h opoia trexei taytoxrona me thn p2*/
		pid3 = fork();
		if (pid3 < 0) {
			fprintf(stderr,"fork failed at %d",__LINE__ - 2);
		}
		if (pid3 == 0) {
			res = close(pipe_fd[1]);
			if (res == -1) {
				fprintf(stderr,"close failed at %d",__LINE__ - 2);
				exit(-1);
			}
			/*Anakateythinsi ths eisodou ths diergasias sto akro anagnoshs tou pipe*/
			res = dup2(pipe_fd[0],STDIN_FILENO);
			if (res == -1) {
				fprintf(stderr,"dup2 failed at %d",__LINE__ - 2);
				exit(-1);
			}
			res = close(pipe_fd[0]);
			if (res == -1) {
				fprintf(stderr,"dup2 failed at %d",__LINE__ - 2);
				exit(-1);
			}
			execl("./p4diff","p4diff",argv[4],NULL);
			perror("execl");
			fprintf(stderr,"%d",__LINE__);
			exit(1);
		}
		/*Kleinoume to pipe*/
		res = close(pipe_fd[0]);
		if (res == -1) {
			fprintf(stderr,"dup2 failed at %d",__LINE__ - 2);
			exit(-1);
		}
		res = close(pipe_fd[1]);
		if (res == -1) {
			fprintf(stderr,"dup2 failed at %d",__LINE__ - 2);
			exit(-1);
		}
		/*Perimenoume na termatisoun oi diergasies me th seira pou prepei*/
		for (i = 0; i < 2; i++) {
			if (i == 0) {
				waitpid(pid2,&status,0);
				if (WIFSIGNALED(status)) {
					if (WTERMSIG(status) == SIGSEGV || WTERMSIG(status) == SIGABRT || WTERMSIG(status) == SIGBUS) {
						mem_access = -15;
					}
				}
			}
			if (i == 1) {
				waitpid(pid3,&status,0);
				if (WIFEXITED(status)) {
					output = WEXITSTATUS(status);
				}
			}
		}
	}
	/*Apodesmeyoume th mnhmh pou desmeysame*/
	for (i = 0; i < array_size; i++) {
		free(args[i]);
	}
	free(args);
	/*Ektyponoyme ta katallhla mhnymata ejodoy*/
	if (comp_error == 1) {
		compilation = -100;
	}
	if (comp_warning == 1) {
		compilation = -10;
	}
	total = compilation + output + mem_access;
	if (total < 0) {
		total = 0;
	}
	printf("Compilation: %d\n",compilation);
	printf("Output: %d\n",output);
	printf("Memory access: %d\n",mem_access);
	printf("Total: %d\n",total);
	return(0);
}
