    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    
    #include "Library.h"
    #include "Memory.h"
    #include "ListHelpers.h"
    #include "AdvancedStructures.h"
    #include "LibraryOperations.h"
    #include "AdvancedOperations.h"

    int SLOTS;
    int main(int argc, char *argv[]) {

        if (argc < 2) {
            fprintf(stderr, "Usage: %s <command-file>\n", argv[0]);
            return 1;
        }

        /* ===================================== Create the library ==================================== */
        library_t *Library = CreateLibrary();
        /* ======================================= Open the file ======================================= */
        FILE *fp = fopen(argv[1], "r");
        if (fp == NULL) {
            perror("Error when opening the FILE");
            return 1;
        }
        /* ==================================== Initialize variables =================================== */
        int gid, bid, sid, score, k;
        char line[256], cmd[16], name[NAME_MAX], title[TITLE_MAX];
        while (fgets(line, sizeof line, fp) != NULL) {  
            
            /* ================ Ignore empty lines '\n' and comments starting with '#' ================= */ 
            if (line[0] == '\n' || line[0] == '#') continue;
            /* ===================================== Remove newline ==================================== */ 
            line[strcspn(line, "\n")] = '\0';
            /* ================================= Fetch the first command =============================== */ 
            if (sscanf(line, "%15s", cmd) != 1) continue;
            /* ======================================= COMMAND S ======================================= */
            if (strcmp(cmd, "S") == 0) {
                if (sscanf(line, "S %d", &SLOTS) == 1) printf("DONE\n");
                else printf("IGNORED\n");      
            /* ======================================= COMMAND G ======================================= */
            }else if (strcmp(cmd, "G") == 0) {
                if (sscanf(line, "G %d \"%63[^\"]\"", &gid, name) == 2) {
                    if ( (CreateGenre(Library,gid,name)) != NULL ) printf("DONE\n");
                    else printf("IGNORED\n");
                }    
            /* ======================================= COMMAND BK ====================================== */
            }else if (strcmp(cmd, "BK") == 0) {
                if (sscanf(line, "BK %d %d \"%127[^\"]\"", &bid, &gid, title) == 3) {
                    if ( (AddBook(Library,bid,gid,title)) != NULL ) printf("DONE\n");
                    else printf("IGNORED\n");
                }
            /* ======================================= COMMAND M ======================================= */
            }else if (strcmp(cmd, "M") == 0) {
                if (sscanf(line, "M %d \"%63[^\"]\"", &sid, name) == 2) {
                    if ( (AddMember(Library,sid,name)) != NULL ) {
                        create_member_activity(Library, sid, 0);         
                        printf("DONE\n");
                    }else printf("IGNORED\n");
                }
            /* ======================================= COMMAND L ======================================= */
            }else if (strcmp(cmd, "L") == 0) {
                if (sscanf(line, "L %d %d", &sid, &bid) == 2) {
                    if ( (LoanBook(Library,sid,bid)) != NULL ) printf("DONE\n"); 
                    else printf("IGNORED\n");
                }
            /* ======================================= COMMAND R ======================================= */
            }else if (strcmp(cmd, "R") == 0) {
                char status[10];
                char Score[10];
                if (sscanf(line, "R %d %d %d %s", &sid, &bid, &score, status) == 4) {
                    if((ReturnBook(Library,sid,bid,Score,status,score)) != NULL) printf("DONE\n");
                    else printf("IGNORED\n");      
                }else if(sscanf(line, "R %d %d %s %s", &sid, &bid, Score, status) == 4){
                    if(strcmp(Score,"NA") == 0){
                        if((ReturnBook(Library,sid,bid,Score,status,score)) != NULL) printf("DONE\n");
                        else printf("IGNORED\n");
                    }
                }else printf("IGNORED\n");
            /* ======================================= COMMAND PG ====================================== */
            }else if (strcmp(cmd, "PG") == 0) {
                int gid;
                if (sscanf(line, "PG %d", &gid) == 1) {
                    if((PrintGenre(Library,gid)) != NULL) printf("DONE\n");
                    else printf("IGNORED\n");
                }            
            /* ======================================= COMMAND PM ====================================== */
            }else if (strcmp(cmd, "PM") == 0) {
                int sid;
                if (sscanf(line, "PM %d", &sid) == 1) {
                    PrintMember(Library,sid);
                }
            /* ======================================= COMMAND PD ====================================== */
            }else if (strcmp(cmd, "PD") == 0) {
                PrintDisplay(Library);
            /* ======================================= COMMAND D ======================================= */
            }else if (strcmp(cmd, "D") == 0) {
                if(AllocatesSlots(Library) != NULL) printf("DONE\n");
            /* ======================================= COMMAND F ======================================= */
            }else if (strcmp(cmd, "F") == 0) {
                if (sscanf(line, "F \"%127[^\"]\"",title) == 1) {
                    print_Book_fromAVL(Library->genres, title);
                }
            /* ======================================= COMMAND TOP ===================================== */
            }else if (strcmp(cmd, "TOP") == 0) {
                if (sscanf(line, "TOP %d", &k) == 1) {
                    print_Top_Books(Library, k);
                }
            /* ======================================= COMMAND AM ====================================== */
            }else if (strcmp(cmd, "AM") == 0) {
                print_Most_Active_Member(Library);
            /* ======================================= COMMAND U ======================================= */
            }else if (strcmp(cmd, "U") == 0) {
                if (sscanf(line, "U %d \"%127[^\"]\"", &bid, title) == 2) {
                    if (Change_Book_Title(Library->genres, bid, title) != NULL) printf("DONE\n");
                    else printf("IGNORED\n");
                }
            /* ======================================= COMMAND BF ====================================== */
            }else if (strcmp(cmd, "BF") == 0) {     
                free_genres(Library->genres);
                free_members(Library->members);
                free_member_activities(Library->activity);
                free_recheap(Library->recommendations);
                printf("DONE\n");     
            /* ======================================= ERROR =========================================== */
            }else {
                printf("Unknown command: %s\n", cmd);
            }
        }
        /* ======================================= Close the file ====================================== */
        fclose(fp);
        /* =========================== Free the memory allocated for the library ======================= */
        free_library(Library);
        return 0;
    }