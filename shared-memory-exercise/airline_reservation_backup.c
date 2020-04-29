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
struct seat seats[18];


#define SHARED_MEMORY_NAME "urnutz2flyus"
#define FALSE 0
#define TRUE 1
void sigint_handler(int);
void init_sig_handlers();
void init_seats(struct seat*, int num_of_rows, int num_of_letters, char seat_letters[]);


int main()
{
  init_sig_handlers();
  struct sharedStuff *myStuff;
  printf("Starting shared memory demo...\n");
  printf("\nType q and hit the enter key to quit.\n");
  int first_person = FALSE;

  int flags, fd;
  mode_t perms;
  size_t size;

  flags = O_RDWR | O_CREAT | O_EXCL;

  size = sizeof(myStuff);
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
    myStuff = (struct sharedStuff*) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (myStuff == MAP_FAILED) {
      printf("mmap failed\n");
    }
    strcpy(myStuff->y, "ZZZZZZZZZZ");
    printf("NOT FIRST PERSON\n");
  }
  else
  {
    //Create the shared memory for the first time
    init_seats(&seats, num_of_rows, num_of_letters, seat_letters);
    first_person = TRUE;
    printf("shm_open failed\n");
    printf("FIRST PERSON\n");
    
    if (ftruncate(fd, size) == -1)
    printf("ftruncate failed\n");

    myStuff = (struct sharedStuff*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (myStuff == MAP_FAILED)
    printf("mmap failed\n");
    strcpy(myStuff->y, "AAAAAAAAA");
  }


  myStuff->x = 21;
  /////////////////////////////////////////////////////////////////////
  printf("Y: %s\n", myStuff->y);

  int c = getchar();
  printf("Before Change: %s\n", myStuff->y);
  if (first_person == TRUE)
  {
    strcpy(myStuff->y, "BBBBBBBBB");
  }
  else
  {
    strcpy(myStuff->y, "YYYYYYYYYY");
  }
  printf("After Change: %s\n", myStuff->y);

  c = getchar();

  printf("Before Change: %s\n", myStuff->y);
  if (first_person == TRUE)
  {
    strcpy(myStuff->y, "CCCCCCCCCCCC");
  }
  else
  {
    strcpy(myStuff->y, "XXXXXXXXXXXXX");
  }
  printf("After Change: %s\n", myStuff->y);

  c = getchar();

  printf("Before Change: %s\n", myStuff->y);
  if (first_person == TRUE)
  {
    strcpy(myStuff->y, "DDDDDDDDD");
  }
  else
  {
    strcpy(myStuff->y, "WWWWWWWWWWWW");
  }
  printf("After Change: %s\n", myStuff->y);

  c = getchar();
  //////////////////////////////////////////////////////////////////////

  if (shm_unlink(SHARED_MEMORY_NAME) == -1){
    printf("shm_unlink failed\n");
  }

  printf("... demo finished.\n");
  return 0;
}

void sigint_handler(int sig){
  if (shm_unlink(SHARED_MEMORY_NAME) == -1){
    printf("shm_unlink failed\nShared memory space is still reserved.\n");
  }
  printf("SIGINT RECEIVED\nExisiting the program safely...\n");
  fflush(stdout);
  exit(0);
}

void init_sig_handlers(){
    signal(SIGINT, sigint_handler);
}

void init_seats(struct seat *seats, int num_of_rows, int num_of_letters, char seat_letters[]){
  int i;
  int row_num = 0;
  int letter_index = 0;
  for (i=0; i<num_of_letters*num_of_rows; i++){
    if (i % num_of_letters == 0){
      letter_index = 0;
      row_num++;
    }else{
      letter_index++;
    };
    // seats[i] = malloc(sizeof(struct seat));
    seats[i].row = row_num;
    seats[i].letter = seat_letters[letter_index];
    strcpy(seats[i].reserver, "NONE");
    printf("Seat data initialized: %d%c - %s\n", seats[i].row, seats[i].letter, seats[i].reserver);
  }
}
