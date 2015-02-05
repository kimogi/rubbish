#include <stdlib.h>
#include <stdio.h>

#define MAX_HEAP 0
#define MIN_HEAP 1
#define INC_SORT 2
#define DEC_SORT 3
#define ROUTE_HEAP 4
#define FREE_HEAP 5

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

typedef struct node_t {
        int key;
        int priority;
        struct node_t *parent;
        struct node_t *child;
        struct node_t *sibling;
        int degree;
} Node;

typedef struct city_t {
        int priority;
        int id;
        int next_id;
	int d_pool_index;
	int ids_sorted_by_d_index;
	Node *priority_heap_node;
	int checked;
	int route_len_behind;
	int accumed;
	int accum_route_len;
	int accum_max_priority_city_id;
	int accum_min_priority_city_id;
} City;

typedef struct pot_route_t {
        int len;
        int min_priority;
        Node *priority_heap;
	Node *firm_route;
} Pot_Route;

City **cities_by_id = NULL;
Node *priority_heap;
Pot_Route *pot_route;

Node *get_max(Node *h) {
        Node *best_node = h;
        Node *root = h;
        while (root != NULL) {
                if (root->priority > best_node->priority) {
                        best_node = root;
                }
                root = root->sibling;
        }
        return best_node;
}

void link (Node *root, Node *branch) {
        branch->parent = root;
        branch->sibling = root->child;
        root->child = branch;
        root->degree = root->degree + 1;

        cities_by_id[root->key]->priority_heap_node = root;
        cities_by_id[branch->key]->priority_heap_node = branch;
}

Node *merge (Node *h1, Node *h2) {
	Node *h = NULL;
	Node *cur_2 = NULL;
	Node *cur_1 = NULL;	
	Node *cur = NULL;
	Node *tail = NULL;

	if (h2->degree < h1->degree) {
		h = h2;
		cur_2 = h2->sibling;
		cur_1 = h1;
	} else {
		h =  h1;
		cur_1 = h1->sibling;
		cur_2 = h2;
	}
	cur = h;
	while (cur_1 != NULL && cur_2 != NULL) {
		if (cur_1->degree > cur_2->degree) {
			cur->sibling = cur_2;
			cities_by_id[cur->key]->priority_heap_node = cur;
			cur = cur->sibling;
			cur_2 = cur_2->sibling;
		} else {
			cur->sibling = cur_1;
			cities_by_id[cur->key]->priority_heap_node = cur;
			cur = cur->sibling;
			cur_1 = cur_1->sibling;
		}
	}

	cities_by_id[cur->key]->priority_heap_node = cur;
	if (cur_1 == NULL && cur_2->key != cur->key) {
		tail = cur_2;
	} else if (cur_2 == NULL && cur_1->key != cur->key){ 
		tail = cur_1;
	}
	
	while (tail != NULL) {
		cur->sibling = tail;
		cities_by_id[cur->key]->priority_heap_node = cur;
		cur = cur->sibling;
		tail = tail->sibling;
	}
	cities_by_id[cur->key]->priority_heap_node = cur;
	return h;
}

void display_root_list(Node *h) {
        Node *print = h;
        while (print != NULL) {
                printf("[D[%d] = %d, %d] ", print->key, print->priority, print->degree);
                if (print->sibling != NULL) {
                        printf(" --> ");
                }
                print = print->sibling;
        }
        printf("\n");
}

Node *unify (Node *h1, Node *h2) {
	if (h1 == NULL) {
		return h2;
	}
	if (h2 == NULL) {
		return h1;
	}

	Node *h = merge (h1, h2);
		
	if (h == NULL) {
		return h;
	}
	
	Node *previous = NULL;
	Node *current = h;
	Node *next = current->sibling;
	
	while (next != NULL) {
		int need_merge = 1;
		if (current->degree != next->degree) {
			need_merge = 0;
		} else {
			if (next->sibling != NULL && next->sibling->degree == next->degree) {
				need_merge = 0;
			}
		}
		if (need_merge) {
			if (current->priority > next->priority) {
				current->sibling = next->sibling;
				link(current, next);

				next = current->sibling;				
			} else {
				if (previous != NULL) {
					previous->sibling = next;
					link(next, current);
				} else {
					h = next;
					link(next, current);
				}

				current = next;
                                next = current->sibling;
			}
		} else {
			previous = current;
                        current = next;
			next = current->sibling;
		}
	}
	return h;
}

Node *create_node(int key) { 
	City *city = cities_by_id[key];
	if (city->priority_heap_node != NULL) {
		free(city->priority_heap_node);
	}

	Node *p = (Node *)calloc(1, sizeof(Node));
	
	p->key = key;
	p->priority = city->priority;
	p->parent = NULL;
	p->child = NULL;
	p->sibling = NULL;
	p->degree = 0;
	
	city->priority_heap_node = p;
	cities_by_id[city->id] = city;
 	return p; 
}

void insert (Node **h, int key) { 
	Node *x = create_node(key);  
   	*h = unify(*h, x); 
} 

Node *extract_root (Node** h, int key) {
	if (*h == NULL) {
		return NULL;
	}

	Node *prev_root = NULL;
	Node *root = NULL; 	

        Node *current = *h;
        Node *previous = NULL;
        while (current != NULL) {
                if (current->key == key) {
                        root = current;
                        prev_root = previous;
			break;
                }
                previous = current;
                current = current->sibling;
        }

	if (root == NULL) {
		return NULL;
	}
        if (prev_root != NULL) {
                prev_root->sibling = root->sibling;
		cities_by_id[prev_root->key]->priority_heap_node = prev_root;
        } else {
		*h = root->sibling;
	}
        root->sibling = NULL;

        Node *root_children = NULL;

        current = root->child;
        Node *next = NULL;
        while (current != NULL) {
                current->parent = NULL;

                next = current->sibling;

                current->sibling = root_children;
                cities_by_id[current->key]->priority_heap_node = current;
                root_children = current;
                current = next;
        }

        *h = unify(*h, root_children);

	root->child = NULL;
	cities_by_id[root->key]->priority_heap_node = root;
	return root;
}

Node *extract_max(Node **h) {
	if (*h == NULL) {
		return NULL;
	} else {
		Node *max_node = get_max(*h);
		return extract_root(h, max_node->key);
	}
}

Node *shift_down (Node *x) {
        if (x == NULL) {
                return NULL;
        }

        Node *node = x;
        Node *child = x->child;
        while (child != NULL && node->priority < child->priority) {
                int temp = node->key;
                node->key = child->key;
                child->key = temp;

                temp = node->priority;
                node->priority = child->priority;
                child->priority = temp;

                cities_by_id[node->key]->priority_heap_node = node;
                cities_by_id[child->key]->priority_heap_node = child;

                node = child;
                child = node->child;
        }
        return node;
}

Node *uncond_shift_up (Node *x) {
	if (x == NULL) {
                return NULL;
        }

        Node *node = x;
        Node *parent = x->parent;
        while (parent != NULL) {
                int temp = node->key;
                node->key = parent->key;
                parent->key = temp;
		
		temp = node->priority;
		node->priority = parent->priority;
		parent->priority = temp;

                cities_by_id[node->key]->priority_heap_node = node;
                cities_by_id[parent->key]->priority_heap_node = parent;

                node = parent;
                parent = node->parent;
        }
	return node;
}

Node *delete (Node **h, Node *x) {  
	if (*h == NULL) { 
      		printf("Heap is empty\n"); 
      		return NULL;
   	}
	
	Node *root = uncond_shift_up(x);
	Node *extracted = extract_root(h, root->key);
	if (extracted == NULL) {
		//Wrong heap. Nothing to delete. revert shift up
		shift_down(root);
		return NULL;
	} else {
		//All good
		return extracted;
	}
} 

void display_binom_heap (Node* h) {  
	if (h == NULL) { 
		printf("Heap is empty\n"); 
		return; 
	} 
	printf("root nodes are:\n"); 
	display_root_list(h);
} 

Node *build_binom_heap (int *keys, int size) {
        Node *h = NULL;
        for (int i=0; i<size; i++) insert (&h, keys[i]);
        return h;
}

void drop(Pot_Route *pot_route) {
	pot_route->len = 0;
	pot_route->min_priority = -1;
}

void add_city(Pot_Route *pot_route, City *city) {
	(pot_route->len)++;
	if (pot_route->min_priority == -1) {
		pot_route->min_priority = city->priority;
	} else if(pot_route->min_priority > city->priority) {
		pot_route->min_priority = city->priority;
	}
}

void swap (int *A, int i_1, int i_2) {
        int buff = A[i_1];
        A[i_1] = A[i_2];
        A[i_2] = buff;
}

int *heapify (int *A, int i, int *V, int heap_size, int heap_mode) {
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
    			swap(A, i, largest);
        		A = heapify(A, largest, V, heap_size, heap_mode);
		}
	} else if (heap_mode == MIN_HEAP) {
		if (left < heap_size && V[A[left]] < V[A[i]]) {
                        smallest = left;
                }
                if (right < heap_size && V[A[right]] < V[A[smallest]]) {
                        smallest = right;
                }
                if (smallest != i) {
                        swap(A, i, smallest);
                        A = heapify(A, smallest, V, heap_size, heap_mode);
                }
	}	
	return A;
}

int *build_heap(int *A, int N, int *V, int heap_mode) {
  	int heap_size = N;
	for (int i = (int)(N/2); i >= 0; i--) {
    		A = heapify(A, i, V, heap_size, heap_mode);
	}
	return A;
}

int *heap_sort(int *A, int N, int *V, int heap_mode) {
	A = build_heap(A, N, V, heap_mode);
	int heap_size = N;

	for (int i = N-1; i >= 0; i--) {
		swap(A, 0, i);
       		heap_size--;
       		A = heapify(A, 0, V, heap_size, heap_mode);
	}
	return A;
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
int max_priority_city_id (int id_1, int id_2) {
	if (id_1 == -1 || id_2 == -1) {
		return id_1 == -1 ? id_2 : id_1;
	}
	City *city_1 = cities_by_id[id_1];
	City *city_2 = cities_by_id[id_2];
	return city_1->priority == MAX(city_1->priority, city_2->priority) ? id_1 : id_2;
}

int min_priority_city_id (int id_1, int id_2) {
	if (id_1 == -1 || id_2 == -1) {
                return id_1 == -1 ? id_2 : id_1;
        }
	City *city_1 = cities_by_id[id_1];
        City *city_2 = cities_by_id[id_2];
        return city_1->priority == MIN(city_1->priority, city_2->priority) ? id_1 : id_2;
}

void accum_knots (int *leafs, int leafs_size, int min_priority) {
	for (int i=0; i<leafs_size; i++) {
		City *city = cities_by_id[leafs[i]];
		City *prev_city = NULL;
		
		if (city->priority >= min_priority) {
			city->accum_route_len = 1;
			city->accum_min_priority_city_id = city->id;
			city->accum_max_priority_city_id = city->id;
		} else {
			city->accum_route_len = 0;
                        city->accum_min_priority_city_id = -1;
                        city->accum_max_priority_city_id = -1;
		}
		city->accumed = 1;

		do {
			prev_city = city;
			city = cities_by_id[prev_city->next_id];
			
			if (city->priority >= min_priority) {
				if (city->accumed == 0) {
					city->accum_route_len =  1 + prev_city->accum_route_len;	
					city->accum_max_priority_city_id = max_priority_city_id(city->id, prev_city->id);
					city->accum_min_priority_city_id = min_priority_city_id(city->id, prev_city->id);
				} else {
					city->accum_route_len = MAX(city->accum_route_len, prev_city->accum_route_len);
					int mx = max_priority_city_id(city->accum_max_priority_city_id, prev_city->accum_max_priority_city_id);                     			city->accum_max_priority_city_id = mx;
					int mn = min_priority_city_id(city->accum_min_priority_city_id, prev_city->accum_min_priority_city_id);
                                        city->accum_min_priority_city_id = mn;
				}
			} else {
				city->accum_route_len =  0;
                        	city->accum_max_priority_city_id = -1;
                        	city->accum_min_priority_city_id = -1;
			}
			city->accumed = 1;
			
		} while (city->next_id != city->id);
	}
}

int find_vacant_index_in_pool (int *ids_sorted_by_d, int pool_size, int id, int *D, int *d_pool) {
	int l_i = left_b_search (ids_sorted_by_d, DEC_SORT, 0, pool_size - 1, D, D[id]);
	int r_i = right_b_search (ids_sorted_by_d, DEC_SORT, 0, pool_size - 1, D, D[id]);

	if (l_i == -1 || r_i == -1) {
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

City *find_city_in_pool_to_replace(int *ids_sorted_by_d, int pool_size, City* city, int *D) {
	int l_i = left_b_search (ids_sorted_by_d, DEC_SORT, 0, pool_size - 1, D, city->priority);
        int r_i = right_b_search (ids_sorted_by_d, DEC_SORT, 0, pool_size - 1, D, city->priority);
	
	if (l_i == -1 || r_i == -1) {
                return NULL;
        }

	City* shortest_brunch_city = NULL;
	for (int i = l_i; i <= r_i; i++) {
		City *candidate = cities_by_id[ids_sorted_by_d[i]];
		if (candidate->d_pool_index != -1) {
			if (shortest_brunch_city == NULL) {
				shortest_brunch_city = candidate;
			} else {
				if (candidate->accum_route_len < shortest_brunch_city->accum_route_len) {
                        		shortest_brunch_city = candidate;
                		}
			}
		}
	}	
	return shortest_brunch_city;
}

void swap_complete (int *ids_sorted_by_d, City *city_1, City *city_2) {
	swap(ids_sorted_by_d, city_1->ids_sorted_by_d_index, city_2->ids_sorted_by_d_index);

	int buff = city_1->ids_sorted_by_d_index;
	city_1->ids_sorted_by_d_index = city_2->ids_sorted_by_d_index;
	city_2->ids_sorted_by_d_index = buff;

	buff = city_1->d_pool_index;
        city_1->d_pool_index = city_2->d_pool_index;
        city_2->d_pool_index = buff;
}

int check_routeness (
	int *ids_sorted_by_c,
	int *ids_sorted_by_d, 
	int **d_pool,
	int N, 
	int K, 
	City *city, 
	int min_priority, 
	int *D, 
	int *C, 
	int **visited,
	int *curr_route_len) {

	int sub_route_len = 0;	

	if ((*visited)[city->id] > 0) {
		return 0;
        }	

	if (city->priority < min_priority) {
		(*visited)[city->id] = 1;
		return 0;
	}


	int vacant_pool_index = find_vacant_index_in_pool(ids_sorted_by_d, K, city->id, D, *d_pool);

	if (vacant_pool_index == -1) {
		(*visited)[city->id] = 1;
		city->d_pool_index == -1;
		return -1;
	}
	
	int sum_route_len = *curr_route_len + pot_route->len;

	(*visited)[city->id] = 1;
	(*d_pool)[vacant_pool_index] = 1;

	City *vacant_pool_city = cities_by_id[ids_sorted_by_d[vacant_pool_index]];
	vacant_pool_city->d_pool_index = vacant_pool_index;

	if (vacant_pool_city->id != city->id) {
		swap_complete (ids_sorted_by_d, vacant_pool_city, city);
	}	
	
	//First delete then add
	Node *deleted = delete(&priority_heap, city->priority_heap_node);
	pot_route->priority_heap = unify(pot_route->priority_heap, deleted);

	add_city (pot_route, city);
	sub_route_len++;
	
	int pot_priority_threshold = priority_heap != NULL ? get_max(priority_heap)->priority : 0;

	int drop_needed = pot_route->min_priority >= pot_priority_threshold;	
	if (drop_needed && sum_route_len <= K) {
		(*curr_route_len) += pot_route->len;
		drop(pot_route);
		pot_route->firm_route = unify(pot_route->firm_route, pot_route->priority_heap);
		pot_route->priority_heap = NULL;
		sub_route_len = 0;
	}

	//find all adj
	int adj_id = find_vacant_adjacent_id(ids_sorted_by_c, N, city->id, C, *visited);
	if (adj_id == -1 && (*visited)[city->next_id] == 0) {
		adj_id = city->next_id;
	}

	while (adj_id != -1) {
		City *adj_city = cities_by_id[adj_id];		

		int ret = check_routeness(
			ids_sorted_by_c, 
			ids_sorted_by_d, 
			d_pool, 
			N, 
			K, 
			adj_city, 
			min_priority, 
			D, 
			C, 
			visited,
			curr_route_len);

		if (ret == -1) {            
			City *unsummoned = find_city_in_pool_to_replace(ids_sorted_by_d, K, adj_city, D);
					
			int worth_to_replace = 0;
			
			if (unsummoned != NULL) {
				
				if (unsummoned->accum_route_len < adj_city->accum_route_len) {
					worth_to_replace = 1;
				} else if (unsummoned->accum_route_len == adj_city->accum_route_len) {
					if (D[unsummoned->accum_max_priority_city_id] < D[adj_city->accum_max_priority_city_id]) {
						worth_to_replace = 1;
					} else if (D[unsummoned->accum_max_priority_city_id] == D[adj_city->accum_max_priority_city_id]) {
						if (D[unsummoned->accum_min_priority_city_id] < D[adj_city->accum_min_priority_city_id]) {
							worth_to_replace = 1;
						}
					}
				}
			}
			
			int need_to_replace = worth_to_replace && *curr_route_len < K;

			if (need_to_replace) {
				
				//First delete then insert
				Node *deleted = delete(&(pot_route->priority_heap), unsummoned->priority_heap_node);
				pot_route->priority_heap = unify(pot_route->priority_heap, deleted);
	
				if (unsummoned->accum_route_len > 1) {
					if (unsummoned->accum_max_priority_city_id != unsummoned->id) {
						City *max_in_branch = cities_by_id[unsummoned->accum_max_priority_city_id];
						
						//First delete then insert
						Node *deleted = NULL;
						deleted = delete(&(pot_route->priority_heap), max_in_branch->priority_heap_node);
						if (deleted != NULL) {
							priority_heap = unify(priority_heap, deleted);
						}
					}
					(pot_route->len) -= unsummoned->route_len_behind;
					
				} else {
					(pot_route->len)--;
				}
				
				swap_complete (ids_sorted_by_d, unsummoned, adj_city);

                                (*visited)[adj_city->id] = 0;
                                (*d_pool)[adj_city->d_pool_index] = 0;
			}	
		} else if (ret > 0 && adj_id != city->next_id) {
			sub_route_len += ret;
			city->route_len_behind = sub_route_len;
		} 

		adj_id = find_vacant_adjacent_id(ids_sorted_by_c, N, city->id, C, *visited);
		if (adj_id == -1 && (*visited)[city->next_id] == 0) {
                	adj_id = city->next_id;
        	}
	}
	return sub_route_len;
}

void free_binom_heap(Node *h) {
	if (h->sibling != NULL) {
		free_binom_heap(h->sibling);
	} else if (h->child != NULL) {
		free_binom_heap(h->child);
	}
}

int solution(int K, int C[], int D[], int N) {

	int *A_D = (int *)calloc(N, sizeof(int));
	int *A_C = (int *)calloc(N, sizeof(int));	
	cities_by_id = (City **)calloc(N, sizeof(City));
 
	for (int i=0; i<N; i++) {
		City *city = (City *)calloc(1, sizeof(City));
		city->id = i;
		city->next_id = C[i];
		city->priority = D[i];
		city->d_pool_index = -1;
		city->ids_sorted_by_d_index = -1;
		city->priority_heap_node = NULL;
		city->checked = 0;
		city->accumed = 0;
		city->accum_route_len = 0;
		city->accum_min_priority_city_id = -1;
		city->accum_max_priority_city_id = -1;
		
		cities_by_id[i] = city;
			
		A_D[i] = i;
                A_C[i] = i;
	}
	
	//sort cities in order decresing by D
        int *ids_sorted_by_d = heap_sort(A_D, N, D, MIN_HEAP);
	for (int i=0; i<N; i++) {
		cities_by_id[ids_sorted_by_d[i]]->ids_sorted_by_d_index = i;
	}
        //Sort cities in order increasing by C
        int *ids_sorted_by_c = heap_sort(A_C, N, C, MAX_HEAP);

	int min_priority = D[ids_sorted_by_d[K-1]];
	int max_priority = D[ids_sorted_by_d[0]];	

	pot_route = (Pot_Route *)calloc(1, sizeof(Pot_Route));
	priority_heap = build_binom_heap(ids_sorted_by_d, N);	
	pot_route->priority_heap = NULL;
	pot_route->firm_route = NULL;

	int *leafs = (int *)calloc(N, sizeof(int));
	int size = 0;
	//n time
	find_leafs (&leafs, &size, ids_sorted_by_c, C, N);
	//n log n time
	accum_knots (leafs, size, min_priority);

	int max_route_len = 0;
	// TOO SLOW with loop
	for (int k=0; k<N; k++) {		
		City *city = cities_by_id[ids_sorted_by_d[k]];
		if (city->priority < max_priority || city->checked == 1) {
			continue;
		}

		int *d_pool = (int *)calloc(K, sizeof(int));
		int *B = (int *)calloc(N, sizeof(int));
		drop(pot_route);
		
		priority_heap = unify(priority_heap, pot_route->priority_heap);
		pot_route->priority_heap = NULL;
		
		//WRONG ANSWER in Linestar if reuse B		

		int curr_route_len = 0;

		check_routeness(
			ids_sorted_by_c, 
			ids_sorted_by_d, 
			&d_pool, 
			N, 
			K, 
			city, 
			min_priority, 
			D, 
			C, 
			&B,
			&curr_route_len);			
		
		while (pot_route->firm_route != NULL) {
			Node *extracted = extract_max(&(pot_route->firm_route));
			cities_by_id[extracted->key]->checked = 1;
			priority_heap = unify(priority_heap, extracted);
		}	
	
		free(d_pool);	
		free(B);

		if (curr_route_len > max_route_len) {
			max_route_len = curr_route_len;
		}
	}

	if (max_route_len > K) {
		max_route_len = K;
	}	

	if (pot_route->priority_heap != NULL) {
		free(pot_route->priority_heap);
	}
	if (pot_route->firm_route != NULL) {
		free(pot_route->firm_route);
	}
	free(pot_route);
	free(priority_heap);
	
	for (int i=0; i<N; i++) free(cities_by_id[i]);
	free(cities_by_id);
	free(leafs);
	free(A_C);
        free(A_D);

	return max_route_len;
}

int main () {
/*medium random tree*/
{
	int N = 200;
	int K = 36;
	
	int C[200] = {172,13,124,53,45,94,54,135,47,148,71,188,91,31,88,1,81,108,158,66,86,57,0,123,34,10,137,119,102,152,126,19,11,178,57,176,175,167,173,41,85,153,108,58,133,43,39,99,76,83,102,195,149,69,44,81,28,14,103,18,9,185,30,131,2,71,18,78,138,148,195,149,98,147,130,158,147,176,9,124,79,35,122,138,167,114,96,38,101,133,182,3,11,161,71,183,19,45,183,126,20,35,171,9,136,168,135,84,6,147,144,8,105,16,114,55,72,66,123,124,13,26,70,51,143,45,160,53,112,5,63,98,124,24,177,123,59,15,147,136,24,110,19,196,95,92,84,162,133,185,95,31,49,24,117,158,13,64,42,199,159,183,3,15,125,73,57,16,97,172,101,21,123,125,141,152,94,16,27,45,62,36,185,59,36,0,108,47,139,36,58,173,138,176,19,180,128,165,101,114};

	int D[200] = {53,23,21,43,50,50,31,100,75,15,50,4,50,28,13,45,10,36,50,50,31,27,50,60,31,50,37,33,12,0,98,15,42,12,13,0,22,0,50,6,70,7,42,43,45,50,43,99,50,50,46,61,50,7,44,30,44,21,44,50,5,50,57,15,25,50,50,20,50,39,80,50,26,50,6,50,50,33,10,10,11,5,80,50,38,93,8,5,35,29,50,11,32,37,50,18,21,50,30,78,24,6,44,39,33,50,53,0,1,50,39,54,16,31,99,45,20,50,76,27,23,34,95,71,45,50,73,48,48,5,42,12,15,44,42,62,50,40,50,15,37,20,14,20,47,0,13,50,27,50,35,32,16,3,3,50,31,3,50,69,63,46,43,16,50,50,21,4,50,66,27,41,84,50,9,30,50,24,1,50,77,9,50,50,12,50,5,52,14,23,40,50,50,46,33,58,0,50,13,74};

	if (36 == solution(K, C, D, N)) {
		printf("Medium random tree : OK\n");
	} else {
		printf("Medium random tree : FAILED\n");
	}
}
/*star*/
{
	int N = 100;
	int K = 100;

	int C[100] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	int D[100] = {0,7,5,7,3,9,9,7,3,6,4,4,10,10,2,9,7,5,4,8,6,8,7,3,8,10,10,7,2,2,3,10,4,5,10,4,6,5,2,7,9,3,4,5,2,6,9,7,6,9,3,7,5,7,3,8,2,3,9,10,5,5,4,4,7,3,3,6,3,7,9,8,6,5,5,2,8,4,4,2,6,7,5,9,8,4,6,10,6,7,2,6,6,5,5,7,6,6,3,5};

	if (100 == solution(K, C, D, N)) {
                printf("Large star : OK\n");
        } else {
                printf("Large star : FAILED\n");
        }
}
/* similar levels*/
{
	int N = 19;
	int K = 18;	

	int C[19] = {8,18,9,1,1,17,12,4,16,13,1,12,13,10,11,9,13,4,18};
	int D[19] = {1,2,9,7,1,7,4,2,0,0,4,8,5,6,5,0,3,9,5};

	if (18 == solution(K, C, D, N)) {
                printf("Similar levels : OK\n");
        } else {
                printf("Similar levels : FAILED\n");
        }
}
/*line */
{
	int N = 20;
	int K = 6;
	
	int C[20] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 19};
	int D[20] = {0, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 8, 8, 8, 0, 8, 8, 0, 0, 0};
	
	if (5 == solution(K, C, D, N)) {
                printf("Custom line : OK\n");
        } else {
                printf("Custom line : FAILED\n");
        }
}
/*star */
{
        int N = 20;
        int K = 7;

        int C[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        int D[20] = {8, 0, 0, 8, 8, 8, 8, 8, 0, 0, 0, 0, 8, 8, 8, 8, 8, 0, 0, 0};

	if (7 == solution(K, C, D, N)) {
                printf("Custom star : OK\n");
        } else {
                printf("Custom star : FAILED\n");
        }

}
/* non conseq levels*/
{
	int N = 7;
	int K = 6;

	int C[7] = {0, 0, 3, 1, 3, 4, 5};
	int D[7] = {1000000, 14, 100, 15, 99, 0, 2};

	if (5 == solution(K, C, D, N)) {
                printf("Non consequtive levels : OK\n");
        } else {
                printf("Non consequtive levels: FAILED\n");
        }
}

/*first test*/
{
	int N = 7;
	int K = 2;

	int C[7] = {1, 3, 0, 3, 2, 4, 4};
	int D[7] = {6, 2, 7, 5, 6, 5, 2};

	if (2 == solution(K, C, D, N)) {
                printf("First test : OK\n");
        } else {
                printf("First test : FAILED\n");
        }
}
/* small random tree */
{
	int N = 15; 
	int K = 8;

	int C[15] = {4,5,7,2,7,8,2,8,10,4,10,9,3,0,6};
	int D[15] = {100,99,100,100,99,99,99,100,99,99,100,97,95,96,96};
	
	if (8 == solution(K, C, D, N)) {
                printf("Small random tree : OK\n");
        } else {
                printf("Small random tree : FAILED\n");
        }
}
/*line star*/
{
	int N = 60;
	int K = 20;

	int C[60] = {1,20,47,17,0,58,9,30,35,21,52,33,16,29,15,43,27,42,28,37,57,59,23,6,10,34,14,8,49,24,3,7,54,25,34,55,4,22,56,2,19,5,32,40,18,11,12,46,26,50,51,39,36,41,13,45,48,53,44,31};
	int D[60] = {10,10,3,9,10,5,2,7,5,3,10,3,0,5,4,5,5,5,9,5,10,4,0,1,10,1,3,5,8,10,8,6,5,2,0,5,10,5,0,4,5,5,5,5,5,4,1,2,2,7,6,5,10,5,5,5,1,10,5,5};

	int sol = solution(K, C, D, N);
	if (10 == sol) {
                printf("Line star : OK\n");
        } else {
                printf("Line star : FAILED, sol : %d\n", sol);
        }
}
	return 0;
}
