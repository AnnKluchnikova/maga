#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define SUCCESS 0
#define FAIL -1

int pid = -1;

char* get_optimizatio_name(int level){
  switch(level){
    case 0: return "none";
    case 1: return "O1";
    case 2: return "O2";
    default: return "unknown";
  }
}

// [Генерация чисел] -----------------------------------------------------------

double random_double_value(void){
  double value = (double)rand()/((double) RAND_MAX + 1);
  return (10 + value * (1000 - 10));
}

// [Тестирование функций] ------------------------------------------------------


// [Обработка опций] -----------------------------------------------------------

struct testArg_t{
  char *file;       // для ключа -f
  int matrix_size;  // для ключа -s
  int optim_access; // для ключа -o
}testArg;

const char *optString = "s:o:f:";

void process_opt_args(int argc, char **argv)
{
  char opt = getopt(argc, argv, optString);

  while(opt != -1){
    switch(opt){
      case 's':
      {
        testArg.matrix_size = strdup(optarg);
        printf("[DEBUG] PID: %d : Action: get -s option: Value: %s\n", 
                pid, testArg.matrix_size);
        break;
      }
      case 'o':
      {
        testArg.optim_access = atoi(optarg);
        printf("[DEBUG] PID: %d : Action: get -o option: Value: %d\n",
                pid, testArg.optim_access);
        break;
      }
      case 'f':
      {
        testArg.file = strdup(optarg);
        printf("[DEBUG] PID: %d : Action: get -f option: Value: %s\n", 
                pid, testArg.file);
        break;
      }

    }
    opt = getopt( argc, argv, optString );
  }
}

// [Главная программа] ---------------------------------------------------------

int main(int argc, char *argv[]){
  FILE* fd;

  pid = getpid();
  printf("[DEBUG] PID: %d : Action: programm started\n", pid);

  if (argc > 1){
    printf("[DEBUG] PID: %d : Action: goto process options\n", pid);
    process_opt_args(argc, argv);
  }else{
    printf("[ERROR] PID: %d : Action: programm stopped: Reason: no options\n",
            pid);
    return FAIL;
  }

  fd = fopen(testArg.file, "a+");
  if(!fd){
      printf("[ERROR] PID: %d : Action: programm stopped: Reason: "
               "failed to open file %s\n", pid, testArg.file);
      return FAIL;
  }else{
    fclose(fd);
  }

  // При каждом запуске программы должны генерироваться разные числа
  srand(time(NULL));

  for(i = 0; i < testArg.test_count; i++){
    testing_cblas_sgemv(INT_T, order, transa, m, n, lda, incx, incy);
  }
  for(i = 0; i < testArg.test_count; i++){
    testing_cblas_sgemv(FLOAT_T, order, transa, m, n, lda, incx, incy);
  }
  for(i = 0; i < testArg.test_count; i++){
    testing_cblas_dgemv(DOUBLE_T, order, transa, m, n, lda, incx, incy);
  }

  if(testArg.file){
    free(testArg.file);
  }

  printf("[DEBUG] PID: %d : Action: programm finished\n", pid);
  return SUCCESS;
}
