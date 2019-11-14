#include "Scene.h"
#include "Camera.h"
#include "Light.h"
#include "Material.h"
#include "Shape.h"
#include "tinyxml2.h"
#include "Image.h"

using namespace tinyxml2;

/* 
 * Must render the scene from each camera's viewpoint and create an image.
 * You can use the methods of the Image class to save the image as a PPM file. 
 */
void Scene::renderScene(void)
{
	int numberOfCameras = this->cameras.size();
	Ray dummy;
	Vector3f color;
	for(int i= 0 ; i<numberOfCameras; i++){
		Camera camnow = *this->cameras.at(i);
		int height = camnow.imgPlane.ny;
		int width = camnow.imgPlane.nx;
		Image img = Image(width, height);
		for(int h = 0 ; h< height; h++){
			for(int w = 0; w< width; w++){
				dummy = camnow.getPrimaryRay(w,h);
				Vector3f clr = this->intersectAvcisi(dummy,camnow,0);
				Color color; 
				color.red = clr.x;
				color.grn = clr.y;
				color.blu = clr.z;
//printf(" --  %f , %f , %f  --\n ", color.grn , color.y , color.z );
				img.setPixelValue(w,h,color);
			}
		}
		img.saveImage(camnow.imageName);
	}

}

Vector3f Scene::intersectAvcisi(Ray& ray, Camera camnow, int recursion){
	Vector3f ret;
	ReturnVal shadowIntersection;
	Vector3f diffuseRadiance, specularRadiance, ambientRadiance, intersectPoint;
	Vector3f  ambientCoef, wo , normalOfIntersect ;
	
	ret.x = backgroundColor.x;
	ret.y = backgroundColor.y;
	ret.z = backgroundColor.z;
	
	int numberOfObjects = this->objects.size();
	int numberOflights = this->lights.size();
	ReturnVal intersected, dummy;
	intersected.isIntersect = false;
    intersected.t = FLT_MAX;
	intersected.normal = Vector3f();
    intersected.materialIndex = -1;


	for( int i = 0 ; i< numberOfObjects ; i++){
		Shape* shapenow = objects.at(i);
		dummy = shapenow->intersect(ray);
		if(dummy.t < intersected.t && dummy.t >0){
			//printf("çakıştı \n");
			intersected = dummy;
		}
	}

	if(intersected.isIntersect == true){
		// ret.x = 255;
		// ret.y = 255;
		// ret.z = 255;
		// return ret;

		//Start colorizing
		ret = coloring(ray, recursion, intersected, camnow);

		if(ret.x>255) ret.x = 255;
		if(ret.y >255) ret.y = 255;
		if(ret.z > 255) ret.z = 255;
		//printf(" --  %f , %f , %f  --\n ", ret.x , ret.y , ret.z );
		return ret;
		}
	
	return ret;
}

Vector3f Scene::coloring(Ray ray, int recursion, ReturnVal intersected, Camera camnow){
	Vector3f ret, diffuseR, specularR, ambientR;

	ret.x =0; // backgroundColor.x;
	ret.y = 0; // backgroundColor.y;
	ret.z = 0; //backgroundColor.z;

	diffuseR = vectorMult(0,ret);
	specularR = vectorMult(0,ret);
	ambientR = vectorMult(0,ret);

	Vector3f pointIntersect = ray.getPoint(intersected.t);
	Vector3f ambientCoef = materials.at(intersected.materialIndex -1)->ambientRef;
	ambientR.x = ambientLight.x * ambientCoef.x;
	ambientR.y = ambientLight.y * ambientCoef.y;
	ambientR.z = ambientLight.z * ambientCoef.z;

	ret = vectorSum(ret, ambientR);

	///ambient done

	Vector3f wo = vectorSubs(ray.origin, pointIntersect);
	wo = vectorDiv(wo, distance(wo));
	// normalized version of intersect normal
	Vector3f intersectionNormal =vectorDiv(intersected.normal , distance(intersected.normal));

	int numberOfLights = lights.size();

	for(int l = 0; l<numberOfLights; l++){
		bool dummy = false; 
		Vector3f ligthPos = lights.at(l)->position;
		//Vector3f lightIntensity = lights.at(l)->computeLightContribution(ligthPos);
		
		Vector3f lightIntensity = lights.at(l)->computeLightContribution(pointIntersect);
		Vector3f wi = vectorSubs(ligthPos,pointIntersect);

		Vector3f ShadowPoint = vectorSum(pointIntersect, vectorMult(shadowRayEps,wi));
		Ray shadowRay = Ray(ShadowPoint,wi);

		int numberOfObjects = objects.size();

		for(int i = 0 ; i < numberOfObjects && dummy != true ; i++){
			Shape* shapenow = objects.at(i);
			ReturnVal temp = shapenow->intersect(shadowRay);
			if(temp.t <= 1 && temp.t >= 0){
				dummy = true;
				//break;	
			}
		}
		if(dummy == true) continue;
		

		wi = vectorDiv(wi, distance(wi));
		//diffuse time
		float cosThetaDiff = findCosinus(wi,intersected.normal);
		if(cosThetaDiff <0 ) cosThetaDiff = 0;
		Vector3f diffuseCoef = materials.at(intersected.materialIndex -1)->diffuseRef;
		float distanceX = findDistance(ligthPos,pointIntersect);
		//printf("Costheta = %f \n", cosThetaDiff);
		if(cosThetaDiff > 0 ){
			//printf("Diffuse color var %f \n", cosThetaDiff);
			diffuseR.x += (lightIntensity.x * diffuseCoef.x ) * (cosThetaDiff);
			diffuseR.y +=(lightIntensity.y * diffuseCoef.y ) * (cosThetaDiff);
			diffuseR.z +=(lightIntensity.z * diffuseCoef.z ) * (cosThetaDiff);
		//	printf(" --  %f , %f , %f  --\n ", lightIntensity.x , diffuseCoef.x , distanceX );
		//	printf("Diff = %f , %f , % f\n" ,diffuseR.x , diffuseR.y , diffuseR.z);
		}

		//specular time

		Vector3f wiwo= vectorSum(wi,wo);
		Vector3f h =  vectorDiv(wiwo,distance(wiwo));
		float cosThetaSpec  = findCosinus(h,intersected.normal);
		if(cosThetaSpec < 0 ) cosThetaSpec = 0;
		Vector3f SpecularCoef =  materials.at(intersected.materialIndex -1)->specularRef;
		float phong = materials.at(intersected.materialIndex -1)->phongExp;
		if(cosThetaSpec > 0 ){
			specularR.x += (lightIntensity.x * SpecularCoef.x ) * ( pow (cosThetaSpec,phong));
			specularR.y +=(lightIntensity.y * SpecularCoef.y ) * ( pow (cosThetaSpec,phong));
			specularR.z +=(lightIntensity.z * SpecularCoef.z ) * ( pow (cosThetaSpec,phong));		
		}	

	}

	ret = vectorSum (ret,diffuseR);
	ret = vectorSum(ret,specularR );


	Vector3f mirror =  materials.at(intersected.materialIndex -1)->mirrorRef;
	
//	printf("MİRRORRRR %f  %d ,, %d\n", mirror.x, recursion, maxRecursionDepth);

	if(recursion == maxRecursionDepth) return ret;


	float ifmirror = distance(mirror);

	if(ifmirror){
	//	printf("mirror var \n");
		Vector3f reflectDir = vectorMult(2 , intersectionNormal);
		reflectDir = vectorMult( findCosinus(wo,intersectionNormal) , reflectDir);
		reflectDir = vectorSubs(reflectDir, wo);
		reflectDir = vectorDiv(reflectDir, distance(reflectDir));
		Ray reflectRay = Ray(vectorSum( pointIntersect, vectorMult(shadowRayEps, reflectDir)),reflectDir);
		Vector3f donupgelen = intersectAvcisi(reflectRay,camnow, recursion +1); //coloring(reflectRay,recursion+1,intersected,camnow);
		donupgelen.x = donupgelen.x * mirror.x;
		donupgelen.y = donupgelen.y * mirror.y;
		donupgelen.z = donupgelen.z * mirror.z;
		ret = vectorSum(ret, donupgelen);
	//	printf("Color values x = %f , y = %f , z = %f \n ", donupgelen.x,donupgelen.y,donupgelen.z);
	}
	//printf("Color values x = %f , y = %f , z = %f \n ", ret.x,ret.y,ret.z);
	return ret;

}


// Parses XML file. 
Scene::Scene(const char *xmlPath)
{
	const char *str;
	XMLDocument xmlDoc;
	XMLError eResult;
	XMLElement *pElement;

	maxRecursionDepth = 1;
	shadowRayEps = 0.001;

	eResult = xmlDoc.LoadFile(xmlPath);

	XMLNode *pRoot = xmlDoc.FirstChild();

	pElement = pRoot->FirstChildElement("MaxRecursionDepth");
	if(pElement != nullptr)
		pElement->QueryIntText(&maxRecursionDepth);

	pElement = pRoot->FirstChildElement("BackgroundColor");
	str = pElement->GetText();
	sscanf(str, "%f %f %f", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);

	pElement = pRoot->FirstChildElement("ShadowRayEpsilon");
	if(pElement != nullptr)
		pElement->QueryFloatText(&shadowRayEps);

	pElement = pRoot->FirstChildElement("IntersectionTestEpsilon");
	if(pElement != nullptr)
		eResult = pElement->QueryFloatText(&intTestEps);

	// Parse cameras
	pElement = pRoot->FirstChildElement("Cameras");
	XMLElement *pCamera = pElement->FirstChildElement("Camera");
	XMLElement *camElement;
	while(pCamera != nullptr)
	{
        int id;
        char imageName[64];
        Vector3f pos, gaze, up;
        ImagePlane imgPlane;

		eResult = pCamera->QueryIntAttribute("id", &id);
		camElement = pCamera->FirstChildElement("Position");
		str = camElement->GetText();
		sscanf(str, "%f %f %f", &pos.x, &pos.y, &pos.z);
		camElement = pCamera->FirstChildElement("Gaze");
		str = camElement->GetText();
		sscanf(str, "%f %f %f", &gaze.x, &gaze.y, &gaze.z);
		camElement = pCamera->FirstChildElement("Up");
		str = camElement->GetText();
		sscanf(str, "%f %f %f", &up.x, &up.y, &up.z);
		camElement = pCamera->FirstChildElement("NearPlane");
		str = camElement->GetText();
		sscanf(str, "%f %f %f %f", &imgPlane.left, &imgPlane.right, &imgPlane.bottom, &imgPlane.top);
		camElement = pCamera->FirstChildElement("NearDistance");
		eResult = camElement->QueryFloatText(&imgPlane.distance);
		camElement = pCamera->FirstChildElement("ImageResolution");	
		str = camElement->GetText();
		sscanf(str, "%d %d", &imgPlane.nx, &imgPlane.ny);
		camElement = pCamera->FirstChildElement("ImageName");
		str = camElement->GetText();
		strcpy(imageName, str);

		cameras.push_back(new Camera(id, imageName, pos, gaze, up, imgPlane));

		pCamera = pCamera->NextSiblingElement("Camera");
	}

	// Parse materals
	pElement = pRoot->FirstChildElement("Materials");
	XMLElement *pMaterial = pElement->FirstChildElement("Material");
	XMLElement *materialElement;
	while(pMaterial != nullptr)
	{
		materials.push_back(new Material());

		int curr = materials.size() - 1;
	
		eResult = pMaterial->QueryIntAttribute("id", &materials[curr]->id);
		materialElement = pMaterial->FirstChildElement("AmbientReflectance");
		str = materialElement->GetText();
		sscanf(str, "%f %f %f", &materials[curr]->ambientRef.r, &materials[curr]->ambientRef.g, &materials[curr]->ambientRef.b);
		materialElement = pMaterial->FirstChildElement("DiffuseReflectance");
		str = materialElement->GetText();
		sscanf(str, "%f %f %f", &materials[curr]->diffuseRef.r, &materials[curr]->diffuseRef.g, &materials[curr]->diffuseRef.b);
		materialElement = pMaterial->FirstChildElement("SpecularReflectance");
		str = materialElement->GetText();
		sscanf(str, "%f %f %f", &materials[curr]->specularRef.r, &materials[curr]->specularRef.g, &materials[curr]->specularRef.b);
		materialElement = pMaterial->FirstChildElement("MirrorReflectance");
		if(materialElement != nullptr)
		{
			str = materialElement->GetText();
			sscanf(str, "%f %f %f", &materials[curr]->mirrorRef.r, &materials[curr]->mirrorRef.g, &materials[curr]->mirrorRef.b);
		}
				else
		{
			materials[curr]->mirrorRef.r = 0.0;
			materials[curr]->mirrorRef.g = 0.0;
			materials[curr]->mirrorRef.b = 0.0;
		}
		materialElement = pMaterial->FirstChildElement("PhongExponent");
		if(materialElement != nullptr)
			materialElement->QueryIntText(&materials[curr]->phongExp);

		pMaterial = pMaterial->NextSiblingElement("Material");
	}

	// Parse vertex data
	pElement = pRoot->FirstChildElement("VertexData");
	int cursor = 0;
	Vector3f tmpPoint;
	str = pElement->GetText();
	while(str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
		cursor++;
	while(str[cursor] != '\0')
	{
		for(int cnt = 0 ; cnt < 3 ; cnt++)
		{
			if(cnt == 0)
				tmpPoint.x = atof(str + cursor);
			else if(cnt == 1)
				tmpPoint.y = atof(str + cursor);
			else
				tmpPoint.z = atof(str + cursor);
			while(str[cursor] != ' ' && str[cursor] != '\t' && str[cursor] != '\n')
				cursor++; 
			while(str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
				cursor++;
		}
		vertices.push_back(tmpPoint);
	}

	// Parse objects
	pElement = pRoot->FirstChildElement("Objects");
	
	// Parse spheres
	XMLElement *pObject = pElement->FirstChildElement("Sphere");
	XMLElement *objElement;
	while(pObject != nullptr)
	{
		int id;
		int matIndex;
		int cIndex;
		float R;

		eResult = pObject->QueryIntAttribute("id", &id);
		objElement = pObject->FirstChildElement("Material");
		eResult = objElement->QueryIntText(&matIndex);
		objElement = pObject->FirstChildElement("Center");
		eResult = objElement->QueryIntText(&cIndex);
		objElement = pObject->FirstChildElement("Radius");
		eResult = objElement->QueryFloatText(&R);

		objects.push_back(new Sphere(id, matIndex, cIndex, R, &vertices));

		pObject = pObject->NextSiblingElement("Sphere");
	}

	// Parse triangles
	pObject = pElement->FirstChildElement("Triangle");
	while(pObject != nullptr)
	{
		int id;
		int matIndex;
		int p1Index;
		int p2Index;
		int p3Index;

		eResult = pObject->QueryIntAttribute("id", &id);
		objElement = pObject->FirstChildElement("Material");
		eResult = objElement->QueryIntText(&matIndex);
		objElement = pObject->FirstChildElement("Indices");
		str = objElement->GetText();
		sscanf(str, "%d %d %d", &p1Index, &p2Index, &p3Index);

		objects.push_back(new Triangle(id, matIndex, p1Index, p2Index, p3Index, &vertices));

		pObject = pObject->NextSiblingElement("Triangle");
	}

	// Parse meshes
	pObject = pElement->FirstChildElement("Mesh");
	while(pObject != nullptr)
	{
		int id;
		int matIndex;
		int p1Index;
		int p2Index;
		int p3Index;
		int cursor = 0;
		int vertexOffset = 0;
		vector<Triangle> faces;
		vector<int> *meshIndices = new vector<int>;

		eResult = pObject->QueryIntAttribute("id", &id);
		objElement = pObject->FirstChildElement("Material");
		eResult = objElement->QueryIntText(&matIndex);
		objElement = pObject->FirstChildElement("Faces");
		objElement->QueryIntAttribute("vertexOffset", &vertexOffset);
		str = objElement->GetText();
		while(str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
			cursor++;
		while(str[cursor] != '\0')
		{
			for(int cnt = 0 ; cnt < 3 ; cnt++)
			{
				if(cnt == 0)
					p1Index = atoi(str + cursor) + vertexOffset;
				else if(cnt == 1)
					p2Index = atoi(str + cursor) + vertexOffset;
				else
					p3Index = atoi(str + cursor) + vertexOffset;
				while(str[cursor] != ' ' && str[cursor] != '\t' && str[cursor] != '\n')
					cursor++; 
				while(str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
					cursor++;
			}
			faces.push_back(*(new Triangle(-1, matIndex, p1Index, p2Index, p3Index, &vertices)));
			meshIndices->push_back(p1Index);
			meshIndices->push_back(p2Index);
			meshIndices->push_back(p3Index);
		}

		objects.push_back(new Mesh(id, matIndex, faces, meshIndices, &vertices));

		pObject = pObject->NextSiblingElement("Mesh");
	}

	// Parse lights
	int id;
	Vector3f position;
	Vector3f intensity;
	pElement = pRoot->FirstChildElement("Lights");

	XMLElement *pLight = pElement->FirstChildElement("AmbientLight");
	XMLElement *lightElement;
	str = pLight->GetText();
	sscanf(str, "%f %f %f", &ambientLight.r, &ambientLight.g, &ambientLight.b);

	pLight = pElement->FirstChildElement("PointLight");
	while(pLight != nullptr)
	{
		eResult = pLight->QueryIntAttribute("id", &id);
		lightElement = pLight->FirstChildElement("Position");
		str = lightElement->GetText();
		sscanf(str, "%f %f %f", &position.x, &position.y, &position.z);
		lightElement = pLight->FirstChildElement("Intensity");
		str = lightElement->GetText();
		sscanf(str, "%f %f %f", &intensity.r, &intensity.g, &intensity.b);

		lights.push_back(new PointLight(position, intensity));

		pLight = pLight->NextSiblingElement("PointLight");
	}
}
