#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <string>
#include <vector>

class Shader{
public:
    Shader(const std::string& vert_path, const std::string& frag_path);
    ~Shader();

    void Bind();

    void setUniform1f(const std::string& name, float x);
    void setUniformMatrix4fv(const std::string& name, glm::mat4 mat);
private:
    unsigned int m_ID = 0;

    unsigned int getUniformLocation(const std::string& name);
    std::vector<std::pair<std::string, unsigned int>> m_UniformCache;
};
