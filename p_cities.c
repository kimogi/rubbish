#include <stdlib.h>
#include <stdio.h>

#define MAX_HEAP 0
#define MIN_HEAP 1
#define INC_SORT 2
#define DEC_SORT 3

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

typedef struct city_t {
        int pos_in_heap;
        int priority;
        int id;
        int next_id;
	int d_pool_index;
	int ids_sorted_by_d_index;
	int accumed;
	int accum_route_len;
	int accum_max_priority;
	int accum_min_priority;
} City;

City **cities_by_id = NULL;

typedef struct pot_route_t {
        int len;
        int min_priority;
	int cities_size;
	int *cities;
} Pot_Route;

void drop(Pot_Route *pot_route) {
	pot_route->len = 0;
	pot_route->min_priority = -1;
	free(pot_route->cities);
	pot_route->cities = (int *)calloc(pot_route->cities_size, sizeof(int));
}

void add_city(Pot_Route *pot_route, City* city) {
	(pot_route->len)++;
	pot_route->cities[city->id] = 1;
	if (pot_route->min_priority == -1) {
		pot_route->min_priority = city->priority;
	} else if(pot_route->min_priority > city->priority) {
		pot_route->min_priority = city->priority;
	}
}

int contains(Pot_Route *pot_route, City *city) {
	return pot_route->cities[city->id];
}

void remove_city(Pot_Route *pot_route, City *city) {
	(pot_route->len)--;
	pot_route->cities[city->id] = 0;
        if (pot_route->len == 0) {
                pot_route->min_priority = -1;
        }
}

int is_empty(Pot_Route *pot_route) {
	int result = pot_route->len ==0 && pot_route->min_priority == -1 ? 1 : 0;
	return result;
}

int *heapify (int *A, int i, int *V, int heap_size, int heap_mode, int store_positions) {
	int left = 2*i + 1;
	int right = 2*i + 2;
	int largest = i;
	int smallest = i;	

	if (heap_mode == MAX_HEAP) {
		if (left < heap_size && V[A[left]] > V[A[i]]) {
    			largest = left;
		}
  		if (right < heap_size && V[A[right]] > V[A[largest]]) {
   	 		largest = right;
		}
  		if (largest != i) {
    			int buff = A[i];
			A[i] = A[largest];
			A[largest] = buff;
			
			if (cities_by_id != NULL && store_positions) {
				cities_by_id[A[i]]->pos_in_heap = i;
				cities_by_id[A[largest]]->pos_in_heap = largest;
			}

        		A = heapify(A, largest, V, heap_size, heap_mode, store_positions);
		}
	} else if (heap_mode == MIN_HEAP) {
		if (left < heap_size && V[A[left]] < V[A[i]]) {
                        smallest = left;
                }
                if (right < heap_size && V[A[right]] < V[A[smallest]]) {
                        smallest = right;
                }
                if (smallest != i) {
                        int buff = A[i];
                        A[i] = A[smallest];
                        A[smallest] = buff;

			if (cities_by_id != NULL && store_positions) {
                                cities_by_id[A[i]]->pos_in_heap = i;
                                cities_by_id[A[smallest]]->pos_in_heap = smallest;
                        }			

                        A = heapify(A, smallest, V, heap_size, heap_mode, store_positions);
                }
	}	
	return A;
}

int *build_heap(int *A, int N, int *V, int heap_mode, int store_positions) {
  	int heap_size = N;
	for (int i = (int)(N/2); i >= 0; i--) {
    		A = heapify(A, i, V, heap_size, heap_mode, store_positions);
	}
	return A;
}

int *heap_sort(int *A, int N, int *V, int heap_mode, int store_positions) {
	A = build_heap(A, N, V, heap_mode, store_positions);
	int heap_size = N;

	for (int i = N-1; i >= 0; i--) {
		int buff = A[0];
		A[0] = A[i];
		A[i] = buff;

		if (cities_by_id != NULL && store_positions) {
			cities_by_id[A[0]]->pos_in_heap = 0;
			cities_by_id[A[i]]->pos_in_heap = i;
		}	

       		heap_size--;
       		A = heapify(A, 0, V, heap_size, heap_mode, store_positions);
	}
	return A;
}

void shift_to_root (int *A, int i, int store_positions) {
    	while (i != 0) {
		int parent_i = (int)(i - i%2)/2;

        	int buff = A[i];
		A[i] = A[parent_i];
		A[parent_i] = buff;
	
		if (cities_by_id != NULL && store_positions) {
			cities_by_id[A[i]]->pos_in_heap = i;
			cities_by_id[A[parent_i]]->pos_in_heap = parent_i;
		}	

        	i = parent_i;
    	}
}
void shift_up (int *A, int i, int *V, int store_positions) {
	int parent_i = (int)(i - i%2)/2;
	while (i != 0 && V[A[parent_i]] < V[A[i]]) {
    		int buff = A[i];
		A[i] = A[parent_i];
		A[parent_i] = buff;
		
		if(cities_by_id != NULL && store_positions) {
			cities_by_id[A[i]]->pos_in_heap = i;
                        cities_by_id[A[parent_i]]->pos_in_heap = parent_i;
		}
      		
		i = parent_i;
		parent_i = (int)(i - i%2)/2;
	}
}

void swap (int *A, int i_1, int i_2, int store_positions) {
	int buff = A[i_1];
	A[i_1] = A[i_2];
	A[i_2] = buff;

	if (cities_by_id != NULL && store_positions) {
		cities_by_id[A[i_1]]->pos_in_heap = i_1;
		cities_by_id[A[i_2]]->pos_in_heap = i_2;
	}
}

int left_b_search(int *A, int sort_mode, int first, int last, int *V, int key) {

        if(first == last) {
                if(V[A[first]] == key) {
                        return first;
                } else {
                        return -1;
                }
        }

	if (sort_mode == INC_SORT) {
		
		if (V[A[first]] > key) {
                	return -1;
        	} else if (V[A[last]] < key) {
                	return -1;
        	}
	
	} else if (sort_mode == DEC_SORT) {
		
		if (V[A[first]] < key) {
                	return -1;
        	} else if (V[A[last]] > key) {
                	return -1;
        	}

	}

        while (first < last) {
		if (V[A[first]] == V[A[last]] && V[A[first]] == key) {
         		return first;
                }

                int mid = first + (last - first) / 2;
		
		if (sort_mode == INC_SORT) {
      			
			if (key <= V[A[mid]]) {
        		 	last = mid;
       			} else {
        			first = mid + 1;
      			}
	
		} else if (sort_mode == DEC_SORT) {

			if (key >= V[A[mid]]) {
                                last = mid;
                        } else {
                                first = mid + 1;
                        }			
		}
        }
	
	if (V[A[first]] == key) {
		return first;
	} else {
		return -1;
	}
}

int right_b_search (int *A, int sort_mode, int first, int last, int *V, int key) {

	if(first == last) {
                if(V[A[first]] == key) {
                        return last;
                } else {
                        return -1;
                }
        }

        if (sort_mode == INC_SORT) {

                if (V[A[first]] > key) {
                        return -1;
                } else if (V[A[last]] < key) {
                        return -1;
                }

        } else if (sort_mode == DEC_SORT) {

                if (V[A[first]] < key) {
                        return -1;
                } else if (V[A[last]] > key) {
                        return -1;
                }
        }
        
	while (first < last) {
                if (V[A[first]] == V[A[last]] && V[A[first]] == key) {
                        return last;
                }

                int mid = (int )(last - (last - first) / 2);

		if (sort_mode == DEC_SORT) {
		
                	if (key <= V[A[mid]]) {
                	        first = mid;
                	} else {
                	        last = mid - 1;
                	}
 
		} else if (sort_mode == INC_SORT) {

			if (key >= V[A[mid]]) {
                                first = mid;
                        } else {
                                last = mid - 1;
                        }
		}
        }
	
	if (V[A[last]] == key) {
		return last;
	} else {
        	return -1;
	}
}

void find_leafs (int **leafs, int *size, int *ids_sorted_by_c, int *C, int N) {
	for (int k=0; k<N; k++) {
		if (-1 == left_b_search(ids_sorted_by_c, INC_SORT, 0, N-1, C, k)) {
			(*leafs)[*size] = k;
			(*size)++;
		}
	}
}

void accum_knots (int *leafs, int leafs_size, int min_priority) {
	for (int i=0; i<leafs_size; i++) {
		City *city = cities_by_id[leafs[i]];
		City *prev_city = NULL;
		
		if (city->priority >= min_priority) {
			city->accum_route_len = 1;
			city->accum_min_priority = city->priority;
			city->accum_max_priority = city->priority;
		} else {
			city->accum_route_len = 0;
                        city->accum_min_priority = -1;
                        city->accum_max_priority = -1;
		}
		city->accumed = 1;

		do {
			prev_city = city;
			city = cities_by_id[prev_city->next_id];
			
			if (city->priority >= min_priority) {
				if (city->accumed == 0) {
					city->accum_route_len =  1 + prev_city->accum_route_len;	
					city->accum_max_priority = MAX(city->priority, prev_city->accum_max_priority);
					city->accum_min_priority = MIN(city->priority, prev_city->accum_min_priority);
				} else {
					city->accum_route_len = MAX(city->accum_route_len, prev_city->accum_route_len);                     
                                        city->accum_max_priority = MAX(city->accum_max_priority, prev_city->accum_max_priority);
                                        city->accum_min_priority = MIN(city->accum_min_priority, prev_city->accum_min_priority);
				}
			} else {
				city->accum_route_len =  0;
                        	city->accum_max_priority = -1;
                        	city->accum_min_priority = -1;
			}
			city->accumed = 1;
			
		} while (city->next_id != city->id);
	}
}

int find_vacant_index_in_pool (int *ids_sorted_by_d, int pool_size, int id, int *D, int *d_pool) {
	int l_i = left_b_search (ids_sorted_by_d, DEC_SORT, 0, pool_size - 1, D, D[id]);
	int r_i = right_b_search (ids_sorted_by_d, DEC_SORT, 0, pool_size - 1, D, D[id]);

	if (l_i == -1 || r_i == -1) {
		printf("error: no such priority in pool\n");
		return -2;
	}
	for (int i = l_i; i <= r_i; i++) {
		if (d_pool[i] == 0) {
			return i;
		}
	}
	return -1;
}

int find_vacant_adjacent_id (int *ids_sorted_by_c, int size, int id, int *C, int *visited) {
        int l_i = left_b_search (ids_sorted_by_c, INC_SORT, 0, size - 1, C, id);
        int r_i = right_b_search (ids_sorted_by_c, INC_SORT, 0, size - 1, C, id);

        if (l_i == -1 || r_i == -1) {
                return -1;
        }
        for (int i = l_i; i <= r_i; i++) {
                if (visited[ids_sorted_by_c[i]] == 0) {
                        return ids_sorted_by_c[i];
                }
        }
        return -1;
}

int find_shortest_branch_id(int *ids_sorted_by_d, int pool_size, int priority, int *D) {
	int l_i = left_b_search (ids_sorted_by_d, DEC_SORT, 0, pool_size - 1, D, priority);
        int r_i = right_b_search (ids_sorted_by_d, DEC_SORT, 0, pool_size - 1, D, priority);
	
	if (l_i == -1 || r_i == -1) {
		printf("error: no such priority in pool\n");
                return -2;
        }

	City* shortest_brunch_city = cities_by_id[ids_sorted_by_d[l_i]];
	for (int i = l_i; i <= r_i; i++) {
		City *city = cities_by_id[ids_sorted_by_d[i]];
		if (city->accum_route_len < shortest_brunch_city->accum_route_len) {
			shortest_brunch_city = city;
		}
	}	
	return shortest_brunch_city->id;
}

void swap_complete (int *ids_sorted_by_d, int i_1, int i_2) {
	City *city_1 = cities_by_id[ids_sorted_by_d[i_1]];
	City *city_2 = cities_by_id[ids_sorted_by_d[i_2]];	

	swap (ids_sorted_by_d, i_1, i_2, 0);
	
	city_1->ids_sorted_by_d_index = i_2;
	city_2->ids_sorted_by_d_index = i_1;
	
	int buff = city_1->d_pool_index;
        city_1->d_pool_index = city_2->d_pool_index;
        city_2->d_pool_index = buff;

        buff = city_1->pos_in_heap;
        city_1->pos_in_heap = city_2->pos_in_heap;
        city_2->pos_in_heap = buff;
}

int check_routeness (
	int *ids_sorted_by_c,
	int *ids_sorted_by_d, 
	int **d_pool,
	int **priority_pool,
	int *priority_pool_size,
	int *priority_threshold,
	Pot_Route* pot_route, 
	int N, 
	int K, 
	City *city, 
	int min_priority, 
	int *D, 
	int *C, 
	int **visited,
	int *curr_route_len) {

	int route_len = 0;
	printf("\nchecking city : %d\n", city->id);

	if ((*visited)[city->id] > 0) {
		printf("%d not added : already visited\n", city->id);
		return 0;
        }	

	if (city->priority < min_priority) {
		(*visited)[city->id] = 1;
		printf("not added : pripority of %d == %d, min priority = %d\n", city->id, city->priority, min_priority);
		return 0;
	}

	int vacant_pool_index = find_vacant_index_in_pool(ids_sorted_by_d, K, city->id, D, *d_pool);

        if (vacant_pool_index == -1) {
                (*visited)[city->id] = 1;
                printf("NO PLACES IN POOL\n");
                return -1;
        }

	int sum_route_len = *curr_route_len + pot_route->len;
	printf("sum route len : %d + %d\n", *curr_route_len, pot_route->len);

        if (sum_route_len > K) {
                (*visited)[city->id] = 1;
                printf("route lne exceeded\n");
                return 0;
        }

	(*visited)[city->id] = 1;
	route_len++;
	(*d_pool)[vacant_pool_index] = 1;

	add_city(pot_route, city);
        printf("pot route increased : %d\n", pot_route->len);

	City *vacant_pool_city = cities_by_id[ids_sorted_by_d[vacant_pool_index]];
	vacant_pool_city->d_pool_index = vacant_pool_index;

	if (vacant_pool_city->id != city->id) {
		printf("swap %d and %d\n", vacant_pool_city->id, city->id);
		swap_complete (ids_sorted_by_d, vacant_pool_city->ids_sorted_by_d_index, city->ids_sorted_by_d_index);
	}	

	printf("summon city : %d, position in heap : %d, pool index : %d\n", city->id, city->pos_in_heap, city->d_pool_index);
	
	shift_to_root(*priority_pool, city->pos_in_heap, 1);
	swap(*priority_pool, 0, (*priority_pool_size) - 1, 1);
	(*priority_pool_size)--;
	*priority_pool = heapify(*priority_pool, 0, D, *priority_pool_size, MAX_HEAP, 1);

        int pot_priority_threshold = *priority_pool_size > 0 ? cities_by_id[(*priority_pool)[0]]->priority : 0;
	printf("pot priority threshold : %d, pool size : %d\n", pot_priority_threshold, *priority_pool_size);	
	for(int i=0; i<N; i++) printf("%d ", ids_sorted_by_d[i]);
        printf("\n");
	for(int i=0; i<N; i++) printf("%d ", D[ids_sorted_by_d[i]]);
        printf("\n");
	for(int i=0; i<K; i++) printf("%d ", (*d_pool)[i]);
	printf("\n");
	for(int i=0; i<N; i++) printf("%d ", (*visited)[i]);
        printf("\n");	
	printf("city prior : %d,  current threshold : %d\n", city->priority, *priority_threshold);


	printf("droping pot route, min prior = %d, pot threshold = %d\n", pot_route->min_priority, pot_priority_threshold);
				
	int drop_needed = pot_route->min_priority >= pot_priority_threshold || is_empty(pot_route);	
	if (drop_needed) {
		*curr_route_len = *curr_route_len + pot_route->len;
		drop(pot_route);
		printf("pot route droped\n");
		*priority_threshold = pot_priority_threshold;
	}
	
	//find all adj
	int adj_id = find_vacant_adjacent_id(ids_sorted_by_c, N, city->id, C, *visited);
	if (adj_id == -1 && (*visited)[city->next_id] == 0) {
		adj_id = city->next_id;
	}

	while (adj_id != -1) {

		printf("ADJ : %d\n", adj_id);	
		int sub_route_len = check_routeness(
			ids_sorted_by_c, 
			ids_sorted_by_d, 
			d_pool, 
			priority_pool,
			priority_pool_size,
			priority_threshold,
        		pot_route,
			N, 
			K, 
			cities_by_id[adj_id], 
			min_priority, 
			D, 
			C, 
			visited,
			curr_route_len);

		if (sub_route_len > 0) {
                        route_len += sub_route_len;
                }

		if (sub_route_len == -1) {            
			
			int worst_city_id = find_shortest_branch_id(ids_sorted_by_d, K, D[adj_id], D);
			City *worst_city_in_route = cities_by_id[worst_city_id];
			City *adj_city = cities_by_id[adj_id];
			
			int need_to_replace = (worst_city_in_route->accum_route_len < adj_city->accum_route_len) || (worst_city_in_route->accum_route_len == adj_city->accum_route_len && worst_city_in_route->accum_max_priority < adj_city->accum_max_priority); 

			if (need_to_replace) {
				printf("swap worst city in route %d and adj city %d\n", worst_city_in_route->id, adj_city->id);
				swap_complete (ids_sorted_by_d, worst_city_in_route->ids_sorted_by_d_index, adj_city->ids_sorted_by_d_index);

				City *unsummoned = worst_city_in_route;
				printf("unsummon worst_city_in_route : %d\n", unsummoned->id);
				
                       		(*priority_pool)[*priority_pool_size] = unsummoned->id;
                        	unsummoned->pos_in_heap = *priority_pool_size;
                        	shift_up(*priority_pool, unsummoned->pos_in_heap, D, 1);
                        	(*priority_pool_size)++;

				printf("unvisit adj city : %d\n", adj_city->id);
				(*visited)[adj_city->id] = 0;
				(*d_pool)[adj_city->d_pool_index] = 0;
				if (contains(pot_route, adj_city)) {
					remove_city(pot_route, adj_city);
				}
			}	
		} 

		adj_id = find_vacant_adjacent_id(ids_sorted_by_c, N, city->id, C, *visited);
		if (adj_id == -1 && (*visited)[city->next_id] == 0) {
                	adj_id = city->next_id;
        	}
	}
	return route_len;
}

int solution(int K, int C[], int D[], int N) {

	int *A_D = (int *)calloc(N, sizeof(int));
	int *A_C = (int *)calloc(N, sizeof(int));	
	cities_by_id = (City **)calloc(N, sizeof(City));
 
	for (int i=0; i<N; i++) {
		City *city = (City *)calloc(1, sizeof(City));
		city->id = i;
		city->next_id = C[i];
		city->pos_in_heap = -1;
		city->priority = D[i];
		city->d_pool_index = -1;
		city->ids_sorted_by_d_index = -1;
		city->accumed = 0;
		city->accum_route_len = 0;
		city->accum_min_priority = -1;
		city->accum_max_priority = -1;
		cities_by_id[i] = city;
			
		A_D[i] = i;
                A_C[i] = i;
	}
	
	//Build heap by D
        int *ids_sorted_by_d = heap_sort(A_D, N, D, MIN_HEAP, 1);
	for (int i=0; i<N; i++) {
		cities_by_id[ids_sorted_by_d[i]]->ids_sorted_by_d_index = i;
	}
        //Sort cities by C to find element in log(n) time
        int *ids_sorted_by_c = heap_sort(A_C, N, C, MAX_HEAP, 0);

	int min_priority = D[ids_sorted_by_d[K-1]];
	int max_priority = D[ids_sorted_by_d[0]];	

	int *leafs = (int *)calloc(N, sizeof(int));
	int size = 0;
	find_leafs (&leafs, &size, ids_sorted_by_c, C, N);
	
	accum_knots (leafs, size, min_priority);
/*
	printf("\n");
	for (int i=0; i<N; i++) {
		City *city = cities_by_id[i];
		printf("\n");
                printf("city %d:\n", city->id);
                printf("\taccum len : %d\n", city->accum_route_len);
                printf("\taccum max : %d\n", city->accum_max_priority);
                printf("\taccum min : %d\n", city->accum_min_priority);
                printf("\n");
	}
	printf("\n");
*/

	City *city = cities_by_id[ids_sorted_by_d[0]];

        int *B = (int *)calloc(N, sizeof(int));		
	int *d_pool = (int *)calloc(K, sizeof(int));
	int *priority_pool = (int *)calloc(K, sizeof(int));		
	int priority_pool_size = K;
	int priority_threshold = max_priority;

	for (int i=0; i<K; i++) {
		priority_pool[i] = ids_sorted_by_d[i];
	}		

	Pot_Route *pot_route = (Pot_Route *)calloc(1, sizeof(Pot_Route));
	pot_route->len = 0;
	pot_route->min_priority = -1;
	pot_route->cities = (int *)calloc(N, sizeof(int));
	pot_route->cities_size = N;	
	int curr_route_len = 0;

	int route_len = check_routeness(
		ids_sorted_by_c, 
		ids_sorted_by_d, 
		&d_pool, 
		&priority_pool,
		&priority_pool_size,
		&priority_threshold,
		pot_route,
		N, 
		K, 
		city, 
		min_priority, 
		D, 
		C, 
		&B,
		&curr_route_len);			

	if (!is_empty(pot_route)) {
		route_len -= pot_route->len;
	}		

	free(B);
	free(d_pool);
	free(pot_route->cities);
        free(pot_route);

	if (route_len > K) {
		route_len = K;
	}	

	for (int i=0; i<N; i++) free(cities_by_id[i]);
	free(cities_by_id);
	free(leafs);
	free(A_C);
        free(A_D);	

	return route_len;
}

int main () {
/*medium random tree*/
 /*
	int N = 200;
	int K = 36;
	
	int C[200] = {172,13,124,53,45,94,54,135,47,148,71,188,91,31,88,1,81,108,158,66,86,57,0,123,34,10,137,119,102,152,126,19,11,178,57,176,175,167,173,41,85,153,108,58,133,43,39,99,76,83,102,195,149,69,44,81,28,14,103,18,9,185,30,131,2,71,18,78,138,148,195,149,98,147,130,158,147,176,9,124,79,35,122,138,167,114,96,38,101,133,182,3,11,161,71,183,19,45,183,126,20,35,171,9,136,168,135,84,6,147,144,8,105,16,114,55,72,66,123,124,13,26,70,51,143,45,160,53,112,5,63,98,124,24,177,123,59,15,147,136,24,110,19,196,95,92,84,162,133,185,95,31,49,24,117,158,13,64,42,199,159,183,3,15,125,73,57,16,97,172,101,21,123,125,141,152,94,16,27,45,62,36,185,59,36,0,108,47,139,36,58,173,138,176,19,180,128,165,101,114};

	int D[200] = {53,23,21,43,50,50,31,100,75,15,50,4,50,28,13,45,10,36,50,50,31,27,50,60,31,50,37,33,12,0,98,15,42,12,13,0,22,0,50,6,70,7,42,43,45,50,43,99,50,50,46,61,50,7,44,30,44,21,44,50,5,50,57,15,25,50,50,20,50,39,80,50,26,50,6,50,50,33,10,10,11,5,80,50,38,93,8,5,35,29,50,11,32,37,50,18,21,50,30,78,24,6,44,39,33,50,53,0,1,50,39,54,16,31,99,45,20,50,76,27,23,34,95,71,45,50,73,48,48,5,42,12,15,44,42,62,50,40,50,15,37,20,14,20,47,0,13,50,27,50,35,32,16,3,3,50,31,3,50,69,63,46,43,16,50,50,21,4,50,66,27,41,84,50,9,30,50,24,1,50,77,9,50,50,12,50,5,52,14,23,40,50,50,46,33,58,0,50,13,74};
*/

/*star*/
/*
	int N = 100;
	int K = 100;

	int C[100] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	int D[100] = {1,7,5,7,3,9,9,7,3,6,4,4,10,10,2,9,7,5,4,8,6,8,7,3,8,10,10,7,2,2,3,10,4,5,10,4,6,5,2,7,9,3,4,5,2,6,9,7,6,9,3,7,5,7,3,8,2,3,9,10,5,5,4,4,7,3,3,6,3,7,9,8,6,5,5,2,8,4,4,2,6,7,5,9,8,4,6,10,6,7,2,6,6,5,5,7,6,6,3,5};
*/

/* similar levels*/

	int N = 19;
	int K = 18;	

	int C[19] = {8,18,9,1,1,17,12,4,16,13,1,12,13,10,11,9,13,4,18};
	int D[19] = {1,2,9,7,1,7,4,2,0,0,4,8,5,6,5,0,3,9,5};

/*line */
/*
	int N = 20;
	int K = 5;
	
	int C[20] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 19};
	int D[20] = {1, 1, 1, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 1, 1, 1, 1};
*/

/* non conseq levels*/
/*
	int N = 7;
	int K = 6;

	int C[7] = {0, 0, 3, 1, 3, 4, 5};
	int D[7] = {1000000, 14, 100, 15, 99, 0, 2};
*/

	printf("solution : %d\n", solution(K, C, D, N));
	return 0;
}
