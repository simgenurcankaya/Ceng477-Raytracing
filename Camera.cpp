#include "Camera.h"

Camera::Camera(int _id,                      // Id of the camera
               const char* _imageName,       // Name of the output PPM file 
               const Vector3f& _pos,         // Camera position
               const Vector3f& _gaze,        // Camera gaze direction
               const Vector3f& _up,          // Camera up direction
               const ImagePlane& _imgPlane)  // Image plane parameters
{
     id = _id;
     strcpy(imageName,_imageName);
     pos = _pos;
     gaze = _gaze;
     up  =_up;
     w = vectorMult((-1.0), _gaze);
     vectorU = crossProduct(up,w);   
     imgPlane = _imgPlane; 
     distance = _imgPlane.distance;
}

/* Takes coordinate of an image pixel as row and col, and
 * returns the ray going through that pixel. 
 */
Ray Camera::getPrimaryRay(int col, int row) const
{
	Vector3f m,q,s;
     float su,sv;
     Ray ret;

     m = vectorSum(this->pos , vectorMult(this->distance, this->gaze));
     q = vectorSum( m , vectorMult(this->imgPlane.left, this->vectorU));
     q = vectorSum( q , vectorMult(this->imgPlane.top, this->up) );
     su = (this->imgPlane.right - this->imgPlane.left) * (col+ 0.5) / this->imgPlane.nx;
     sv = (this->imgPlane.top - this->imgPlane.bottom ) * (row + 0.5)/ this->imgPlane.ny;

     s = vectorSum(q, vectorMult(su, this->vectorU));
     s = vectorSubs( s, vectorMult(sv, this->up));
     ret.origin = this->pos;
     ret.direction = vectorSubs(s, this->pos);
     return ret;

}

