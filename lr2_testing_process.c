#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <cblas.h>

#define SGEMV_INSTRUCTION 25
#define DGEMV_INSTRUCTION 25
#define SUCCESS 0
#define FAIL -1

int pid = -1;

typedef enum VALUE_TYPE{
  INT_T = 0,
  FLOAT_T,
  DOUBLE_T
}VALUE_TYPE;

char* get_value_type_name(enum VALUE_TYPE type){
  switch(type){
    case INT_T: return "int";
    case FLOAT_T: return "float";
    case DOUBLE_T: return "double";
    default: return "unknown";
  }
}

char* get_optim_key_name(int level){
  switch(level){
    case 0: return "none";
    case 1: return "O1";
    case 2: return "O2";
    default: return "unknown";
  }
}

// [Обработка опций] -----------------------------------------------------------

struct testArg_t{
  char *cpu_model; // для ключа -m
  char *file;      // для ключа -f
  int test_count;        // для ключа -c
  int optim_num;         // для ключа -o
}testArg;

const char *optString = "m:c:o:f:";

void process_opt_args(int argc, char **argv)
{
  char opt = getopt(argc, argv, optString);

  while(opt != -1){
    switch(opt){
      case 'm':
      {
        testArg.cpu_model = strdup(optarg);
        printf("[DEBUG] PID: %d : Action: get -m option: Value: %s\n", 
                pid, testArg.cpu_model);
        break;
      }
      case 'c':
      {
        testArg.test_count = atoi(optarg);
        printf("[DEBUG] PID: %d : Action: get -c option: Value: %d\n",
                pid, testArg.test_count);
        break;
      }
      case 'o':
      {
        testArg.optim_num = atoi(optarg);
        printf("[DEBUG] PID: %d : Action: get -o option: Value: %d\n",
                pid, testArg.optim_num);
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

// [Генерация чисел] -----------------------------------------------------------

int random_int_value(void){
  double num = (double)rand()/((double) RAND_MAX + 1);
  int value = (int)(num *(1000 - 10 + 1));
  return (10 + value);
}

float random_float_value(void){
  float value = ((float)rand()/RAND_MAX)*(float)(1000.0);
  return value;
}

double random_double_value(void){
  double value = (double)rand()/((double) RAND_MAX + 1);
  return (10 + value * (1000 - 10));
}

// [Тестирование функций] ------------------------------------------------------

int testing_cblas_sgemv(enum VALUE_TYPE type, enum CBLAS_ORDER order,
                        enum CBLAS_TRANSPOSE transa, int m, int n, int lda,
                        int incx, int incy){
  static int test_num_i = 0;
  static int test_num_f = 0;
  int test_num = 0;

  int i, j;
  float *a, *x, *y;
  float alpha = 1, beta = 0;
  clock_t start, stop;
  double cpu_time = 0;

  FILE* fd;

  if((type != INT_T) && (type != FLOAT_T)){
    printf("[ERROR] PID: %d : Func: %s: Reason: failed type %s\n",
          pid, __FUNCTION__, get_value_type_name(type));
    return FAIL;
  }

  // printf("[DEBUG] PID: %d : Action: generating matrix values: Type: %s\n",
  //         pid, get_value_type_name(type));

  a = (float *)malloc(sizeof(float)*m*n);
  x = (float *)malloc(sizeof(float)*n);
  y = (float *)malloc(sizeof(float)*n);

  for(i = 0; i < m; i++){
    for(j = 0; j < n; j++){

      //a[0..m*m+n]
      if(type == INT_T){
        a[i*i+j] = random_int_value();
      } else{
        a[i*i+j] = random_float_value();
      }

      // x[0..n]
      if(i == 0){
        if(type == INT_T){
          x[j] = random_int_value();
        } else{
          x[j] = random_float_value();
        }
      }
    }

    //y[0..m]
    y[j] = 0;
  }

  // printf("[INFO] PID: %d : Matrix:\n", pid);
  // for(i = 0; i < m; i++){
  //   for(j = 0; j < n; j++){
  //     if(type == INT_T){
  //         printf(" %d ", (int)a[i*i+j]);
  //      } else {
  //         printf(" %f ",  a[i*i+j]);
  //      }
  //   }
  //   printf("\n");
  // }

  start = clock ();
  cblas_sgemv(order, transa, m, n, alpha, a, lda, x, incx,
               beta, y, incy);
  stop = clock();

  cpu_time = (double)(stop - start) / CLOCKS_PER_SEC;
  // printf("[INFO] PID: %d : CPU time: %f seconds\n", pid, cpu_time);

  // printf("[INFO] PID: %d : y = [", pid);
  // for( i = 0; i < n; i++ ){
  //   if(type == INT_T){
  //       printf(" %d ", (int)y[i]);
  //    } else {
  //       printf(" %f ", y[i]);
  //    }
  // }
  // printf("]\n");

  // Увеличиваем счетчик испытаний | Счет всегда начинается с "1"
  if(type == INT_T){
    test_num_i++;
    test_num = test_num_i;
  }else{
    test_num_f++;
    test_num = test_num_f;
  }

  fd = fopen(testArg.file, "a+");
  if(fd){
    fprintf(fd, "%s;sgemv;%s;%s;%d;clock;%f;%d\n",
                  testArg.cpu_model, get_value_type_name(type),
                  get_optim_key_name(testArg.optim_num), SGEMV_INSTRUCTION,
                  cpu_time, test_num);
    fclose(fd);
  }

  free(a);
  free(x);
  free(y);

  return SUCCESS;
}

int testing_cblas_dgemv(enum VALUE_TYPE type, enum CBLAS_ORDER order,
                               enum CBLAS_TRANSPOSE transa, int m, int n,
                               int lda, int incx, int incy){
  static int test_num = 0;
  int i, j;
  double *a, *x, *y;
  double alpha = 1, beta = 0;
  clock_t start, stop;
  double cpu_time = 0;
  FILE* fd;

  if(type != DOUBLE_T){
    printf("[ERROR] PID: %d : Func: %s: Reason: failed type %s\n",
          pid, __FUNCTION__, get_value_type_name(type));
    return FAIL;
  }

  // printf("[DEBUG] PID: %d : Action: generating matrix values: Type: %s\n",
          // pid, get_value_type_name(type));

  a = (double *)malloc(sizeof(double)*m*n);
  x = (double *)malloc(sizeof(double)*n);
  y = (double *)malloc(sizeof(double)*n);

  for(i = 0; i < m; i++){
    for(j = 0; j < n; j++){
      //a[0..m*n]
      a[i*i+j] = random_double_value();
      // x[0..n]
      x[j] = random_double_value();
    }
    //y[0..m]
    y[j] = 0;
  }

  // printf("[INFO] PID: %d : Matrix:\n", pid);
  // for(i = 0; i < m; i++){
  //   for(j = 0; j < n; j++){
  //     printf("%f ", a[i*i+j]);
  //   }
  //   printf("\n");
  // }

  start = clock ();
  cblas_dgemv(order, transa, m, n, alpha, a, lda, x, incx,
               beta, y, incy);
  stop = clock();

  cpu_time = (double)(stop - start) / CLOCKS_PER_SEC;
  // printf("[INFO] PID: %d : CPU time: %f seconds\n", pid, cpu_time);

  // printf("[INFO] PID: %d : y = [", pid);
  // for( i = 0; i < n; i++ ){
  //     printf(" %f ", y[i]);
  // }
  // printf("]\n");

  // Увеличиваем счетчик испытаний | Счет всегда начинается с "1"
  test_num++;

  fd = fopen(testArg.file, "a+");
  if(fd){
    fprintf(fd, "%s;dgemv;%s;%s;%d;clock;%f;%d\n",
                  testArg.cpu_model, get_value_type_name(type),
                  get_optim_key_name(testArg.optim_num), DGEMV_INSTRUCTION,
                  cpu_time, test_num);
    fclose(fd);
  }

  free(a);
  free(x);
  free(y);

  return SUCCESS;
}

// [Главная программа] ---------------------------------------------------------

int main(int argc, char *argv[]){
  FILE* fd;

  enum CBLAS_ORDER order = CblasColMajor;
  enum CBLAS_TRANSPOSE transa = CblasNoTrans;

  int m = 100;   // Размер столбца (количество строк)
  int n = 100;   // Размер строки (количество столбцов)
  int lda = 100;
  int incx, incy, i, ret;
  incx = incy = 1;

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

  if(testArg.cpu_model){
    free(testArg.cpu_model);
  }

  printf("[DEBUG] PID: %d : Action: programm finished\n", pid);
  return SUCCESS;
}
