#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <matelib.h>

// void* io_thread(void* mate_ref) {
//     mate_sem_wait(mate_ref, "SEM1");
//     printf("And this one second...\n");


//     // Memory tooling usage
//     mate_pointer ptr = mate_memalloc(mate_ref, sizeof(char) * 8);
//     mate_memwrite(mate_ref, "PRINTER", ptr, sizeof(char) * 8);
//     char *io_type = malloc(sizeof(char) * 8);
//     mate_memread(mate_ref, ptr, io_type, sizeof(char) * 8);

//     // IO Usage
//     mate_call_io(mate_ref, io_type, "I'm content to print...");

//     // Freeing Memory
//     // Closing Lib
//     free(io_type);
//     mate_memfree(mate_ref, ptr);


//     return 0;
// }


int main(int argc, char *argv[])
{
  // Lib instantiation
  //TODO: Esto lo podria hacer el mate_init
  mate_instance mate_ref;
  mate_init(&mate_ref, "./config.config");
  //mate_sem_init(&mate_ref, "1", 80); 
  //mate_sem_init(&mate_ref, "2", 14); 
  //mate_sem_init(&mate_ref, "3", 8);   
  // // mate_sem_destroy(&mate_ref, "holachau"); 
  // mate_sem_wait(&mate_ref, "1"); 
  // sleep(30);
  // mate_sem_wait(&mate_ref, "2"); 

  // mate_sem_wait(&mate_ref, "2");  
  // sleep(30);
  // mate_sem_wait(&mate_ref, "3");
  // mate_sem_post(&mate_ref, "2");

  // mate_sem_wait(&mate_ref, "3");  
  // sleep(30);
  // mate_sem_wait(&mate_ref, "1");

  // mate_sem_wait(&mate_ref, "4");  
  // sleep(40);
  // mate_sem_wait(&mate_ref, "1");
  // mate_sem_post(&mate_ref, "1");
  // mate_sem_post(&mate_ref, "4");
  
  
  // mate_sem_post(&mate_ref, "holachau");

  //IO
  // char* prueba = strdup("hola");
  // mate_call_io(&mate_ref, "hierbitas", prueba);
  // free(prueba);
  /*
  printf("1 %d\n",mate_memalloc(&mate_ref, 81));
  
  printf("3 %d\n",mate_memalloc(&mate_ref, 8)); 

  mate_memwrite(&mate_ref, "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec et mollis libero.", 9, 81);
  char* buff = malloc(81);
  mate_memread(&mate_ref, 9, buff, 81);
  printf("2 --%s--\n", buff);
  free(buff);

  printf("2 %d\n",mate_memalloc(&mate_ref, 18));

  mate_memwrite(&mate_ref, "Ahi va la bala123", 116, 18);
  char* buff2 = malloc(18);

  printf("3 %d\n",mate_memalloc(&mate_ref, 8));  

  mate_memread(&mate_ref, 116, buff2, 18);
  printf("2 --%s--\n", buff2);
  free(buff2);

  printf("2 %d\n",mate_memalloc(&mate_ref, 2));

  mate_memwrite(&mate_ref, "R", 133, 2);
  char* buff3 = malloc(2);
  mate_memread(&mate_ref, 133, buff3, 2);
  printf("2 --%s--\n", buff3);
  free(buff3);
  */
  printf("4 %d\n",mate_memalloc(&mate_ref, 13));  
  printf("5 %d\n",mate_memalloc(&mate_ref, 1));
  printf("6 %d\n",mate_memalloc(&mate_ref, 24));
  printf("7 %d\n",mate_memalloc(&mate_ref, 8));
  printf("8 %d\n",mate_memalloc(&mate_ref, 4));
  printf("9 %d\n",mate_memalloc(&mate_ref, 81));




  printf(" aaaaaaaa %d\n",mate_memfree(&mate_ref,9));
  printf(" aaaaaaaa %d\n",mate_memfree(&mate_ref,91));/*
  printf(" aaaaaaaa %d\n",mate_memfree(&mate_ref,121));
  printf(" aaaaaaaa %d\n",mate_memfree(&mate_ref,9));
  printf("9 %d\n",mate_memalloc(&mate_ref, 200));

  // printf("5 %d\n",mate_memalloc(&mate_ref, 1));
  // printf("6 %d\n",mate_memalloc(&mate_ref, 24));
  // printf("7 %d\n",mate_memalloc(&mate_ref, 8));
  // printf("8 %d\n",mate_memalloc(&mate_ref, 4));
  // printf("9 %d\n",mate_memalloc(&mate_ref, 81));
  

  // printf(" aaaaaaaa %d\n",mate_memfree(&mate_ref, 24));
  /*
  printf("7 %d\n",mate_memfree(&mate_ref, 8));
  printf("8 %d\n",mate_memfree(&mate_ref, 4));
  printf("9 %d\n",mate_memfree(&mate_ref, 81));
  */
  
  /*
  mate_memfree(&mate_ref, 1);
  
  void * prueba;
  mate_memread(&mate_ref, 1, prueba, 1);
  mate_memwrite(&mate_ref, prueba, 1, 1);  */
  // // Let's work with semaphores
  // mate_sem_init(&mate_ref, "SEM1", 0);
  // pthread_t thread_id;
  // pthread_create(&thread_id, NULL, &io_thread, &mate_ref);
  
  // usleep(1000);
  // printf("This message should go first...\n");
  // mate_sem_post(&mate_ref, "SEM1");
  
  // pthread_join(thread_id, NULL);

  

  mate_close(&mate_ref);
  return 0;
}