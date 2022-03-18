#pragma once
#include <glm/glm.hpp>

namespace CDB
{
	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		void SetPosition(const glm::vec3& NewPos);
		const glm::vec3& GetPosition() const { return this->Position; }

		void SetRotation(const float NewRotation);
		const float GetRotation() const { return this->Rotation; }

		const glm::mat4& GetPVMatrix() const { return this->ProjectionViewMatrix; }
		const glm::mat4& GetViewMatrix() const { return this->ViewMatrix; }
		const glm::mat4& GetProjectionMatrix() const { return this->ViewMatrix; }

	private:
		void RecalculateViewMatrix();

		float Rotation;
		glm::mat4 ProjectionMatrix;
		glm::mat4 ViewMatrix;
		glm::mat4 ProjectionViewMatrix;

		glm::vec3 Position = { 0.0, 0.0, 0.0 };
	};
}