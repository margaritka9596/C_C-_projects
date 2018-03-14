#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iomanip>		//setw()

using namespace std;

#define Size 100		//размер матрицы кратности 2	

void print_array(double * Arr, int Size_Of_Array) 
{
    int i, j;
	for (i = 0; i < Size_Of_Array; ++i)
	{
		for(j = 0; j < Size_Of_Array; ++j)
			printf("%7.2f ", Arr[i * Size_Of_Array + j] );
			printf("\n");
	}
}

int main()
{
	srand(time(NULL));
	int i , j, k, Real_Size;
	double Sum1, Sum2;
	clock_t t;
	Real_Size = Size * Size;
	double *A = new double [Real_Size], *L = new double[Real_Size],*U = new double[Real_Size],*NewA = new double[Real_Size];
	
	/*заполним матрицу А*/
	/*диагональные элементы*/
	for (i = 0; i < Size; ++i)		
	{
		A[i * Size + i] = rand()%100 + 100;		//Преобладание элементов на диагонали
		/*заполним диагональные элементы матрицы L единицами*/
		L[i * Size + i] = 1;
	}
	/*оставшиеся элементы матриц*/
	for (i = 0; i < Size; ++i)
		for (j = 0; j < Size; ++j)
		{
				if (i != j)
				{
					A[i * Size + j] = rand()%100;
					L[i * Size + j] = 0;			//Инициализируем матрицу L
				}
				U[i * Size + j] = 0;				//Инициализируем матрицу U
				NewA[i * Size + j] = 0;
		}
	
	/*printf("\nArray A\n");
	print_array(A, Size);*/

	/*найдем элементы первой строки матрицы U*/
	for (j = 0; j < Size; ++j)
		U[0 + j] = A[0 + j];
	
	/*найдем элементы первого столбца матрицы L*/
	for (j = 1; j < Size; ++j)
		L[j * Size] = (A[j * Size] / U[0]);
	//
	/*вычисляем оставшиеся элементы матрицы U*/
	for (i = 1; i < Size; ++i)
	{
		for (j = i; j < Size; ++j)
		{
			Sum1 = 0;
			/*считаем сумму для вычисления элементов матрицы U*/
			for (k = 0; k < i ; ++k)
				Sum1 += L[i * Size + k] * U[k  * Size + j];
			U[i * Size + j] = A[i * Size + j] - Sum1;
		}
		/*вычисляем оставшиеся элементы матрицы L*/
		for (j = i + 1; j < Size; ++j)
		{
			Sum2 = 0;
			/*считаем сумму для вычисления элементов матрицы L*/
			for (k = 0; k < i ; ++k)
				Sum2 += L[j * Size + k] * U[k * Size + i];
			L[j * Size + i] = (A[j * Size + i] - Sum2) / U[i * Size + i];
		}
	}
	/*printf("Array L\n");
	print_array(L, Size);

	printf("Array U\n");
	print_array(U, Size);*/

	/* проверка*/
	for(i = 0; i < Size; ++i)
		for(j = 0; j < Size; ++j)
			for (k = 0; k < Size; ++k)
			{
				NewA[i * Size + j] += U[k * Size + j] * L[i * Size + k] ;
			}

	/*printf("\nArray NewA\n");
	print_array(NewA, Size);*/
	t = clock();
	printf ("%f seconds\n",((float)t)/CLOCKS_PER_SEC);
	return 0;
}

