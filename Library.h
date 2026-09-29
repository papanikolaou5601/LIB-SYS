#ifndef LIBRARY_H
#define LIBRARY_H

#include <stddef.h>

#define TITLE_MAX 128
#define NAME_MAX  64
#define RECHEAP_CAPACITY 64      

/* Global display slots (set via command S) */
extern int SLOTS;

/* Active loan record */
typedef struct loan {
   int sid;            /* Member ID */
   int bid;            /* Book ID */
   struct loan *next;  /* Next member loan */
} loan_t;

/* Book record (sorted by avg desc in genre) */
typedef struct book {
   int  bid;                         /* Unique Book ID */
   int  gid;                         /* Genre ID */
   char title[TITLE_MAX];            /* Book title */

   int heap_pos;                     /* Heap index (-1 if absent) */

   /* Review stats */
   int sum_scores;                   /* Total valid rating points */
   int n_reviews;                    /* Total valid reviews */
   int avg;                          /* Cached average rating */
   int lost_flag;                    /* Lost status (1=lost, 0=active) */

   /* Genre doubly-linked list */
   struct book *prev;
   struct book *next;
} book_t;

/* Member activity metrics */
typedef struct MemberActivity { 
   int sid;                          /* Member ID */
   int loans_count;                  /* Total loans */
   int reviews_count;                /* Total reviews */
   int score_sum;                    /* Total review score sum */
   struct MemberActivity *next;      /* Next member activity */
} member_activity_t;

/* Library member */
typedef struct member {
   int  sid;                         /* Unique Member ID */
   char name[NAME_MAX];              /* Member name */

   loan_t* loans;                    /* Active loans sentinel list */
   member_activity_t *activity;      /* Member activity tracking */   
   struct member *next;              /* Sorted member list */
} member_t;

/* Title lookup AVL node */
typedef struct BookIndex { 
   char title[TITLE_MAX];            /* Book title */ 
   book_t *book;                     /* Pointer to book record */
   struct BookIndex *lc;             /* Left child */
   struct BookIndex *rc;             /* Right child */
   int height;                       /* Height of the node */
} book_index_t;                      

/* Book genre category */
typedef struct genre {
   int  gid;                         /* Unique Genre ID */
   char name[NAME_MAX];              /* Genre name */

   book_t* books;                    /* Head of books (sorted by avg desc) */
   book_index_t *book_index;         /* Title lookup AVL root */ 
   int lost_count;                   /* Count of lost books in genre */
   int invalid_count;                /* Count of invalid books in genre */

   /* Cached display allocation */
   int slots;                        /* Assigned display slots */
   book_t **display;                 /* Pointers to displayed books */

   struct genre *next;               /* Sorted genre list */
} genre_t;

/* Top recommendations max-heap */
typedef struct RecHeap {
   book_t *heap[RECHEAP_CAPACITY];   /* Heap array of book pointers */
   int size;                         /* Current number of books in heap */
} recheap_t;

/* Main library root structure */
typedef struct library {
   genre_t  *genres;                 /* Genre list (sorted by gid) */
   member_t *members;                /* Member list (sorted by sid) */
   member_activity_t *activity;      /* Global activity list */   
   recheap_t *recommendations;       /* Recommendation heap */   
} library_t;

#endif