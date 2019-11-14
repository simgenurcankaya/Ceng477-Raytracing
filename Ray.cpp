#include "Ray.h"

Ray::Ray()
{
}

Ray::Ray(const Vector3f& origin, const Vector3f& direction)
    : origin(origin), direction(direction)
{
}

/* Takes a parameter t and returns the point accoring to t. t is the parametric variable in the ray equation o+t*d.*/
Vector3f Ray::getPoint(float t) const 
{
	// o + t *d  = ray 
    Vector3f ret;
    ret = vectorSum(this->origin , vectorMult(t,this->direction));
    return ret;
}

/* Takes a point p and returns the parameter t according to p such that p = o+t*d. */
float Ray::gett(const Vector3f & p) const
{
    float ret = 0;
    if(direction.x != 0)
        ret = (p.x - this->origin.x)/direction.x;
    else if(direction.y != 0)
        ret = ( p.y - this->origin.y)/direction.y;
    else if(direction.z != 0)
        ret = (p.z - this->origin.z)/direction.z;
    else 
        ret = 0;
    return ret;
}

