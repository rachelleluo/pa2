#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define RESTOCK "RESTOCK"
#define SHOW_STOCK "SHOW_STOCK"
#define SALE "SALE"

// Author: @nate-blum

typedef struct HashNode {
    int count;
    char* album_name;
    struct HashNode* next;
} HashNode;

static HashNode** table = NULL;
static int table_fullness = 0, table_size = 4;

int hash(char* str) {
    unsigned long long hash = 0;
	int len = (int) strlen(str);
    for (int i = 0; i < len; i++)
        hash += str[i] * pow(31, len - (i + 1));
    return hash % table_size;
}

void free_table() {
	//iterates through the hash table
	for (int i = 0; i < table_size; i++) {
        //sees if there is anything at the index
        HashNode* current = table[i];
        //if there is, loop through the index and free the next and album name pointers
        while (current) {
            HashNode* temp = current;
            current = current->next;
            free(temp->album_name);
            free(temp);
        }
    }
    //free the table
    free(table);
    return;
}

void pretty_print_table() {
    printf("-------- FINAL COUNTS --------\n");
    for(int i = 0; i < table_size; i++){
        if(table[i] != NULL){
            HashNode* current = table[i];
            while(current){
                printf("%s: %d\n", current->album_name, current->count);
                current = current->next;
            }
        }
    }
    printf("------------------------------\n");
}


void resize() {
	//doubles the hash table size
    printf("Resizing the table from %d to %d\n", table_size, (table_size*2));
    int old_size = table_size;
    table_size = table_size * 2;
    //creates a new hash table and sets all elements in it to NULL
    HashNode** new_table = malloc(table_size * sizeof(HashNode*));
    for (int i = 0; i < table_size; i++) {
        new_table[i] = NULL;
    }

    //loops through the current table and moves all of its contents into the new table
    for(int j = 0; j < old_size; j++){
        //gets the current
        HashNode* current = table[j];
        //while current is not null
        while(current){
            //creates a new node with the information from the old table
            HashNode* newNode = malloc(sizeof(HashNode));
            newNode->count = current->count;
            newNode->album_name = current->album_name;
            newNode->next = NULL;

            //hash a new index with the new table_size
            int index = hash(current->album_name);
            //if there is nothing at that index yet, place the newNode at the index
            if(new_table[index] == NULL){
                new_table[index] = newNode;
            //otherwise, iterate through the index until you find the last HashNode place it at the end
            }else{
                HashNode* temp = new_table[index];
                while(temp->next){
                    temp = temp->next;
                }
                temp->next = newNode;
            }
            HashNode* next = current->next;
            free(current);
            current = next;
        }
    }

    //free old table
    free(table);
    table = new_table;

}


void update(char* album, int k){
    //get the index of the album name
    int index = hash(album);
    HashNode* current = table[index];
    //loop through the index
    while(current){
        //if any of the album names in the index matches the album we're looking for
        if(strcmp(current->album_name, album) == 0){
            //check to see if k < 0 (sale not restock)
            if(k < 0){
                //if there is not enough inventory to go through with the sale, print out
                //that there isn't enough stock
                if((current->count + k) < 0){
                    printf("Not enough stock of %s\n", album);
                    return;
                }
            }
            //otherwise, add/subtract k from the count
            current->count = current->count + k;
            return;
        }
        current = current->next;
    }
    //if k < 0 (sale not restock) and the album was not found in the has table
    if(k < 0){
        //print out that there is no stock
        printf("No stock of %s\n", album);
        return;
    }
    //otherwise create a new node with the information and add it at the index
    HashNode* newNode = malloc(sizeof(HashNode));
    newNode->count = k;
    newNode->album_name = strdup(album);
    newNode->next = table[index];
    table[index] = newNode;
    //since it's a new node, add 1 to table_fullness
    table_fullness += 1;
    //check to see if the table_fullness is == table_size/2, if it is resize
    if(table_fullness == (table_size/2)){
        resize();
    }
    return;
}

int retrieve(char* album_name) {
	//hashes the index
	int index = hash(album_name);
    //loops through the elements at that index
	HashNode* current = table[index];
	while(current){
        //if the album name matches, return the count for the album
		if(strcmp(current->album_name, album_name) == 0){
			return current->count;
		}
		current = current->next;
	}
    //return -1 if the album was not found
	return -1;
}

int main(int argc, char* argv[]) {
	FILE* fp = fopen(argv[1], "r");
    if (!fp) {
        perror("fopen failed");
        return EXIT_FAILURE;
    }

	if (!fscanf(fp, "%d\n", &table_size)) {
        perror("Reading the initial size of the table failed.\n");
        exit(EXIT_FAILURE);
    }

    //create a new_table to allocate the right amount of space
	HashNode** new_table = malloc(table_size * sizeof(HashNode*));
	for (int i = 0; i < table_size; i++) {
        new_table[i] = NULL;
    }
	free(table);
	table = new_table;

    char command[20], album[150];
	int count;
    while (fscanf(fp, "%s %d %[^\n]s", command, &count, album) == 3) {
		if (strcmp(command, SALE) == 0) {
            //for sale, multiply count by -1 so that it subtracts
            update(album, (count * -1));
		} else if (strcmp(command, RESTOCK) == 0) {
            update(album, count);
		} else if (strcmp(command, SHOW_STOCK) == 0) {
            int stock = retrieve(album);
			if(stock == -1){
				printf("No stock of %s\n", album);
			}else{
				printf("Current stock of %s: %d\n", album, stock);
			}
		}
    }

    pretty_print_table();
    free_table();
}