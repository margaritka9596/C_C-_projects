#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iomanip>		//setw()
#include <cassert>

using namespace std;

#define Size 100	//размер матрицы кратности 2

void print_array(double * Arr, int Size_Of_Array, int Step) 
{
    int i, j;
	for (i = 0; i < Size_Of_Array; ++i)
	{
		int l = i * Step;
		for(j = 0; j < Size_Of_Array; ++j)
			printf("%7.2f ", Arr[l + j] );
			printf("\n");
	}
}
/*перемножение матриц*/
void Multiple(double * A, double * B, double * Result, int N, int Step1, int Step2, int Step3)
{
	int i, j ,k;
	for(i = 0; i < N; ++i)
	{
		int l1 = i * Step3;
		int l2 = i * Step2;
		for(j = 0; j < N; ++j)
			for (k = 0; k < N; ++k)
				Result[l1 + j] += A[l2 + k] * B[k * Step2 + j] ;
	}
}
/*для блока A11*/
void Func1(double * L, double * U, double * A, int Block, int Step)
{
	int i, j, k;
	double Sum1, Sum2;
	/*найдем элементы первой строки матрицы U11*/
	for (j = 0; j < Block; ++j)
		U[j] = A[j];
	
	/*найдем элементы первого столбца матрицы L11*/
	for (j = 1; j < Block; ++j)
	{
		L[j * Size] = (A[j * Step] / U[0]);					//Step -> Size
		assert(abs(L[j * Size]) < 1000);
	}
	

	/*вычисляем оставшиеся элементы матрицы U11*/							
	for (i = 1; i < Block; ++i)
	{
		for (j = i; j < Block; ++j)
		{
			Sum1 = 0;
			/*считаем сумму для вычисления элементов матрицы U11*/
			for (k = 0; k < i ; ++k)
				Sum1 += L[i * Size + k] * U[k  * Size + j];			//Step -> Size
			U[i * Size + j] = A[i * Step + j] - Sum1;				//Step -> Size
		}
		/*вычисляем оставшиеся элементы матрицы L11*/
		for (j = i + 1; j < Block; ++j)
		{
			Sum2 = 0;
			/*считаем сумму для вычисления элементов матрицы L11*/
			for (k = 0; k < i ; ++k)
				Sum2 += L[j * Size + k] * U[k * Size + i];					//Step -> Size
			L[j * Size + i] = (A[j * Step + i] - Sum2) / U[i * Size + i];	//Step -> Size
			assert(abs(L[j * Size + i]) < 1000);
		}
	}
}
/*для блока A12 = L11U12*/
/*обратный ход метода Гаусса, ищем U12 */
void Gauss(double * L, double * U, double * A, int N, int Step)
{
	int i, j, k;
	for(i = 0; i < N ; ++i)
		for(j = 0; j < N; ++j)
		{
			double Sum = 0;
			/*вычислим сумму*/
			for(k = 0; k < i; ++k)
				Sum += L[i * Size + k] * U[k * Size + j];			//Step -> Size
			U[i * Size + j] = A[i * Step + j] - Sum ;				//Step -> Size
		}
}
/*для блока L21U11=A21*/
/*нахождение L21 из уравнения L21U11=A21*/
void Func2(double * L, double * U, double * A, int N, int Step)
{
	int i, j, k;
	for(j = 0; j < N; ++j)
		for(i = 0; i < N; ++i)
		{
			double Sum = 0;
			for(k = 0; k < j; ++k)
				Sum += L[i * Size + k] * U[k * Size + j];						//Step -> Size
			L[i * Size + j] = (A[i * Step + j] - Sum) / U[j * Size + j];		//Step -> Size
			assert(abs(L[i * Size + j]) < 1000);
		}
}

/*для редуцированной матрицы A22_red = A22 - L21U12*/
void Func3(double * Result, double * A22, double * L21, double * U12, int N, int Step1)
{
	int i, j;
	double * temp = new double [N * N];
	for(i = 0; i < N; ++i)
		for(j = 0; j < N; ++j)
		{
				temp[i * N + j] = 0;
				Result[i * N + j] = 0;
		}
	
	Multiple(L21, U12, temp, N, Size, Size, N );				//Step1, Step2 -> Size
	
	for(i = 0; i < N; ++i)
		for(j = 0; j < N; ++j)
			Result[i * N + j] = A22[i * Step1 + j] - temp[i * N + j];
	delete temp;
}
void Recursive(double * A, double * L, double * U, int N)
{
	if (N == 1)
	{
		U[0] = A[0];
		return;
	}
	int Block_Size;
	Block_Size = N / 2;

	/*применим разложение к блоку A11*/
	Func1(L, U, A, Block_Size, N);
	/*нашли первый блок A11
	мы знаем блоки L11 и U11, найдем U12 и L21  из L11U12 = A12 и L21U11 = A21*/
	/*решим уравнение L11U12 = A12 методом Гаусса, так как L уже нижнетреугольная матрица
	нам нужно сделать только обратный ход, Lii = 1*/
		
	Gauss(L, U + Block_Size, A + Block_Size, Block_Size, N);

	/*найдем L21 из L21U11 = A21*/
	Func2(L + Size * Block_Size, U, A + N * Block_Size, Block_Size, N);
	
	/*теперь ищем L22 и U22, A22_red = A22- L21U12 = L22U22*/
	/*найдем A22_red*/
	double * A22_red = new double [Block_Size * Block_Size];
	
	Func3(A22_red, A + N * Block_Size + Block_Size, L + Size * Block_Size, U + Block_Size, Block_Size, N);
		
	if (Block_Size > 0)		
		Recursive(A22_red, L + Size * Block_Size + Block_Size, U + Size * Block_Size + Block_Size, Block_Size);
	else
		printf("End\n");
	delete A22_red;
}

int main()
{
	srand(time(NULL));
	int i , j, Real_Size;
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
	print_array(A, Size, Size);*/

	Recursive(A, L, U, Size);

	/*printf("Array L\n");
	print_array(L, Size, Size);

	printf("Array U\n");
	print_array(U, Size, Size);*/

	/* проверка*/
	Multiple(L, U, NewA, Size, Size, Size, Size);

	/*printf("\nArray NewA\n");
	print_array(NewA, Size, Size);*/
	delete A;
	delete L;
	delete U;
	delete NewA;
	t = clock();
	printf ("%f seconds\n",((float)t)/CLOCKS_PER_SEC);
	return 0;
}

