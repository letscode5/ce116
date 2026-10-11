#include<stdio.h>
#include"objdb.h"


int main(int argc,char *argv[]) {
	int res = 0,i = 0;
	FILE *fp = NULL,*exp_ptr = NULL;
	char user_choice;
	struct list_names result;
	char db_name[256],f_name[256],obj_name[256],name[256];
	while (1) {
		scanf(" %c",&user_choice);
		if (user_choice == 'o') {
			scanf("%255s",db_name);
			res = open(db_name,&fp);
			if (res == 0) {
				printf("\nError opening %s.\n",db_name);
			}
			if (res == -1) {
				printf("\nInvalid db file %s.\n",db_name);
			}
		}
		if (user_choice == 'i') {
			scanf("%255s",f_name);
			scanf("%255s",obj_name);
			res = import(f_name,obj_name,&fp,&result);
			if (res == 0) {
				printf("\nNo open db file.\n");
			}
			if (res == -1) {
				printf("\nFile %s not found.\n",f_name);
			}
			if (res == -2) {
				printf("\nObject %s already in db.\n",obj_name);
			}
		}
		if (user_choice == 'f') {
			scanf("%s",name);
			res = find(name,&fp,&result);
			if (res == 0) {
				printf("\nNo open db file.\n");
			}
			if (res == 1) {
				printf("\n##\n");
				for (i = 0; result.list_names[i][0] != '\0'; i++) {
					printf("%s\n",result.list_names[i]);
				}
			}
		}
		if (user_choice == 'e') {
			scanf("%255s",obj_name);
			scanf("%255s",f_name);
			res = f_export(obj_name,f_name,&fp,&exp_ptr,&result);
			if (res == -2) {
				printf("\nObject %s not in db.\n",obj_name);
			}
			if (res == -1) {
				printf("\nCannot open file %s.\n",f_name);
			}
			if (res == 0) {
				printf("\nNo open db file.\n");
			}
		}
		if (user_choice == 'd') {
			scanf("%s",obj_name);
			res = f_delete(obj_name,db_name,&fp,&result);
			if (res == 0) {
				printf("\nNo open db file.\n");
			}
			if (res == -1) {
				printf("\nObject %s not in db.\n",obj_name);
			}
			
		}
		if (user_choice == 'c') {
			res = close_p(&fp);
			if (res == 0) {
				printf("\nNo open db file.\n");
			}
		}
		if (user_choice == 'q') {
			quit(&fp);
			return(0);
		}
	}
	return(0);
}
