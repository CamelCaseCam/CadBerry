#include <cdbpch.h>
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace CDB
{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top) : 
		ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f))
	{
		RecalculateViewMatrix();
	}

	void OrthographicCamera::SetPosition(const glm::vec3& NewPos)
	{
		this->Position = NewPos;
		RecalculateViewMatrix();
	}

	void OrthographicCamera::SetRotation(const float NewRotation)
	{
		this->Rotation = NewRotation;
		RecalculateViewMatrix();
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		glm::mat4 Transform = glm::translate(glm::mat4(1.0), this->Position) * glm::rotate(glm::mat4(1.0), glm::radians(this->Rotation), 
			glm::vec3(0.0, 0.0, 1.0));

		this->ViewMatrix = glm::inverse(Transform);
		this->ProjectionViewMatrix = this->ProjectionMatrix * this->ViewMatrix;
	}
}