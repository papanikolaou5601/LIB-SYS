#ifndef LIBRARY_OPERATIONS_H
#define LIBRARY_OPERATIONS_H

    /* Insert Book in Genre */
    book_t *AddBook(library_t *lib,int Bid,int Gid,char Title[TITLE_MAX]) {
        /* ================================ Check if the genre exists ================================== */
        genre_t *genre = find_genre(lib, Gid);
        if (!genre) return NULL;
        /* ========================= Check if the book already exists in the genre ===================== */
        genre_t *g = lib->genres;
        while (g) {
            if (find_book(g, Bid)) return NULL;
            g = g->next;
        }
        /* ============================== Create the book ============================================== */
        book_t *new_book = malloc(sizeof(book_t));
        if (!new_book) return NULL;
        new_book->bid = Bid;
        new_book->gid = Gid;
        strncpy(new_book->title, Title, TITLE_MAX - 1);
        new_book->title[TITLE_MAX - 1] = '\0';
        new_book->sum_scores = 0;
        new_book->n_reviews = 0;
        new_book->avg = 0;
        new_book->lost_flag = 0;
        new_book->next = NULL;
        new_book->prev = NULL;
        new_book->heap_pos = -1;         
        /* ========================= After creating and inserting the book ============================= */
        if (new_book) {
            insert_book_sorted(genre, new_book);
            genre->book_index = insert_book_inAVL(genre->book_index, new_book);
        }
        return new_book;
    }

    /* Insert Member in List */
    member_t *AddMember(library_t *lib,int Sid,char Name[NAME_MAX]) {
        /* =========================== Check if the member already exists ============================== */
        if (find_member(lib, Sid)) return NULL;
        /* ========================= Create the member and loan ======================================== */
        member_t *new_member = malloc(sizeof(member_t));
        if (!new_member) return NULL;
        new_member->sid = Sid;
        strncpy(new_member->name, Name, NAME_MAX - 1);
        new_member->name[NAME_MAX - 1] = '\0';
        new_member->loans = malloc(sizeof(loan_t));
        if (!new_member->loans) {
            free(new_member);
            return NULL;
        }
        new_member->loans->sid = -1;
        new_member->loans->bid = -1;
        new_member->loans->next = NULL;   
        /* ========================= Insert the member in the correct position ========================= */
        member_t *current_m = lib->members;
        member_t *prev = NULL; 
        while (current_m && current_m->sid < Sid) {
            prev = current_m;
            current_m = current_m->next;
        }
        if (!prev) {
            new_member->next = lib->members;
            lib->members = new_member;
        } else {
            new_member->next = prev->next;
            prev->next = new_member;
        }
        return new_member;
    }

    /* Change the status of a books loan */
    loan_t *LoanBook(library_t *lib,int Sid,int Bid) {
        /* ========================== Check if the member exists ======================================= */
        member_t *member = find_member(lib, Sid);
        if (!member) return NULL;
        /* ========================== Find the book in any genre ======================================= */
        book_t *book = NULL;
        genre_t *genre = lib->genres;
        while (genre && !book) {
            book = find_book(genre, Bid);
            if (!book) genre = genre->next;
        }
        if (!book) return NULL;
        /* ========================== Check if the book is already loaned ============================== */
        if (find_loan(member, Bid)) return NULL;
        /* ======================================= Create the loan ===================================== */
        loan_t *new_loan = malloc(sizeof(loan_t));
        if (!new_loan) return NULL;
        new_loan->sid = Sid;
        new_loan->bid = Bid;
        new_loan->next = member->loans->next; 
        member->loans->next = new_loan;
        create_member_activity(lib, Sid, 1);         
        return new_loan;
    }

    /* Update the library's list to reflect that a book has been returned */
    book_t *ReturnBook(library_t *lib,int Sid,int Bid,char Score[10],char status[10],int score){
        /* ========================== Check if the member exists ======================================= */
        member_t *member = find_member(lib, Sid);
        if (!member) return NULL;
        /* ========================== Check if the loan exists ========================================= */
        loan_t *loan = find_loan(member, Bid);
        if (!loan){
            return NULL;
        }
        /* ========================== Find the book in any genre ======================================= */
        book_t *book = NULL;
        genre_t *genre = lib->genres;
        while (genre && !book) {
            book = find_book(genre, Bid);
            if (!book) genre = genre->next;
        }
        if (!book) return NULL;
        /* ========================== Update the book's status ========================================= */
        if (strcmp(status, "lost") == 0) {
            book->lost_flag = 1;
            genre->lost_count++;
        } 
        else if (strcmp(status, "ok") == 0) {
            if (strcmp(Score, "NA") == 0) {
                /* ========================== Remove the loan ========================================== */
                loan_t *prev = member->loans;
                loan_t *curr = member->loans->next;
                while (curr && curr->bid != Bid) {
                    prev = curr;
                    curr = curr->next;
                }
                if (curr) {
                    prev->next = curr->next;
                    free(curr);
                }
                return book;    
            } else {
                if (score >= 0 && score <= 10) {
                    /* ========================== Remove the loan ====================================== */
                    loan_t *prev = member->loans;
                    loan_t *curr = member->loans->next;
                    while (curr && curr->bid != Bid) {
                        prev = curr;
                        curr = curr->next;
                    }
                    if (curr) {
                        prev->next = curr->next;
                        free(curr);
                    }
                    /* ==================== Update the book's review information ======================= */
                    book->sum_scores = book->sum_scores + score;
                    book->n_reviews++;
                    if (book->n_reviews > 0) {
                        book->avg = book->sum_scores / book->n_reviews;
                    }
                    /* ==================== Update the book's review information ======================= */
                    if (book->n_reviews == 1) {                    
                        add_to_recheap(lib, book);                
                    } else {                                       
                        update_book_in_heap(lib, book);            
                    }                                              
                    /* ========================= Update the book's ranking ============================= */
                    update_book_ranking(genre, book);
                    create_member_activity(lib, Sid, 2);
                } else { 
                    genre->invalid_count++;
                    return NULL;
                }
            }
        }
        return book;
    }

    /* Allocates slots to genres based on their review scores */
    library_t *AllocatesSlots(library_t *lib) {
        /* ==================== If no slots are available, return the library as is ==================== */
        if (SLOTS == 0) return lib;
        /* ========================= Calculate the total points for all genres ========================= */
        int total_points = 0;
        genre_t *genre = lib->genres;
        while (genre) {
            genre->slots = 0; 
            int points = 0;
            book_t *book = genre->books;
            while (book) {
                if (!book->lost_flag && book->n_reviews > 0) {
                    points = points + book->sum_scores;
                }
                book = book->next;  
            }
            total_points = total_points + points;
            genre = genre->next;
        }
        if (total_points == 0) return lib;
        int quota = total_points / SLOTS;
        if (quota == 0) return lib;
        /* ======================== Calculate the number of slots for each genre ======================= */
        int assigned_seats = 0;
        genre_t **genres_array = NULL;
        int genre_count = 0;
        genre = lib->genres;
        while (genre) {
            genre_count++;
            genre = genre->next;
        }
        genres_array = malloc(genre_count * sizeof(genre_t*));
        int *remainders = (int*)malloc(genre_count * sizeof(int)); 
        genre = lib->genres;
        for (int i = 0; i < genre_count; i++) {
            genres_array[i] = genre;
            int points = 0;
            book_t *book = genre->books;
            while (book) {
                if (!book->lost_flag && book->n_reviews > 0) {
                    points += book->sum_scores;
                }
                book = book->next;
            }     
            genres_array[i]->slots = points / quota;
            remainders[i] = points % quota;
            assigned_seats += genres_array[i]->slots;
            genre = genre->next;
        }
        /* ================================ Allocate remaining seats =================================== */
        int remaining_seats = SLOTS - assigned_seats;
        while (remaining_seats > 0) {
            int max_remainder = -1;
            int max_index = -1; 
            /* ========================== Find the genre with the highest remainder ==================== */
            for (int i = 0; i < genre_count; i++) {
                if (remainders[i] > max_remainder || 
                    (remainders[i] == max_remainder && genres_array[i]->gid < genres_array[max_index]->gid)) {
                    max_remainder = remainders[i];
                    max_index = i;
                }
            }
            if (max_index == -1) break;
            genres_array[max_index]->slots++;
            remainders[max_index] = -1; 
            remaining_seats--;
        }
        /* ================================ Free allocated memory ====================================== */
        free(genres_array);
        free(remainders);
        return lib;
    }

    /* Print Genre */
    genre_t *PrintGenre(library_t *lib, int Gid){
        genre_t *genre = find_genre(lib, Gid);
        if (!genre) return NULL;
        book_t *book = genre->books;
        while (book) {
            printf("%d, %d\n", book->bid, book->avg);
            book = book->next;
        }
        return genre;
    }

    /* Print Member */
    void PrintMember(library_t *lib, int Sid) {
        member_t *member = find_member(lib, Sid);
        if (!member){
            printf("Loans:.\n");
            return;
        }else if(member && member->loans->next == NULL){
            printf("Loans:.\n");
            return;
        }
        loan_t *loan = member->loans->next;
            while (loan) {
                printf("%d\n", loan->bid);
                loan = loan->next;
            }
    }

    /* Print Display */
    void PrintDisplay(library_t *lib) {
        printf("Display:\n");
        genre_t *genre = lib->genres;
        int display_printed = 0;
        while (genre) {
            if (genre->slots > 0) {
                printf("%d:\n", genre->gid);
                display_printed = 1;
                book_t *book = genre->books;
                int count = 0;
                while (book && count < genre->slots) {
                    if (!book->lost_flag) {
                        printf("%d, %d\n", book->bid, book->avg);
                        count++;
                    }
                    book = book->next;
                }
            }
            genre = genre->next;
        }
        if (!display_printed) {
            printf("(empty)\n");
        }
    }

#endif
