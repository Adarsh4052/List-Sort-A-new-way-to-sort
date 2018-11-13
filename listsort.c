#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <inttypes.h>


typedef struct Node Node;
typedef struct List List;

int *random_data;
int element_count, seed, num_range, max_lists;

//profiling
int merge_calls, insert_calls, createlist_calls;
double merge_time, insert_time, createlist_time;

//estimation
unsigned int merge_comparisons=0, merge_shifts=0;

struct Node
{
    int data;
    Node *next;
};


struct List
{
    Node *head;
    Node *tail;
    List *next;
    List *prev;
    int position;
    int size;
};
List *first_l;
/*
void generate_data(int max)
{
	int i, x;
	time_t t;

	if(seed > 0)
		srand(time(&t));

	for(i=0; i<max; i++)
	{
		x = rand() % num_range;
		random_data[i] = x;
	}
}
*/
void print_list(List *su_l)
{	int i=0;
    Node *n;
    n = su_l->head;
    while(n != NULL)
    {	i++;
        printf("\t->%d", n->data);
        n = n->next;
    }
    printf("\t[%d]", i);
}

void print_all_lists()
{
	List *l;
	l = first_l;

	while(l!=NULL)
	{	//printf("\nList[%d](%u) \t next(%u)", l->position, l, l->next);
		printf("\nList[%d]", l->position);
		print_list(l);
		l = l->next;
	}
	printf("\n-------");
}
List *create_list(List **parent_l)
{
	List *tmp_l;
	//time_t t1, t2;

	//createlist_calls++;
	//t1 = clock();

	tmp_l = (List *) malloc(sizeof(List));
	tmp_l->head = NULL;
	tmp_l->tail = NULL;
	tmp_l->next = NULL;
	tmp_l->position = (*parent_l)->position + 1;
	tmp_l->size = 0;

	if(*parent_l == NULL)
	{
		//printf("\nNull parent!");
		tmp_l->prev = NULL;
	}
	else
	{
		//printf("\nNot Null parent!");
		tmp_l->prev = *parent_l;
		(*parent_l)->next = tmp_l;
	}

	//t2 = clock();

	//createlist_time += ((t2-t1)/(double)CLOCKS_PER_SEC);
	return tmp_l;
}

void merge(List **current_l)
{
	List *prev_l;
	Node *a, *b, *c, *d;
	//	int x=0;

	//merge_calls++;

	//if we are already at first list, return calmly :P
	if((*current_l)->position > 1)
	{
		prev_l = (*current_l)->prev;

		//printf("\nMerging [%d->(%d)] with [%d->(%d)]: ", (*current_l)->position, (*current_l)->size, prev_l->position, prev_l->size);
		//print_all_lists();

		/* We always merge the smaller list into bigger list */

		//if current_l is larger than previous, then merge prev_l into current_l and delete prev_l
		if((*current_l)->size > prev_l->size)
		{	//printf("\n\nlower larger by %d\n", (*current_l)->size - prev_l->size);
			/* prev_l will always have atleast 2 elements due to above if condition */
			c = prev_l->head;
			d = c->next;
			b = (*current_l)->head;
			//now append c before b and make it new head of current_l
			c->next = b;
			(*current_l)->head = c;
			(*current_l)->size++;	//we added c to it
			//now prepare for diving into while loop
			a = c;
			c = d;

			//now compare and insert c between a and b if c->data
			while( b != NULL)
			{
				if(c->data <= b->data)
				{

					/*	prepend c before b */
					a->next = c;
					d = c->next;
					c->next = b;
					a = c;
					c = d;
					(*current_l)->size++;
					//merge_shifts++;
				}
				else
				{	a = b;
					b = b->next;
				}
				//merge_comparisons++;
			}
			//append all the left over elements of prev_l at the end of *current_l


			a->next = c;	//b is NULL now
			//count the remaining nodes after c and add it to size of current_l
			while(c!=NULL)
			{
				((*current_l)->size)++;
				c = c->next;
			}

			/*	now prepare to delete prev_l	*/
			(*current_l)->tail = prev_l->tail;
			(*current_l)->prev = prev_l->prev;
			(*current_l)->position = prev_l->position;

			if(prev_l->prev != NULL)
				prev_l->prev->next = (*current_l);
			else	//else prev_l is the first pointer. update first_l to point to current_l
				first_l = (*current_l);

			free(prev_l);
		}
		//else current_l is smaller than prev_l. so merge current_l into prev_l and free current_l
		else
		{	//printf("\n\nupper larger by %d\n", (*current_l)->size - prev_l->size);
			c = (*current_l)->head;
			a = prev_l->head;
			b = a->next;
						d = c->next;
			while(c != NULL)
			{
				if(c->data < b->data)
				{	a->next = c;
					d = c->next;
					c->next = b;
					a = c;
					c = d;
					prev_l->size++;
					//merge_shifts++;
				}
				else
				{	a = b;
					b = b->next;
				}
				//merge_comparisons++;
			}

			free((*current_l));
			prev_l->next = NULL;
			(*current_l) = prev_l;
		}

		// now recursively call merge with current position
		//printf("\n\nAfter Merging: ");
		//print_all_lists();

		merge(current_l);
	}
}

void insert(int data, List **su_l)
{
    Node *tmp;
    List *next_l;
	//time_t t_m1, t_m2;

	//insert_calls++;
    //create a new node and fill in the data

	//printf("\nInserting %d in [%d]", data, (*su_l)->position);
    //if list is empty yet, make the head and tail point to tmp.
    if( (*su_l)->head == NULL )
    {
		tmp = (Node *) malloc(sizeof(Node));
		tmp->data = data;

    	tmp->next = NULL;
        (*su_l)->head = tmp;
        (*su_l)->tail = tmp;
        insert_comparisons++;
    }
	else
	{
		//if the data is only less than head, prepend tmp before current head  (if data is equal, it shoud go to next list for stability)
		if(data < (*su_l)->head->data)
		{
			tmp = (Node *) malloc(sizeof(Node));
			tmp->data = data;

			tmp->next = (*su_l)->head;
			(*su_l)->head = tmp;
			(*su_l)->size++;
		}
		else
		{
			//else if data is more than or equal (for stability) to current tail, append tmp after current tail
			if(data >= (*su_l)->tail->data)
			{
				tmp = (Node *) malloc(sizeof(Node));
				tmp->data = data;

				(*su_l)->tail->next = tmp;
				(*su_l)->tail = tmp;
				tmp->next = NULL;
				(*su_l)->size++;
			}
			//otherwise, see if we are at the last permissible node, then merging is required
			else
			{
				if((*su_l)->position == max_lists )		//position starts from 1
				{
					//t_m1 = clock();
					merge(su_l);
					//t_m2 = clock();
					//merge_time += ( (t_m2 - t_m1)/(double) CLOCKS_PER_SEC);
					//printf("\nMergeTime: %f", merge_time);
					//now insert into second list
					next_l = first_l->next;
			        insert_comparisons += 4;
				}
				else
				{
					if((*su_l)->next == NULL)
					{
						//printf("\n---[%d]->next = NULL",(*su_l)->position);
						next_l = create_list(su_l);
					}
					else
					{
						next_l = (*su_l)->next;
					}
					insert_comparisons += 5;
				}
				//now try to insert data into next_l as it could not be fitted in current list (su_l)
				insert(data, &next_l);
			}
		}
	}

	return;
}



int main(int argc,char *argv[])
{
	List *l, *l2;
	int i;
	clock_t start, end;
	clock_t t1, t2;
	double sort_time;
	long unsigned ticks;

	//profiling
/*	merge_calls = 0;
	insert_calls = 0;
	createlist_calls = 0;
	merge_time = 0;
	insert_time = 0;
	createlist_time = 0;

	merge_comparisons = 0;
	insert_comparisons = 0;
*/
	if(argc < 4)
	{
		printf("\nProper format is \n%s [element-count] [num-range] [max-lists] [seed]\n\n", argv[0]);
		return 0;
	}
	else
	{
		element_count = (int) atoi(argv[1]);
		num_range = (int) atoi(argv[2]);
		max_lists = (int) atoi(argv[3]);
		seed = (int) atoi(argv[4]);
	}
	//random_data = (int *) malloc(sizeof(int)*element_count);
	//start = clock();
	// generate_data(element_count);
	//end = clock();

	//time_spent = (float) ((end - start) / CLOCKS_PER_SEC);
	//printf("\nRandom data generated in :  %f seconds", time_spent);

	first_l = ( (List *) malloc(sizeof(List)) );
	first_l->head = NULL;
	first_l->tail = NULL;
	first_l->prev = NULL;
	first_l->next = NULL;
	first_l->position = 1;	//position starts from 1

	if(seed > 0)
			srand(time(&t1));

	start = clock();
	for(i=0; i<element_count; i++)
	{
		int x = rand() % num_range;
		//random_data[i] = x;
	   //	printf("\n\nMain insertion: ->%d",x);
		insert(x, &first_l);
		//print_all_lists();
	}

	l = first_l;
	while(l!=NULL)
	{
		l2 = l;
		l = l->next;
	}

	//t1 = clock();
	merge(&l2);
	//t2 = clock();

	end = clock();

	sort_time = ( (end - start) / ((double) CLOCKS_PER_SEC));
	//ticks =  end - start;

	//insert_time = time_spent - merge_time;

	printf("\nTime: \t\t\t%5.5f seconds \nMerge Comparisons: \t%lu \nMerge Shifts: \t\t%lu\n", sort_time, merge_comparisons, merge_shifts);

/*	printf("\nFor total elements= %d \tnumber range= %d \tnumber of lists= %d \tseed= %d", element_count, num_range, max_lists, seed);
	printf("\nMerging took %5.5f seconds in %d calls \t final call: %f seconds", (merge_time), merge_calls, ( (t2-t1) / ((double) CLOCKS_PER_SEC)) );
	printf("\nInsertion and others took (%5.5f) seconds in %d calls", (time_spent -merge_time -createlist_time), insert_calls);
	printf("\nCreating Lists took (%5.5f) seconds in %d calls",createlist_time ,  createlist_calls );
	printf("\nMerge Comaprisons: %d\n\n", merge_comparisons);
*/
	//print_all_lists();
	printf("\n--end--\n");
    return 0;
}


