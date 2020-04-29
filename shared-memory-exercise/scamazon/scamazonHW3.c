#include <fcntl.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<stddef.h>
#include<string.h>
#include <unistd.h>
#include <sys/wait.h>

int line_length = 200;

struct item{
        char name[64];
        int quantity;
        float price;
        int id_num;
	struct item *next;
	struct item *previous;
};

void delete_file(char file_name[]);
int accept_command();
void fill_items(struct item *items_ptr, int);
int read_inventory(struct item **items_ptr, int,  char[]);
char accept_item_id();
struct item *get_item_by_id(struct item *items_ptr, int);
void modify_item(struct item *items_ptr, int, int);
void display_specific_item(struct item *items_ptr, int);
void display_all_items(struct item *items_ptr);
void replace_linebreak(char *str, int);
void save_inventory(struct item *head_ptr, char file_name[]);
void delete_item(struct item **head_ptr_add, int id);
void add_item(struct item **head_ptr_addr);

int main(){
        int command;
        int inventory_size;
        struct item *items_ptr;
	char inventory_file[30] = "inventory.txt";
	inventory_size = read_inventory(&items_ptr, line_length, inventory_file);
        printf("Inventory size: %d\n", inventory_size);
	//printf("\ni=0:%s",**items_ptr->name);
	printf("------Samazon Inventory------\n");
        command = accept_command();
        if (command == 1){}
	else if (command == 2){
      		int id;
      		printf("\t<<Modify Mode>>\n\tPlease enter the ID for the item you want to modify:");
      		scanf("%d", &id);
      		modify_item(items_ptr, inventory_size, id);
      	}else if (command == 3){
          printf("\n<<Display by ID mode>>\n\tPlease enter the ID for the item you want to see:");
      		int id;
      		scanf("%d", &id);
          	printf("\n");
      		display_specific_item(items_ptr, id);
        }else if (command == 4){
          printf("\n<<ADD MODE>>");
          add_item(&items_ptr);
      	}else if (command == 5){
		printf("\n<<DELETE MODE>>");
		int id = 333;
		delete_item(&items_ptr, id);
	}else if (command == 6){
	  	printf("Deleting the old file...");
	  	//delete_file("inventory.text");
	  	save_inventory(items_ptr, "inventory.txt");
          	exit(1);
        }else{
          printf("No command was detected.\n");
        }
  save_inventory(items_ptr, "inventory.txt");
	display_all_items(items_ptr);

}

void delete_file(char file_name[]){
	int status = fork();
	if (status < 0){
		printf("Failed to fork() in delete_file()");
	}else if (status == 0){
		execl("/bin/rm", "rm", "inventory.txt", NULL);
	}else{
		wait(NULL);
		printf("Deleted!\n");
	}
}

void save_inventory(struct item *head_ptr, char file_name[]){
	struct item *curr = head_ptr;
	FILE *outfile;
	outfile = fopen(file_name, "w");
	char attr[10000];
  printf("\n-----------Saving the inventory-----------\n");
	while (curr != NULL){
		char id_str[20];
    sprintf(id_str, "%d", curr->id_num);
		strcpy(attr, id_str);
    printf("Writing id: %s\n", attr);
		fputs(attr, outfile);
		fputs(",", outfile);
		strcpy(attr, curr->name);
    printf("Writing name: %s\n", attr);
		fputs(attr, outfile);
		fputs(",", outfile);
		char price_str[20];
    sprintf(price_str, "%f", curr->price);
		strcpy(attr, price_str);
    printf("Writing price: %s\n", attr);
 		fputs(attr, outfile);
		fputs(",", outfile);
		char quantity_str[20];
    sprintf(quantity_str, "%d", curr->quantity);
		strcpy(attr,quantity_str);
    printf("Writing quantity: %s\n", attr);
 		fputs(attr, outfile);
    fputs("\n", outfile);
		curr = curr->next;
	};
  fclose(outfile);

}

int accept_command(){
  int command;
  printf("1: Display all items.\n2: Modify an item.\n3: Display a specific item.\n4: Add a product(Not implemented)\n5: Delete a product(Not implemented)\n6: Exit.\nPlease enter a command:");
  scanf("%d", &command);
  printf("\n");
  printf("Command detected: %d\n", command);
  return command;
}

void fill_items(struct item *items_ptr, int len){
        strcpy(items_ptr[0].name, "Mac book pro");
        items_ptr[0].price = 990.00;
        items_ptr[0].quantity = 99;
        items_ptr[0].id_num = 10001;

        strcpy(items_ptr[1].name, "Apple Watch");
        items_ptr[1].price = 150.00;
        items_ptr[1].quantity = 19;
        items_ptr[1].id_num = 10002;

        strcpy(items_ptr[2].name, "iPhone XX");
        items_ptr[2].price = 570.00;
        items_ptr[2].quantity = 50;
        items_ptr[2].id_num = 10003;
}


int read_inventory(struct item **head_ptr, int line_length, char file_name[]){
	char line[line_length];
	FILE *infile;
   	char token[100];
	int item_num = 0;
	struct item *curr_ptr;
	struct item *previous_ptr;
	infile = fopen(file_name, "r");
	if (infile == NULL){
		printf("Error reading the inventory file.\n");
		exit(1);
	}else{
		int i=0;
		while (fgets(line, line_length, infile) != NULL){
			curr_ptr = malloc(sizeof(struct item));
			if (i==0){
				*head_ptr = curr_ptr;
			}
			item_num++;
			if (curr_ptr == NULL){
			  printf("Memory allocation failed in read_inventory()\n");
				exit(1);
			}
      // else{
			// 	printf("SUCCESS REALLOCATION\n");
			// }
			int id = atoi(strtok(line, ","));
			curr_ptr->id_num = id;
			char *name;
			name = strtok(NULL, ",");
			strcpy(curr_ptr->name, name);
			float price = atof(strtok(NULL, ","));
			curr_ptr->price = price;
			int quantity = atoi(strtok(NULL, ","));
			curr_ptr->quantity = quantity;
			if ((struct item*)previous_ptr != NULL){
				previous_ptr->next = curr_ptr;
        curr_ptr->previous = previous_ptr;
			}
			previous_ptr = curr_ptr;
			i++;
			}
		}
		//printf("HEAD_PTR name:%s", head_ptr->name);
		return item_num;
	}

struct item *get_item_by_id(struct item *head_ptr, int id){
	struct item *target = NULL;
	struct item *current = head_ptr;
	int found = 0;
	while (current != NULL){
		if (current->id_num == id){
			target = current;
			return target;
		}
		current = current->next;
	}
	printf("\nCouldn't find the item with the maching id(%d)\n", id);
	exit(1);
}

void modify_item(struct item *items_ptr, int len, int id){
	int i;
	struct item *item;
	char *new_name;
	new_name = malloc(sizeof(char)*64);
	float new_price;
	int new_quantity;
	item = get_item_by_id(items_ptr, id);
  	printf("\tYou are modiying '%s' with id '%d'.\n", item->name, item->id_num);
  	printf("\tPlease enter a new value if you want to edit the field. Leave it blank if not.\n");
  	printf("\tName: %s => ", item->name);
  	scanf("\n");// To Make scanf cosume the last new line.
  	fgets(new_name, 64, stdin);
  	replace_linebreak(new_name, 64);
  	printf("\tPrice: %g =>", item->price);
  	scanf("%g", &new_price);
  	printf("\tName: %i =>", item->quantity);
  	scanf("%d", &new_quantity);
 	//Modify an item
	strcpy(item->name, new_name);
  	item->price = new_price;
  	item->quantity = new_quantity;
}

void add_item(struct item **head_ptr_addr){
	struct item *new_item_ptr = malloc(sizeof(struct item));
	char *new_name;
	new_name = malloc(sizeof(char)*64);
	float new_price;
	int new_quantity;
  int new_id;
  printf("\tPlease enter a new value for each field to create a new item.\n");
  printf("\tName:   => ");
  scanf("\n");// To Make scanf cosume the last new line.
  fgets(new_name, 64, stdin);
  replace_linebreak(new_name, 64);
  printf("\tPrice   =>");
  scanf("%g", &new_price);
  printf("\tQuantity=>");
  scanf("%d", &new_quantity);
  printf("\tID:     => ");
  scanf("%d", &new_id);
    strcpy(new_item_ptr->name, new_name);
  new_item_ptr->price = new_price;
  new_item_ptr->quantity = new_quantity;
  new_item_ptr->id_num = new_id;
  //Add the new item to the front
  struct item *head_ptr = *head_ptr_addr;
  struct item head_item = *head_ptr;
  new_item_ptr->next = head_ptr;//1
  head_item.previous = new_item_ptr;//2
  *head_ptr_addr = new_item_ptr;
  display_specific_item(*head_ptr_addr, new_id);
}

void delete_item(struct item **head_ptr_addr, int id){
	struct item *curr_ptr = NULL;
	struct item *prev_ptr = NULL;
	struct item *target = get_item_by_id(*head_ptr_addr, id);
	printf("DELETING: %s (ID: %d)\n", target->name, target->id_num);
	struct item *prev_item_ptr = target->previous;
	struct item *next_item_ptr = target->next;
	//printf("next:%s\n",next_item_ptr->name);
	//printf("prev:%s\n", prev_item_ptr->name);
	prev_item_ptr->next = next_item_ptr;
	next_item_ptr->previous = prev_item_ptr;
}

void display_specific_item(struct item *items_ptr, int id){
	printf("\t-----------------Item ID:%d----------------\n", id);
  struct item *target_ptr;
	target_ptr = get_item_by_id(items_ptr, id);
	printf("\t%d,%s,%g,%d\n", target_ptr->id_num, target_ptr->name, target_ptr->price, target_ptr->quantity);
	printf("\t----------------------------------------------\n\n\n");
}

void display_all_items(struct item *head_ptr){
        int i;
	printf("\t-----------------Item List--------------------\n");
      	struct item *curr = head_ptr;
	if (curr != NULL){
		do{
        		printf("\t%d,%s,%g,%d\n", curr->id_num, curr->name, curr->price, curr->quantity);
			curr = curr->next;
		}while (curr != NULL);
	}
        printf("\t----------------------------------------------\n");

}

void replace_linebreak(char *string, int len){
  int i;
  for (i=0; i<len; i++){
  	if (string[i] == '\n'){
		//printf("Found linebreak");
		string[i] = '\0';
		}
	}
}
