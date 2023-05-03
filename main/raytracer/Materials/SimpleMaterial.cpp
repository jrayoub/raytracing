#include "headers/SimpleMaterial.hpp"
#include <cmath>

RT::SimpleMaterial::SimpleMaterial()
{}

RT::SimpleMaterial::~SimpleMaterial()
{}

// function to compute illumination cintribition
qbVector<double> RT::SimpleMaterial::ComputeColor(const std::vector<std::shared_ptr<RT::ObjectBase>> &objectList,
		const std::vector<std::shared_ptr<RT::LightBase>> &lightList,
		const std::shared_ptr<RT::ObjectBase> &currentObject,
		const qbVector<double> &intPoint, const qbVector<double> &localNormal,
		const RT::Ray &cameraRay)
{
	//Define an initial Material
	qbVector<double> mtColor {3};
	qbVector<double> refColor {3};
	qbVector<double> difColor {3};
	qbVector<double> specColor {3};

	// clc the diffuse colors
	difColor = ComputeDiffuseColoe(objectList, lightList, currentObject, intPoint, localNormal, m_baseColor);

	//calc the reflection Component
	if(m_reflectivity > 0.0)
	{
			refColor = ComputeReflectionColor(objectList, lightList, currentObject,
					intPoint, localNormal, cameraRay);
	}

	//combain reflection and diffuse components
	mtColor = (refColor * m_reflectivity) + (difColor * ( 1 - m_reflectivity));

	// comput the specular compunent
	if(m_shininess > 0.0)
		specColor = ComputeSpectular(objectList, lightList, intPoint, localNormal, cameraRay);

	//Add spectular component to final color 
	mtColor = mtColor + specColor;
	return (mtColor);
}
/*

qbVector<double> RT::SimpleMaterial::ComputeSpectular(
		const std::vector<std::shared_ptr<RT::ObjectBase>> &objectList,
		const std::vector<std::shared_ptr<RT::LightBase>> &lightList,
		const qbVector<double> &intPoint, const qbVector<double> &localNormal,
		const RT::Ray &cameraRay)
{
	double red = 0, green = 0 , blue = 0;
	qbVector<double> specColor {3};

	for(auto curLight: lightList)
	{
		double SpectacularIntisity = 0.0;

		qbVector<double> lightDir = (curLight->m_location - intPoint).Normalized();

		// add small padding from start point and the surface 
		// to avoid the ray from inetersact with the same object again.
		qbVector<double> startPoint = intPoint + (lightDir * 0.001);

		// construct a ray from the  point of interstaction to the light.
		RT::Ray lightRay (startPoint, startPoint + lightDir);


		qbVector<double> poi {3};
		qbVector<double> poiNormal {3};
		qbVector<double> poiColor {3};
		bool validInt = false;

		//loop through all the objects in thee scene to check if and object abstract light in other words casting a shadow
		for(auto curObj : objectList)
		{
			validInt = curObj->TestIntersectioons(lightRay, poi, poiNormal, poiColor);
			if(validInt)
				break;
		}

		// if no interstaction with any object found then there is no shadows
		// and proceed with calculating the the I(s,v)

		if(!validInt)
		{

			qbVector<double> d = lightRay.m_lab;

			qbVector<double> r;

			r = d - (2 * qbVector<double>::dot(d, localNormal) * localNormal);
			r.Normalized();

			qbVector<double> v = cameraRay.m_lab;
			v.Normalized();

			double dotProduct = qbVector<double>::dot(r, v);

			if(dotProduct > 0.0)
			{
				SpectacularIntisity = 0.5 * std::pow(dotProduct, m_shininess);
				printf("{RGB :: SpectacularIntisity %f}\n", SpectacularIntisity);
			}
		}
		red += curLight->m_color.GetElement(0) * SpectacularIntisity;
		green += curLight->m_color.GetElement(1) * SpectacularIntisity;
		blue += curLight->m_color.GetElement(2) * SpectacularIntisity;

	}
	specColor.SetElement(0, red);
	specColor.SetElement(1, green);
	specColor.SetElement(2, blue);
	return specColor;
}
*/

// Function to compute the specular highlights.
qbVector<double> RT::SimpleMaterial::ComputeSpectular(
		const std::vector<std::shared_ptr<RT::ObjectBase>> &objectList,
		const std::vector<std::shared_ptr<RT::LightBase>> &lightList,
		const qbVector<double> &intPoint, const qbVector<double> &localNormal,
		const RT::Ray &cameraRay)
{
	qbVector<double> spcColor	{3};
	double red = 0.0;
	double green = 0.0;
	double blue = 0.0;
	
	// Loop through all of the lights in the scene.
	for (auto currentLight : lightList)
	{
		/* Check for intersections with all objects in the scene. */
		double intensity = 0.0;
		
		// Construct a vector pointing from the intersection point to the light.
		qbVector<double> lightDir = (currentLight->m_location - intPoint).Normalized();
		
		// Compute a start point.
		qbVector<double> startPoint = intPoint + (lightDir * 0.001);
		
		// Construct a ray from the point of intersection to the light.
		RT::Ray lightRay (startPoint, startPoint + lightDir);
		
		/* Loop through all objects in the scene to check if any
			obstruct light from this source. */
		qbVector<double> poi				{3};
		qbVector<double> poiNormal	{3};
		qbVector<double> poiColor		{3};
		bool validInt = false;
		for (auto sceneObject : objectList)
		{
			validInt = sceneObject -> TestIntersectioons(lightRay, poi, poiNormal, poiColor);
			if (validInt)
				break;
		}
		
		/* If no intersections were found, then proceed with
			computing the specular component. */
		if (!validInt)
		{
			// Compute the reflection vector.
			qbVector<double> d = lightRay.m_lab;
			qbVector<double> r = d - (2 * qbVector<double>::dot(d, localNormal) * localNormal);
			r.Normalize();
			
			// Compute the dot product.
			qbVector<double> v = cameraRay.m_lab;
			v.Normalize();

			double dotProduct = qbVector<double>::dot(r, v);
			
			// Only proceed if the dot product is positive.
			if (dotProduct > 0.0)
			{
				printf("{RGB :: dotProduct %f}\n", dotProduct);
				intensity = m_reflectivity * std::pow(dotProduct, m_shininess);
				printf("{RGB :: intensity %f}\n", intensity);
			}
		}
		
		red += currentLight->m_color.GetElement(0) * intensity;
		green += currentLight->m_color.GetElement(1) * intensity;
		blue += currentLight->m_color.GetElement(2) * intensity;
	}
	
	spcColor.SetElement(0, red);
	spcColor.SetElement(1, green);
	spcColor.SetElement(2, blue);
	return spcColor;
}
