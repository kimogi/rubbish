#include <stdlib.h>
#include <stdio.h>

#define INC_SORT 2
#define DEC_SORT 3

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

int main () {
	int A[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	int inc_sorted[10] = {1, 1, 1, 2, 2, 3, 4, 4, 5, 5};
	int dec_sorted[10] = {9, 9, 9, 7, 4, 4, 3, 2, 1, 1};	

	printf("\n");
	for (int i=0; i<10; i++) printf("%d ", inc_sorted[i]);
	printf("\nkey == 5\n");
	printf("left : %d\n", left_b_search(A, INC_SORT, 0, 9, inc_sorted, 5));
	printf("right : %d\n", right_b_search(A, INC_SORT, 0, 9, inc_sorted, 5));

	return 0;
}
