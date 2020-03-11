#include "GlobalMethod.h"

void Vector_Add_Vector(double *vector1,double *vector2,double *result,int dimension)
{
	int i;
	for(i=0;i<dimension;i++)  
		result[i] = vector1[i] + vector2[i];
}

void Vector_Substract_Vector(double *vector1,double *vector2,double *result,int dimension)
{
	int i;
	for(i=0;i<dimension;i++) 
		result[i] = vector1[i] - vector2[i]; 
}

void Multiply_6(double A[][6],double B[6],double *C,int row)
{
	int i,j;
	for(i=0;i<row;i++)
	{
		C[i]=0;
		for(j=0;j<6;j++)
			C[i] += A[i][j]*B[j];
	}
}

void MatrixMultiplyVector(double A[3][3],double V[3],double C[3])
{
	int i;
	for (i=0;i<3;i++)
	{
		C[i]=A[i][0]*V[0]+A[i][1]*V[1]+A[i][2]*V[2];
	}
}


void VectorPlus(double A[],double B[],double C[],int Diemsion)
{
	for(int i=0;i<Diemsion;i++)
		C[i] = A[i]+B[i];
}

void MatrixMultiplyMatrix(double A[3][3],double B[3][3],double C[3][3])
{
	int i,j;
	for(i=0; i<3; i++)
		for(j=0; j<3; j++)
			C[i][j] = A[i][0]*B[0][j] + A[i][1]*B[1][j] + A[i][2]*B[2][j];
}

void GetRotationMatrix(double CurPosture[3],double RotateMatrix[3][3])
{
	double Rz[3][3] = {cos(CurPosture[2]),-sin(CurPosture[2]),0, sin(CurPosture[2]),cos(CurPosture[2]),0, 0,0,1};
	double Ry[3][3] = {cos(CurPosture[1]),0,sin(CurPosture[1]), 0,1,0, -sin(CurPosture[1]),0,cos(CurPosture[1])};
	double Rx[3][3] = {1,0,0, 0,cos(CurPosture[0]),-sin(CurPosture[0]), 0,sin(CurPosture[0]),cos(CurPosture[0])};
	double tmp[3][3];
	MatrixMultiplyMatrix(Rz,Ry,tmp);  //tmp=Rz*Ry
	MatrixMultiplyMatrix(tmp,Rx,RotateMatrix); //RotateMatrix=Rz*Ry*Rx

}


//判断向量元素是否全为0，返回该向量中有几个元素等于0; B[]中不等于0 的元素表示该处A不等于0
int JudgeVectorZero(double A[],int n,int B[])
{
	int count=n;
	for(int i=0;i<n;i++)
	{
		if(fabs(A[i])<ERRORa) {  B[i]=0; }
		else { B[i]=1; count--; }
	}
	return count;
}

void AssembleMatrix(double (*Ainq)[6],int count,int *flag,double (*result)[6],int nRet)
{
	int i,j,k;
	i=0;
	for(k=0;k<nRet;k++)
	{	
		if(flag[k]==0) 
		{
			for(j=0;j<6;j++) 
			{	result[i][j]=Ainq[k][j];	}
			i++;
		}
	}
	for(i=count;i<nRet;i++)
	{
		for(j=0;j<6;j++)
		{	result[i][j]=0;	}
	}	
}



void ModifyVector(int Vector[],int m,int k) ///把向量中的第K个等于0的元素修改为1
{
	int i,j=0;
	if(k!=0)
	{
		for(i=0;i<m;i++)
		{
			if(Vector[i]==0)
				j++;
			if(j==k)
				Vector[i]=1;
		}
	}	
}

//用分块矩阵A，B，C，D构成矩阵[A B;C D]:(m+p)*(n+q)
//A:m*n,B:m*q, C:p*n;, D:p*q
void CombineMatrix(double *A,double *B,double *C, double *D,double *result,
				   int m,int n,int p ,int q)
{
	int i,j;
	for(i=0;i<m+p;i++)
	{
		if(i<m)
		{
			for(j=0;j<n;j++) { *(result+i*(n+q)+j) = *(A+i*n+j); }
			for(j=n;j<n+q;j++) {  *(result+i*(n+q)+j) = *(B+i*q+j-n); }
		}
		if(i>=m)
		{
			for(j=0;j<n;j++) { *(result+i*(n+q)+j) = *(C+(i-m)*n+j); }
			for(j=n;j<n+q;j++) {  *(result+i*(n+q)+j) = *(D+(i-m)*q+j-n); }
		}
	}
}

//A[m][m]*B[m]=C[m]
void pSquareMatrixMultiply(double *A,double *B,int m, double *C)
{
	int i,j;
	for (i=0;i<m;i++)
	{
		*(C+i)=0;
		for(j=0;j<m;j++)
			*(C+i) += (*(A+i*m+j))*(*(B+j));
	}
}

void ChangeCoordinate(double p[3], double trans[3], double rotate[3][3])
{
	double tmp[3] = {rotate[0][0]*p[0] + rotate[0][1]*p[1] + rotate[0][2]*p[2] + trans[0],
		rotate[1][0]*p[0] + rotate[1][1]*p[1] + rotate[1][2]*p[2] + trans[1],
		rotate[2][0]*p[0] + rotate[2][1]*p[1] + rotate[2][2]*p[2] + trans[2]};
	p[0] = tmp[0];
	p[1] = tmp[1];
	p[2] = tmp[2];
}
