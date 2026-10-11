#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<time.h>


typedef struct  {
	long unsigned int aem;
	char name[64];
	short unsigned int lessons_failed;
	struct lesson *head;
} entry;


struct lesson {
	unsigned short course_nr;
	struct lesson *next;
};


void clear_list(struct lesson **head) {
	struct lesson *temp;
	struct lesson *p = *head;
	while (p != NULL) {
		temp = p->next;
		free(p);
		p = temp;
	}
	*head = NULL;
}


int issort(entry **array_ptrs,int *entries) {
	int sorted = 1,i;
	for (i = 0; i < *entries - 1; i++) {
		if (array_ptrs[i+1]->aem < array_ptrs[i]->aem) {
			sorted = 0;
		}
	}
	return(sorted);
}


int linear_search (entry **array_ptrs,long unsigned int aem,int *entries,int no_fcomps,int *pos) {
	for (*pos = 0; *pos < *entries; (*pos)++) {
		no_fcomps +=1;
		if (array_ptrs[*pos]->aem == aem) {
			return(no_fcomps);
		}
	}
	return(no_fcomps); 
}


int binary_search (entry **array_ptrs,long unsigned int aem,int *entries,int no_fcomps,int *pos) {
	int from,to,temp = 0;
	from = 0;
	to = *entries-1;
	while (from <= to) {
		temp = (from + to)/2;
		no_fcomps += 1;
		if (array_ptrs[temp]->aem == aem) {
			*pos = temp;
			return(no_fcomps);
		}
		no_fcomps += 1;
		if (array_ptrs[temp]->aem < aem) {
			from = temp + 1;
		}
		else {
			to = temp - 1;
		}
	}
	*pos = -1;
	return(no_fcomps);
}


int find0 (entry **array_ptrs,long unsigned int aem,int *entries,int no_fcomps,int *pos) {
	int res,sorted;
	sorted = issort(array_ptrs,entries);
	if (sorted == 0) {
		res = linear_search(array_ptrs,aem,entries,no_fcomps,pos);
		return(res);
	}
	res = binary_search(array_ptrs,aem,entries,no_fcomps,pos);
	return(res);
}

int find (entry **array_ptrs,long unsigned int aem,int *entries,int no_fcomps,int *pos) {
	int res = 0;
	res = find0(array_ptrs,aem,entries,no_fcomps,pos);
	if (*pos == *entries || *pos == -1) {
		*pos = -1;
	}
	return(res);
}


int add(entry ***array_ptrs,int *array_size,int size_change,long unsigned int aem,short unsigned int lessons_failed,char name[],int *entries,int no_fcomps,int *pos) {
	int k = 0,i = 0,new_size;
	entry **temp = NULL;
	if (*entries != 0) {
		find0(*array_ptrs,aem,entries,no_fcomps,pos);
		if (*pos < *entries && *pos > -1) {
			return(-1);
		}
	}
	if (*entries < *array_size) {
		(*array_ptrs)[*entries] = calloc(1,sizeof(entry));
		if ((*array_ptrs)[*entries] == NULL) {
			return (0);
		}
		(*array_ptrs)[*entries]->aem = aem;
		k = strlen(name);
		for (i = 0; i < k; i++) {
			name[i] = toupper(name[i]);
		}
		strcpy((*array_ptrs)[*entries]->name,name);
		(*array_ptrs)[*entries]->lessons_failed = lessons_failed;
		*entries = *entries + 1;
		return(1);
	}
	*array_size = *array_size + size_change;
	new_size = *array_size;
	temp = (entry**) realloc(*array_ptrs,new_size*sizeof(entry*));
	if (temp == NULL) {
		return(0);
	}
	*array_ptrs = temp;
	(*array_ptrs)[*entries] = calloc(1,sizeof(entry));
	if ((*array_ptrs)[*entries] == NULL) {
		return (0);
	}
	(*array_ptrs)[*entries]->aem = aem;
	k = strlen(name);
	for (i = 0; i < k; i++) {
		name[i] = toupper(name[i]);
	}
	strcpy((*array_ptrs)[*entries]->name,name);
	(*array_ptrs)[*entries]->lessons_failed = lessons_failed;
	*entries = *entries + 1;
	return(1);
}


int rmv(entry **array_ptrs,long unsigned int aem,int *entries,int *array_size,int size_change,int no_fcomps,int *pos) {
	entry **temp = NULL;
	int new_size = 0;
	find0(array_ptrs,aem,entries,no_fcomps,pos);
	if (*pos == *entries || *pos == -1) {
		return(0);
	}
	*entries = *entries - 1;
	clear_list(&(array_ptrs[*pos]->head));
	free(array_ptrs[*pos]);
	array_ptrs[*pos] = array_ptrs[*entries];
	array_ptrs[*entries] = NULL;
	if (*array_size - *entries > size_change) {
		*array_size = *array_size - size_change;
		new_size = *array_size;
		temp = (entry**) realloc(array_ptrs,new_size*sizeof(entry*));
		if (temp == NULL) {
			return(0);
		}
		array_ptrs = temp;
	}
	return(1);
}


void print(entry **array_ptrs,int *entries) {
	int i;
	printf("\n#\n");
	for (i = 0; i < *entries; i++) {
		printf("%ld %s %hd\n",array_ptrs[i]->aem,array_ptrs[i]->name,array_ptrs[i]->lessons_failed);
	}
}


int mod(entry **array_ptrs,long unsigned int aem,short unsigned int lessons_failed,int *entries,int no_fcomps,int *pos) {
	find0(array_ptrs,aem,entries,no_fcomps,pos);
	if (*pos == *entries || *pos == -1) {
		*pos = -1;
		return(*pos);
	}
	array_ptrs[*pos]->lessons_failed = lessons_failed;
	return(*pos);
}


long unsigned int sort(entry **array_ptrs,int *entries) {
	int i,k;
	long unsigned int no_scomps = 0;
	entry *temp = NULL;
	for (i = 1; i < *entries; i++) {
		k = i;
		if (k > 0 && array_ptrs[k-1]->aem <= array_ptrs[k]->aem) {
			no_scomps++;
		}
		while (k > 0 && array_ptrs[k-1]->aem > array_ptrs[k]->aem) {
			no_scomps++;
			temp = array_ptrs[k];
			array_ptrs[k] = array_ptrs[k-1];
			array_ptrs[k-1] = temp;
			k--;
		}
	}
	return(no_scomps);
}


void clear (entry ***array_ptrs,int *array_size,int *entries) {
	int i;
	for (i = 0; i < *array_size; i++) {
		if ((*array_ptrs)[i] != NULL) {
			clear_list(&(*array_ptrs[i])->head);
			free((*array_ptrs)[i]);
			(*array_ptrs)[i] = NULL;
		}
	}
	free(*array_ptrs);
	*array_ptrs = NULL;
	*array_size = 0;
	*entries = 0;
}


int isreg(entry **array_ptrs,int course_nr,int *entries,int no_fcomps,int *pos,int aem) {
	find0(array_ptrs,aem,entries,no_fcomps,pos);
	if (*pos == *entries || *pos == -1) {
		return(0);
	}
	struct lesson *p;
	for (p = array_ptrs[*pos]->head; p != NULL && p->course_nr != course_nr; p = p->next);
	if (p == NULL) {
		return(-1);
	}
	return(1);
}


void list_courses(entry **array_ptrs,int *entries,int no_fcomps,int *pos,long unsigned int aem) {
	find0(array_ptrs,aem,entries,no_fcomps,pos);
	if (*pos == *entries || *pos == -1) {
		printf("\nL-NOK %ld\n",aem);
		return;
	}
	struct lesson *p;
	printf("\nL-OK\n");
	for (p = array_ptrs[*pos]->head; p != NULL; p = p->next) {
		printf("%hd\n",p->course_nr);
	}
}

int reg(entry **array_ptrs,int course_nr,int *entries,int no_fcomps,int *pos,long unsigned int aem) {
	int res;
	res = isreg(array_ptrs,course_nr,entries,no_fcomps,pos,aem);
	if (res == 0 || res == 1) {
		return(res);
	}
	struct lesson *temp,*p;
	temp = calloc(1,sizeof(struct lesson));
	temp->course_nr = course_nr;
	if (array_ptrs[*pos]->head == NULL || temp->course_nr < array_ptrs[*pos]->head->course_nr) {
		temp->next = array_ptrs[*pos]->head;
		array_ptrs[*pos]->head = temp;
		return(-1);
	}
	for (p = array_ptrs[*pos]->head; p->next != NULL && p->next->course_nr < course_nr; p = p->next);
	temp->next = p->next;
	p->next = temp;
	return(-1);
}


int unreg(entry **array_ptrs,int course_nr,int *entries,int no_fcomps,int *pos,long unsigned int aem) {
	int res;
	res = isreg(array_ptrs,course_nr,entries,no_fcomps,pos,aem);
	if (res == 0 || res == -1) {
		return(res);
	}
	struct lesson *p = NULL;
	struct lesson *prev = NULL;
	for (p = array_ptrs[*pos]->head; p != NULL && p->course_nr != course_nr;prev = p,p = p->next);
	if (p == array_ptrs[*pos]->head) {
		array_ptrs[*pos]->head = p->next;
		free(p);
		return(1);
	}
	prev->next = p->next;
	free(p);
	return(1);
}


int main(int argc,char *argv[]) {
	int array_size,size_change,entries = 0,res,no_fcomps = 0,pos = 0;
	long unsigned int  no_scomps;
	long unsigned int aem;
	unsigned short course_nr;
	short unsigned int lessons_failed;
	char user_choice,name[64];
	array_size = atoi(argv[1]);
	size_change = atoi(argv[2]);
	entry **array_ptrs = NULL;
	array_ptrs = (entry**) calloc(array_size,sizeof(entry*));
	while (1) {
		scanf(" %c",&user_choice);
		if (user_choice == 'a') {
			scanf("%lu",&aem);
			scanf("%63s",name);
			scanf("%hu",&lessons_failed);
			res = add(&array_ptrs,&array_size,size_change,aem,lessons_failed,name,&entries,no_fcomps,&pos);
			if (res == 0 || res == -1) {
				printf("\nA-NOK %ld, %d %d\n",aem,entries,array_size);
			}
			if (res == 1) {
				printf("\nA-OK %ld, %d %d\n",array_ptrs[entries-1]->aem,entries,array_size);
			}
		}
		if (user_choice == 'r') {
			scanf("%ld",&aem);
			res = rmv(array_ptrs,aem,&entries,&array_size,size_change,no_fcomps,&pos);
			if (res == 0) {
				printf("\nR-NOK %ld, %d %d\n",aem,entries,array_size);
			}
			if (res == 1) {
				printf("\nR-OK %ld, %d %d\n",aem,entries,array_size);
			}
		}
		if (user_choice == 'm') {
			scanf("%ld %hd",&aem,&lessons_failed);
			res = mod(array_ptrs,aem,lessons_failed,&entries,no_fcomps,&pos);
			if (res == -1) {
				printf("\nM-NOK %ld\n",aem);
			}
			if (res != -1) {
				printf("\nM-OK %ld\n",array_ptrs[res]->aem);
			}
		}
		if (user_choice ==  's') {
			no_scomps = sort(array_ptrs,&entries);
			printf("\nS-OK\n");
			fprintf(stderr,"\n%lu\n",no_scomps);
		}
		if (user_choice == 'f') {
			scanf("%ld",&aem);
			res = find(array_ptrs,aem,&entries,no_fcomps,&pos);
			if (pos == -1) {
				printf("\nF-NOK %ld\n",aem);
				fprintf(stderr,"\n%d\n",res);
			}
			else {
				printf("\nF-OK %s %hd\n",array_ptrs[pos]->name,array_ptrs[pos]->lessons_failed);
				fprintf(stderr,"\n%d\n",res);
			}
		}
		
		if (user_choice == 'g') {
			scanf("%ld",&aem);
			scanf("%hd",&course_nr);
			res = reg(array_ptrs,course_nr,&entries,no_fcomps,&pos,aem);
			if (res == 0) {
				printf("\nG-NOK %ld\n",aem);
			}
			if (res == 1) {
				printf("\nG-NOK %hd\n",course_nr);
			}
			if (res == -1) {
				printf("\nG-OK %ld %hd\n",aem,course_nr);
			}
		}
		if (user_choice == 'u') {
			scanf("%ld",&aem);
			scanf("%hd",&course_nr);
			res = unreg(array_ptrs,course_nr,&entries,no_fcomps,&pos,aem);
			if (res == 0) {
				printf("\nU-NOK %ld\n",aem);
			}
			if (res == -1) {
				printf("\nU-NOK %hd\n",course_nr);
			}
			if (res == 1) {
				printf("\nU-OK %ld %hd\n",aem,course_nr);
			}
		}
		if (user_choice == 'i') {
			scanf("%ld",&aem);
			scanf("%hd",&course_nr);
			res = isreg(array_ptrs,course_nr,&entries,no_fcomps,&pos,aem);
			if (res == 0) {
				printf("\nI-NOK %ld\n",aem);
			}
			if (res == -1) {
				printf("\nNO\n");
			}
			if (res == 1) {
				printf("\nYES\n");
			}
		}
		if (user_choice == 'l') {
			scanf("%ld",&aem);
			list_courses(array_ptrs,&entries,no_fcomps,&pos,aem);
		}
		if (user_choice == 'p') {
			print(array_ptrs,&entries);
		}
		if (user_choice == 'c') {
			clear(&array_ptrs,&array_size,&entries);
			printf("\nC-OK\n");
		}
		if (user_choice == 'q') {
			clear(&array_ptrs,&array_size,&entries);
			return(0);
		}
	}
	return (0);    
}



















