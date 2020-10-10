#pragma once

#include <glm/glm.hpp>

namespace Tracer
{
	class Camera
	{
	public:
		Camera(glm::vec3 eye, glm::vec3 lookAt, float fov);
		~Camera() = default;

		void OnKeyChanged(int key, int scanCode, int action, int mod);

		void OnCursorPositionChanged(double xpos, double ypos);

		void OnMouseButtonChanged(int button, int action, int mods);

		[[nodiscard]] glm::mat4 GetView() const;

		[[nodiscard]] glm::mat4 GetProjection() const;

		[[nodiscard]] glm::vec3 GetDirection() const;

		void static UpdateTime();

	private:
		void Update();

		glm::vec3 position;
		glm::vec3 front{};
		glm::vec3 worldUp{};
		glm::vec3 right{};
		glm::vec3 up{};

		glm::vec3 pivot{};

		float lastX{};
		float lastY{};
		bool firstMouse = false;
		bool activeMouse = false;

		float pitch, yaw, fov, focalDist, aperture, radius;

		static float DELTA_TIME;
		static float LAST_FRAME_TIME;
	};
}
