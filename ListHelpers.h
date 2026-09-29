#ifndef LIST_HELPERS_H
#define LIST_HELPERS_H

    /* Find a genre by its ID */
    genre_t *find_genre(library_t *lib, int gid) {
        genre_t *current = lib->genres;
        while (current) {
            if (current->gid == gid) return current;
            current = current->next;
        }
        return NULL;
    }

    /* Find a book by its ID within a genre */
    book_t *find_book(genre_t *genre, int bid) {
        if (!genre) return NULL;    
        book_t *current = genre->books;
        while (current) {
            if (current->bid == bid) return current;
            current = current->next;
        }
        return NULL;
    }

    /* Find a member by their ID */
    member_t *find_member(library_t *lib, int sid) {
        member_t *current = lib->members;
        while (current) {
            if (current->sid == sid) return current;
            current = current->next;
        }
        return NULL;
    }

    /* Find a loan by its ID */
    loan_t *find_loan(member_t *member, int bid) {
        if (!member || !member->loans) return NULL;
        loan_t *current = member->loans->next; 
        while (current) {
            if (current->bid == bid) return current;
            current = current->next;
        }
        return NULL;
    }

    /* Insert a book into the list in the correct position */
    void insert_book_sorted(genre_t *genre, book_t *new_book) {
        if (!genre || !new_book) return;    
        book_t *current = genre->books;
        book_t *prev = NULL;
        while (current) {
            if (current->avg < new_book->avg || 
                (current->avg == new_book->avg && current->bid > new_book->bid)) {
                break;
            }
            prev = current;
            current = current->next;
        }
        if (!prev) {
            new_book->next = genre->books;
            new_book->prev = NULL;
            if (genre->books) genre->books->prev = new_book;
            genre->books = new_book;
        } else {
            new_book->next = prev->next;
            new_book->prev = prev;
            prev->next = new_book;
            if (new_book->next) new_book->next->prev = new_book;
        }
    }

    /* Remove a book from the list */
    void remove_book_from_list(genre_t *genre, book_t *book) {
        if (!genre || !book) return;
        if (book->prev) {
            book->prev->next = book->next;
        } else {
            genre->books = book->next;
        }
        if (book->next) {
            book->next->prev = book->prev;
        }
    }

    /* Update the ranking of a book in the list */
    void update_book_ranking(genre_t *genre, book_t *book) {
        if (!genre || !book) return;
        remove_book_from_list(genre, book);
        insert_book_sorted(genre, book);
    }

    /* Create a new library */
    library_t *CreateLibrary(){
        library_t *lib = malloc(sizeof(library_t));
        if(!lib) return NULL;    
        lib->genres = NULL;
        lib->members = NULL;
        lib->activity = NULL;
        lib->recommendations = NULL;
        return lib;
    }

    /* Create a new genre */
    genre_t *CreateGenre(library_t *lib,int Gid,char Name[NAME_MAX]) {
        /* ============================ See if the genre already exists ================================ */
        if (find_genre(lib, Gid)) return NULL;
        /* =================================== Create the genre ======================================== */
        genre_t *new_genre = malloc(sizeof(genre_t));
        if (!new_genre) return NULL;
        new_genre->gid = Gid;
        strncpy(new_genre->name, Name, NAME_MAX - 1);
        new_genre->name[NAME_MAX - 1] = '\0';
        new_genre->books = NULL;
        new_genre->lost_count = 0;
        new_genre->invalid_count = 0;
        new_genre->slots = 0;
        new_genre->display = NULL;
        new_genre->book_index = NULL;          
        /* ======================== Insert the genre in the correct position =========================== */
        genre_t *current_g = lib->genres;
        genre_t *prev = NULL;    
        while (current_g && current_g->gid < Gid) {
            prev = current_g;
            current_g = current_g->next;
        }
        if (!prev) {
            new_genre->next = lib->genres;
            lib->genres = new_genre;
        } else {
            new_genre->next = prev->next;
            prev->next = new_genre;
        }
        return new_genre;
    }

#endif
