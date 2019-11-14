#include "Shape.h"
#include "Scene.h"
#include <cstdio>

Shape::Shape(void)
{
}

Shape::Shape(int id, int matIndex)
    : id(id), matIndex(matIndex)
{
}


Sphere::Sphere(void)
{}

/* Constructor for sphere. You will implement this. */
Sphere::Sphere(int id, int matIndex, int cIndex, float R, vector<Vector3f> *pVertices)
    : Shape(id, matIndex)
{
	center = pVertices->at(cIndex-1);
    radius = R;
}

/* Sphere-ray intersection routine. You will implement this. 
Note that ReturnVal structure should hold the information related to the intersection point, e.g., coordinate of that point, normal at that point etc. 
You should to declare the variables in ReturnVal structure you think you will need. It is in defs.h file. */
ReturnVal Sphere::intersect(const Ray & ray) const
{
    
	float delta, A,B,C, r1, r2;
    Vector3f center = this->center;
    Vector3f dummy;
    ReturnVal ret;
    
  //  printf("Direction %f , %f ,%f \n",ray.direction.x, ray.direction.y, ray.direction.z);
    A = dotProduct(ray.direction, ray.direction);
    B = dotProduct( ray.direction,vectorSubs(ray.origin, center));
    B = 2*B;
    C = dotProduct( vectorSubs(ray.origin, center),vectorSubs(ray.origin,center));
    C = C - this->radius * this->radius;

   
    delta =  B * B - 4 * A * C ;

    
    if (delta < 0 ) {
        ret.isIntersect = false;
        ret.t = FLT_MAX;
        ret.normal = Vector3f();
        ret.materialIndex = -1;
        return ret;
    }
    // printf("A  = %f , B = %f , C = %f\n", A,B,C);
   // printf("Delta = %f \n", delta);
  //  printf("Delta is %f\n",delta);    
    r1 = ((-1) * B - sqrt(delta))/ (2 *A);
    r2 = ((-1) * B + sqrt(delta)) / (2*A);

    if(r1<r2){
    //    printf("Sphere intersect 1 \n");
        ret.t = r1;
        dummy = ray.getPoint(r1); // vectorSum(ray.origin , vectorMult(r1, ray.direction));
        dummy = vectorSubs(dummy, center);
        ret.normal = dummy;
        ret.materialIndex = this->matIndex;
        ret.isIntersect = false;
        if(ret.t>0) ret.isIntersect = true;
        return ret;
    }
    else{
      //  printf("Sphere intersect 2 \n");
        ret.t = r2;
        dummy =ray.getPoint(r2); //  vectorSum(ray.origin , vectorMult(r2, ray.direction));
        dummy = vectorSubs(dummy, center);
        ret.normal = dummy;
        ret.materialIndex = this->matIndex;
         if(ret.t>0) ret.isIntersect = true;
         //printf("T = %f \n", r2);
        return ret;
    }
}

Triangle::Triangle(void)
{}

/* Constructor for triangle. You will implement this. */
Triangle::Triangle(int id, int matIndex, int p1Index, int p2Index, int p3Index, vector<Vector3f> *pVertices)
    : Shape(id, matIndex)
{
	p1 = pVertices->at(p1Index-1);
    p2 = pVertices->at(p2Index-1);
    p3 = pVertices->at(p3Index-1);
}

/* Triangle-ray intersection routine. You will implement this. 
Note that ReturnVal structure should hold the information related to the intersection point, e.g., coordinate of that point, normal at that point etc. 
You should to declare the variables in ReturnVal structure you think you will need. It is in defs.h file. */
ReturnVal Triangle::intersect(const Ray & ray) const
{
    
	float allArea, alphaArea, betaArea, alpha, beta, gamma;
    ReturnVal ret, planePoint;
    Vector3f p1,p2,p3, p3p1, p2p1, normal, point;

    p1 = this->p1;
    p2 = this->p2;
    p3 = this ->p3;

    p3p1 = vectorSubs(p3,p1);
    p2p1 = vectorSubs(p2,p1);
    normal = crossProduct(p2p1,p3p1);

  //  printf("p1 = %f, p2 = %f\n",p1.x, p2.y);
    if(dotProduct(normal,ray.direction)== 0){
        // no intersect;

        planePoint.isIntersect = false;
        planePoint.t = FLT_MAX;
        planePoint.normal = Vector3f();
        planePoint.materialIndex = -1;
        return planePoint;

    }
    else{
        // intersect
        
        planePoint.normal = normal;
        planePoint.t = dotProduct(normal, vectorSubs(p2, ray.origin))/ dotProduct(normal, ray.direction);
        planePoint.materialIndex =this->matIndex;
        planePoint.isIntersect = false;
        if(planePoint.t > 0) planePoint.isIntersect = true;
    }
   
    allArea = findTriangleArea(p1,p2,p3);
   // printf("Ray.origin x = %f y  =%f  z = %f \n",ray.origin.x, ray.origin.y, ray.origin.z);
   // printf("Ray.direction x = %f y  =%f  z = %f \n",ray.direction.x, ray.direction.y, ray.direction.z);
    
    
    point = vectorSum(ray.origin, vectorMult(planePoint.t, ray.direction));

    //printf("point x = %f y  =%f  z = %f \n",point.x, point.y, point.z);
    
    alpha = findTriangleArea(p1,p2,point) / allArea;
    beta = findTriangleArea(p1,p3,point) / allArea;
    gamma = findTriangleArea(p3,p2,point) / allArea;

  // printf("Area = %f , alpha = %f , beta = %f , gamma = %f  SUM = %f\n",allArea, alpha,beta,gamma,(alpha+beta+gamma));
 // epsilon value eklenecek 
    if((alpha+beta+gamma) -1 < 1e-6 && alpha <= 1 && beta <= 1 && gamma <= 1 ){
       //  printf("Area = %f , alpha = %f , beta = %f , gamma = %f  SUM = %f\n",allArea, alpha,beta,gamma,(alpha+beta+gamma));
      //  printf("Ray.dirext %f , %f , %f \n",ray.direction.x, ray.direction.y, ray.direction.z);
        ret.normal = normal;
        ret.t = planePoint.t;
        ret.materialIndex = this->matIndex;
        ret.isIntersect = true;
        return ret;

    }
    else {
        
        ret.isIntersect = false;
        ret.t = FLT_MAX;
        ret.normal = Vector3f();
        ret.materialIndex = -1;
        return ret;
    }

}

Mesh::Mesh()
{}


/* Constructor for mesh. You will implement this. */
Mesh::Mesh(int id, int matIndex, const vector<Triangle>& face, vector<int> *pIndice, vector<Vector3f> *pVertice)
    : Shape(id, matIndex)
{
	faces = face;
    pIndices = pIndice;
    vertices = pVertice;
}

/* Mesh-ray intersection routine. You will implement this. 
Note that ReturnVal structure should hold the information related to the intersection point, e.g., coordinate of that point, normal at that point etc. 
You should to declare the variables in ReturnVal structure you think you will need. It is in defs.h file. */
ReturnVal Mesh::intersect(const Ray & ray) const
{
    
    ReturnVal ret, dummy;
	ret.isIntersect = false;
    ret.t = FLT_MAX;
	ret.normal = Vector3f();
    ret.materialIndex = -1;

	int numberOfFaces; 
    numberOfFaces  = faces.size();
    for( int i = 0 ; i<numberOfFaces ; i++){
        Triangle tri = faces.at(i);
        dummy = tri.intersect(ray);
        if(dummy.t < ret.t && dummy.t >0){
			//printf("Mesh intersect \n");
			ret = dummy;
           // printf("meshten cıkıs alınıyor %d  mat = %d\n", ret.isIntersect, ret.materialIndex);
            ret.materialIndex = this->matIndex;
            //return ret;
		}
    }
    return ret;
}
