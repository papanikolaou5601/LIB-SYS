#ifndef ADVANCED_OPERATIONS_H
#define ADVANCED_OPERATIONS_H

    /* Deletes a book from the AVL tree */
    book_index_t *delete_book_fromAVL(book_index_t *root, char title[TITLE_MAX]) {
        if (root == NULL) {
            return root;
        }
        if (strcmp(title, root->book->title) < 0) {
            root->lc = delete_book_fromAVL(root->lc, title);
        } else if (strcmp(title, root->book->title) > 0) {
            root->rc = delete_book_fromAVL(root->rc, title);
        } else {
            if ((root->lc == NULL) ) {
                book_index_t *temp;
                temp = root->rc;
                root = NULL;
                return temp;
                free(temp);

            } else if (root->rc == NULL) {
                book_index_t *temp;
                temp = root->lc;
                root = NULL;
                return temp;
                free(temp);
                
            } else {
                book_index_t *temp = root->rc;
                while (temp->lc != NULL) {
                    temp = temp->lc;
                }
                root->book = temp->book;
                root->rc = delete_book_fromAVL(root->rc, temp->book->title);
            }
        }
        if (root == NULL) {
            return root;
        }
        root->height = 1 + max_value(get_height(root->lc), get_height(root->rc));
        /* kanei update to balance factor kai isoropoiei to dentro */
        int balance = get_balance(root);
        if (balance > 1 && get_balance(root->lc) >= 0) {
            return right_rotate(root);
        }
        if (balance > 1 && get_balance(root->lc) < 0) {
            root->lc = left_rotate(root->lc);
            return right_rotate(root);
        }
        if (balance < -1 && get_balance(root->rc) <= 0) {
            return left_rotate(root);
        }
        if (balance < -1 && get_balance(root->rc) > 0) {
            root->rc = right_rotate(root->rc);
            return left_rotate(root);
        }
        return root;
    }

    /* Changes the title of a book */
    book_index_t *Change_Book_Title(genre_t *genre, int Bid, char new_title[TITLE_MAX]) {
        book_t *book = find_book(genre, Bid);
        if (!book) return NULL;
        genre->book_index = delete_book_fromAVL(genre->book_index, book->title);
        strncpy(book->title, new_title, TITLE_MAX - 1);
        book->title[TITLE_MAX - 1] = '\0';
        genre->book_index = insert_book_inAVL(genre->book_index, book);
        return genre->book_index;
    }

    

    /* Prints a book from the AVL tree */
    void print_Book_fromAVL(genre_t *genre, char title[TITLE_MAX]) {
        book_index_t *node = find_book_inAVL(genre, title);
        if (node != NULL) {
            book_t *book = node->book;
            printf("FOUND: %d, %s, avg=%d\n", book->bid, book->title, book->avg);
        } else {
            printf("NOT FOUND\n");
        }
    }

    /* Finds the most active member */
    member_activity_t *find_Most_Active_Member(library_t *lib) {
        if (!lib || !lib->activity) return NULL;
        member_activity_t *most_active = NULL;
        member_activity_t *current = lib->activity;
        while (current) {
            if (!most_active || 
                current->score_sum > most_active->score_sum ||
                (current->score_sum == most_active->score_sum && 
                current->sid < most_active->sid)) {
                most_active = current;
            }
            current = current->next;
        }
        return most_active;
    }

    /* Prints the most active member */
    void print_Most_Active_Member(library_t *lib) {
        printf("Most Active Members:\n");
        if (!lib || !lib->activity) {
            printf("(empty)\n");
            return;
        }
        member_activity_t *current = lib->activity;
        int max_score = 0;
        while (current) {
            if (current->score_sum > max_score) {
                max_score = current->score_sum;
            }
            current = current->next;
        }
        if (max_score == 0) {
            printf("(empty)\n");
            return;
        }
        current = lib->activity;
        while (current) {
            if (current->score_sum == max_score) {
                member_t *member = find_member(lib, current->sid);
                if (member) {
                    printf("%d, %s, loans=%d, reviews=%d\n", 
                        current->sid, member->name, 
                        current->loans_count, current->reviews_count);
                }
            }
            current = current->next;
        }
    }

#endif
