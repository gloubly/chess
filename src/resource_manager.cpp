#include "resource_manager.hpp"

// define static members
std::map<std::string, Shader> ResourceManager::shaders;
std::map<std::string, Texture> ResourceManager::single_textures;
std::map<std::string, std::vector<Texture>> ResourceManager::array_textures;

ResourceManager::ResourceManager() {}

Shader ResourceManager::loadShader(const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path, const std::string& name) {
    shaders[name] = Shader(vertex_path, fragment_path, geometry_path);
    return shaders[name];
}

Shader ResourceManager::getShader(const std::string& name) {
    if (!shaders.count(name)) {
        throw std::runtime_error("SHADER NOT FOUND: " + name);
    }
    return shaders[name];
}

Texture ResourceManager::loadSingleTexture(const std::string& path, const std::string& name) {
    auto texture = Texture();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
           
    int width, height, n_channels;

    std::unique_ptr<unsigned char, void(*)(void*)> data(stbi_load(path.c_str(), &width, &height, &n_channels, 0), stbi_image_free);
    GLenum format;
    if (data.get()) {
        switch (n_channels) {
            case 1:
                format = GL_RED;
                break;
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
            default:
                std::cerr << "ERROR IMAGE (" << path << ") FORMAT NOT SUPPORTED: " << n_channels << std::endl;
                throw std::runtime_error("");
                break;
        }
    }
    else {
        std::cerr << "ERROR LOADING IMAGE: " << path << std::endl;
        throw std::runtime_error("");
    }

    texture.generate(width, height, format, data.get());

    single_textures[name] = texture;
    return texture;
}

std::vector<Texture> ResourceManager::loadMultipleTextures(const std::string& path, size_t n_cols, size_t n_rows, const std::string& name) {
    auto textures = std::vector<Texture>(n_rows * n_cols);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    int width, height, n_channels;

    std::unique_ptr<unsigned char, void(*)(void*)> data(stbi_load(path.c_str(), &width, &height, &n_channels, 0), stbi_image_free);
        
    unsigned int sub_width = width / n_cols;
    unsigned int sub_height = height / n_rows;

    GLenum format = GL_RED;
    if (data.get()) {
        switch (n_channels) {
            case 1:
                format = GL_RED;
                break;
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
            default:
                std::cerr << "ERROR IMAGE (" << path << ") FORMAT NOT SUPPORTED: " << n_channels << std::endl;
                throw std::runtime_error("");
                break;
        }
    }
    else {
        std::cerr << "ERROR LOADING IMAGE: " << path << std::endl;
        throw std::runtime_error("");
    }

    std::vector<unsigned char> sub_image(sub_width * sub_height * n_channels);
    for (size_t row = 0; row < n_rows; ++row) {
        size_t y_offset = row * sub_height * width * n_channels; // offset for the next sub image row
        for (size_t col = 0; col < n_cols; ++col) {

            size_t x_offset = col * sub_width * n_channels; // offset for the next sub image col
                
            for (size_t y = 0; y < sub_height; ++y) {
                unsigned char* src = data.get() + (y * width * n_channels + x_offset + y_offset);
                unsigned char* dst = &sub_image[y * sub_width * n_channels];
                if (x_offset + sub_width * n_channels > width * n_channels) {
                    std::string msg = "Out of bounds";
                    std::cerr << msg << std::endl;
                    throw std::runtime_error(msg);
                }
                std::memcpy(dst, src, sub_width * n_channels);
            }

            textures[row * n_cols + col].generate(sub_width, sub_height, format, sub_image.data());
        }
    }
    array_textures[name] = textures;
    return textures;
}

Texture ResourceManager::getTexture(const std::string& name) {
    if (!single_textures.count(name)) {
        std::cerr << "TEXTURE NOT FOUND: " + name << std::endl;
        throw std::runtime_error("");
    }
    return single_textures[name];
}

std::vector<Texture> ResourceManager::getTextureArray(const std::string& name) {
    if (!array_textures.count(name)) {
        std::cerr << "TEXTURE ARRAY NOT FOUND: " << name << std::endl;
        throw std::runtime_error("");
    }
    return array_textures[name];
}