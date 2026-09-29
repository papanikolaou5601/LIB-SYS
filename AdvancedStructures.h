#ifndef ADVANCED_STRUCTURES_H
#define ADVANCED_STRUCTURES_H

    /* Finds a book in the AVL tree */
    book_index_t *find_book_inAVL(genre_t *genre , char title[TITLE_MAX] ) { 
        book_index_t *current = genre->book_index;
        while (current != NULL && strcmp(current->title, title) != 0) {
            if (strcmp(title, current->title) < 0) {
                current = current->lc;
            } else {
                current = current->rc;
            }
        }
        return current;
    } 

    /* Returns the height of a node */
    int get_height(book_index_t *node) {
        if (node == NULL){
            return 0;
        }else{
            return node->height;
        }
        
    }

    /* Returns the maximum of two integers */
    int max_value(int a, int b) {
        if(a> b) return a;
        else return b;
    }

    /* Returns the balance factor of a node */
    int get_balance(book_index_t *node) {
        if (node == NULL) return 0;
        return get_height(node->lc) - get_height(node->rc);
    }

    /* Returns the node after a left rotation */
    book_index_t *left_rotate(book_index_t *x) {
        if (x == NULL || x->rc == NULL) return x;
        book_index_t *y = x->rc;
        book_index_t *T2 = y->lc;
        y->lc = x;
        x->rc = T2;
        x->height = 1 + max_value(get_height(x->lc), get_height(x->rc));
        y->height = 1 + max_value(get_height(y->lc), get_height(y->rc));
        return y;
    }

    /* Returns the node after a right rotation */
    book_index_t *right_rotate(book_index_t *y) {
        if (y == NULL || y->lc == NULL) return y;
        book_index_t *x = y->lc;
        book_index_t *T2 = x->rc;
        x->rc = y;
        y->lc = T2;
        y->height = 1 + max_value(get_height(y->lc), get_height(y->rc));
        x->height = 1 + max_value(get_height(x->lc), get_height(x->rc));
        return x;
    }

    /* Inserts a book into the AVL tree */
    book_index_t *insert_book_inAVL(book_index_t *root, book_t *book) {
        if (book == NULL) {
            printf("Error: NULL book pointer!\n");
            return root;
        }
        if (root == NULL) {
            book_index_t *new_node = (book_index_t *)malloc(sizeof(book_index_t));
            if (!new_node) {
                printf("Memory allocation failed!\n");
                return NULL;
            }
            strncpy(new_node->title, book->title, TITLE_MAX - 1);
            new_node->title[TITLE_MAX - 1] = '\0';
            new_node->book = book;
            new_node->lc = NULL;
            new_node->rc = NULL;
            new_node->height = 1;
            return new_node;
        }
        if (root->book == NULL) {
            printf("Error: Invalid root book data!\n");
            return root;
        }
        int cmp = strcmp(book->title, root->book->title);
        if (cmp < 0) {
            root->lc = insert_book_inAVL(root->lc, book);
        } else if (cmp > 0) {
            root->rc = insert_book_inAVL(root->rc, book);
        } else {
            return root;  
        }
        root->height = 1 + max_value(get_height(root->lc), get_height(root->rc));
        int balance = get_balance(root);
        if (balance > 1 && root->lc && root->lc->book && 
            strcmp(book->title, root->lc->book->title) < 0) {
            return right_rotate(root);
        }
        if (balance < -1 && root->rc && root->rc->book &&
            strcmp(book->title, root->rc->book->title) > 0) {
            return left_rotate(root);
        }
        if (balance > 1 && root->lc && root->lc->book &&
            strcmp(book->title, root->lc->book->title) > 0) {
            root->lc = left_rotate(root->lc);
            return right_rotate(root);
        }
        if (balance < -1 && root->rc && root->rc->book &&
            strcmp(book->title, root->rc->book->title) < 0) {
            root->rc = right_rotate(root->rc);
            return left_rotate(root);
        }
        return root;
    }

    /* Creates or updates a member's activity record */
    void create_member_activity(library_t *lib, int Sid, int is_loan) {
        member_activity_t *current = lib->activity;
        member_activity_t *prev = NULL;
        while (current) {
            if (current->sid == Sid) {
                break;
            }
            prev = current;
            current = current->next;
        }
        if (!current) {
            member_activity_t *new_activity = malloc(sizeof(member_activity_t));
            if (!new_activity) return;
            member_t *member = find_member(lib, Sid);
            member->activity = new_activity;
            new_activity->sid = Sid;
            new_activity->loans_count = 0;
            new_activity->reviews_count = 0;
            new_activity->score_sum = 0;
            new_activity->next = NULL;
            if (!prev) {
                lib->activity = new_activity;
            } else {
                prev->next = new_activity;
            }
            current = new_activity;
        }
        if (is_loan) {
            current->loans_count++;
            current->score_sum += 1;
        } else {
            current->reviews_count++;
            current->score_sum += 2;
        }
    }

    /* Swaps two books */
    void swap_books(book_t **a, book_t **b) {
        book_t *tmp = *a;
        *a = *b;
        *b = tmp;
    }

    /* Returns 1 if book a should be above book b in the min-heap */
    int should_be_above(book_t *a, book_t *b) {
        return (a->avg < b->avg) || 
            (a->avg == b->avg && a->bid < b->bid);
    }

    /* Maintains the min-heap property by moving an element down */
    void heapify_down(recheap_t *heap, int i) {
        int smallest = i;
        int left = 2 * i;
        int right = 2 * i + 1;
        if (left <= heap->size && should_be_above(heap->heap[left], heap->heap[smallest])) {
            smallest = left;
        }
        if (right <= heap->size && should_be_above(heap->heap[right], heap->heap[smallest])) {
            smallest = right;
        }
        if (smallest != i) {
            swap_books(&heap->heap[i], &heap->heap[smallest]);
            heapify_down(heap, smallest);
        }
    }

    /* Maintains the min-heap property by moving an element up */
    void heapify_up(recheap_t *heap, int i) {
        while (i > 1) {
            int parent = i / 2;
            if (should_be_above(heap->heap[i], heap->heap[parent])) {
                swap_books(&heap->heap[i], &heap->heap[parent]);
                i = parent;
            } else {
                break;
            }
        }
    }

    /* Adds a book to the recommendation heap */
    void add_to_recheap(library_t *lib, book_t *book) {
        if (!lib->recommendations) {
            lib->recommendations = malloc(sizeof(recheap_t));
            lib->recommendations->size = 0;
        }
        recheap_t *heap = lib->recommendations;
        if (heap->size < RECHEAP_CAPACITY) {
            heap->size++;
            heap->heap[heap->size] = book;
            book->heap_pos = heap->size;
            heapify_up(heap, heap->size);
        } else {
            if (book->avg > heap->heap[1]->avg ||
                (book->avg == heap->heap[1]->avg && book->bid < heap->heap[1]->bid)) {
                heap->heap[1] = book;
                book->heap_pos = 1;
                heapify_down(heap, 1);
            }
        }
    }

    /* Prints the top N books from the recommendation heap */
    void print_Top_Books(library_t *lib, int N) {
        if (!lib || !lib->recommendations || N <= 0) {
            printf("Invalid input or no recommendations available\n");
            return;
        }
        printf("Top %d Books:\n", N);
        recheap_t *recheap = lib->recommendations;
        int limit = (N < recheap->size) ? N : recheap->size;
        for (int i = 1; i <= limit; i++) {
            book_t *temp = recheap->heap[i];
            if (temp != NULL) {
                printf("%d, %s, avg=%d\n", temp->bid, temp->title, temp->avg);
            } else {
                printf("empty\n");
            }
        }
        if (N > recheap->size) {
            for (int i = recheap->size + 1; i <= N; i++) {
                printf("empty\n");
            }
        }
    }

    /* Updates a book in the recommendation heap */
    void update_book_in_heap(library_t *lib, book_t *book) {
        if (!lib || !lib->recommendations) return;
        recheap_t *heap = lib->recommendations;
        int found_index = -1;
        for (int i = 1; i <= heap->size; i++) {
            if (heap->heap[i] == book) {
                found_index = i;
                break;
            }
        }
        if (found_index != -1) {
            heapify_up(heap, found_index);
        } else {
            add_to_recheap(lib, book);
        }
    }

#endif
