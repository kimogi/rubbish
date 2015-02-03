void heap_levels(int *A, int size, int i, int **out_buff) {
        if (i == 0) {
                (*out_buff)[i] = 0;
        } else {
                int parent_i = (int)((i-1) - (i-1)%2)/2;
                (*out_buff)[i] = (*out_buff)[parent_i] + 1;
        }

        int left = 2*i + 1;
        if (left < size) {
                heap_levels (A, size, left, out_buff);
        }
        int right = 2*i + 2;
        if (right < size) {
                heap_levels (A, size, right, out_buff);
        }
}

void heap_display (int *A, int size) {
        int *levels = (int *)calloc(size, sizeof(int));
        heap_levels(A, size, 0, &levels);

        int curr_level = levels[0];
        for (int i=0; i<size; i++) {
                if (curr_level < levels[i]) {
                        printf("\n");
                        curr_level = levels[i];
                }
                printf("[%d]", A[i]);
        }

        printf("\n");
        free(levels);
}

