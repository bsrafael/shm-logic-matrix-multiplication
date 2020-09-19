#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>

/*------------------------------------------------------------------*/
#define SIZE 30

#define N 4 // matriz NxN
#define P 2 // numero de processos

/*------------------------------------------------------------------*/

void ler_matriz(int matriz[N][N])
{
  int x, y;
  for (x = 0; x < N; x++)
  {
    for (y = 0; y < N; y++)
    {
      printf("Digite o valor de [%d][%d]:", x, y);
      scanf("%d", &matriz[x][y]);
    }
  }
}

void escrever_matriz(int matriz[N][N])
{
  int x, y;
  for (x = 0; x < N; x++)
  {
    for (y = 0; y < N; y++)
    {
      printf("%d\t", matriz[x][y]);
    }
    printf("\n");
  }
}

int max(int a, int b)
{
  return a > b ? a : b;
}

int max_arr(int arr[], int size)
{
  int m = arr[0];
  int i;
  for (i = 1; i < size; i++)
  {
    m = max(m, arr[i]);
  }
  return m;
}

int min(int a, int b)
{
  return a < b ? a : b;
}

void array_para_matriz(int *array, int matriz[N][N])
{
  int x, y;
  for (x = 0; x < N; x++)
  {
    for (y = 0; y < N; y++)
    {
      matriz[x][y] = array[x * N + y];
    }
  }
}

void mock(int mat1[N][N], int mat2[N][N])
{
  srandom(1);

  int x, y, m;

  for (x=0; x<N; x++) {
    for (y=0; y<N; y++) {
      mat1[x][y] = random()%25;
      mat2[x][y] = random()%25;
    }
  }

}

int main()
{
  int mat1[N][N];
  int mat2[N][N];

  printf("Multiplicação Lógica de Matrizes\n");
  printf("Vamos multiplicar duas matrizes %d x %d\n", N, N);
  printf("\nPrimeira matriz\n");
  ler_matriz(mat1);

  printf("\nSegunda matriz\n");
  ler_matriz(mat2);

  // mock(mat1, mat2);

  printf("\n\nPrimeira matriz\n");
  escrever_matriz(mat1);

  printf("\nSegunda matriz\n");
  escrever_matriz(mat2);

  int i, shmid, pid;
  int chave = 1;
  int *memoria;

  if ((shmid = shmget(chave, N * N * sizeof(int), IPC_CREAT | 0600)) < 0)
  {
    printf("Erro na criacao da memoria compartilhada");
  }

  if ((memoria = shmat(shmid, 0, 0)) < 0)
  {
    printf("Erro na alocacao");
  }

  pid = fork();

  if (pid > 0) // pai
  {
    int xF, yF, i;

    for (xF = 0; xF < N; xF += 2)
    {
      for (yF = 0; yF < N; yF++)
      {
        int mins[N];
        for (i = 0; i < N; i++)
        {
          mins[i] = min(mat1[xF][i], mat2[i][yF]);
        }
        memoria[xF * N + yF] = max_arr(mins, N);
      }
    }

    wait(NULL);
    int final[N][N];
    array_para_matriz(memoria, final);
    printf("\n\nResultado:\n");
    escrever_matriz(final);
    shmdt(memoria);
    shmctl(shmid, IPC_RMID, NULL);
  }
  else // filho
  {
    int xP, yP, i;

    for (xP = 1; xP < N; xP += 2)
    {
      for (yP = 0; yP < N; yP++)
      {
        int mins[N];
        for (i = 0; i < N; i++)
        {
          mins[i] = min(mat1[xP][i], mat2[i][yP]);
        }
        memoria[xP * N + yP] = max_arr(mins, N);
      }
    }
    shmdt(memoria);
  }

  return 0;
}
/*------------------------------------------------------------------*/
