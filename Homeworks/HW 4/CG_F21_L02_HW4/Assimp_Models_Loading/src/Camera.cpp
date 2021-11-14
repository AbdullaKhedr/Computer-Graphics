#include "Camera.h"

Camera::Camera()
	:mPosition(glm::vec3(0.0f, 0.0f, 0.0f)),
	mTargetPos(glm::vec3(0.0f, 0.0f, 0.0f)),
	mUp(glm::vec3(0.0f, 1.0f, 0.0f)),
	mRight(glm::vec3(0.0f, 0.0f, 0.0f)),
	worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
	mYaw(glm::pi<float>()),
	mPitch(0.0f),
	mFOV(45.0f)
{}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(mPosition, mTargetPos, mUp);
}

const glm::vec3& Camera::getLook() const
{
	return mLook;
}
const glm::vec3& Camera::getRight() const
{
	return mRight;
}
const glm::vec3& Camera::getUp() const
{
	return mUp;
}

FPSCamera::FPSCamera(glm::vec3 position, float yaw, float pitch)
{
	mPosition = position;
	mYaw = yaw;
	mPitch = pitch;
}

FPSCamera::FPSCamera(glm::vec3 position, glm::vec3 target)
{
	mPosition = position;
	mTargetPos = target;

	glm::vec3 lookDir = position - target;

	mYaw = atan2(lookDir.x, lookDir.z) + glm::pi<float>();
	mPitch = -atan2(lookDir.y, sqrt(pow(lookDir.x, 2) + pow(lookDir.z, 2)));
}

void FPSCamera::setPosition(const glm::vec3& position)
{
	mPosition = position;
}

void FPSCamera::rotate(float yaw, float pitch)
{
	mYaw += glm::radians(yaw);
	mPitch += glm::radians(pitch);

	// add constrains on the values to be on the range
	mPitch = glm::clamp(mPitch, -glm::pi<float>() / 2.0f + 0.1f, glm::pi<float>() / 2.0f - 0.1f);
	
	// Constraints to keep on rangr 0 to 2PI
	if (mYaw > glm::two_pi<float>())
		mYaw -= glm::two_pi<float>();
	if (mYaw < 0.0)
		mYaw += glm::two_pi<float>();

	updateCameraVectors();
}

void FPSCamera::move(const glm::vec3& offsetPos)
{
	mPosition += offsetPos;
	updateCameraVectors();
}

void FPSCamera::updateCameraVectors() 
{
	glm::vec3 look;
	look.x = cosf(mPitch) * sinf(mYaw);
	look.y = sinf(mPitch);
	look.z = cosf(mPitch) * cosf(mYaw);

	mLook = glm::normalize(look);

	mRight = glm::normalize(glm::cross(mLook, worldUp));
	mUp = glm::normalize(glm::cross(mRight, mLook)); // ??

	mTargetPos = mPosition + mLook;
}