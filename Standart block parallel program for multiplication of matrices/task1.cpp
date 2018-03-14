#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iomanip>		//setw()
#include <math.h>
#include <omp.h>

using namespace std;

#define N 2880 		//размер матрицы
#define BLOCK_N 60
//размер блока
#define BLOCK_ELEM BLOCK_N * BLOCK_N
#define NUM_BLOCKS N/BLOCK_N
#define BLOCK_TRIANGLE_N  N *( N + BLOCK_N) / 2

#pragma warning(disable: 4996)

//функции дл€ проверки перемножени€(стандартный алгоритм)
void fill_check_A(double *arr, int size)
{
	srand(time(NULL));
	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
			if (i > j)
				arr[i * size + j] = arr[j * size + i];
			else
				arr[i * size + j] = (rand() % 100) / 10.0;
}
void fill_check_B(double *arr, int size)
{
	
	//srand(time(NULL));
	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
			if (i > j)
				arr[i * size + j] = 0.0;
			else
				arr[i * size + j] = (rand() % 100) / 10.0;
}
void initialize_C(double* arr, int size)
{
	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
			arr[i * size + j] = 0.0;
}
void mult_check_A_B(double *A, double *B, double *C, int size)
{
	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
			for (int k = 0; k < size; ++k)
				C[i * size + j] += A[i * size + k] * B[k * size + j];

}
//функции дл€ блочного перемножени€
void fill_A_from_arr(double *A, double *arr, int size, int b_s)
{
	int num_blocks = size / b_s;
	//в терминах блоков 
	//по столбцам: слева-направо
	for (int dj = 0; dj < num_blocks; ++dj)
	{
		//сверху-вниз
		for (int di = 0; di <= dj; ++di)
			//внутри блока по строкам
			for (int k = 0; k < b_s; ++k) //~i
				for (int l = 0; l < b_s; ++l) //~j
				{
					int start_block = di * b_s * b_s + (dj * (dj + 1) / 2) * b_s * b_s;
					A[start_block + k * b_s + l] = arr[(di * b_s + k) * size + dj * b_s + l];
					if (di == dj && k > l)
						A[start_block + k * b_s + l] = A[start_block + l * b_s + k];
				}
	}
}
void fill_B_from_arr(double *B, double *arr, int size, int b_s)
{
	int num_blocks = size / b_s;
	//в терминах блоков 
	//по строкам: сверху-вниз
	for (int di = 0; di < num_blocks; ++di)
		// слева-направо
		for (int dj = di; dj < num_blocks; ++dj)
			//внутри блока по строкам
			for (int k = 0; k < b_s; ++k) //~i
				for (int l = 0; l < b_s; ++l) //~j
				{
					int start_block = (dj - di) * b_s * b_s + (di * (size)* b_s) - (di * (di - 1) / 2) * b_s * b_s;
					B[start_block + k * b_s + l] = arr[(di * b_s + k) * size + dj * b_s + l];
					if (di == dj && k > l)
						B[start_block + k * b_s + l] = 0.0;
				}
}
void fill_A(double *arr, int size, int b_s) 
{
	double count = 1.0;
	int num_blocks = size / b_s;
	int wrong_lines = 0;
	//в терминах блоков 
	//по столбцам: слева-направо
	for (int dj = 0; dj < num_blocks; dj++)
	{
		//сверху-вниз
		for (int di = 0; di <= dj; ++di)
		{
			//внутри блока по строкам
			for (int k = 0; k < b_s; ++k) //~i
			{
				for (int l = 0; l < b_s; ++l) //~j
				{
					arr[di * b_s * b_s + (dj * (dj + 1) / 2) * b_s * b_s + k * b_s + l] = count;
					count += 1.0;
					//условие дл€ диагональных блоков
					if (di == dj && k > l)
						arr[di * b_s * b_s + (dj * (dj + 1) / 2) * b_s * b_s + k * b_s + l] = arr[di * b_s * b_s + (dj * (dj + 1) / 2) * b_s * b_s + l * b_s + k];
				}
			}
		}
	}
}
void fill_B(double *arr, int size, int b_s)
{
	double count = 1.0;
	int num_blocks = size / b_s;
	//в терминах блоков 
	//по строкам: сверху-вниз
	for (int di = 0; di < num_blocks; ++di)
		// слева-направо
		for (int dj = di; dj < num_blocks; ++dj)
			//внутри блока по строкам
			for (int k = 0; k < b_s; ++k) //~i
				for (int l = 0; l < b_s; ++l) //~j
				{
					arr[(dj - di) * b_s * b_s + (di * N * b_s) - di * (di - b_s) / 2] = count;
					count += 1.0;
					//условие дл€ диагональных блоков    ! потом вынести за цикл - не эффективно
					if (di == dj && k > l)
						arr[(dj - di) * b_s * b_s + (di * N * b_s) - di * (di - b_s) / 2] = 0.0;
				}
}
void multBlocks(double* blockA, double* blockB, double* blockZ, int b_s, int simm)
{
	int i, j, k, zi, zk, n = 0;
//#pragma omp parallel for shared(blockZ, blockA, blockB, simm) private(i, j, k, zi, zk) schedule(static) num_threads (b_s)
		for (i = 0; i < b_s; ++i)
		{
			zi = i * b_s;
			for (j = 0; j < b_s; ++j)
				for (k = 0; k < b_s; ++k)
				{
					zk = k * b_s;
					blockZ[zi + j] += blockA[simm ? zk + i : zi + k] * blockB[zk + j];
				}
			//printf("Ќить \%d сложила элементы с номером %d\n", n, i);
		}
	//}
}
int findIndA(int di, int dj, int b_s, int &simm)
{
	//индекс начала блока
	//ниже диагонали
	if (di > dj)
	{
		simm = 1;
		int z = di;
		di = dj;
		dj = z;
	}
	return di * b_s * b_s + (dj * (dj + 1) / 2) * b_s * b_s;
}
int findIndB(int di, int dj, int b_s)
{
	return (dj - di) * b_s * b_s + (di * N * b_s) - (di * (di - 1) / 2) * b_s * b_s;
}
int findIndC(int di, int dj, int b_s)
{
	return di * b_s * b_s * NUM_BLOCKS + dj * b_s * b_s;
}
void mult_block_A_B(double *A, double *B, double *C, int size, int b_s)
{
	int num_blocks = size / b_s;
	int block_size = b_s * b_s;
	//выдел€ем пам€ть под блок результата произведени€ двух блоков
	//double z_block[BLOCK_ELEM];
	double *z_block = (double*)malloc(BLOCK_ELEM * sizeof(double));
	for (int i = 0; i < BLOCK_ELEM; ++i)
		z_block[i] = 0;

	int flag = 1, simm = 0;
	int indA = 0, indB = 0, indC = 0;
	int di, dj, dk, i;
	//перемножение поблочное
//#pragma omp parallel for shared(C, A, B) private(di, dj, dk, i, indA, indB, indC, simm, flag, z_block) schedule(static) num_threads (num_blocks)
	for (di = 0; di < num_blocks; ++di)
		for (dj = 0; dj < num_blocks; ++dj)
			for (dk = 0; dk < num_blocks; ++dk)
			{
				flag = 1;//не ноль блок
				simm = 0;
				for (i = 0; i < BLOCK_ELEM; ++i)
					z_block[i] = 0;

				//вычисление адреса блока матрицы A
				indA = findIndA(di, dk, b_s, simm);

				//вычисление адреса блока матрицы B
				if (dk > dj)
					flag = 0;//нулевой блок
				else
				{
					indB = findIndB(dk, dj, b_s);
					//printf("indB=%d\n", indB);
				}
				//вычисление адреса блока матрицы C
				indC = findIndC(di, dj, b_s);
				//printf("indC = %d, thread number %d\n", indC, omp_get_thread_num());
				//если не нулевой блок B, тогда перемножаем блоки и суммируем, а если нет, то останетс€ тот же —
				if (flag)
				{
					multBlocks(A + indA, B + indB, z_block, b_s, simm);
					for (i = 0; i < block_size; ++i)
					{
						C[indC + i] += z_block[i];
						//printf("indC = %d, thread number %d, z_block[i]= %f\n", indC, omp_get_thread_num(), z_block[i]);
					}
				}
			}
	free(z_block);
}
//проверка равенства двух полученных матриц
void check_results(double *C, double *C_block, int size, int b_s)
{
	printf("Are arrays C and C_block equal? - ");
	int num_blocks = size / b_s;
	bool flag = false;
	//в терминах блоков 
	//по строкам: сверху-вниз
	for (int di = 0; di < num_blocks; ++di)
	{
		// слева-направо
		for (int dj = 0; dj < num_blocks; ++dj)
			//внутри блока по строкам
			for (int k = 0; k < b_s; ++k) //~i
				for (int l = 0; l < b_s; ++l) //~j
				{
					int start_block = di * b_s * b_s * num_blocks + dj * b_s * b_s;
					if (fabs(C[(di * b_s + k) * size + dj * b_s + l] - C_block[start_block + k * b_s + l]) > 0.000001)
						flag = true;
				}
	}
	if (flag)
		printf("No!\n");
	else
		printf("Yes!\n");
}
//функции печати блочных матриц, которые расположены как одномерный массив
void print_Arr_lines(double *arr, int size)
{
	printf("Size =%d\n", size);
	for (int i = 0; i < size; ++i)
		printf("%7.2f, ", arr[i]);
	printf("\n");
}
//печать в двух циклах
void print_Arr(double *arr, int size)
{
	printf("Size =%d\n", size * size);
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
			printf("%7.2f, ", arr[i * size + j]);
		printf("\n");
	}
}
int main()
{
	//матрицы дл€ проверки
	double *check_A = (double*)malloc(N * N * sizeof(double)), //симметрична€
		*check_B =  (double*)malloc(N * N * sizeof(double)), //верхнетреугольна€, внизу нули
		*check_C = (double*)malloc(N * N * sizeof(double));

	//матрицы дл€ блочного перемножени€
	double *A = (double*)malloc(BLOCK_TRIANGLE_N * sizeof(double)), //симметрична€, хранитс€ как верхетреугольна€
		*B = (double*)malloc(BLOCK_TRIANGLE_N * sizeof(double)),  //верхнетреугольна€
		*C = (double*)malloc(N * N * sizeof(double));

	fill_check_A(check_A, N);
	/*printf("Check_A:\n");
	print_Arr(check_A, N);*/

	fill_check_B(check_B, N);
	/*printf("\nCheck_B:\n");
	print_Arr(check_B, N);*/

	initialize_C(check_C, N);

	clock_t t_start, t_end;
	t_start = clock();

//	mult_check_A_B(check_A, check_B, check_C, N);
	
	t_end = clock();
	printf("\nMultiplication of arrays done in %f seconds\n", ((double)(t_end - t_start)) / CLOCKS_PER_SEC);

	/*printf("\nCheck multiplic Matrix C\n");
	print_Arr(check_C, N);//*/
	
	//блочное перемножение
	/*заполним матрицу ј*/
	//fill_A(A, N, BLOCK_N);
	fill_A_from_arr(A, check_A, N, BLOCK_N);

	/*printf("\nMatrix A\n");
	print_Arr_lines(A, BLOCK_TRIANGLE_N);//*/

	/*заполним матрицу B*/
	//fill_B(B, N, BLOCK_N);
	fill_B_from_arr(B, check_B, N, BLOCK_N);

	/*printf("\nMatrix B\n");
	print_Arr_lines(B, BLOCK_TRIANGLE_N);//*/

	initialize_C(C, N);

	clock_t t_block_start, t_block_end;
	t_block_start = clock();

	mult_block_A_B(A, B, C, N, BLOCK_N);

	t_block_end = clock();
	printf("\nMultiplication of block arrays done in %f seconds\n", ((double)(t_block_end - t_block_start)) / CLOCKS_PER_SEC);

	/*printf("\nBlock multiplic Matrix C\n");
	print_Arr_lines(C, N * N);//*/
	
//	check_results(check_C, C, N, BLOCK_N);
	
	free(A);
	free(B);
	free(C);
	free(check_A);
	free(check_B);
	free(check_C);

	return 0;
}