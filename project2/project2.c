#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<time.h>
#include<string.h>


struct entry  {
	long unsigned int aem;
    char name[64];
    short unsigned int lessons_failed;
    struct lesson *head;
    struct entry *next;
    struct entry *prev;
};
typedef struct entry entry;


struct lesson {
    unsigned short course_nr;
    struct lesson *next;
};


typedef struct {
    entry **hashtable;
    double size;
    int min_size;
    double load_factor;
} hashing;


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


unsigned long hash_func(char *name)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *name++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}


unsigned long find_pos(char *name,int hashtable_size) {
    unsigned long res;
    int hash_pos; 
    res = hash_func(name);
    hash_pos = res % hashtable_size;
    return(hash_pos);
}


void hash_init(entry **head) {
    *head = (entry*)calloc(1,sizeof(entry));
    (*head)->aem = 0;
    (*head)->next = *head;
    (*head)->prev = *head;
}


int hash_len(entry *head) {
    int len = 0;
    struct entry *p = NULL;
    if (head != NULL) {
        for (p = head->next; p != head; p = p->next) {
            len = len + 1;
        }
    }
    return(len);
}


int find_larg_bucket(hashing hash_data) {
    int i = 0,len,len_larg;
    len_larg = hash_len(hash_data.hashtable[i]);
    for (i = 1; i < hash_data.size; i++) {
        len = hash_len(hash_data.hashtable[i]);
        if (len > len_larg) {
            len_larg = len;
        }
    }
    return(len_larg);
}


void hash_find_by_name(entry *head,char name[],int *hash_compar) {
	
    struct entry *p;
    for (p = head->next; p != head && strcmp(p->name,name) != 0; p = p->next,(*hash_compar)++);
    if (p == head) {
        printf("\nN-NOK %s\n",name);
        return;
    }
    printf("\nN-OK %s\n",name);
    for(; p != head && strcmp(p->name,name) == 0; p = p->next) {
        printf("%ld %hd\n",p->aem,p->lessons_failed);
    }
    (*hash_compar)++;
}


void hash_clear(entry **head) {
    if (*head != NULL)  {
        entry *p = (*head)->next;
        while (p != *head) {
            p = p->next;
            free(p->prev);
            p->prev = NULL;
        }
        free(*head);
        *head = NULL;
    }
}


void hash_add(entry **head,entry *temp) {
    if (*head == NULL) {
        hash_init(head);
    }
    entry *p = *head;
    if ((*head)->next != (*head)) {
        for (p = (*head); strcmp(p->next->name,temp->name) < 0 && p->next != (*head); p = p->next);
        if (strcmp(p->next->name,temp->name) == 0) {
            for (; p->next->aem < temp->aem && p->next != (*head) && strcmp(p->next->name,temp->name) == 0; p = p->next);
        }
    }
    if (p != (*head) && p->next == (*head)) {
        temp->next = (*head);
        temp->prev = p;
        p->next = temp;
        (*head)->prev = temp;
        return;
    }
    temp->prev = p;
    temp->next = p->next;
    p->next = temp;
    temp->next->prev = temp;
}


void hash_remove(entry *head,long unsigned int aem) {
    entry *p;
    for (p = head->next; p != head && p->aem != aem; p = p->next);
    p->prev->next = p->next;
    p->next->prev = p->prev;
}


void clear_list(struct lesson **head) {
    struct lesson *temp;
    struct lesson *p;
    p = *head;
    while (p != NULL) {
        temp = p->next;
        free(p);
        p = temp;
    }
    *head = NULL;
}


void find_by_name(hashing hash_data,char name[],int *hash_compar) {
    int i;
    for (i = 0; name[i] != '\0'; i++) {
        name[i] = toupper(name[i]);
    }
    int hash_pos;
    hash_pos = find_pos(name,hash_data.size);
    hash_find_by_name(hash_data.hashtable[hash_pos],name,hash_compar);
	fprintf(stderr, "\n$%d\n", *hash_compar);
}


void rehashing(hashing *hash,int *entries,entry **array_ptrs) {
    int i,hash_pos;
    entry **new_hashtable;
    if (hash->load_factor >= 4) {
        hash->size = 2*hash->size;
        new_hashtable = (entry**) calloc(hash->size,sizeof(entry*));
        if (new_hashtable == NULL) {
            return;
        }
        for (i = 0; i < *entries; i++) {
            hash_pos = find_pos(array_ptrs[i]->name,hash->size);
            hash_add(&(new_hashtable[hash_pos]),&(*(array_ptrs[i])));
        }
        for (i = 0; i < (hash->size)/2; i++) {
            free(hash->hashtable[i]);
            hash->hashtable[i] = NULL;
        }
        free(hash->hashtable);
        hash->hashtable = new_hashtable;
        hash->load_factor = (*entries)/(hash->size);
        return;
    }
    if (hash->load_factor == 1 && (hash->size)/2 >= hash->min_size) {
        hash->size = hash->size/2;
        new_hashtable = (entry**) calloc(hash->size,sizeof(entry*));
        if (new_hashtable == NULL) {
            return;
        }
        for (i = 0; i < *entries; i++) {
            hash_pos = find_pos(array_ptrs[i]->name,hash->size);
            hash_add(&(new_hashtable[hash_pos]),&(*(array_ptrs)[i]));
        }
        for (i = 0; i < 2*hash->size; i++) {
            free(hash->hashtable[i]);
            hash->hashtable[i] = NULL;
        }
        free(hash->hashtable);
        hash->hashtable = new_hashtable;
        hash->load_factor = (*entries)/(hash->size);
    }
}


int add(entry ***array_ptrs,int *array_size,int size_change,long unsigned int aem,short unsigned int lessons_failed,char name[],int *entries,int no_fcomps,int *pos,hashing *hash) {
    int k = 0,i = 0,new_size,hash_pos;
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
        hash_pos = find_pos(name,hash->size);
        hash_add(&(hash->hashtable[hash_pos]),&(*(*array_ptrs)[*entries-1]));
        hash->load_factor = (*entries) / (hash->size);
        if (hash->load_factor >= 4) {
            rehashing(hash,entries,*array_ptrs);
        }
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
    hash_pos = find_pos((*array_ptrs)[*entries-1]->name,hash->size);
    hash_add(&(hash->hashtable[hash_pos]),&(*(*array_ptrs)[*entries-1]));
    hash->load_factor = (*entries) / (hash->size);
    if (hash->load_factor >= 4) {
        rehashing(hash,entries,*array_ptrs);
    }
    return(1);
}


int rmv(entry **array_ptrs,long unsigned int aem,int *entries,int *array_size,int size_change,int no_fcomps,int *pos,hashing *hash) {
    entry **temp = NULL;
    int new_size = 0,hash_pos;
    find0(array_ptrs,aem,entries,no_fcomps,pos);
    if (*pos == *entries || *pos == -1) {
        return(0);
    }
    *entries = *entries - 1;
    hash_pos = find_pos(array_ptrs[*pos]->name,hash->size);
    hash_remove(hash->hashtable[hash_pos],aem);
    hash->load_factor = (*entries) / (hash->size);
    clear_list(&(array_ptrs[*pos]->head));
    free(array_ptrs[*pos]);
    array_ptrs[*pos] = array_ptrs[*entries];
    array_ptrs[*entries] = NULL;
    if (hash->load_factor == 1) {
        rehashing(hash,entries,array_ptrs);
    }
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
    printf("\nL-OK %s\n",array_ptrs[*pos]->name);
    for (p = array_ptrs[*pos]->head; p != NULL; p = p->next) {
        printf("%hd\n",p->course_nr);
    }
}


int reg(struct lesson **head,int course_nr) {
    struct lesson *temp,*p;
    temp = calloc(1,sizeof(struct lesson));
    if (temp == NULL) {
        return(0);
    }
    temp->course_nr = course_nr;
    if (*head == NULL || temp->course_nr < (*head)->course_nr) {
        temp->next = *head;
        *head = temp;
        return(-1);
    }
    for (p = *head; p->next != NULL && p->next->course_nr < course_nr; p = p->next);
    temp->next = p->next;
    p->next = temp;
    return(-1);
}


int unreg(struct lesson **head,int course_nr) {
    struct lesson *p = NULL;
    struct lesson *prev = NULL;
    for (p = *head; p != NULL && p->course_nr != course_nr; prev = p,p = p->next);
    if (p == *head) {
        *head = p->next;
        free(p);
        return(1);
    }
    prev->next = p->next;
    free(p);
    return(1);
}


void clear (entry ***array_ptrs,int *array_size,int *entries,hashing *hash) {
    int i;
    entry **temp = NULL;
    for (i = 0; i < *array_size; i++) {
        if ((*array_ptrs)[i] != NULL) {
            clear_list(&(*array_ptrs)[i]->head);
            free((*array_ptrs)[i]);
            (*array_ptrs)[i] = NULL;
        }
    }
    free(*array_ptrs);
    *array_ptrs = NULL;
    *array_size = 0;
    *entries = 0;
    for (i = 0; i < hash->size; i++) {
        free(hash->hashtable[i]);
        hash->hashtable[i] = NULL;
    }
    do {
    temp = realloc(hash->hashtable,hash->min_size*sizeof(entry*));
    } while (temp == NULL);
    hash->hashtable = temp;
    hash->load_factor = 0;
    hash->size = hash->min_size;
}


void print(hashing hash_data,int *entries) {
    struct entry *p;
    int len_larg,i,len;
    len_larg = find_larg_bucket(hash_data);
    printf("\n#\n");
    printf("%d %d %.2lf %d\n",(int)hash_data.size,*entries,hash_data.load_factor,len_larg);
    for (i = 0; i < hash_data.size; i++) {
        printf("%d ",i);
        len = hash_len(hash_data.hashtable[i]);
        printf("%d",len);
        if (hash_data.hashtable[i] != NULL) {
            for (p = hash_data.hashtable[i]->next; p != hash_data.hashtable[i]; p = p->next) {
                printf(" [%ld %s %hd]",p->aem,p->name,p->lessons_failed);
            }
        }
        printf("\n\n");
    }
}


void quit(entry ***array_ptrs,int *array_size,int *entries,hashing *hash) {
    clear(array_ptrs,array_size,entries,hash);
    free(hash->hashtable);
}


int main(int argc,char *argv[]) {
    int array_size,size_change,entries = 0,res,no_fcomps = 0,pos = 0;
    long unsigned int  no_scomps;
    long unsigned int aem;
    unsigned short course_nr;
    short unsigned int lessons_failed;
    char user_choice,name[64];
    hashing hash_data;
    array_size = atoi(argv[1]);
    size_change = atoi(argv[2]);
    hash_data.size = atoi(argv[3]);
    hash_data.min_size = hash_data.size;
    entry **array_ptrs = NULL;
    array_ptrs = (entry**) calloc(array_size,sizeof(entry*));
    hash_data.hashtable = (entry**) calloc(hash_data.size,sizeof(entry*));
    
    while (1) {
        scanf(" %c",&user_choice);
        if (user_choice == 'a') {
            scanf("%lu",&aem);
            scanf("%63s",name);
            scanf("%hu",&lessons_failed);
            res = add(&array_ptrs,&array_size,size_change,aem,lessons_failed,name,&entries,no_fcomps,&pos,&hash_data);
            if (res == 0 || res == -1) {
                printf("\nA-NOK %ld, %d %d\n",aem,entries,array_size);
            }
            if (res == 1) {
                printf("\nA-OK %ld, %d %d\n",array_ptrs[entries-1]->aem,entries,array_size);
            }
        }
        
        if (user_choice == 'r') {
            scanf("%ld",&aem);
            res = rmv(array_ptrs,aem,&entries,&array_size,size_change,no_fcomps,&pos,&hash_data);
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
            res = isreg(array_ptrs,course_nr,&entries,no_fcomps,&pos,aem);
            if (res == 0) {
                printf("\nG-NOK %ld\n",aem);
            }
            if (res == 1) {
                printf("\nG-NOK %hd\n",course_nr);
            }
            if (res == -1) {
                reg(&(array_ptrs[pos]->head),course_nr);
                printf("\nG-OK %ld %hd\n",aem,course_nr);
            }
        }
        
        if (user_choice == 'u') {
            scanf("%ld",&aem);
            scanf("%hd",&course_nr);
            res = isreg(array_ptrs,course_nr,&entries,no_fcomps,&pos,aem);
            if (res == 0) {
                printf("\nU-NOK %ld\n",aem);
            }
            if (res == -1) {
                printf("\nU-NOK %hd\n",course_nr);
            }
            if (res == 1) {
                unreg(&(array_ptrs[pos]->head),course_nr);
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
            print(hash_data,&entries);
        }
        
        if (user_choice == 'n') {
            scanf("%63s",name);
			int hash_compar = 0;
			struct timespec start, end;
			unsigned long elapsed_time;
			clock_gettime(CLOCK_MONOTONIC_RAW, &start);
			
            find_by_name(hash_data,name,&hash_compar);
			
			clock_gettime(CLOCK_MONOTONIC_RAW, &end);
			elapsed_time = (end.tv_sec - start.tv_sec) * 1000000
                + (end.tv_nsec - start.tv_nsec) / 1000;
			fprintf(stderr, "%lu microseconds\n", elapsed_time);
        }
        
        if (user_choice == 'c') {
            clear(&array_ptrs,&array_size,&entries,&hash_data);
            printf("\nC-OK\n");
        }
        
        if (user_choice == 'q') {
            quit(&array_ptrs,&array_size,&entries,&hash_data);
            return(0);
        }
    }
    return (0);    
}
