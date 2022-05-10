#include "Camera.hpp"

namespace gps {

	//Camera constructor
	Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
		this->cameraPosition = cameraPosition;
		this->cameraTarget = cameraTarget;
		this->cameraUpDirection = cameraUp;

		//TODO - Update the rest of camera parameters
		this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
		this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
	}

	//return the view matrix, using the glm::lookAt() function
	glm::mat4 Camera::getViewMatrix() {
		return glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
	}

	//update the camera internal parameters following a camera move event
	void Camera::move(MOVE_DIRECTION direction, float speed) {
		//TODO
		switch (direction) {
		case MOVE_FORWARD: cameraPosition += cameraFrontDirection * speed; cameraTarget += cameraFrontDirection * speed; break;
		case MOVE_BACKWARD: cameraPosition -= cameraFrontDirection * speed; cameraTarget -= cameraFrontDirection * speed; break;
		case MOVE_RIGHT: cameraPosition += cameraRightDirection * speed; cameraTarget += cameraRightDirection * speed; break;
		case MOVE_LEFT: cameraPosition -= cameraRightDirection * speed; cameraTarget -= cameraRightDirection * speed; break;
		case MOVE_UP: cameraPosition += cameraUpDirection * speed; cameraTarget += cameraUpDirection * speed; break;
		default:break;
		}
	}

	//update the camera internal parameters following a camera rotate event
	//yaw - camera rotation around the y axis
	//pitch - camera rotation around the x axis
	void Camera::rotate(float pitch, float yaw) {
		//TODO
		totalPITCH += pitch;
		totalYAW += yaw;

		glm::mat4 rotateMatrix(1.0f);

		rotateMatrix = glm::rotate(rotateMatrix, totalYAW, glm::vec3(0.0f, 1.0f, 0.0f));
		rotateMatrix = glm::rotate(rotateMatrix, totalPITCH, glm::vec3(1.0f, 0.0f, 0.0f));


		cameraFrontDirection = rotateMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
		cameraUpDirection = rotateMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

		cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
		cameraTarget = cameraPosition + cameraFrontDirection;
	}

	glm::vec3 Camera::getCameraTarget() {
		return cameraTarget;
	}

	glm::vec3 Camera::getCameraPosition() {
		return cameraPosition;
	}

	glm::vec3 Camera::getCameraUpDirection() {
		return cameraUpDirection;
	}

	float Camera::getCameraYaw() {
		return totalYAW;
	}
	float Camera::getCameraPitch() {
		return totalPITCH;
	}

	void Camera::setCameraParams(glm::vec3 position, glm::vec3 target, glm::vec3 upDir, float pitch, float yaw, bool final) {
		yaw /= 2;
		pitch /= 2;
		this->totalYAW = yaw;
		this->totalPITCH = pitch;
		cameraPosition = position;
		rotate(pitch, yaw);
	}
}