#include "Camera.h"

#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

float Tracer::Camera::DELTA_TIME = 0;
float Tracer::Camera::LAST_FRAME_TIME = 0;
float SPEED = 4.f;
float SENSITIVITY = 0.10f;

namespace Tracer
{
	Camera::Camera(glm::vec3 eye, glm::vec3 lookAt, float fov, float aspect)
		: position(eye), fov(fov), aspect(aspect)
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
		switch (key)
		{
		case GLFW_KEY_S:
			isCameraDown = action != GLFW_RELEASE;
			break;
		case GLFW_KEY_W:
			isCameraUp = action != GLFW_RELEASE;
			break;
		case GLFW_KEY_A:
			isCameraLeft = action != GLFW_RELEASE;
			break;
		case GLFW_KEY_D:
			isCameraRight = action != GLFW_RELEASE;
			break;
		default:
			break;
		}
	}

	bool Camera::OnCursorPositionChanged(double xpos, double ypos)
	{
		if (isFirstMouseEvent)
		{
			lastX = xpos;
			lastY = ypos;
			isFirstMouseEvent = false;
			return false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;

		lastX = xpos;
		lastY = ypos;

		if (!isMousePressed) return false;

		xoffset *= SENSITIVITY;
		yoffset *= SENSITIVITY;

		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;

		Update();

		return true;
	}

	void Camera::OnMouseButtonChanged(int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			isMousePressed = action == GLFW_PRESS;
		}
	}

	bool Camera::OnBeforeRender()
	{
		if (isCameraDown) Move(Action::DOWN);
		if (isCameraLeft) Move(Action::LEFT);
		if (isCameraRight) Move(Action::RIGHT);
		if (isCameraUp) Move(Action::UP);

		return isCameraDown | isCameraLeft | isCameraRight | isCameraUp;
	}

	glm::mat4 Camera::GetView() const
	{
		return lookAt(position, position + front, up);
	}

	glm::mat4 Camera::GetProjection() const
	{
		auto projection = glm::perspective(glm::radians(fov), aspect, 0.1f, 1000.0f);
		projection[1][1] *= -1;

		return projection;
	}

	glm::vec3 Camera::GetDirection() const
	{
		return front;
	}

	glm::vec3 Camera::GetPosition() const
	{
		return position;
	}

	void Camera::TimeDeltaUpdate()
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

	void Camera::Move(Action action)
	{
		const float distance = SPEED * DELTA_TIME;

		switch (action)
		{
		case Action::DOWN:
			position -= front * distance;
			break;
		case Action::UP:
			position += front * distance;
			break;
		case Action::LEFT:
			position -= right * distance;
			break;
		case Action::RIGHT:
			position += right * distance;
			break;
		default:
			break;
		}
	}
}
