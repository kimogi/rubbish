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

typedef struct pot_route_t {
        int len;
        int min_priority;
	int *A;
} Pot_Route;

City **cities_by_id = NULL;

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
/*
	if (C[A[last]] == i && visited[A[last]] == 0) {
		if (adj_number == 0) {
			return A[last];
		} else {
			last += adj_number;
			if (last < N && C[A[last]] == i) {
				return A[last];
			}	
		}
	} */
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

	printf("\n");
	for(int i=0; i<K; i++) printf("%d ", cities_by_id[ids_sorted_by_d[i]]->priority);		
	printf("\n");
	for(int i=0; i<K; i++) printf("%d ", (*d_pool)[i]);
	printf("\n");

	int spare_index = spare(ids_sorted_by_d, 0, K-1, city->id, D, d_pool);
	city->d_pool_index = spare_index;
	if (spare_index != -1) {
		(*d_pool)[spare_index] = 1;
	}

	printf("pos in heap : %d\n", city->pos_in_heap);
	if (city->pos_in_heap < *priority_pool_size) {
		shift_to_root(*priority_pool, city->pos_in_heap, 1);
               	swap(*priority_pool, 0, (*priority_pool_size) - 1, 1);
              	(*priority_pool_size)--;
                *priority_pool = heapify(*priority_pool, 0, D, *priority_pool_size, MAX_HEAP, 1);
       	}

        int pot_priority_threshold = *priority_pool_size > 0 ? cities_by_id[(*priority_pool)[0]]->priority : 0;
	printf("pot priority threshold : %d, pool size : %d\n", pot_priority_threshold, *priority_pool_size);	
	for(int i=0; i<K; i++) printf("%d ", (*d_pool)[i]);
	printf("\n");	

	(*visited)[city->id] = 1;
	
	if (-1 != spare_index) {

		printf("current threshold : %d\n", *priority_threshold);

		(*route_len)++;

		if (city->priority < *priority_threshold) {
			
			(*pot_route).A[(*pot_route).len] = city->id;
			(*pot_route).len++;
			
			if ((*pot_route).min_priority > city->priority || (*pot_route).min_priority == -1) {
				(*pot_route).min_priority = city->priority;
			}

			printf("pot route len = %d\n", (*pot_route).len);

		} else if (city->priority == *priority_threshold) {				

			printf("droping pot route, route len = %d, min prior = %d",*route_len, (*pot_route).min_priority);
		
		
		
			if ((*route_len <= K && (*pot_route).min_priority >= pot_priority_threshold) ||
				(*pot_route).min_priority == -1) {
				
				(*pot_route).len = 0;
				(*pot_route).min_priority = -1;

				printf("pot route droped\n");

				city->checked = 1;
                                printf("city %d checked\n", city->id);
				printf("route len %d\n", *route_len);			

				*priority_threshold = pot_priority_threshold;
			} else {
				(*pot_route).A[(*pot_route).len] = city->id;
				(*pot_route).len++;
				printf("pot route len : %d\n", (*pot_route).len);
			}
		}

		//(*d_pool)[spare_index] = 1;

	}  else {
		printf("No spare places, pot len = %d, route len = %d\n", (*pot_route).len, *route_len);	

		if ((*route_len <= K && (*pot_route).min_priority >= pot_priority_threshold) ||
			(*pot_route).min_priority == -1) {
	
			(*pot_route).len = 0;
			(*pot_route).min_priority = -1;
			city->checked = 1;
			*priority_threshold = pot_priority_threshold;
		} else {
			(*route_len) -= (*pot_route).len;
			//TODO : return all canceled cities to priority_pool. Use binomial heaps instead		
			printf("1 : pool size = %d, len = %d\n", *priority_pool_size, (*pot_route).len);
			while((*pot_route).len > 0 && (*priority_pool_size) < K) {
				(*pot_route).len--;
				City *unsummoned = cities_by_id[(*pot_route).A[(*pot_route).len]];				
				(*visited)[unsummoned->id] = 0;

				printf("2 : pool size = %d, len = %d\n", *priority_pool_size, (*pot_route).len);
				
				if (unsummoned->pos_in_heap < K) {
					printf("3 : pos in heap : %d\n", unsummoned->pos_in_heap);					

					(*priority_pool)[*priority_pool_size] = unsummoned->id;
					
					printf("4\n");

					unsummoned->pos_in_heap = *priority_pool_size;
					
					printf("5\n");
					
					shift_up(*priority_pool, unsummoned->pos_in_heap, D, 1);
					
					printf("6\n");
					
					(*priority_pool_size)++;
					
				}
				if (unsummoned->d_pool_index != -1) {
					(*d_pool)[unsummoned->d_pool_index] = 0;
				}
			}
			(*pot_route).min_priority = -1;
		}

		return;
	}
	
	//find all adj
	int adj_i = find_next_adj(ids_sorted_by_c, N, C, city->id, visited);
	while (adj_i != -1) {
		
		printf("FIND ADG : %d\n", adj_i);
		for (int i=0; i<N; i++) printf("%d ", ids_sorted_by_c[i]);
		printf("\n");		
		for (int i=0; i<N; i++) printf("%d ", C[ids_sorted_by_c[i]]);
                printf("\n");
		
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
		int *priority_pool = (int *)calloc(N, sizeof(int));		
		int priority_pool_size = K;
		int priority_threshold = max_priority;

		for (int i=0; i<K; i++) {
			priority_pool[i] = ids_sorted_by_d[i];
		}		

		Pot_Route *pot_route = (Pot_Route *)calloc(1, sizeof(Pot_Route));
		pot_route->len = 0;
		pot_route->min_priority = -1;
		pot_route->A = (int *)calloc(N, sizeof(int));
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

		if (route_len > max_route_len) {
			max_route_len = route_len;
		}

		printf("\nmax route len : %d\nroute len : %d\n", max_route_len, route_len);		
		printf("--------------------------------------------\n\n");	

		free(B);
		free(d_pool);
		free(pot_route->A);
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
	int N = 20;
	int K = 15;	

//	int C[8] = {1, 3, 0, 3, 2, 4, 4, 1}; 
	//int C[8] = {0, 0, 1, 2, 3, 4, 5, 6};
//	int C[8] = {1, 2, 3, 4, 5, 6, 7, 7};
//	int C[20] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 19};
	int C[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	//int C[8] = {0, 0, 0, 0, 0, 0, 0, 0};
//	int D[8] = {5, 6, 4, 4, 4, 4, 6, 3};
	//int D[8] = {7, 6, 5, 4, 3, 2, 1, 0};
	//int D[8] = {6, 4, 6, 4, 6, 4, 5, 4};
	//int D[8] = {6, 5, 4, 4, 4, 4, 4, 6};
//	int D[8] = {2, 6, 6, 6, 6, 3, 1, 6};
//	int D[8] = {6, 2, 7, 5, 6, 5, 2, 3};
	int D[20] = {1, 9, 1, 9, 1, 9, 1, 9, 1, 1, 9, 1, 1, 9, 1, 1, 1, 1, 9, 1};

	printf("solution : %d\n", solution(K, C, D, N));
	return 0;
}
