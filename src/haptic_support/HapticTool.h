#pragma once
#ifndef HAPRICTOOL
#define HAPTICTOOL
#include <math.h>
#include <deque>
#include <set>
#include <vector>
#include "GlobalMethod.h"
#include <glad/glad.h>
#include "../shader.h"
#include "../mass_spring/DynamicWorld.h"
#define EPS 0.000001
#define MAXFORCESQ 27.0f
#define MAXFORCE 5.196152422706632f//sqrt(27)

struct CDInfo
{
	int tool_key;
	int object_key;
	float toolx, tooly, toolz, toolr;
	float objx, objy, objz, objr;
	bool nearstate;
	CDInfo() { nearstate = false; }
};

typedef std::deque <CDInfo> CDdeque;

class HapticTools
{
public:
	unsigned int HVAO;
	unsigned int HVBO;
	//rendering
	void DrawHaptic(StandardShader*);

protected:
	CDdeque m_CDInfoArray;
public:
	double	m_G[6][6];
	double	m_qg[6], m_qh[6];			//physic pos and grapic pos
	float	m_trans[3], m_rotate[3][3];		//current pos
	float  m_transpre[3], m_rotatepre[3][3];
	double  m_globalAngX, m_globalAngY, m_globalAngZ;
	double  m_deviceStiffnessGingval;
	double  m_stiffnessRatio;
	double	m_deviceRadius;

public:

	HapticTools();	// init haptic setting

	~HapticTools();

	void ForceBefore(float* deviceTrans);	//fetch transform to m_trans and m_rotate

	void ForceCompute(float* force, DynamicWorld* world);		//calculate collision and force

	void ForceAfter(float* trans);			//send transform to grapich trans to draw

	void calMatrixAngle(float* matrixInput, double& angX, double& angY, double& angZ);	//calculate axis angle from rotate matrix

	//void GetConstraints(CDdeque& CDInfoArray, double(*m_JNorm)[6], double *DD);

	//int OptiAcc(double M[6][6], double(*Jnorm)[6], double *DD, double au[6], double ac[6], int nRet);

	//void RestoreQgPrevious();

	void copyTrans();


};

void HapticTools::DrawHaptic(StandardShader* shader)
{
	shader->use();
	shader->setMatrix4("model", glm::mat4(1.0f));
	glBindVertexArray(HVAO);
	glBindBuffer(GL_ARRAY_BUFFER, HVBO);
	//update haptic point position
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(GL_FLOAT), m_transpre, GL_DYNAMIC_DRAW);

	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
	std::cout << m_transpre[0] << "," << m_transpre[1] << "," << m_transpre[2] << std::endl;
}

HapticTools::HapticTools() {

	m_globalAngX = 0;
	m_globalAngY = 0;
	m_globalAngZ = 0;

	memset(m_G, 0, 32 * sizeof(double));
	m_G[0][0] = m_G[1][1] = m_G[2][2] = 1.0;
	m_G[3][3] = m_G[4][4] = m_G[5][5] = 1000.0;

	m_deviceStiffnessGingval = 0.9f;

	m_stiffnessRatio = 1000.0f;

	m_deviceRadius = 0.01f;

	//rendering setting
	glGenVertexArrays(1, &HVAO);
	glGenBuffers(1, &HVBO);
	glBindVertexArray(HVAO);
	glBindBuffer(GL_ARRAY_BUFFER, HVBO);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(GL_FLOAT), m_transpre, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
	glBindVertexArray(0);

}

HapticTools::~HapticTools() {

}

void HapticTools::ForceBefore(float* deviceTrans) {
	m_trans[0] = deviceTrans[12];
	m_trans[1] = deviceTrans[13];
	m_trans[2] = deviceTrans[14];
	m_rotate[0][0] = deviceTrans[0];
	m_rotate[1][0] = deviceTrans[1];
	m_rotate[2][0] = deviceTrans[2];
	m_rotate[0][1] = deviceTrans[4];
	m_rotate[1][1] = deviceTrans[5];
	m_rotate[2][1] = deviceTrans[6];
	m_rotate[0][2] = deviceTrans[8];
	m_rotate[1][2] = deviceTrans[9];
	m_rotate[2][2] = deviceTrans[10];
	calMatrixAngle(deviceTrans, m_qh[3], m_qh[4], m_qh[5]);
	m_qh[0] = m_trans[0];
	m_qh[1] = m_trans[1];
	m_qh[2] = m_trans[2];
}

void HapticTools::ForceCompute(float* force, DynamicWorld* world) {
	m_rotatepre[0][0] = m_rotate[0][0];
	m_rotatepre[1][0] = m_rotate[1][0];
	m_rotatepre[2][0] = m_rotate[2][0];
	m_rotatepre[0][1] = m_rotate[0][1];
	m_rotatepre[1][1] = m_rotate[1][1];
	m_rotatepre[2][1] = m_rotate[2][1];
	m_rotatepre[0][2] = m_rotate[0][2];
	m_rotatepre[1][2] = m_rotate[1][2];
	m_rotatepre[2][2] = m_rotate[2][2];
	m_transpre[0] = m_qh[0];
	m_transpre[1] = m_qh[1];
	m_transpre[2] = m_qh[2];
	vec3d forceHaptic = vec3d(0.0);
	vec3d pos(m_transpre[0], m_transpre[1], m_transpre[2]);
	//Force compute
	for (int i = 0; i < world->m_meshes.size(); i++) {
		for (int j = 0; j < world->m_meshes[i].m_particles.size(); j++) {
			vec3d nodePos = world->m_meshes[i].m_particles[j].m_position;
			vec3d f = computeForce(pos, m_deviceRadius, nodePos,0.1f, 1.0f);
			if (glm::length(f)> 0)
			{
				vec3d tmpfrc = -f;
				world->m_meshes[i].m_particles[j].m_externalForce =tmpfrc;
			}
			forceHaptic+=f;
		}
	}

	force[0] = force[1] = force[2] = 0.0;
}

vec3d computeForce(const vec3d& a_cursor,
	double a_cursorRadius,
	const vec3d& a_spherePos,
	double a_radius,
	double a_stiffness)
{

	// compute the reaction forces between the tool and the ith sphere.
	vec3d force = vec3d(0.0);
	vec3d vSphereCursor = a_cursor - a_spherePos;

	// check if both objects are intersecting
	if (glm::length(vSphereCursor) < 0.0000001)
	{
		return (force);
	}

	if (glm::length(vSphereCursor) > (a_cursorRadius + a_radius))
	{
		return (force);
	}

	// compute penetration distance between tool and surface of sphere
	double penetrationDistance = (a_cursorRadius + a_radius) - glm::length(vSphereCursor);
	vec3d forceDirection = glm::normalize(vSphereCursor);
	force = (penetrationDistance * a_stiffness) *forceDirection;

	// return result
	return (force);
}

//calculate axis angle from rotation matrix
void HapticTools::calMatrixAngle(float* matrixInput, double& angX, double& angY, double& angZ)
{
	double matrixR[4][4];
	double cosY;

	for (int i = 0; i < 12; ++i)
	{
		if (i % 4 != 3)
		{
			int ti = i / 4 + 1;
			int tj = i % 4 + 1;

			matrixR[tj][ti] = matrixInput[i];
		}
	}
	cosY = sqrt(matrixR[1][1] * matrixR[1][1] + matrixR[2][1] * matrixR[2][1]);
	if (fabs(cosY) >= EPS)
	{
		angY = atan2(-matrixR[3][1], cosY);
		angZ = atan2(matrixR[2][1], matrixR[1][1]);
		angX = atan2(matrixR[3][2], matrixR[3][3]);
	}
	else
	{
		if (-matrixR[3][1] - 1 < EPS)
		{
			angY = PI / 2;
			angZ = m_globalAngZ;
			angX = atan2(matrixR[2][3], matrixR[1][3]) + angZ;
		}
		else
		{
			angY = -PI / 2;
			angZ = m_globalAngZ;
			angX = atan2(matrixR[2][3], matrixR[1][3]) - angZ;
		}
	}

	if (angX * m_globalAngX < 0 && fabs(angX - m_globalAngX) > 2.8)
	{
		if (m_globalAngX > 0)
			angX = angX + 2 * PI;
		else
			angX = angX - 2 * PI;
	}

	if (angY * m_globalAngY < 0 && fabs(angY - m_globalAngY) > 2.8)
	{
		if (m_globalAngY > 0)
			angY = angY + 2 * PI;
		else
			angY = angY - 2 * PI;
	}

	if (angZ * m_globalAngZ < 0 && fabs(angZ - m_globalAngZ) > 2.8)
	{
		if (m_globalAngZ > 0)
			angZ = angZ + 2 * PI;
		else
			angZ = angZ - 2 * PI;
	}

	m_globalAngX = angX;
	m_globalAngY = angY;
	m_globalAngZ = angZ;
}

void HapticTools::ForceAfter(float* trans)
{//获取虚拟位置
	trans[0] = m_rotatepre[0][0];
	trans[1] = m_rotatepre[1][0];
	trans[2] = m_rotatepre[2][0];
	trans[4] = m_rotatepre[0][1];
	trans[5] = m_rotatepre[1][1];
	trans[6] = m_rotatepre[2][1];
	trans[8] = m_rotatepre[0][2];
	trans[9] = m_rotatepre[1][2];
	trans[10] = m_rotatepre[2][2];
	trans[12] = m_transpre[0];
	trans[13] = m_transpre[1];
	trans[14] = m_transpre[2];
}

//void HapticTools::GetConstraints(CDdeque& CDInfoArray, double(*m_JNorm)[6], double *DD)
//{
//
//	double pre[42];
//	double qg3sin = sin(m_qg[3]);
//	double qg4sin = sin(m_qg[4]);
//	double qg5sin = sin(m_qg[5]);
//	double qg3cos = cos(m_qg[3]);
//	double qg4cos = cos(m_qg[4]);
//	double qg5cos = cos(m_qg[5]);
//
//	pre[13] = qg4cos*qg5sin;
//	pre[14] = qg5sin*qg4sin*qg3sin;
//	pre[15] = qg5cos*qg3cos;
//	pre[16] = qg5sin*qg4sin*qg3cos;
//	pre[17] = qg5cos*qg3sin;
//	pre[18] = qg5cos*qg4cos;
//	pre[19] = qg5cos*qg4sin*qg3sin;
//	pre[20] = qg5sin*qg3cos;
//	pre[21] = qg5cos*qg4sin*qg3cos;
//	pre[22] = qg5sin*qg3sin;
//	pre[23] = qg5cos*qg4sin;
//	pre[24] = qg5cos*qg4cos*qg3sin;
//	pre[25] = qg5cos*qg4cos*qg3cos;
//	pre[26] = qg5sin*qg4sin;
//	pre[27] = qg5sin*qg4cos*qg3sin;
//	pre[28] = qg5sin*qg4cos*qg3cos;
//	pre[29] = qg4cos;
//	pre[30] = qg4sin*qg3sin;
//	pre[31] = qg4sin*qg3cos;
//	pre[32] = qg5cos*qg4sin*qg3cos;
//	pre[33] = qg5sin*qg3sin;
//	pre[34] = qg5cos*qg4sin*qg3sin;
//	pre[35] = qg5sin*qg3cos;
//	pre[36] = qg5sin*qg4sin*qg3cos;
//	pre[37] = qg5cos*qg3sin;
//	pre[38] = qg5sin*qg4sin*qg3sin;
//	pre[39] = qg5cos*qg3cos;
//	pre[40] = qg4cos*qg3cos;
//	pre[41] = qg4cos*qg3sin;
//
//	int DequeSize = (int)CDInfoArray.size();
//
//	double tempx, tempy, tempz, r, T1, T2, T3, temp[8];
//	for (int j = 0; j<DequeSize; j++)
//	{
//		CDInfo& cdinfo = CDInfoArray[j];
//		CzSphere& orgsphere = m_sphereList[cdinfo.tool_key];
//		tempx = orgsphere.m_x;
//		tempy = orgsphere.m_y;
//		tempz = orgsphere.m_z;
//		r = cdinfo.objr + orgsphere.m_radius;
//		temp[0] = -tempx * pre[13] - tempy * pre[14] - tempy * pre[15] - tempz * pre[16] + tempz * pre[17];
//		temp[1] = tempx * pre[18] + tempy * pre[19] - tempy * pre[20] + tempz * pre[21] + tempz * pre[22];
//		temp[2] = -tempx * pre[23] + tempy * pre[24] + tempz * pre[25];
//		temp[3] = -tempx * pre[26] + tempy * pre[27] + tempz * pre[28];
//		temp[4] = -tempx * pre[29] - tempy * pre[30] - tempz * pre[31];
//		temp[5] = tempy * pre[32] + tempy * pre[33] - tempz * pre[34] + tempz * pre[35];
//		temp[6] = tempy * pre[36] - tempy * pre[37] - tempz * pre[38] - tempz * pre[39];
//		temp[7] = tempy * pre[40] - tempz * pre[41];
//
//		T1 = 2.0*(cdinfo.toolx - cdinfo.objx);
//		T2 = 2.0*(cdinfo.tooly - cdinfo.objy);
//		T3 = 2.0*(cdinfo.toolz - cdinfo.objz);
//		m_JNorm[j][0] = T1;
//		m_JNorm[j][1] = T2;
//		m_JNorm[j][2] = T3;
//		m_JNorm[j][5] = T1 * temp[0] + T2 * temp[1];
//		m_JNorm[j][4] = T1 * temp[2] + T2 * temp[3] + T3 * temp[4];
//		m_JNorm[j][3] = T1 * temp[5] + T2 * temp[6] + T3 * temp[7];
//		DD[j] = (T1*T1 + T2 * T2 + T3 * T3)*0.25 - r * r;
//	}
//}
//
//int HapticTools::OptiAcc(double M[6][6], double(*Jnorm)[6], double *DD, double au[6], double ac[6], int nRet)
//{
//	int i, j, k;
//	for (i = 0; i<nRet; i++)
//		DD[i] = -DD[i];
//
//	double pMatrixG[36];
//	memcpy(pMatrixG, M, 36 * sizeof(double));
//	//	double d[6] = {0};
//	double d[6] = { -au[0] + ac[0],-au[1] + ac[1],-au[2] + ac[2],m_stiffnessRatio*(ac[3] - au[3]),m_stiffnessRatio*(ac[4] - au[4]),m_stiffnessRatio*(ac[5] - au[5]) };
//	int iterations = 0;
//	double(*Active_Matrix)[6] = new double[nRet][6];
//	int CountZero = 0;
//	double xk[6] = { 0 };
//	//	Vector_Substract_Vector(ac,au,xk,6);//选取上一时刻图形显示为初始迭代点
//	double *tmp1 = new double[nRet];
//	double *C = new double[nRet];
//	int *flagzero = new int[nRet];
//	BOOL Update_xk = true;
//	iterations = 0;
//	while (iterations<20)
//	{
//		iterations++;
//
//		///不等式约束条件是:Ainq*x >= binq;
//		if (Update_xk == true)
//		{
//			//给定的迭代点xk,不等式约束中有几个有效,xk满足等式约束
//			Multiply_6(Jnorm, xk, tmp1, nRet);  //tmp1=Ainq*xk
//												//C=Ainq*xk-binq
//			Vector_Substract_Vector(tmp1, DD, C, nRet);
//			//判断向量C中0元素的个数，返回值表示0元素的个数,flagzero的元素标记了C的哪些元素为0
//			CountZero = JudgeVectorZero(C, nRet, flagzero);
//		}
//
//		//将不等式约束矩阵Ainq中起作用的行，构成作用矩阵Active_Matrix
//		AssembleMatrix(Jnorm, CountZero, flagzero, Active_Matrix, nRet);
//		/// 定义一个CountZero*6矩阵存贮Active_Matrix中有用的元素
//		double *pActive_Aeq = new double[CountZero * 6];
//		double *Minus_Transpose_pAeq = new double[6 * CountZero];
//
//		for (i = 0; i<CountZero; i++)
//			for (j = 0; j<6; j++)
//				*(pActive_Aeq + i * 6 + j) = double((int)((Active_Matrix[i][j] * 10000))) / 10000;
//
//		for (i = 0; i<CountZero; i++)
//			for (j = 0; j<6; j++)
//				*(Minus_Transpose_pAeq + j*CountZero + i) = -*(pActive_Aeq + i * 6 + j);
//
//		double *pZeroMatrix = new double[CountZero*CountZero];
//
//		for (i = 0; i<CountZero; i++)
//			for (j = 0; j<CountZero; j++)
//				*(pZeroMatrix + i*CountZero + j) = 0;
//
//		///构造KKT矩阵
//		double *pKKTMatrix = new double[(6 + CountZero)*(6 + CountZero)];
//		CombineMatrix(pMatrixG, Minus_Transpose_pAeq, pActive_Aeq, pZeroMatrix, pKKTMatrix, 6, 6, CountZero, CountZero);
//		CMatrix m_temptMatrix(6 + CountZero, 6 + CountZero, pKKTMatrix);
//		////求KKT矩阵的逆矩阵
//
//		///释放一些申请的内存
//		delete[]pActive_Aeq;
//		delete[]Minus_Transpose_pAeq;
//		delete[]pZeroMatrix;
//		delete[]pKKTMatrix;
//
//		double *Inverse_pKKTMatrix = new double[(6 + CountZero)*(6 + CountZero)];
//		if (m_temptMatrix.InvertGaussJordan())  //求逆成功，即KKT矩阵非奇异
//			memcpy(Inverse_pKKTMatrix, m_temptMatrix.m_pData, (6 + CountZero)*(6 + CountZero) * sizeof(double));
//		else ///KKT矩阵奇异，则后面的算法不适用
//		{
//			delete[]Inverse_pKKTMatrix;
//			break;
//		}
//
//		double Gxk[6], gk[6];
//		Multiply_6(M, xk, Gxk, 6); //Gxk=G*xk
//		Vector_Add_Vector(Gxk, d, gk, 6); //gk=G*xk+d;
//
//		double *pKKTVector = new double[6 + CountZero];
//		double *pKKTResult = new double[6 + CountZero];
//		double *pKKTLamda = new double[CountZero];
//
//
//
//		double KKTp[6];
//		int flag[6];
//		for (j = 0; j<6 + CountZero; j++)
//		{
//			if (j<6)
//				*(pKKTVector + j) = -gk[j];
//			else
//				*(pKKTVector + j) = 0;
//		}
//
//		pSquareMatrixMultiply(Inverse_pKKTMatrix, pKKTVector, 6 + CountZero, pKKTResult);
//
//		for (i = 0; i<6; i++)
//			KKTp[i] = *(pKKTResult + i);
//
//		for (i = 6; i<6 + CountZero; i++)
//			*(pKKTLamda + i - 6) = *(pKKTResult + i);
//
//
//		delete[]Inverse_pKKTMatrix;
//		delete[]pKKTVector;
//		delete[]pKKTResult;
//
//		if (JudgeVectorZero(KKTp, 6, flag) == 6)  //所有的KKTp=0
//		{
//			if (CountZero != 0)  //不等式约束有有效的
//			{
//				k = 0;
//				for (i = 0; i<CountZero; i++)
//					if (*(pKKTLamda + i) >= 0)
//						k++;
//
//				if (k == CountZero) //所有的pKKTLamda>=0,找到了优化解
//				{
//					delete[]pKKTLamda;
//					break;
//				}
//				else            //不是所有的pKKTLamda>=0,找到了优化解
//				{
//					double minLamda = *pKKTLamda;
//					int Subj = 0;
//					for (i = 1; i<CountZero; i++)
//					{
//						if (minLamda>*(pKKTLamda + i))
//						{
//							minLamda = *(pKKTLamda + i);
//							Subj = i;
//						}
//					}
//					///去掉一个有效的不等式约束
//					ModifyVector(flagzero, nRet, Subj + 1);
//					CountZero--;
//					Update_xk = false;
//				}
//			}
//			else
//			{
//				delete[]pKKTLamda;
//				break;
//			}
//		}
//		else   //不是所有的KKTp=0
//		{
//			double minAlfa = 1;
//			int Addj = -1;
//			for (i = 0; i<nRet; i++)
//			{
//				if (flagzero[i])
//				{
//					double aTpk = 0;
//					for (j = 0; j<6; j++)
//						aTpk += Jnorm[i][j] * KKTp[j];
//
//					if (aTpk<0)
//					{
//						double atxk = 0;
//						for (j = 0; j<6; j++)
//							atxk += Jnorm[i][j] * xk[j];
//
//						double bi_atxk = DD[i] - atxk;
//						double tmp = bi_atxk / aTpk;
//						if (tmp<minAlfa)
//						{
//							minAlfa = tmp;
//							Addj = i;
//						}
//					}
//				}
//			}
//			if (minAlfa<1)
//				Update_xk = true;
//			else
//				Update_xk = false;
//
//			for (i = 0; i<6; i++)
//				xk[i] = xk[i] + minAlfa*KKTp[i];
//		}
//		delete[]pKKTLamda;
//	}
//
//	delete[]tmp1;
//	delete[]C;
//	delete[]flagzero;
//	delete[]Active_Matrix;
//	//	memcpy(ac, xk, sizeof(xk));
//	Vector_Add_Vector(ac, xk, ac, 6);
//	//	Vector_Add_Vector(au,xk,ac,6);
//	return iterations;
//}
//
//void HapticTools::RestoreQgPrevious()
//{
//	m_transpre[0] = m_qg[0];
//	m_transpre[1] = m_qg[1];
//	m_transpre[2] = m_qg[2];
//	double sin_x = sin(m_qg[3]);
//	double cos_x = cos(m_qg[3]);
//	double sin_y = sin(m_qg[4]);
//	double cos_y = cos(m_qg[4]);
//	double sin_z = sin(m_qg[5]);
//	double cos_z = cos(m_qg[5]);
//	m_rotatepre[0][0] = cos_y * cos_z;
//	m_rotatepre[0][1] = -cos_y * sin_z;
//	m_rotatepre[0][2] = sin_y;
//	m_rotatepre[1][0] = cos_x * sin_z + sin_x * sin_y * cos_z;
//	m_rotatepre[1][1] = cos_x * cos_z - sin_x * sin_y * sin_z;
//	m_rotatepre[1][2] = -sin_x * cos_y;
//	m_rotatepre[2][0] = sin_x * sin_z - cos_x * sin_y * cos_z;
//	m_rotatepre[2][1] = sin_x * cos_z + cos_x * sin_y * sin_z;
//	m_rotatepre[2][2] = cos_x * cos_y;
//}


void HapticTools::copyTrans() {
	m_rotatepre[0][0] = m_rotate[0][0];
	m_rotatepre[1][0] = m_rotate[1][0];
	m_rotatepre[2][0] = m_rotate[2][0];
	m_rotatepre[0][1] = m_rotate[0][1];
	m_rotatepre[1][1] = m_rotate[1][1];
	m_rotatepre[2][1] = m_rotate[2][1];
	m_rotatepre[0][2] = m_rotate[0][2];
	m_rotatepre[1][2] = m_rotate[1][2];
	m_rotatepre[2][2] = m_rotate[2][2];
	m_transpre[0] = m_trans[0];
	m_transpre[1] = m_trans[1];
	m_transpre[2] = m_trans[2];
}

#endif // !HAPRICTOOL




//// basic class of sphere tree
//class CzSphere
//{
//public:
//	int			m_ID;
//	//geomatric properties
//	float		m_x, m_y, m_z, m_radius;
//	float m_threhold;
//
//public:
//	CzSphere() {
//		m_ID = 0;
//		m_threhold = 0.15f;
//	}
//	CzSphere(int kID, float x, float y, float z, float radius, float t = 0.15f)
//	{
//		m_ID = kID;
//		m_x = x;
//		m_y = y;
//		m_z = z;
//		m_radius = radius;
//		m_threhold = t;
//	}
//	//if the sphere intersecr with current sphere
//	inline bool IntersectSphere(float p1, float p2, float p3, float r)
//	{
//		if (r <= 0)
//			return false;
//		float m_dx = m_x - p1;
//		float m_dy = m_y - p2;
//		float m_dz = m_z - p3;
//		float m_dr = m_radius + r;
//		float Dist_2 = m_dx * m_dx + m_dy * m_dy + m_dz * m_dz;
//		if (Dist_2 <= (m_dr + m_threhold)*(m_dr + m_threhold))
//			return true;
//		else
//			return false;
//	}
//
//	inline char IntersectSphereTwoState(float p1, float p2, float p3, float r){
//		if (r <= 0)
//			return 0;
//
//		float m_dx = m_x - p1;
//		float m_dy = m_y - p2;
//		float m_dz = m_z - p3;
//		float m_dr = m_radius + r;
//		float Dist_2 = m_dx * m_dx + m_dy * m_dy + m_dz * m_dz;
//		if (Dist_2 <= (m_dr + m_threhold)*(m_dr + m_threhold)) {
//			if (Dist_2 <= m_dr*m_dr)
//				return 2;
//			return 1;
//		}			
//		else
//			return 0;
//	}
//
//};
//
//struct RigidCollision {
//	int rigidNum;
//	int particalsNum;
//	float particalRadius;
//	std::vector<float> rigidRadius;
//	std::vector<int>    rigidOffsets;
//	std::vector<int>    rigidIndices;
//	std::vector<bool> particalFlag;
//	float* rigidTrans;
//	float* flexParticals;
//	void ReSetFlag() {
//		particalFlag.clear();
//		particalFlag.resize(particalsNum, false);
//	}
//	bool CheckFlag(int idx) {
//		if (particalFlag[idx])
//			return true;
//		particalFlag[idx] = true;
//		return false;
//	}
//	~RigidCollision() {
//		if (rigidTrans) delete rigidTrans;
//		if (flexParticals) delete flexParticals;
//	}
//	RigidCollision() {
//		rigidTrans = NULL;
//		flexParticals = NULL;
//		particalRadius = 0.15f;
//	}
//};

