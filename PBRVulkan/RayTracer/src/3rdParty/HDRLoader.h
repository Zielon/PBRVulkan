#pragma once

#include <glm/glm.hpp>
#include <iostream>


/***********************************************************************************
    Created:    17:9:2002
    FileName:     hdrloader.h
    Author:        Igor Kravtchenko

    Info:        Load HDR image and convert to a set of float32 RGB triplet.
************************************************************************************/

/*
    This is a modified version of the original code. Addeed code to build marginal & conditional densities for IBL importance sampling
*/

class HDRData
{
public:
	HDRData() : width(0), height(0), cols(nullptr), marginalDistData(nullptr), conditionalDistData(nullptr)
	{
	}

	~HDRData()
	{
		delete cols;
		delete marginalDistData;
		delete conditionalDistData;
	}

	int width, height;
	// each pixel takes 3 float32, each component can be of any value...
	glm::float32* cols;
	glm::vec<3, glm::float32>* marginalDistData; // y component holds the pdf
	glm::vec<3, glm::float32>* conditionalDistData; // y component holds the pdf
};

class HDRLoader
{
private:
	static void buildDistributions(HDRData* res);
public:
	static HDRData* load(const char* fileName);
};
