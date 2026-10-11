#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include"objdb.h"

void error_handling(FILE **fp,const int line) {
	if (ferror(*fp) != 0) {
		perror("Failure");
		fprintf(stderr,"line %d\n",line);
		clearerr(*fp);
		exit(1);
	}
}

int base_init(char db_name[],FILE **fp) {
	int res = 0;
	*fp = fopen(db_name,"w+b");
	error_handling(fp,__LINE__ - 1);
	fwrite("base",sizeof(char),4,*fp);fflush(*fp);
	error_handling(fp,__LINE__ - 1);
	res = fclose(*fp);
	if (res != 0 ) {
		perror("fclose failed line 21");
	}
	*fp = fopen(db_name,"r+b");
	error_handling(fp,__LINE__ - 1);
	fseek(*fp,4L,SEEK_SET);
	error_handling(fp,__LINE__ - 1);
	return(1);
}


int open (char db_name[],FILE **fp) {
	int i;
	if (*fp != NULL) {
		close_p(fp);
	}
	char str[5];
	for (i = 0; i < 5; i++) {
		str[i] = '\0';
	}
	int res = 0;
	res = f_exists(db_name);
	if (res == 0) {
		res = base_init(db_name,fp);
		if (res == 0) {
			return(0);
		}
		return(1);
	}
	*fp = fopen(db_name,"r+b");
	error_handling(fp,__LINE__ - 1);
	fread(str,sizeof(char),4,*fp);
	error_handling(fp,__LINE__ - 1);
	if (strcmp(str,"base") == 0) {
		return(1);
	}
	else {
		fclose(*fp);
		error_handling(fp,__LINE__ - 1);
		*fp = NULL;
		return(-1);
	}
}


int f_exists(char filename[]) {
	FILE *fp = NULL;
	fp = fopen(filename,"r+b"); /*1 exists,0 dont exist*/
	if (fp != NULL) {
		fclose(fp);
		return(1);
	}
	return(0);
}


int find_file_size(char filename[]) {
	int file_size;
	FILE *fp;
	fp = fopen(filename,"rb");
	error_handling(&fp,__LINE__ - 1);
	fseek(fp,0L,SEEK_END);
	error_handling(&fp,__LINE__ - 1);
	file_size = ftell(fp);
	fclose(fp);
	error_handling(&fp,__LINE__ - 1);
	return(file_size);
}


int import (char f_name[],char obj_name[],FILE **fp,struct list_names *result) {
	FILE *obj = NULL;
	char backslash = '\0';
	int res = 0,div = 0,i = 0,mod = 0;
	int file_size = 0;
	char transfer[512],check_obj_name[256];
	for (i = 0; i < 512; i++) {
		transfer[i] = '\0';
	}
	for (i = 0; i < 256; i++) {
		check_obj_name[i] = '\0';
	}
	if (*fp == NULL) {
		return(0);
	}
	res = f_exists(f_name);
	if (res == 0) {
		return(-1);
	}
	find(obj_name,fp,result);
	fseek(*fp,-1*(strlen(obj_name)+1),SEEK_CUR);
	fread(check_obj_name,sizeof(char),strlen(obj_name)+1,*fp);
	error_handling(fp,__LINE__ - 1);
	if (strcmp(obj_name,check_obj_name) == 0) {
		return(-2);
	}
	fseek(*fp,0L,SEEK_END);
	error_handling(fp,__LINE__ - 1);
	fwrite(obj_name,sizeof(char),strlen(obj_name),*fp);fflush(*fp);
	error_handling(fp,__LINE__ - 1);
	fwrite(&backslash,sizeof(char),1,*fp);fflush(*fp);
	error_handling(fp,__LINE__ - 1);
	file_size = find_file_size(f_name);
	fwrite(&file_size,sizeof(int),1,*fp);fflush(*fp);
	error_handling(fp,__LINE__ - 1);
	obj = fopen(f_name,"r+b");
	error_handling(&obj,__LINE__ - 1);
	div = file_size / 512;
	mod = file_size % 512;
	if (div > 0) {
		for (i = 0; i < div; i++) {
			fread(transfer,sizeof(char),512,obj);
			error_handling(&obj,__LINE__ - 1);
			fwrite(transfer,sizeof(char),512,*fp);fflush(*fp);
			error_handling(fp,__LINE__ - 1);
		}
	}
	fseek(*fp,0L,SEEK_CUR);
	error_handling(fp,__LINE__ - 1);
	fread(transfer,sizeof(char),mod,obj);
	error_handling(fp,__LINE__ - 1);
	fwrite(transfer,sizeof(char),mod,*fp);fflush(*fp);
	error_handling(fp,__LINE__ - 1);
	fclose(obj);
	error_handling(&obj,__LINE__ - 1);
	return(1);
}


int find (char name[],FILE **fp,struct list_names *result) {
	int i,j,k = 0,file_size = 0,exp_save = 0;
	char obj_name[256],*strstr_check = NULL,test = '\0';
	char check_for_null = 'a';
	for (i = 0; i < 256; i++) {
		obj_name[i] = '\0';
	}
	if (*fp == NULL) {
		return(0);
	}
	for (i = 0; i < 25; i++) {
		for (j = 0; j < 256; j++) {
			result->list_names[i][j] = '\0';
		}
	}
	fseek(*fp,0L,SEEK_SET);
	error_handling(fp,__LINE__ - 1);
	fseek(*fp,4L,SEEK_CUR);
	error_handling(fp,__LINE__ - 1);
	fread(&test,sizeof(char),1,*fp);
	while(feof(*fp) == 0) {
		fseek(*fp,-1L,SEEK_CUR);
		error_handling(fp,__LINE__ - 1);
		i = 0;
		check_for_null = 'a';
		while (check_for_null != '\0') {
			fread(&obj_name[i],sizeof(char),1,*fp);
			error_handling(fp,__LINE__ - 1);
			check_for_null = obj_name[i];
			i++;
		}
		strstr_check = strstr(obj_name,name);
		if (strstr_check != NULL) {
			strcpy(result->list_names[k],obj_name);
			k++;
		}
		else {
			if (strcmp(name,"*") == 0) {
				strcpy(result->list_names[k],obj_name);
				k++;
			}
		}
		if (strcmp(obj_name,name) == 0) {
			exp_save = ftell(*fp);
		}
		fread(&file_size,sizeof(int),1,*fp);
		error_handling(fp,__LINE__ - 1);
		fseek(*fp,file_size,SEEK_CUR);
		error_handling(fp,__LINE__ - 1);
		fread(&test,sizeof(char),1,*fp);
		error_handling(fp,__LINE__ - 1);
	}
	fseek(*fp,exp_save,SEEK_SET);
	error_handling(fp,__LINE__ - 1);
	return(1);
}


int f_export (char obj_name[],char f_name[],FILE **fp,FILE **exp_ptr,struct list_names *result) {
	int div = 0,mod = 0,res = 0,i = 0,file_size = 0;
	char transfer[512],check_obj_name[256];
	if (*fp == NULL) {
		return(0);
	}
	for (i = 0; i < 256; i++) {
		check_obj_name[i] = '\0';
	}
	find(obj_name,fp,result);
	fseek(*fp,-1*(strlen(obj_name)+1),SEEK_CUR);
	fread(check_obj_name,sizeof(char),strlen(obj_name)+1,*fp);
	error_handling(fp,__LINE__ - 1);
	if (strcmp(obj_name,check_obj_name) != 0) {
		return(-2);
	}
	*exp_ptr = fopen(f_name,"r+b");
	if (*exp_ptr != NULL) {
		fclose(*exp_ptr);
		return(-1);
	}
	*exp_ptr = fopen(f_name,"w+b");
	error_handling(exp_ptr,__LINE__ - 1);
	fread(&file_size,sizeof(int),1,*fp);
	error_handling(fp,__LINE__ - 1);
	div = file_size / 512;
	mod = file_size % 512;
	if  (div > 0) {
		for (i = 0; i < div; i++) {
			fseek(*fp,0L,SEEK_CUR);
			fread(transfer,sizeof(char),512,*fp);
			error_handling(fp,__LINE__ - 1);
			fwrite(transfer,sizeof(char),512,*exp_ptr);fflush(*exp_ptr);
			error_handling(exp_ptr,__LINE__ - 1);
		}
	}
	fseek(*fp,0L,SEEK_CUR);
	fread(transfer,sizeof(char),mod,*fp);
	error_handling(fp,__LINE__ - 1);
	fwrite(transfer,sizeof(char),mod,*exp_ptr);fflush(*exp_ptr);
	error_handling(exp_ptr,__LINE__ - 1);
	res = fclose(*exp_ptr);
	if (res != 0) {
		perror("fclose failed");
		exit(-1);
	}
	fseek(*fp,0L,SEEK_SET);
	error_handling(fp,__LINE__ - 1);
	return(1);
}


int f_delete(char obj_name[],char db_name[],FILE **fp,struct list_names *result) {
	int i = 0,start_pos = 0,overwrite_pos = 0,file_size = 0,res = 0;
	int end_pos = 0,div = 0,mod = 0,orig_file_size = 0;
	char transfer[512],check_obj_name[256],testing[256];
	char test = '\0',check_for_null = 'a';
	for (i = 0; i < 512; i++) {
		transfer[i] = '\0';
	}
	for (i = 0; i < 256; i++) {
		check_obj_name[i] = '\0';
	}
	if (*fp == NULL) {
		return(0);
	}
	find(obj_name,fp,result);
	fseek(*fp,-1*(strlen(obj_name)+1),SEEK_CUR);
	fread(check_obj_name,sizeof(char),strlen(obj_name)+1,*fp);
	error_handling(fp,__LINE__ - 1);
	if (strcmp(obj_name,check_obj_name) != 0) {
		return(-1);
	}
	
	fseek(*fp,-1*(strlen(obj_name) + 1),SEEK_CUR);
	error_handling(fp,__LINE__ - 1);
	start_pos = ftell(*fp);
	fseek(*fp,(strlen(obj_name) + 1),SEEK_CUR);
	error_handling(fp,__LINE__ - 1);
	fread(&file_size,sizeof(int),1,*fp);
	orig_file_size = file_size;
	error_handling(fp,__LINE__ - 1);
	fseek(*fp,file_size,SEEK_CUR);
	error_handling(fp,__LINE__ - 1);
	overwrite_pos = ftell(*fp);
	while (fread(&test,sizeof(char),1,*fp) == 1) {
		for (i = 0; i < 512; i++) {
		transfer[i] = '\0';
		}
		fseek(*fp,overwrite_pos,SEEK_SET);
		error_handling(fp,__LINE__ - 1);
		i = 0;
		check_for_null = 'a';
		while (check_for_null != '\0') {
			fseek(*fp,0L,SEEK_CUR);
			error_handling(fp,__LINE__ - 1);
			fread(&testing[i],sizeof(char),1,*fp);
			error_handling(fp,__LINE__ - 1);
			check_for_null = testing[i];
			i++;
		}
		fseek(*fp,start_pos,SEEK_SET);
		error_handling(fp,__LINE__ - 1);
		fwrite(testing,sizeof(char),strlen(testing) + 1,*fp);fflush(*fp);
		error_handling(fp,__LINE__ - 1);
		fseek(*fp,overwrite_pos + strlen(testing) + 1,SEEK_SET);
		error_handling(fp,__LINE__ - 1);
		fread(&file_size,sizeof(int),1,*fp);
		error_handling(fp,__LINE__ - 1);
		fseek(*fp,start_pos + strlen(testing) + 1,SEEK_SET);
		error_handling(fp,__LINE__ - 1);
		fwrite(&file_size,sizeof(int),1,*fp);fflush(*fp);
		error_handling(fp,__LINE__ - 1);
		div = file_size / 512;
		mod = file_size % 512;
		if (div > 0) {
			for (i = 0; i < div; i++) {
				fseek(*fp,overwrite_pos + strlen(testing) + 1 +sizeof(int),SEEK_SET);
				fread(transfer,sizeof(char),512,*fp);
				error_handling(fp,__LINE__ - 1);
				fseek(*fp,start_pos + strlen(testing) + 1 + sizeof(int),SEEK_SET);
				error_handling(fp,__LINE__ - 1);
				fwrite(transfer,sizeof(char),512,*fp);fflush(*fp);
				error_handling(fp,__LINE__ - 1);
				start_pos = start_pos + 512;
				overwrite_pos = overwrite_pos + 512;
			}
		}
		fseek(*fp,overwrite_pos + strlen(testing) + 1 +sizeof(int),SEEK_SET);
		error_handling(fp,__LINE__ - 1);
		fread(transfer,sizeof(char),mod,*fp);
		overwrite_pos = overwrite_pos + mod + strlen(testing) + 1 +sizeof(int);
		overwrite_pos = ftell(*fp);
		error_handling(fp,__LINE__ - 1);
		fseek(*fp,start_pos + strlen(testing) + 1 + sizeof(int),SEEK_SET);
		error_handling(fp,__LINE__ - 1);
		fwrite(transfer,sizeof(char),mod,*fp);fflush(*fp);
		error_handling(fp,__LINE__ - 1);
		start_pos = start_pos + mod + strlen(testing)+ 1+sizeof(int);
		start_pos = ftell(*fp);
		fseek(*fp,overwrite_pos,SEEK_SET);
		error_handling(fp,__LINE__ - 1);
	}
	fseek(*fp,0L,SEEK_END);
	error_handling(fp,__LINE__ - 1);
	end_pos = ftell(*fp);
	res = fclose(*fp);
	if (res != 0) {
		perror("fclose failed");
		exit(-1);
	}
	*fp = NULL;
	res = truncate(db_name,end_pos-orig_file_size-strlen(obj_name)-1-sizeof(int));
	if (res != 0) {
		perror("truncate failed");
		exit(-1);
	}
	*fp = fopen(db_name,"r+b");
	error_handling(fp,__LINE__ - 1);
	return(1);
}


int close_p(FILE **fp) {
	int res = 0;
	if (*fp == NULL) {
		return(0);
	}
	res = fclose(*fp);
	if (res != 0) {
		perror("close_p function failed");
		exit(-1);
	}
	*fp = NULL;
	return(1);
}


void quit(FILE **fp) {
	if (*fp != NULL) {
		close_p(fp);
	}
}
















