#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class EditorCamera {
public:
    EditorCamera(float fov, float aspect, float nearClip, float farClip);

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;

    void OnMouseRotate(float deltaX, float deltaY);
    void OnMousePan(float deltaX, float deltaY);
    void OnMouseScroll(float delta);

    void SetViewportSize(float width, float height);

    float GetDistance() const { return m_Distance; }
    glm::vec3 GetPosition() const { return m_Position; }

private:
    void UpdateCamera();

private:
    float m_FOV, m_AspectRatio, m_NearClip, m_FarClip;
    float m_Yaw = 0.0f, m_Pitch = 0.0f;

    float m_Distance = 10.0f;
    glm::vec3 m_Position = glm::vec3(0.0f);
    glm::vec3 m_Target = glm::vec3(0.0f);
    glm::vec3 m_Up = glm::vec3(0, 1, 0);

    float m_ViewportWidth = 640.0f;
    float m_ViewportHeight = 480.0f;
};
