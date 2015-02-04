#include <stdlib.h>
#include <stdio.h>

typedef struct node_t {
	int key;
	struct node_t *parent;
	struct node_t *child;
	struct node_t *sibling;
	int degree;
} Node;

Node **nodes_by_key;

Node *get_max(Node *h) {
	Node *best_node = h;
	Node *root = h;
	while (root != NULL) {
		if (root->key > best_node->key) {
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
	
	nodes_by_key[root->key] = root;
	nodes_by_key[branch->key] = branch;
} 

Node *merge (Node *h1, Node *h2) {
	Node *h;
	Node *cur_2;
	Node *cur_1;	
	Node *cur;
	Node *tail;

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
			nodes_by_key[cur->key] = cur;
			cur = cur->sibling;
			cur_2 = cur_2->sibling;
		} else {
			cur->sibling = cur_1;
			nodes_by_key[cur->key] = cur;
			cur = cur->sibling;
			cur_1 = cur_1->sibling;
		}
	}
	if (cur_1 == NULL) {
		tail = cur_2;
	} else { 
		tail = cur_1;
	}
	
	while (tail != NULL) {
		cur->sibling = tail;
		nodes_by_key[cur->key] = cur;
		cur = cur->sibling;
		tail = tail->sibling;
	}
	return h;
}

void display_root_list(Node *h) {
        Node *print = h;
        while (print != NULL) {
                printf("[%d, %d] ", print->key, print->degree);
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
	printf("Merged root lost");
	display_root_list(h);
		
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
			printf("curr key : %d, next key : %d\n", current->key, next->key);
			if (current->key > next->key) {
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
			display_root_list(h);
		} else {
			previous = current;
                        current = next;
			next = current->sibling;
		}
	}
	return h;
}

Node *create_node(int key) { 
	Node *p = (Node *)calloc(1, sizeof(Node)); 
	p->key = key;

	nodes_by_key[key] = p; 
 	return p; 
}

Node *insert (Node *h, int key) { 
	Node *x = create_node(key);  
   	h = unify(h, x); 
   	return h; 
} 

Node *extract_max (Node **h) {
	Node *prev_max_node = NULL;
	Node *max_node = *h;

        Node *current = *h;
	Node *previous = NULL;
        while (current != NULL) {
                if (current->key > max_node->key) {
                        max_node = current;
			prev_max_node = previous;
                }
		previous = current;
                current = current->sibling;
        }
	
	if (prev_max_node != NULL) {
		prev_max_node->sibling = max_node->sibling;
	}
	max_node->sibling = NULL;

	Node *root_list = NULL;
	
	current = max_node->child;	
	Node *next = NULL;
	while (current != NULL) {
		current->parent = NULL;
		
		next = current->sibling;
			
		current->sibling = root_list;
		nodes_by_key[current->key] = current;
		root_list = current;
		printf("route_list : \n");		
		display_root_list(root_list);
		current = next;
	}
	
	*h = unify(*h, root_list);

	nodes_by_key[max_node->key] = NULL;
	return max_node;
}


Node *extract_root (Node** h, int key) {
  	printf("extract root : %d\n", key);
	Node *prev_root = NULL;
	Node *root = *h; 	

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

        if (prev_root != NULL) {
                prev_root->sibling = root->sibling;
        }
        root->sibling = NULL;

        Node *root_children = NULL;

        current = root->child;
        Node *next = NULL;
        while (current != NULL) {
                current->parent = NULL;

                next = current->sibling;

                current->sibling = root_children;
                nodes_by_key[current->key] = current;
                root_children = current;
                printf("route_list : \n");
                display_root_list(root_children);
                current = next;
        }

        *h = unify(*h, root_children);

        nodes_by_key[root->key] = NULL;
	return root;
}

Node *shift_to_root (Node *x) {
        printf("shift to root\n");
	if (x == NULL) {
                printf("INVALID CHOICE OF NODE TO SHIFT\n");
                return x;
        }

        Node *node = x;
        Node *parent = x->parent;
        while (parent != NULL) {
                int temp = node->key;
                node->key = parent->key;
                parent->key = temp;

                nodes_by_key[node->key] = node;
                nodes_by_key[parent->key] = parent;

                node = parent;
                parent = node->parent;
        }
	return node;
}

void delete (Node **h, Node *x) {  
	if (*h == NULL) { 
      		printf("HEAP EMPTY\n"); 
      		return;
   	} 
	Node *root = shift_to_root(x); 
	display_root_list(*h);
 	extract_root(h, root->key); 
} 

void display_binom_heap (Node* h) {  
	if (h == NULL) { 
		printf("HEAP EMPTY\n"); 
		return; 
	} 
	printf("THE ROOT NODES ARE:\n"); 
	display_root_list(h);
} 

Node *build_binom_heap (int *keys, int size) {
        Node *h = NULL;
        for (int i=0; i<size; i++) {
		printf("inserting key : %d\n", keys[i]);
		h = insert (h, keys[i]);
		display_binom_heap(h);
		printf("---------------------------------\n");
        }
        return h;
}

int main () {
	nodes_by_key = (Node **)calloc(10, sizeof(Node*));	
	int size = 10;
	int keys[10] = {1, 0, 2, 8, 3, 9, 4, 5, 6, 7};
	
	Node *h1 = build_binom_heap(keys, 10);
	
	printf("heap\n");
	for (int i=0; i<size; i++) {
                if (nodes_by_key[i] != NULL) 
                        printf("[%d, %d]", i, nodes_by_key[i]->key);
                else
                        printf("NULL ");
        }
	printf("\n");
	display_binom_heap(h1);

	printf("get max : %d\n", get_max(h1)->key);
	printf("extract max : %d\n", extract_max(&h1)->key);
		
	printf("heap\n");
        for (int i=0; i<size; i++) {
		if (nodes_by_key[i] != NULL)
			printf("[%d, %d]", i, nodes_by_key[i]->key);
		else
			printf("NULL ");
	}
        printf("\n"); 
	display_binom_heap(h1);

	printf("Delete [%d, %d]\n", 5, nodes_by_key[5]->key);
	delete(&h1, nodes_by_key[5]);
	printf("heap\n");
        for (int i=0; i<size; i++) {
                if (nodes_by_key[i] != NULL)
                        printf("[%d, %d]", i, nodes_by_key[i]->key);
                else
                        printf("NULL ");
        }
        printf("\n"); 
	display_binom_heap(h1);	

	return 0; 
}
