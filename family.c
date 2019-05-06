#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "family.h"

/* Number of word pointers allocated for a new family.
   This is also the number of word pointers added to a family
   using realloc, when the family is full.
*/
static int family_increment = 0;


/* Set family_increment to size, and initialize random number generator.
   The random number generator is used to select a random word from a family.
   This function should be called exactly once, on startup.
*/
void init_family(int size) {
    family_increment = size;
    srand(time(0));
}


/* Given a pointer to the head of a linked list of Family nodes,
   print each family's signature and words.

   Do not modify this function. It will be used for marking.
*/
void print_families(Family* fam_list) {
    int i;
    Family *fam = fam_list;
    
    while (fam) {
        printf("***Family signature: %s Num words: %d\n",
               fam->signature, fam->num_words);
        for(i = 0; i < fam->num_words; i++) {
            printf("     %s\n", fam->word_ptrs[i]);
        }
        printf("\n");
        fam = fam->next;
    }
}


/* Return a pointer to a new family whose signature is 
   a copy of str. Initialize word_ptrs to point to 
   family_increment+1 pointers, numwords to 0, 
   maxwords to family_increment, and next to NULL.
*/
Family *new_family(char *str) {
    Family *fam; 
     
    if ((fam = malloc(sizeof(Family))) == NULL) {
        perror("malloc");
        exit(1);
    }
    if ((fam->next = malloc(sizeof(Family))) == NULL ){
        perror("malloc");
        exit(1);
    }

    if ((fam->word_ptrs = malloc(family_increment*sizeof(char *))) == NULL ){
        perror("malloc");
        exit(1);
    }
    
    if ((fam->signature = malloc(strlen(str)+1 * sizeof(char))) == NULL) {
        perror("malloc");
        exit(1);
    }

    strncpy(fam->signature, str, strlen(str));
    fam->signature[strlen(str)] = '\0';
    fam->num_words = 0;
    fam->max_words = family_increment;
    fam->word_ptrs[0] = NULL;
    fam->next =  NULL;

    return fam;
}


/* Add word to the next free slot fam->word_ptrs.
   If fam->word_ptrs is full, first use realloc to allocate family_increment
   more pointers and then add the new pointer.
*/
void add_word_to_family(Family *fam, char *word) {
    if (fam->num_words == fam->max_words) {
        fam->max_words += family_increment;
        if ((fam->word_ptrs = realloc(fam->word_ptrs, (fam->max_words)*sizeof(char *))) == NULL) {
            perror("malloc");
            exit(1);
        }
    } 
    
    if(fam->num_words < fam->max_words) {
        fam->num_words+=1;
        int curr_index = fam->num_words-1;
        if ( (fam->word_ptrs[curr_index] = malloc(strlen(word)+1 * sizeof(char))) == NULL) {
            perror("malloc");
            exit(1);
        }
        strcpy(fam->word_ptrs[curr_index], word);
        fam->word_ptrs[curr_index][strlen(word)] = '\0';
        fam->word_ptrs[fam->num_words] = NULL;
    }
    return;
}


/* Return a pointer to the family whose signature is sig;
   if there is no such family, return NULL.
   fam_list is a pointer to the head of a list of Family nodes.
*/
Family *find_family(Family *fam_list, char *sig) {

    Family *fam = fam_list;
    while(fam!=NULL){
        if (strcmp(fam->signature, sig) == 0) {
            return fam;
        }
        fam=fam->next;
    }
    return NULL;
}


/* Return a pointer to the family in the list with the most words;
   if the list is empty, return NULL. If multiple families have the most words,
   return a pointer to any of them.
   fam_list is a pointer to the head of a list of Family nodes.
*/
Family *find_biggest_family(Family *fam_list) {
    Family *largest_fam = NULL;
    int largest_amt = 0;
    Family *fam = fam_list; 
    while (fam != NULL) {
        if (fam->num_words > largest_amt) {
            largest_amt = fam->num_words;
            largest_fam = fam;
        }
        fam=fam->next;
    }
    return largest_fam;
}


/* Deallocate all memory rooted in the List pointed to by fam_list. */
void deallocate_families(Family *fam_list) {
    Family *tmp = fam_list;

    while(fam_list !=NULL) {
        
        tmp = fam_list;

        int i = 0;
        while(tmp->word_ptrs[i] != NULL) {
            free(tmp->word_ptrs[i]);
            i++;
        } 
        free(tmp->word_ptrs);

        free(tmp->signature);

        fam_list = fam_list->next;
        free(tmp);
    }
    
    return;
    
}

// Groups the words whose letter on index i of the signature
void group(int i, char **word_list, Family *fam, char letter) {
    int j = 0;
    while(word_list[j] != NULL) {
         
        const char *e = strchr(word_list[j], letter);
        int index;
        // if i is zero then it belongs to the family without
        // matching words to the letter
        if (i==0 && e == NULL) { 
            
            add_word_to_family(fam, word_list[j]);
        }
        // we are on the next family whose index should match the letter
        else if ( e != NULL && (index=e-word_list[j]) == i-1) {
            add_word_to_family(fam, word_list[j]);
        }
        j++;
    }
    
    return; 
}

/* Generate and return a linked list of all families using words pointed to
   by word_list, using letter to partition the words.

   Implementation tips: To decide the family in which each word belongs, you
   will need to generate the signature of each word. Create only the families
   that have at least one word from the current word_list.
*/
Family *generate_families(char **word_list, char letter) {
    if (word_list[0] != NULL) {

        int sig_len = strlen(word_list[0]);  
        int families_amt = strlen(word_list[0]) + 1;

        char sig[sig_len+1];
        memset(sig, '-', sig_len);
        sig[sig_len] = '\0';

        Family *fam = NULL ;
        Family *prev = NULL;
        Family *head = NULL;

        int i = 0;

        while(i<families_amt) {
            if (i-1 < 0) {
                fam = new_family(sig);
                // helper function
                group(i, word_list, fam, letter);
            } else {
                sig[i-1] = letter;
                fam = new_family(sig);
                group(i, word_list, fam, letter);
                sig[i-1] = '-';
            }
            if (head == NULL) {
                head = fam;
            }
            if (prev != NULL) {
                prev->next = fam;
            }
            prev = fam;
            i++;
        }

        return head;
    }
    return NULL;
}


/* Return the signature of the family pointed to by fam. */
char *get_family_signature(Family *fam) {

    return fam->signature;
}


/* Return a pointer to word pointers, each of which
   points to a word in fam. These pointers should not be the same
   as those used by fam->word_ptrs (i.e. they should be independently malloc'd),
   because fam->word_ptrs can move during a realloc.
   As with fam->word_ptrs, the final pointer should be NULL.
*/
char **get_new_word_list(Family *fam) {
    char **words_list;
    
    if ((words_list =  malloc(sizeof(char *) * fam->max_words+1)) == NULL) {
        perror("malloc");
        exit(1);
    }
    
    int i = 0;
    char **word_ptrs = fam->word_ptrs; 
    
    // iterate through the word_ptrs to copy
    while(word_ptrs[i] != NULL) {
        if ((words_list[i] = malloc(sizeof(char) * strlen(word_ptrs[i])+1)) == NULL) {
            perror("malloc");
            exit(1);
        }
        strncpy(words_list[i], word_ptrs[i], strlen(word_ptrs[i]));
        words_list[i][strlen(word_ptrs[i])] = '\0';
        i++;

    }// fill the remaning empty word pointers with NULL
    while (i < fam->max_words) {
        words_list[i] = NULL;
        i++;
    } 
    return words_list;
}


/* Return a pointer to a random word from fam. 
   Use rand (man 3 rand) to generate random integers.
*/
char *get_random_word_from_family(Family *fam) {
    int num_words = fam->num_words;
    int chosen_one = rand() % num_words;
    return fam->word_ptrs[chosen_one];
}
