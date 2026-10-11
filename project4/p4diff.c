#include<stdio.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc,char *argv[]) {
	int fd,b_similar = 0,b_stud = 0,b_autol = 0,res = 0,i,j,k,score = 0,total_res = 0;
	char stud_out[64],autol_out[64];
	for (i = 0; i < 64; i++) {
		stud_out[i] = '\0';
		autol_out[i] = '\0';
	}
	fd = open(argv[1],O_RDWR,S_IRWXU);
	if (fd == -1) {
		fprintf(stderr,"open failed at %d",__LINE__ - 2);
		exit(-1);
	}
	while(1) {
		total_res = 0;
		/*Katallhlh read oste na diavazei osa bytes mporei kai ths menoun kai oxi panta 64*/
		while(total_res < 64 && (res = read(fd,&autol_out[total_res],64 - total_res)) > 0) {
			if (res == -1) {
				fprintf(stderr,"read failed at %d",__LINE__ - 2);
				exit(-1);
			}
			total_res = total_res + res;
		}
		total_res = 0;
		/*To idio kai gia thn ejodo toy mathiti*/
		while (total_res < 64 && (res = read(0,&stud_out[total_res],64 - total_res)) != 0) {
			if (res == -1) {
				fprintf(stderr,"read failed at %d",__LINE__ - 2);
				exit(-1);
			}
			total_res = total_res + res;
		}
		/*Ta string exoyn to megisto arithmo bytes pou mporoune*/
		if (stud_out[63] != '\0' && autol_out[63] != '\0') {
			for (i = 0; i < 64; i++) {
				if (stud_out[i] == autol_out[i]) {
					b_similar++;
				}
			}
			b_stud = b_stud + 64;
			b_autol = b_autol + 64;
		}
		/*Ta bytes ths ejodou tou mathiti exoun teleiosei eno ths sosths ejodou oxi*/
		if (stud_out[63] == '\0' && autol_out[63] != '\0') {
			for (i = 0; stud_out[i] != '\0'; i++);
			for (j = 0; j < i; j++) {
				if (stud_out[j] == autol_out[j]) {
					b_similar++;
				}
			}
			b_autol = b_autol + 64;
			b_stud = b_stud + i;
		}
		/*Ta bytes ths sosths ejodoy exoyn teleiosei eno tou mathiti oxi*/
		if (stud_out[63] != '\0' && autol_out[63] == '\0') {
			for (i = 0; autol_out[i] != '\0'; i++);
			for (j = 0; j < i; j++) {
				if (stud_out[j] == autol_out[j]) {
					b_similar++;
				}
			}
			b_stud = b_stud + 64;
			b_autol = b_autol + i;
		}
		/*Ta bytes ths sosths ejodoy kai ta bytes ths ejodou tou mathiti exoun teleiosei*/
		if (stud_out[63] == '\0' && autol_out[63] == '\0') {
			for (i = 0; autol_out[i] != '\0'; i++);
			for (j = 0; stud_out[j] != '\0'; j++);
			if (i <= j) {
				for (k = 0; k < i; k++) {
					if (stud_out[k] == autol_out[k]) {
						b_similar++;
					}
				}
			}
			else {
				for (k = 0; k < j; k++) {
					if (stud_out[k] == autol_out[k]) {
						b_similar++;
					}
				}
			}
			b_autol = b_autol + i;
			b_stud = b_stud + j;
			break;
		}
		/*Arxikopoiisi ek neou gia na gnorizoume pou teleionei to string*/
		for (i = 0; i < 64; i++) {
			stud_out[i] = '\0';
			autol_out[i] = '\0';
		}
	}
	if (b_stud >= b_autol) {
		/*Mini-elegxos gia thn periptosh tou kenoy arxeioy*/
		if (b_stud == 0) {
			score = 100;
		}
		else {
			score = b_similar * 100 / b_stud;
		}
	}
	else {
		score = b_similar * 100 / b_autol;
	}
	/*Epistrofh ths telikhs vathmologias*/
	exit(score);
}
