#ifndef MEMORY_H
#define MEMORY_H

    /* Free memory allocated for loans */
    void free_loans(loan_t *head) {
        if (!head) return;
        loan_t *curr = head;
        while (curr) {
            loan_t *next = curr->next;
            free(curr);
            curr = next;
        }
    }

    /* Free memory allocated for books */
    void free_books(book_t *head) {
        if (!head) return;
        book_t *curr = head;
        while (curr) {
            book_t *next = curr->next;
            free(curr);
            curr = next;
        }
    }

    /* Free memory allocated for book index (AVL tree) */
    void free_book_index(book_index_t *root) {          
        if (!root) return;
        free_book_index(root->lc);
        free_book_index(root->rc);
        free(root);
    }

    /* Free memory allocated for genres */
    void free_genres(genre_t *head) {
        if (!head) return;
        genre_t *curr = head;
        while (curr) {
            genre_t *next = curr->next;
            free_book_index(curr->book_index);
            free(curr->display);
            free_books(curr->books);
            free(curr);
            curr = next;
        }
    }

    /* Free memory allocated for member activities */
    void free_member_activities(member_activity_t *head) {          
        if (!head) return;
        member_activity_t *curr = head;
        while (curr) {
            member_activity_t *next = curr->next;
            free(curr);
            curr = next;
        }
    }

    /* Free memory allocated for members */
    void free_members(member_t *head) {
        if (!head) return;
        member_t *curr = head;
        while (curr) {
            member_t *next = curr->next;
            free_loans(curr->loans);
            free(curr);
            curr = next;
        }
    }

    /* Free memory allocated for recommendations heap */
    void free_recheap(recheap_t *recheap) {          
        if (!recheap) return;
        free(recheap);
    }

    /* Free memory allocated for the entire library */
    int free_library(library_t *lib) {
        if (!lib) return 0;
        free_genres(lib->genres);
        free_members(lib->members);
        free_member_activities(lib->activity);       
        free_recheap(lib->recommendations);          
        free(lib);
        return 0;
    }

#endif
