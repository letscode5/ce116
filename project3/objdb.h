#ifndef _OBJDB_H
#define _OBJDB_H

struct list_names {
	char list_names[25][256];
};


extern void error_handling(FILE **fp,int line);
extern int open (char db_name[],FILE **fp);
extern int base_init(char db_name[],FILE **fp);
extern int f_exists(char filename[]);
extern int import (char f_name[],char obj_name[],FILE **fp,struct list_names *result);
extern int find (char name[],FILE **fp,struct list_names *result);
extern int f_export (char obj_name[],char f_name[],FILE **fp,FILE **exp_ptr,struct list_names *result);
extern int f_delete (char obj_name[],char db_name[],FILE **fp,struct list_names *result); 
extern int close_p (FILE **fp);
extern void quit (FILE **fp);


#endif
