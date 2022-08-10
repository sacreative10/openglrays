#version 460 core

#define MAXOBJECTS 100
#define MAXLIGHTS 100



struct Ray {
	vec3 origin;
	vec3 direction;
};


struct Material
{
	vec3 albedo;
	vec3 specular;
	vec3 emission;
	float roughness;
	float metallic;
};


struct Object
{
	vec3 position;
	vec3 color;
	float radius;
	Material material;
};

struct Light
{
	vec3 position;
	vec3 color;
	float radius;
	float intensity;
	float reach;
};

struct SurfaceIntersection
{
	vec3 position;
	vec3 normal;
	Material material;
};

in vec2 fragUVs;

uniform Object objects[MAXOBJECTS];
uniform Light lights[MAXLIGHTS];
uniform Material planeMaterial;
uniform float aspectRatio;
uniform mat4 rotationMatrix;
uniform vec3 cameraPosition;
out vec4 FragColor;

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}


bool sphereIntersect(vec3 position, float radius, Ray ray, out float hitDist)
{
	float t = dot(position - ray.origin, ray.direction);
	vec3 p = ray.origin + ray.direction * t;

	float d = length(position - p);
	if (d < radius)
	{
		float x = sqrt(radius * radius - d * d);
		float t1 = t - x;
		if(t1 > 0.0)
		{
			hitDist = t1;
			return true;
		}
	}

	return false;
}

bool planeIntersect(vec3 planeNormal, vec3 planePosition, Ray ray, out float hitDist)
{
	float denom = dot(planeNormal, ray.direction);
	if (denom > 1e-6)
	{
		vec3 d = planePosition - ray.origin;
		hitDist = dot(d, planeNormal) / denom;
		return (hitDist > 0.0);
	}
	return false;
}

bool raycast(Ray ray, out SurfaceIntersection hitPoint)
{
	bool hit = false;
	float minHitDist = 1000000.0;

	float hitDist;
	for(int i = 0; i < objects.length(); i++)
	{
		if(sphereIntersect(objects[i].position, objects[i].radius, ray, hitDist))
		{
			hit = true;
			if(hitDist < minHitDist)
			{
				minHitDist = hitDist;
				hitPoint.position = ray.origin + ray.direction * minHitDist;
				hitPoint.normal = normalize(hitPoint.position - objects[i].position);
				hitPoint.material = objects[i].material;
			}
		}
	}
	if(planeIntersect(vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 0.0), ray, hitDist))
	{
		hit = true;
		if(hitDist < minHitDist)
		{
			minHitDist = hitDist;
			hitPoint.position = ray.origin + ray.direction * minHitDist;
			hitPoint.normal = vec3(0.0, 1.0, 0.0);
			hitPoint.material = planeMaterial; 
		}
	}

	return hit;
}

// check if place is in shadow, by seeing how much light is gained by the observer position looking at the hit point
vec3 DirectIllumination(SurfaceIntersection hitPoint, vec3 observerPosition)
{
	vec3 directIllumination = vec3(0.0, 0.0, 0.0);

	for(int lightIdx = 0; lightIdx < lights.length(); lightIdx++)
	{
		Light light = lights[lightIdx];

		float lightDist = length(light.position - hitPoint.position);
		if(lightDist > light.reach)
		{
			continue;
		}

		// diffuse
		float diffuse = clamp(dot(light.color, hitPoint.material.albedo), 0.0, 1.0);

		if(hitPoint.material.roughness < 1.0)
		{
			// shadow raycasting
			int numShadowRays = 10;
			int shadowHits = 0;
			for(int i = 0; i < numShadowRays; i++)
			{
				// sample a point on the light source
				vec3 lightPosition = light.position + light.radius * normalize(vec3(rand(vec2(i, 0)), rand(vec2(i, 1)), rand(vec2(i, 2))));
				vec3 lightDirection = normalize(lightPosition - hitPoint.position);
				vec3 rayOrigin = hitPoint.position + hitPoint.normal * 0.001;
				float maxRayDist = length(lightPosition - rayOrigin);
				Ray shadowRay = Ray(rayOrigin, lightDirection);
				SurfaceIntersection shadowHitPoint;
				if(raycast(shadowRay, shadowHitPoint))
				{
					if(length(shadowHitPoint.position - lightPosition) < light.radius)
					{
						shadowHits++;
					}
				}
			}

			float attenuation = 1.0 - shadowHits / float(numShadowRays);
			directIllumination += light.color * diffuse * attenuation;
		}
	}
	return directIllumination;
}


vec3 computeScreenColor(Ray ray)
{
	vec3 totalIllumination = vec3(0);
	SurfaceIntersection hitPoint;
	if(raycast(ray, hitPoint))
	{
		totalIllumination =  hitPoint.material.albedo;
	}
	return totalIllumination;
}






void main()
{
	vec2 centeredUV = (fragUVs * 2.0 - vec2(1)) * vec2(aspectRatio, 1);

	vec3 rayDir = (normalize(vec4(centeredUV, -1.0, 1.0)) * rotationMatrix).xyz;

	Ray cameraRay = Ray(cameraPosition, rayDir);

	vec3 screenColor = computeScreenColor(cameraRay);
	FragColor = vec4(screenColor, 1.0);
}
