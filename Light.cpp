#include "Light.h"

/* Constructor. Implemented for you. */
PointLight::PointLight(const Vector3f & position, const Vector3f & intensity)
    : position(position), intensity(intensity)
{
}

// Compute the contribution of light at point p using the
// inverse square law formula
Vector3f PointLight::computeLightContribution(const Vector3f& p)
{
    //return intensity;
	 Vector3f ret;
     float distance; 
     distance = findDistance(this->position,p);
     ret.x = intensity.x / (float)(distance * distance);
     ret.y= intensity.y / (float) (distance * distance);
     ret.z = intensity.z / (float) (distance * distance);
     return ret;
}
