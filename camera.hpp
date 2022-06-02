#pragma once

#include <glm/glm.hpp>
#include <memory>

class Camera
{
public:
	Camera(double distance, double min_distance, double max_distance);

	void rotate(double x, double y);

	void pan(double x, double y);

	void zoom(double z);

	glm::dmat4& get() { return view_matrix_; }

private:
	glm::dmat4 view_matrix_;
	double distance_;
	double min_distance_;
	double max_distance_;

    constexpr static double ROTATE_SPEED = 1.0 / 8.0;
    constexpr static double PAN_SPEED = 1.0 / 2048.0;
	constexpr static double ZOOM_SPEED = 1.0 / 8.0;
};
