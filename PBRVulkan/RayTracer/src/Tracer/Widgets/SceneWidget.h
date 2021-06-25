#pragma once

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "../../path.h"

#include "Widget.h"

namespace Interface
{
	class SceneWidget final : public Widget
	{
	public:
		~SceneWidget() override = default;

		void Render(Settings& settings) override;

		static std::string GetScenePath(uint32_t index)
		{
			auto root = Path::Root({"PBRVulkan", "Assets", "PBRScenes"});
			const auto path = root / CONFIGS[index];
			std::cout << "[SCENE] Scene selected " << path.string() << std::endl;
			return path.string();
		}

	private:
		const char* integrators[3] = {
			"Path tracer",
			"SMS",
			"AO"
		};

		const char* scenes[18] = {
			"Ajax",
			"Bedroom",
			"Boy",
			"Coffee cart",
			"Coffee maker",
			"Cornell box",
			"Dining room",
			"Dragon",
			"Hyperion",
			"Panther",
			"Spaceship",
			"Staircase",
			"Stormtroopers",
			"Teapot",
			"Hyperion II",
			"Mustang",
			"Mustang Red",
			"Furnace"
		};

		static const inline std::vector<std::string> CONFIGS =
		{
			"ajax.scene",
			"bedroom.scene",
			"boy.scene",
			"coffee_cart.scene",
			"coffee_maker.scene",
			"cornell_box.scene",
			"diningroom.scene",
			"dragon.scene",
			"hyperion.scene",
			"panther.scene",
			"spaceship.scene",
			"staircase.scene",
			"stormtrooper.scene",
			"teapot.scene",
			"hyperion2.scene",
			"mustang.scene",
			"mustang_red.scene",
			"furnace.scene"
		};
	};
}
