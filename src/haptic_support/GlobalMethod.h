#ifndef    _GLOBALMETHOD
#define    _GLOBALMETHOD

#include <math.h>
#include "CzMatrix.h"
using namespace std;

#define ERRORa	1e-8
#define EPS		0.000001
#define PI		3.1415926


void GetRotationMatrix(double CurPosture[3],double RotateMatrix[3][3]);
void Multiply_6(double A[][6],double B[6],double *C,int row);
void MatrixMultiplyVector(double A[3][3],double V[3],double C[3]);
void MatrixMultiplyMatrix(double A[3][3],double B[3][3],double C[3][3]);
void VectorPlus(double A[],double B[],double C[],int Diemsion);
void Vector_Add_Vector(double *vector1,double *vector2,double *result,int dimension);
void Vector_Substract_Vector(double *vector1,double *vector2,double *result,int dimension);

//�ж�����Ԫ���Ƿ�ȫΪ0�����ظ��������м���Ԫ�ص���0; B[]�в�����0 ��Ԫ�ر�ʾ�ô�A������0
int JudgeVectorZero(double A[],int n,int B[]);
void AssembleMatrix(double (*Ainq)[6],int count,int *flag,double (*result)[6],int nRet);
void ModifyVector(int Vector[],int m,int k); ///�������еĵ�K������0��Ԫ���޸�Ϊ1

//�÷ֿ����A��B��C��D���ɾ���[A B;C D]:(m+p)*(n+q)
//A:m*n,B:m*q, C:p*n;, D:p*q
void CombineMatrix(double *A,double *B,double *C, double *D,double *result,
				   int m,int n,int p ,int q);
void pSquareMatrixMultiply(double *A,double *B,int m, double *C);//A[m][m]*B[m]=C[m]

void ChangeCoordinate(double p[3], double trans[3], double rotate[3][3]);


#endif