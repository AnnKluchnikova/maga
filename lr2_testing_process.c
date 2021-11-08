#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <cblas.h>

#define SUCCESS 0
#define FAIL -1

int pid = -1;

// [Обработка опций] -----------------------------------------------------------

struct testArg_t{
  const char *cpu_model; // для ключа -m
  int test_count;        // для ключа -c
  int optim_num;         // для ключа -o
}testArg;

const char *optString = "m:c:o:";

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

int testing_int_cblas_sgemv(enum CBLAS_ORDER order,
                         enum CBLAS_TRANSPOSE transa, int m, int n, int lda,
                         int incx, int incy){
  int i, j;
  const float *a, *x, *y;
  float alpha = 1, beta = 0;
  clock_t start, stop;

  printf("[DEBUG] PID: %d : Action: generating matrix values: Type: int\n",
          pid);

  a = (int *)malloc(sizeof(int)*m*n);
  x = (int *)malloc(sizeof(int)*n);
  y = (int *)malloc(sizeof(int)*n);

  for(i = 0; i < m; i++){
    for(j = 0; j < n; j++){
      a[i*i+j] = random_int_value();
    }

    x[i] = random_int_value();
    y[i] = 0;
  }

  printf("[INFO] PID: %d : Matrix:\n", pid);
  for(i = 0; i < m; i++){
    for(j = 0; j < n; j++){
      printf("%d ", a[i*i+j]);
    }
    printf("\n");
  }

  start = clock ();
  cblas_sgemv(order, transa, m, n, alpha, a, lda, x, incx,
               beta, y, incy);
  stop = clock();
  printf("Loop required %f seconds", (stop - start) / CLK_TCK);

  for( i = 0; i < n; i++ ) 
    printf(" y%d = %d\n", i, y[i]);

  free(a);
  free(x);
  free(y);

  return SUCCESS;
}

int testing_float_cblas_sgemv(enum CBLAS_ORDER order,
                              enum CBLAS_TRANSPOSE transa, int m, int n,
                              int lda, int incx, int incy){
  int i, j;
  float *a, *x, *y;
  float alpha = 1, beta = 0;

  printf("[DEBUG] PID: %d : Action: generating matrix values: Type: float\n",
          pid);

  a = (float *)malloc(sizeof(float)*m*n);
  x = (float *)malloc(sizeof(float)*n);
  y = (float *)malloc(sizeof(float)*n);

  for(i = 0; i < m; i++){
    for(j = 0; j < n; j++){
      a[i*i+j] = random_float_value();
    }
  }

  printf("[INFO] PID: %d : Matrix:\n", pid);
  for(i = 0; i < m; i++){
    for(j = 0; j < n; j++){
      printf("%f ", a[i*i+j]);
    }
    printf("\n");
  }

  free(a);
  free(x);
  free(y);

  return SUCCESS;
}

int testing_double_cblas_dgemv(enum CBLAS_ORDER order,
                              enum CBLAS_TRANSPOSE transa, int m, int n,
                              int lda, int incx, int incy){
  int i, j;
  double *a, *x, *y;
  double alpha = 1, beta = 0;

  printf("[DEBUG] PID: %d : Action: generating matrix values: Type: double\n",
          pid);

  a = (double *)malloc(sizeof(double)*m*n);
  x = (double *)malloc(sizeof(double)*n);
  y = (double *)malloc(sizeof(double)*n);

  for(i = 0; i < m; i++){
    for(j = 0; j < n; j++){
      a[i*i+j] = random_double_value();
    }
  }

  printf("[INFO] PID: %d : Matrix:\n", pid);
  for(i = 0; i < m; i++){
    for(j = 0; j < n; j++){
      printf("%f ", a[i*i+j]);
    }
    printf("\n");
  }

  free(a);
  free(x);
  free(y);

  return SUCCESS;
}

// [Главная программа] ---------------------------------------------------------

int main(int argc, char *argv[]){
  enum CBLAS_ORDER order = CblasColMajor;
  enum CBLAS_TRANSPOSE transa = CblasNoTrans;

  int m = 4;   // Размер столбца (количество строк)
  int n = 4;   // Размер строки (количество столбцов)
  int lda = 4;
  int incx, incy;
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

  testing_int_cblas_sgemv(order, transa, m, n, lda, incx, incy);
  // testing_float_cblas_sgemv(order, transa, m, n, lda, incx, incy);
  // testing_double_cblas_dgemv(order, transa, m, n, lda, incx, incy);

  printf("[DEBUG] PID: %d : Action: programm finished\n", pid);
  return SUCCESS;
}