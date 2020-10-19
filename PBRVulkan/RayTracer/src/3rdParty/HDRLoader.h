#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

/***********************************************************************************
	Created:	17:9:2002
	FileName: 	hdrloader.h
	Author:		Igor Kravtchenko
	
	Info:		Load HDR image and convert to a set of float32 RGB triplet.
************************************************************************************/

/* 
	This is modified version of the original code. Addeed code to build marginal & conditional densities for IBL importance sampling
*/

namespace Assets
{
	enum HDRType
	{
		COLOR,
		MARGINAL,
		CONDITIONAL
	};

	class HDRData
	{
	public:
		HDRData() : width(0), height(0), cols(nullptr), marginalDistData(nullptr), conditionalDistData(nullptr) {}

		~HDRData()
		{
			if (cols != nullptr)
				delete cols;
			if (marginalDistData != nullptr)
				delete marginalDistData;
			if (conditionalDistData != nullptr)
				delete conditionalDistData;

			cols = nullptr;
			marginalDistData = nullptr;
			conditionalDistData = nullptr;
		}

		int width, height;
		// each pixel takes 3 float32, each component can be of any value...
		float* cols;
		glm::vec2* marginalDistData; // y component holds the pdf
		glm::vec2* conditionalDistData; // y component holds the pdf
	};

	class HDRLoader
	{
	private:
		static void BuildDistributions(HDRData* res);

	public:
		static HDRData* Load(const char* fileName);
	};
}
