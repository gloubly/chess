#include "shader.hpp"
#include "texture.hpp"

#include <map>
#include <string>
#include <vector>
#include <stdexcept>

class ResourceManager {
public:
    ResourceManager();

    static Shader loadShader(const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path, const std::string& name);

    static Shader getShader(const std::string& name);

    static Texture loadSingleTexture(const std::string& path, const std::string& name);

    static std::vector<Texture> loadMultipleTextures(const std::string& path, size_t n_cols, size_t n_rows, const std::string& name);

    static Texture getTexture(const std::string& name);

    static std::vector<Texture> getTextureArray(const std::string& name);

    static std::map<std::string, Shader> shaders;
    static std::map<std::string, Texture> single_textures;
    static std::map<std::string, std::vector<Texture>> array_textures;
};