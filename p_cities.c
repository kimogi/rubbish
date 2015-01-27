#include <stdlib.h>
#include <stdio.h>

#define MAX_HEAP 0
#define MIN_HEAP 1

typedef struct city_t {
        int pos_in_heap;
        int priority;
        int id;
        int next_id;
	int checked;
	int d_pool_index;
} City;

City **cities_by_id = NULL;

typedef struct pot_route_t {
        int len;
        int min_priority;
	int *priority_pool;
	int priority_pool_size;
} Pot_Route;

void add_city (Pot_Route *pot_route, City *city) {
	(pot_route->len)++;
	if (pot_route->min_priority == -1) {
		pot_route->min_priority = city->priority;
	} else if (pot_route->min_priority > city->priority) {
		pot_route->min_priority = city->priority;
	}
}

void add_to_pool (Pot_Route *pot_route, City *city) {
	pot_route->priority_pool[pot_route->priority_pool_size] = city->id;
        (pot_route->priority_pool_size)++;
}

City *extract_from_pool(Pot_Route *pot_route) {
	if (pot_route->priority_pool_size > 0) {
		(pot_route->priority_pool_size)--;
		return cities_by_id[pot_route->priority_pool[pot_route->priority_pool_size]];

	}
	return NULL;
}

void drop(Pot_Route *pot_route) {
	pot_route->len = 0;
	pot_route->min_priority = -1;
	pot_route->priority_pool_size = 0;
}

int is_pool_empty(Pot_Route *pot_route) {
	int result = pot_route->priority_pool_size == 0 ? 1 : 0;
	return result;
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

int find_next_adj(int *A, int N, int *V, int i, int **visited) {
	int  first = 0;
	int last = N-1;

	if(first == last) {
		if(V[A[first]] == i && (*visited)[A[first]] == 0) {
                        return A[first];
                } else {
                        return -1;
                }
	} else if (V[A[0]] > i) {
		return -1;
	} else if (V[A[N-1]] < i) {
		return -1;
	}

	while (first < last) {
		if (V[A[first]] == V[A[last]] && V[A[first]] == i) {
			break;
		}

		int mid = first + (last - first) / 2;
		
		if (i <= V[A[mid]]) {
			last = mid;
		} else {
			first = mid + 1;
		}
	}

	for (int j=first; j < N; j++) {
                if (V[A[j]] == i && (*visited)[A[j]] == 0) {
                        return A[j];
                } else if(V[A[j]] != i) {
                        return -1;
                }
        }
	return -1;
}

int spare (int *d_cities, int first, int last, int city, int *D, int **d_pool) {

        if (first == last) {
                if((*d_pool)[first] == 0) {
			return first;
		} else {
			return -1;
		}
        } else if (D[d_cities[first]] < D[city]) {
                return -1;
        } else if (D[d_cities[last]] > D[city]) {
                return -1;
        }

        while (first < last) {
		if (D[d_cities[first]] == D[d_cities[last]] && D[d_cities[first]] == D[city]) {
			break;
                }

                int mid = (int )(last - (last - first) / 2);

                if (D[city] <= D[d_cities[mid]]) {
                        first = mid;
                } else {
                        last = mid - 1;
                }
        }

	for (int i=last; i >= 0; i--) {
		if (D[d_cities[i]] == D[city] && (*d_pool)[i] == 0) {
			return i;
		} else if(D[d_cities[i]] > D[city]) {
			return -1;
		}
	}
        return -1;
}

void check_routeness (
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
	int *route_len) {

	printf("checking city : %d\n", city->id);
	for (int i=0; i<N; i++) printf("%d ", (*visited)[i]);

	if ((*visited)[city->id] > 0) {
		printf("%d not added : already visited\n", city->id);
        	return;
        }	

	if (city->priority < min_priority) {
		(*visited)[city->id] = 1;
		printf("not added : pripority of %d == %d, min priority = %d\n", city->id, city->priority, min_priority);
		return;
	}

	(*visited)[city->id] = 1;

	int spare_index = spare(ids_sorted_by_d, 0, K-1, city->id, D, d_pool);

	if (spare_index != -1) {
		(*d_pool)[spare_index] = 1;
		
		printf("\n");
       		for(int i=0; i<K; i++) printf("%d ", cities_by_id[ids_sorted_by_d[i]]->priority);
        	printf("\n");
        	for(int i=0; i<K; i++) printf("%d ", (*d_pool)[i]);
       	 	printf("\n");

		
		City *summoned = cities_by_id[ids_sorted_by_d[spare_index]];
		summoned->d_pool_index = spare_index;
		
		printf("summoned pos in heap : %d, summoned d pool index : %d\n", summoned->pos_in_heap, summoned->d_pool_index);
		shift_to_root(*priority_pool, summoned->pos_in_heap, 1);
        	swap(*priority_pool, 0, (*priority_pool_size) - 1, 1);
        	(*priority_pool_size)--;
        	*priority_pool = heapify(*priority_pool, 0, D, *priority_pool_size, MAX_HEAP, 1);
		add_to_pool(pot_route, summoned);
		
        	int pot_priority_threshold = *priority_pool_size > 0 ? cities_by_id[(*priority_pool)[0]]->priority : 0;
		printf("pot priority threshold : %d, pool size : %d\n", pot_priority_threshold, *priority_pool_size);	
		for(int i=0; i<K; i++) printf("%d ", (*d_pool)[i]);
		printf("\n");	

		printf("current threshold : %d\n", *priority_threshold);
		(*route_len)++;

		if (city->priority < *priority_threshold) {
			add_city(pot_route, city);

		} else if (city->priority == *priority_threshold) {				

			printf("droping pot route, route len = %d, min prior = %d, pot pool size = %d", *route_len, pot_route->min_priority, pot_route->priority_pool_size);
		
			if (*route_len <= K && (pot_route->min_priority >= pot_priority_threshold || is_empty(pot_route))) {
				
				drop(pot_route);

				printf("pot route droped\n");

				//city->checked = 1;
                                printf("city %d checked\n", city->id);

				*priority_threshold = pot_priority_threshold;
			} else {
				add_city(pot_route, city);
			}
		}

		printf("route len = %d, pot route len = %d\n", *route_len, pot_route->len);

	}  else {
		printf("No spare places, pot len = %d, route len = %d, pot pool size = %d\n", pot_route->len, *route_len, pot_route->priority_pool_size);

		printf("\n");
        	for(int i=0; i<K; i++) printf("%d ", cities_by_id[ids_sorted_by_d[i]]->priority);
        	printf("\n");
        	for(int i=0; i<K; i++) printf("%d ", (*d_pool)[i]);
        	printf("\n");
	

		int pot_priority_threshold = *priority_pool_size > 0 ? cities_by_id[(*priority_pool)[0]]->priority : 0;		

		if (*route_len <= K && (pot_route->min_priority >= pot_priority_threshold || is_empty(pot_route))) {
			
			drop(pot_route);
			//city->checked = 1;
			*priority_threshold = pot_priority_threshold;

		} else {
			(*route_len) -= (*pot_route).len;
			//TODO : unsummon all summon cities to priority_pool. Use binomial heaps instead		
			printf("1 : pool size = %d, pot len = %d, route len = %d\n", *priority_pool_size, pot_route->len, *route_len);			

			City *unsummoned = extract_from_pool(pot_route);
			while (unsummoned != NULL) {			
				
				(*priority_pool)[*priority_pool_size] = unsummoned->id;
				unsummoned->pos_in_heap = *priority_pool_size;
				shift_up(*priority_pool, unsummoned->pos_in_heap, D, 1);
				(*priority_pool_size)++;
	
				(*d_pool)[unsummoned->d_pool_index] = 0;
				unsummoned->d_pool_index = -1;

				unsummoned = extract_from_pool(pot_route);
				printf("pool size : %d\n", *priority_pool_size);
			}
			drop(pot_route);
		}
		return;
	}
	
	//find all adj
	int adj_i = find_next_adj(ids_sorted_by_c, N, C, city->id, visited);
	while (adj_i != -1) {
		
		check_routeness(
			ids_sorted_by_c, 
			ids_sorted_by_d, 
			d_pool, 
			priority_pool,
			priority_pool_size,
			priority_threshold,
        		pot_route,
			N, 
			K, 
			cities_by_id[adj_i], 
			min_priority, 
			D, 
			C, 
			visited,
			route_len);

		adj_i = find_next_adj(ids_sorted_by_c, N, C, city->id, visited);
	};

	check_routeness(
		ids_sorted_by_c,
		ids_sorted_by_d,
		d_pool,
		priority_pool,
		priority_pool_size,
		priority_threshold,
		pot_route,
		N,
		K,
		cities_by_id[city->next_id],
		min_priority,
		D,
		C,
		visited,
		route_len);
	return;
}

int solution(int K, int C[], int D[], int N) {

	printf("K : %d, N : %d\n", K, N);
	for (int i=0; i<N; i++) printf("%d ", C[i]);
	printf("\n");
	for (int i=0; i<N; i++) printf("%d ", D[i]);
	pritnf("\n");

	int *A_D = (int *)calloc(N, sizeof(int));
	int *A_C = (int *)calloc(N, sizeof(int));	
	cities_by_id = (City **)calloc(N, sizeof(City));
 
	for (int i=0; i<N; i++) {
		City *city = (City *)calloc(1, sizeof(City));
		city->id = i;
		city->next_id = C[i];
		city->pos_in_heap = -1;
		city->priority = D[i];
		city->checked = 0;
		city->d_pool_index = -1;
		cities_by_id[i] = city;
		
		A_D[i] = i;
                A_C[i] = i;
	}
	
	//Build heap by D
        int *ids_sorted_by_d = heap_sort(A_D, N, D, MIN_HEAP, 1);
        //Sort cities by C to find element in log(n) time
        int *ids_sorted_by_c = heap_sort(A_C, N, C, MAX_HEAP, 0);

	int max_route_len = 0;

	int min_priority = D[ids_sorted_by_d[K-1]];
	int max_priority = D[ids_sorted_by_d[0]];	

	//For all not checked cities with maximum atraction
	for (int i=0; i<N; i++) {
		City *city = cities_by_id[ids_sorted_by_d[i]];
		if (city->priority < max_priority) {
			break;
		}
		if (city->checked == 1) {
			continue;
		}

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
		pot_route->priority_pool = (int *)calloc(K, sizeof(int));
		pot_route->priority_pool_size = 0;
		int route_len = 0;	

		check_routeness(
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
			&route_len);
		
		city->checked = 1;		
		if (!is_empty(pot_route)) {
			route_len -= pot_route->len;
		}		

		if (route_len > max_route_len) {
			max_route_len = route_len;
		}

		printf("\nmax route len : %d\nroute len : %d\n", max_route_len, route_len);		
		printf("--------------------------------------------\n\n");	

		free(B);
		free(d_pool);
		free(pot_route->priority_pool);
        	free(pot_route);

	}
	
	if (max_route_len > K) {
		max_route_len = K;
	}	

	for (int i=0; i<N; i++) free(cities_by_id[i]);
	free(cities_by_id);
	free(A_C);
        free(A_D);	

	return max_route_len;
}

int main () {
/*medium random tree*/
/* 
	int N = 200;
	int K = 36
	
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
/*
	int N = 19;
	int K = 18;	

	int C[19] = {8,18,9,1,1,17,12,4,16,13,1,12,13,10,11,9,13,4,18};
	int D[19] = {1,2,9,7,1,7,4,2,0,0,4,8,5,6,5,0,3,9,5};

*/

/*line */
/*
	int N = 20;
	int K = 18;
	
	int C[20] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 19};
	int D[20] = {1, 9, 1, 9, 1, 9, 1, 9, 1, 1, 9, 1, 1, 9, 1, 1, 1, 1, 9, 1};
*/

/* non conseq levels*/

	int N = 7;
	int K = 6;

	int C[7] = {0, 0, 3, 1, 3, 4, 5};
	int D[7] = {1000000, 14, 100, 15, 99, 0, 2};


	printf("solution : %d\n", solution(K, C, D, N));
	return 0;
}
