#ifndef _MATRIX_H_
#define _MATRIX_H_
class CMatrix
{
public:
	CMatrix();
	CMatrix(int nRows, int nCols);
    CMatrix(int nRows, int nCols, double value[]);
	CMatrix(int nSize);
    CMatrix(int nSize, double value[]);
	CMatrix(const CMatrix& other);
	~CMatrix();
	
public:
    bool Init(int nRows, int nCols);
    bool MakeUnitMatrix(int nSize);
	void SetData(double value[]);
	bool SetElement(int nRow, int nCol, double value);
	double GetElement(int nRow, int nCol)const; 
	int	GetNumColumns()const; 
	int	GetNumRows()const; 
	double* GetData()const; 
	int GetRowVector(int nRow, double* pVector)const; 
	int GetColVector(int nCol, double* pVector)const; 
	CMatrix& operator=(const CMatrix& other);
	bool operator==(const CMatrix& other)const; 
	bool operator!=(const CMatrix& other)const; 
	CMatrix	operator+(const CMatrix& other)const; 
	CMatrix	operator-(const CMatrix& other)const; 
	CMatrix	operator*(double value)const; 
	CMatrix	operator*(const CMatrix& other)const; 
	bool CMul(const CMatrix& AR, const CMatrix& AI, const CMatrix& BR, const CMatrix& BI, CMatrix& CR, CMatrix& CI)const; 
	CMatrix Transpose()const; 
	bool InvertGaussJordan();
	bool InvertGaussJordan(CMatrix& mtxImag);
	bool InvertSsgj();
	bool InvertTrench();
	double DetGauss();
	int RankGauss();
	bool DetCholesky(double* dblDet);
	bool SplitLU(CMatrix& mtxL, CMatrix& mtxU);
	bool SplitQR(CMatrix& mtxQ);
	bool SplitUV(CMatrix& mtxU, CMatrix& mtxV, double eps /*= 0.000001*/);
	void ppp(double a[], double e[], double s[], double v[], int m, int n);
	void sss(double fg[2], double cs[2]);
	bool GInvertUV(CMatrix& mtxAP, CMatrix& mtxU, CMatrix& mtxV, double eps /*= 0.000001*/);
	bool MakeSymTri(CMatrix& mtxQ, CMatrix& mtxT, double dblB[], double dblC[]);
	bool SymTriEigenv(double dblB[], double dblC[], CMatrix& mtxQ, int nMaxIt /*= 60*/, double eps /*= 0.000001*/);
	void MakeHberg();
	bool HBergEigenv(double dblU[], double dblV[], int nMaxIt /*= 60*/, double eps /*= 0.000001*/);
	bool JacobiEigenv(double dblEigenValue[], CMatrix& mtxEigenVector, int nMaxIt /*= 60*/, double eps /*= 0.000001*/);
	bool JacobiEigenv2(double dblEigenValue[], CMatrix& mtxEigenVector, double eps /*= 0.000001*/);
	
public:
	int m_nNumColumns, m_nNumRows;
	double *m_pData;
};


void MatrixCopyBtoA(float A[4][4],float B[4][4]);
void MatrixInit(float A[4][4]);
void ReverseMatrix(float A[4][4], float B[4][4]);
void MultiMatrix(float a[4][4],float b[4][4],float c[4][4]);
void MultiMatrixVector(float A[3][3], float B[3], float C[3]);
void MultiMatrix_double(double a[4][4],double b[4][4],double c[4][4]);//æÿ’Ûc=a*b

#endif