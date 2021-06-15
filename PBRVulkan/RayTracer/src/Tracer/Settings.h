#pragma once

struct Settings final
{
	bool UseRasterizer{};
	bool UseGammaCorrection = true;
	bool DoubleSidedLight = false;
	bool UseComputeShaders = false;
	int IntegratorType{};
	int ComputeShaderId{};
	int SceneId = 5;
	int MaxDepth = 3;
	int SSP = 1;
	float Fov{};
	bool UseEnvMap = false;
	float HdrMultiplier = 0.f;
	float Aperture = 0.001f;
	float FocalDistance = 1.f;
	float AORayLength = 0.5f;

	[[nodiscard]] bool RequiresShaderRecompliation(const Settings& prev) const
	{
		return UseGammaCorrection != prev.UseGammaCorrection || IntegratorType != prev.IntegratorType;
	}

	[[nodiscard]] bool RequiresAccumulationReset(const Settings& prev) const
	{
		return
			UseRasterizer != prev.UseRasterizer ||
			UseGammaCorrection != prev.UseGammaCorrection ||
			IntegratorType != prev.IntegratorType ||
			SceneId != prev.SceneId ||
			MaxDepth != prev.MaxDepth ||
			SSP != prev.SSP ||
			Fov != prev.Fov ||
			Aperture != prev.Aperture ||
			FocalDistance != prev.FocalDistance ||
			AORayLength != prev.AORayLength ||
			HdrMultiplier != prev.HdrMultiplier ||
			UseEnvMap != prev.UseEnvMap ||
			DoubleSidedLight != prev.DoubleSidedLight;
	}
};
