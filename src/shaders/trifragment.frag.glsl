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

uniform Object objects[MAXOBJECTS];
uniform Light lights[MAXLIGHTS];
uniform Material planeMaterial;
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
	float viewPortHeight = 2.0;
	float viewPortWidth = viewPortHeight * 800.0 / 600.0;

	vec3 origin = vec3(0.0, 1.0, 1.0);
	vec3 horizontal = vec3(viewPortWidth, 0.0, 0.0);
	vec3 vertical = vec3(0.0, viewPortHeight, 0.0);
	vec3 lowerLeft = origin - horizontal / 2.0 - vertical / 2.0 - vec3(0.0, 0.0, 1.0);

	vec2 uv = gl_FragCoord.xy / vec2(800.0, 600.0);
	vec3 rayDirection = normalize(lowerLeft + uv.x * horizontal + uv.y * vertical - origin);
	vec3 color = computeScreenColor(Ray(origin, rayDirection));

	// vec3 cameraPosition = vec3(0.0, 1.0, 1.0);
	// vec3 cameraDirection = normalize(vec3(0.0, 0.0, -1.0));
	// vec3 cameraRight = normalize(cross(cameraDirection, vec3(0.0, 1.0, 0.0)));
	// vec3 cameraUp = normalize(cross(cameraRight, cameraDirection));
	// vec2 uv = gl_FragCoord.xy / vec2(800.0, 600.0);
	// vec3 rayDirection = normalize(cameraPosition + uv.x * cameraRight + uv.y * cameraUp - cameraPosition);
	// vec3 color = computeScreenColor(Ray(cameraPosition, rayDirection));
	FragColor = vec4(color, 1.0);
}
