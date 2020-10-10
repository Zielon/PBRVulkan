#include "Camera.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

float Tracer::Camera::DELTA_TIME = 0;
float Tracer::Camera::LAST_FRAME_TIME = 0;
float SPEED = 15.5f;
float SENSITIVITY = 0.10f;

namespace Tracer
{
	Camera::Camera(glm::vec3 eye, glm::vec3 lookAt, float fov)
		: position(eye), fov(fov)
	{
		position = eye;
		pivot = lookAt;
		worldUp = glm::vec3(0, 1, 0);
		front = glm::vec3(0, 0, -1);

		glm::vec3 dir = normalize(pivot - position);
		pitch = glm::degrees(asin(dir.y));
		yaw = glm::degrees(atan2(dir.z, dir.x));

		radius = distance(eye, lookAt);

		focalDist = 0.1f;
		aperture = 0.0;

		Update();
	}

	void Camera::OnKeyChanged(int key, int scanCode, int action, int mod)
	{
		const float velocity = SPEED * DELTA_TIME;

		switch (key)
		{
		case GLFW_KEY_S:
			position -= front * velocity;
			break;
		case GLFW_KEY_W:
			position += front * velocity;
			break;
		case GLFW_KEY_A:
			position -= right * velocity;
			break;
		case GLFW_KEY_D:
			position += right * velocity;
			break;
		default:
			break;
		}

		Update();
	}

	void Camera::OnCursorPositionChanged(double xpos, double ypos)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
			return;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;

		lastX = xpos;
		lastY = ypos;

		if (!activeMouse) return;

		xoffset *= SENSITIVITY;
		yoffset *= SENSITIVITY;

		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;

		Update();
	}

	void Camera::OnMouseButtonChanged(int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			activeMouse = action == GLFW_PRESS;
		}
	}

	glm::mat4 Camera::GetView() const
	{
		return lookAt(position, position + front, up);
	}

	glm::mat4 Camera::GetProjection() const
	{
		auto projection = glm::perspective(glm::radians(fov), 1.f, 0.1f, 100.0f);
		projection[1][1] *= -1;

		return projection;
	}

	glm::vec3 Camera::GetDirection() const
	{
		return front;
	}

	void Camera::UpdateTime()
	{
		const auto currentFrame = static_cast<float>(glfwGetTime());
		DELTA_TIME = currentFrame - LAST_FRAME_TIME;
		LAST_FRAME_TIME = currentFrame;
	}

	void Camera::Update()
	{
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		front = normalize(direction);

		right = normalize(cross(front, worldUp));
		up = normalize(cross(right, front));
	}
}
