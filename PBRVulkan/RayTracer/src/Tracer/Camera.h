#pragma once

#include <glm/glm.hpp>

namespace Tracer
{
	enum class Action
	{
		LEFT,
		RIGHT,
		UP,
		DOWN
	};

	class Camera
	{
	public:
		Camera(glm::vec3 eye, glm::vec3 lookAt, float fov, float aspect);
		~Camera() = default;

		void OnKeyChanged(int key, int scanCode, int action, int mod);

		bool OnCursorPositionChanged(double xpos, double ypos);

		void OnMouseButtonChanged(int button, int action, int mods);

		bool OnBeforeRender();

		void static TimeDeltaUpdate();

		[[nodiscard]] glm::mat4 GetView() const;

		[[nodiscard]] glm::mat4 GetProjection() const;

		[[nodiscard]] glm::vec3 GetDirection() const;

		[[nodiscard]] glm::vec3 GetPosition() const;

	private:
		void Update();

		void Move(Action action);

		glm::vec3 position;
		glm::vec3 front{};
		glm::vec3 worldUp{};
		glm::vec3 right{};
		glm::vec3 up{};

		glm::vec3 pivot{};

		float lastX{};
		float lastY{};

		bool isFirstMouseEvent = false;
		bool isMousePressed = false;
		bool isCameraLeft = false;
		bool isCameraRight = false;
		bool isCameraUp = false;
		bool isCameraDown = false;

		float pitch, yaw, fov, focalDist, aperture, radius, aspect;

		static float DELTA_TIME;
		static float LAST_FRAME_TIME;
	};
}
