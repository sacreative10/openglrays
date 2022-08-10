#include "arena.h"
#include <string>


namespace Arena
{
    std::vector<Object> objects;
    std::vector<Light> lights;
    Material planeMaterial;

	glm::vec3 cameraPosition(0, 1, 2);
	float cameraYaw = 0.0f, cameraPitch = 0.0f;

    Object::Object(const std::initializer_list<float>& vertices, float radius, const Material& material)
    {
        for (int i = 0; i < 3; i++)
            this->vertices[i] = vertices.begin()[i];
        this->radius = radius;
        this->material = material;
    }
    Object::Object() = default;

    Light::Light(const std::initializer_list<float>& position, const std::initializer_list<float>& color, float radius, float intensity, float reach)
    {
        for (int i = 0; i < 3; i++)
            this->position[i] = position.begin()[i];
        for (int i = 0; i < 3; i++)
            this->color[i] = color.begin()[i];
        this->radius = radius;
        this->intensity = intensity;
        this->reach = reach;
    }
    Light::Light() = default;

    Material::Material(const std::initializer_list<float>& color)
    {
        for (int i = 0; i < 3; i++)
            this->albedo[i] = color.begin()[i];
        for (int i = 0; i < 3; i++)
            this->specular[i] = color.begin()[i];
        for (int i = 0; i < 3; i++)
            this->emission[i] = color.begin()[i];
        this->roughness = 0.5f;
        this->metallic = 0.5f;
    }
    Material::Material(const std::initializer_list<float>& color, const std::initializer_list<float>& specular, const std::initializer_list<float>& emission, float roughness, float metallic)
    {
        for (int i = 0; i < 3; i++)
            this->albedo[i] = color.begin()[i];
        for (int i = 0; i < 3; i++)
            this->specular[i] = specular.begin()[i];
        for (int i = 0; i < 3; i++)
            this->emission[i] = emission.begin()[i];
        this->roughness = roughness;
        this->metallic = metallic;
    }
    Material::Material() = default;

    void bindObject(GLuint shader, int index)
    {
        std::string s_index = std::to_string(index);
        glUniform3f(glGetUniformLocation(shader, ("objects[" + s_index + "].position").c_str()), objects[index].vertices[0], objects[index].vertices[1], objects[index].vertices[2]);
        glUniform1f(glGetUniformLocation(shader, ("objects[" + s_index + "].radius").c_str()), objects[index].radius);
        glUniform3f(glGetUniformLocation(shader, ("objects[" + s_index + "].material.albedo").c_str()), objects[index].material.albedo[0], objects[index].material.albedo[1], objects[index].material.albedo[2]);
        glUniform3f(glGetUniformLocation(shader, ("objects[" + s_index + "].material.specular").c_str()), objects[index].material.specular[0], objects[index].material.specular[1], objects[index].material.specular[2]);
        glUniform3f(glGetUniformLocation(shader, ("objects[" + s_index + "].material.emission").c_str()), objects[index].material.emission[0], objects[index].material.emission[1], objects[index].material.emission[2]);
        glUniform1f(glGetUniformLocation(shader, ("objects[" + s_index + "].material.roughness").c_str()), objects[index].material.roughness);
        glUniform1f(glGetUniformLocation(shader, ("objects[" + s_index + "].material.metallic").c_str()), objects[index].material.metallic);

    }

    void bindLight(GLuint shader, int index)
    {
        std::string s_index = std::to_string(index);
        glUniform3f(glGetUniformLocation(shader, ("lights[" + s_index + "].position").c_str()), lights[index].position[0], lights[index].position[1], lights[index].position[2]);
        glUniform3f(glGetUniformLocation(shader, ("lights[" + s_index + "].color").c_str()), lights[index].color[0], lights[index].color[1], lights[index].color[2]);
        glUniform1f(glGetUniformLocation(shader, ("lights[" + s_index + "].radius").c_str()), lights[index].radius);
        glUniform1f(glGetUniformLocation(shader, ("lights[" + s_index + "].intensity").c_str()), lights[index].intensity);
        glUniform1f(glGetUniformLocation(shader, ("lights[" + s_index + "].reach").c_str()), lights[index].reach);
    }
    void bindAll(GLuint shader)
    {
        for (int i = 0; i < objects.size(); i++)
        {
            bindObject(shader, i);
        }
        for (int i = 0; i < lights.size(); i++)
        {
            bindLight(shader, i);
        }
        // bind the plane material
        glUniform3f(glGetUniformLocation(shader, "planeMaterial.albedo"), planeMaterial.albedo[0], planeMaterial.albedo[1], planeMaterial.albedo[2]);
        glUniform3f(glGetUniformLocation(shader, "planeMaterial.specular"), planeMaterial.specular[0], planeMaterial.specular[1], planeMaterial.specular[2]);
        glUniform3f(glGetUniformLocation(shader, "planeMaterial.emission"), planeMaterial.emission[0], planeMaterial.emission[1], planeMaterial.emission[2]);
        glUniform1f(glGetUniformLocation(shader, "planeMaterial.roughness"), planeMaterial.roughness);
        glUniform1f(glGetUniformLocation(shader, "planeMaterial.metallic"), planeMaterial.metallic);
    }


};