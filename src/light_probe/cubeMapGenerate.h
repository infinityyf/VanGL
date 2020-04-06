#pragma once
#ifndef CUBE_MAP
#define CUBE_MAP

class CubeMap {
public:
	unsigned int upID;
	unsigned int buttomID;
	unsigned int leftID;
	unsigned int rightID;
	unsigned int frontID;
	unsigned int backID;

public:
	void cubeMapFromHDR();
	void cubeMapFromProbe();

};

#endif // !CUBE_MAP
