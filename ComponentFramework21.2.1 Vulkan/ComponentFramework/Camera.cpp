#include "Camera.h"


Camera::Camera() {
	projection.loadIdentity();
	view.loadIdentity();
}

Camera::~Camera() {

}

void Camera::Perspective(const float fovy, const float aspectRatio, const float near, const float far) {
	projection = MMath::perspective(fovy, aspectRatio, near,far);
}

void Camera::LookAt(const Vec3& eye, const Vec3& at, const Vec3& up) {
	view = MMath::lookAt(eye, at, up);
}