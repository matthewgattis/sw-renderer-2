#include "camera.hpp"

#include <cmath>
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

constexpr glm::dmat4 IDENTITY = glm::identity<glm::dmat4>();
constexpr double RAD = glm::pi<double>() / 180.0;

Camera::Camera(double distance, double min_distance, double max_distance) :
	view_matrix_(IDENTITY),
	distance_(std::max(min_distance, std::min(distance, max_distance))),
	min_distance_(min_distance),
	max_distance_(max_distance)
{
	view_matrix_[3].z = -distance;
}

constexpr glm::dvec3 right(1.0, 0.0, 0.0);
constexpr glm::dvec3 up(0.0, 1.0, 0.0);
constexpr glm::dvec3 forward(0.0, 0.0, 1.0);

void Camera::rotate(double x, double y)
{
	glm::dmat4 r =
		glm::rotate(IDENTITY, ROTATE_SPEED * RAD * x, up) *
		glm::rotate(IDENTITY, ROTATE_SPEED * RAD * y, right);
	glm::dvec3 a = forward * distance_;
	glm::dmat4 b = glm::translate(
		IDENTITY,
		glm::dvec3(r * glm::dvec4(a, 0.0)) - a);
	view_matrix_ = b * r * view_matrix_;
}

void Camera::pan(double x, double y)
{
	view_matrix_ =
		glm::translate(
			IDENTITY,
			PAN_SPEED * distance_ * glm::dvec3(x, -y, 0.0)) *
		view_matrix_;
}

void Camera::zoom(double z)
{
	double d = ZOOM_SPEED * distance_ * z;
	if (distance_ - d >= max_distance_)
		d = distance_ - max_distance_;
	if (distance_ - d <= min_distance_)
		d = distance_ - min_distance_;
	view_matrix_ = glm::translate(IDENTITY, forward * d) * view_matrix_;
	distance_ = glm::max(min_distance_, glm::min(distance_ - d, max_distance_));
}

