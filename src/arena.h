#include <initializer_list>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Arena
{
    struct Material
    {
        float albedo[3];
        float specular[3];
        float emission[3];
        float roughness;
        float metallic;
        Material(const std::initializer_list<float>& color);
        Material(const std::initializer_list<float>& color, const std::initializer_list<float>& specular, const std::initializer_list<float>& emission, float roughness, float metallic);
        Material();
    };
    struct Object
    {
        float vertices[3];
        float radius;
        Material material;

        Object(const std::initializer_list<float>& vertices, float radius, const Material& material);
        Object();
    };
    struct Light
    {
        float position[3];
        float color[3];
        float radius;
        float intensity;
        float reach;
        Light(const std::initializer_list<float>& position, const std::initializer_list<float>& color, float radius, float intensity, float reach);
        Light();
    };

    extern std::vector<Object> objects;
    extern std::vector<Light> lights;
    extern Material planeMaterial;
    
    extern glm::vec3 cameraPosition;
    extern float cameraYaw, cameraPitch;


    void bindObject(GLuint shader, int index);
    void bindLight(GLuint shader, int index);
    void bindAll(GLuint shader);
};
