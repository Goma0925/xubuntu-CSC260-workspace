// shmdemo.c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>

struct sharedStuff
{
  int x;
  char y[32];
};
struct seat
{
  int row;
  char letter;
  char reserver[32];
};
// int num_of_seats = 18;
int num_of_letters = 3;
int num_of_rows = 3;
char seat_letters[] = {'A', 'B', 'C'};
struct seat *seats_ptr;


#define SHARED_MEMORY_NAME "urnutz2flyus!"
#define FALSE 0
#define TRUE 1
void safe_exit_handler(int);
void init_sig_handlers();
void init_seats(struct seat*, int num_of_rows, int num_of_letters, char seat_letters[]);
void print_seats(struct seat *seats_ptr, int num_of_rows, int num_of_letters);
int reserve_seat(struct seat *seats_ptr, int num_of_rows, int num_of_letters, int row_num, char seat_letter, char reserver_name[32]);


int main()
{
  init_sig_handlers();
  // struct sharedStuff *myStuff;
  printf("Starting shared memory demo...\n");
  printf("\nType q and hit the enter key to quit.\n");
  int first_person = FALSE;

  int flags, fd;
  mode_t perms;
  size_t size;

  flags = O_RDWR | O_CREAT | O_EXCL;

  size = sizeof(struct seat) * num_of_letters * num_of_rows;
  printf("seats_ptr size:%d\n", (int)size);
  perms =  S_IRUSR | S_IWUSR;

  fd = shm_open(SHARED_MEMORY_NAME, flags, perms);
  if (fd == -1) //If fd == -1, that means the memory has already been created. Now, just connect to it.
  {
    flags = O_RDWR;
    fd = shm_open(SHARED_MEMORY_NAME, flags, perms);
    if (fd == -1)
    {
      perror("shm_open failed\n");
      exit(1);
    }

    if (ftruncate(fd, size) == -1) {
      printf("ftruncate failed\n");
    }

    //Initialize the shared memory instance
    seats_ptr = (struct seat*) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (seats_ptr == MAP_FAILED) {
      printf("mmap failed\n");
    }
    // strcpy(seats->y, "ZZZZZZZZZZ"); HERE
    printf("NOT FIRST PERSON\n");
  }
  else
  {
    //Create the shared memory for the first time
    first_person = TRUE;
    printf("shm_open failed\n");
    printf("FIRST PERSON\n");

    if (ftruncate(fd, size) == -1){
      printf("ftruncate failed\n");
    }

    seats_ptr = (struct seat*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    init_seats(seats_ptr, num_of_rows, num_of_letters, seat_letters);
    if (seats_ptr == MAP_FAILED){
      printf("mmap failed\n");
    };
    // strcpy(myStuff->y, "AAAAAAAAA"); HERE
  }


  // myStuff->x = 21; HERE
  /////////////////////////////////////////////////////////////////////
  // printf("Y: %s\n", myStuff->y); HERE
  print_seats(seats_ptr, num_of_rows, num_of_letters);
  // int c = getchar();
  printf("Reserving...\n\n");
  int reservation_success_flag;
  char reserver_name[32];
  char seat_letter = 'A';
  int row_num = 2;
  strcpy(reserver_name, "Amon Otsuki");
  reservation_success_flag = reserve_seat(seats_ptr, num_of_rows, num_of_letters, row_num, seat_letter, reserver_name);
  printf("Reserved Amon Otsuki at A2: %d\n", reservation_success_flag);
  print_seats(seats_ptr, num_of_rows, num_of_letters);
  reservation_success_flag = reserve_seat(seats_ptr, num_of_rows, num_of_letters, row_num, seat_letter, reserver_name);
  printf("\nAfter trying to reserve the same seat: %d\n", reservation_success_flag);

  printf("Enter to exit\n");
  int c = getchar();
  //////////////////////////////////////////////////////////////////////
  print_seats(seats_ptr, num_of_rows, num_of_letters);

  if (shm_unlink(SHARED_MEMORY_NAME) == -1){
    printf("shm_unlink failed\n");
  }

  printf("... demo finished.\n");
  return 0;
}

void safe_exit_handler(int sig){
  if (shm_unlink(SHARED_MEMORY_NAME) == -1){
    printf("shm_unlink failed\nShared memory space is still reserved.\n");
  }
  printf("|SIGINT RECEIVED\nExisiting the program safely...\n");
  fflush(stdout);
  exit(0);
}

void init_sig_handlers(){
    signal(SIGINT, safe_exit_handler);//Handle the ctrl+c exit here.
    signal(SIGBUS, safe_exit_handler);
}

void init_seats(struct seat *seats_ptr, int num_of_rows, int num_of_letters, char seat_letters[]){
  int i;
  int row_num = 0;
  int letter_index = 0;
  printf("init_seats() - size of ptr:%d\n", (int)sizeof(*seats_ptr));
  for (i=0; i<num_of_letters*num_of_rows; i++){
    if (i % num_of_letters == 0){
      letter_index = 0;
      row_num++;
    }else{
      letter_index++;
    };
    // seats[i] = malloc(sizeof(struct seat));
    seats_ptr[i].row = row_num;
    seats_ptr[i].letter = seat_letters[letter_index];
    strcpy(seats_ptr[i].reserver, "AVAILABLE");
    printf("Seat data initialized: %d%c - %s\n", seats_ptr[i].row, seats_ptr[i].letter, seats_ptr[i].reserver);
  }
}

void print_seats(struct seat *seats_ptr, int num_of_rows, int num_of_letters){
  int i;
  printf("Seat | Reserver name\n");
  for (i=0; i<num_of_rows*num_of_letters; i++){
    printf("%d%c   -   %s\n", seats_ptr[i].row, seats_ptr[i].letter, seats_ptr[i].reserver);
  }
}

int reserve_seat(struct seat *seats_ptr, int num_of_rows, int num_of_letters, int row_num, char seat_letter, char reserver_name[32])
{
  //Args:
    //struct seat *seats_ptr: Pointer to a seat array.
    //int num_of_rows: Number of rows of seat that are available.
    //int num_of_letters: Number of letters of seat that are available.
    //int row_num: Specified row number of the seat to reserve a new seat
    //char seat_letter:Specified seat letter of the seat to reserve a new seat
    //char reserver_name[32]: A reserver's name
  //Return:
    //  0 if reservation succeeds.
    // -1 if the seat is already taken.
    // -2 if the seat does not exit not.
  int i;
  for (i=0; i<num_of_rows*num_of_letters; i++){
    if (seats_ptr[i].row == row_num){
      if(seats_ptr[i].letter == seat_letter){
        if (strcmp(seats_ptr[i].reserver, "AVAILABLE") == 0){
          strcpy(seats_ptr[i].reserver, reserver_name);
          return 0;
        }else{
          return -1;
        }
      }
    }
  };
  return -2;
}
