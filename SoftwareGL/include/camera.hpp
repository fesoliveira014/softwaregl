#ifndef CAMERA_H_
#define CAMERA_H_

#include "common.hpp"

class Camera
{
public:
    Camera();
    ~Camera();

    void update();

    void setPosition(const glm::vec3& position);
    void setPosition(float x, float y, float z);

    void setTarget(const glm::vec3& target);
    void setTarget(float x, float y, float z);

    void rotate(const float yaw, const float pitch, const float row = 0.0f);

    glm::vec3 getPosition() { return m_position; }
    glm::vec3 getTarget() { return m_target; }

    glm::mat4 getProjection() { return m_projection; }
    glm::mat4 getView() { return m_view; }
    glm::mat4 getModel() { return m_model; }

protected:
    glm::vec3 m_position;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_forward;
    glm::vec3 m_target;

    glm::mat4 m_projection;
    glm::mat4 m_view;
    glm::mat4 m_model;
};

#endif