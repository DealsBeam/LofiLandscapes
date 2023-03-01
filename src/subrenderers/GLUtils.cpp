#include "GLUtils.h"

#include "glad/glad.h"

#include <cstddef>

Quad::Quad() {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_VertexData), 
            &m_VertexData, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_IndexData), 
                    &m_IndexData, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

Quad::~Quad() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

void Quad::Draw() {
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

Texture::Texture() {}

Texture::~Texture() {}

void Init(unsigned int &id, TextureSpec spec) {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, spec.InternalFormat, 
                 spec.ResolutionX, spec.ResolutionY, 0, 
                 spec.Format, spec.Type, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, spec.MinFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, spec.MagFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, spec.Wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, spec.Wrap);

    if (spec.Wrap == GL_CLAMP_TO_BORDER)
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, spec.Border);  
}

void Init(unsigned int& id, Texture3dSpec spec) {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_3D, id);

    glTexImage3D(GL_TEXTURE_3D, 0, spec.InternalFormat,
        spec.ResolutionX, spec.ResolutionY, spec.ResolutionZ,
        0, spec.Format, spec.Type, NULL);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, spec.MinFilter);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, spec.MagFilter);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, spec.Wrap);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, spec.Wrap);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, spec.Wrap);

    if (spec.Wrap == GL_CLAMP_TO_BORDER)
        glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, spec.Border);
}

void Texture::Initialize(TextureSpec spec) {
    Init(m_Texture, spec);
    m_Spec = spec;
}

void Texture::Bind(int id) {
    glActiveTexture(GL_TEXTURE0 + id);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
}

void Texture::BindImage(int id, int mip) {
    int format = m_Spec.InternalFormat; 

    glBindImageTexture(id, m_Texture, mip, GL_FALSE, 0, GL_READ_WRITE, format);
}

void Texture::AttachToFramebuffer() {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, m_Texture, 0);
}

FramebufferTexture::FramebufferTexture() {}

FramebufferTexture::~FramebufferTexture() {
    glDeleteFramebuffers(1, &m_FBO);
}

void FramebufferTexture::Initialize(TextureSpec spec) {
    //Initialize FBO:
    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    //Initialize texture:
    Init(m_Texture, spec);

    //Attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, m_Texture, 0);

    m_Spec = spec;
}

void FramebufferTexture::BindFBO() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
}

void FramebufferTexture::BindTex(int id) {
    glActiveTexture(GL_TEXTURE0 + id);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
}

Texture3d::Texture3d() {}

Texture3d::~Texture3d() {}

void Texture3d::Initialize(Texture3dSpec spec) {
    Init(m_Texture, spec);
    m_Spec = spec;
}

void Texture3d::Bind(int id) {
    glActiveTexture(GL_TEXTURE0 + id);
    glBindTexture(GL_TEXTURE_3D, m_Texture);
}

void Texture3d::BindImage(int id, int mip) {
    int format = m_Spec.InternalFormat;

    glBindImageTexture(id, m_Texture, mip, GL_TRUE, 0, GL_READ_WRITE, format);
}

Cubemap::Cubemap() {}

Cubemap::~Cubemap() {}

void Cubemap::Initialize(CubemapSpec spec) {
    glGenTextures(1, &m_Texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_Texture);

    for (int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, spec.InternalFormat,
            spec.Resolution, spec.Resolution, 0,
            spec.Format, spec.Type, NULL);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, spec.MinFilter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, spec.MagFilter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    m_Spec = spec;
}

void Cubemap::Bind(int id) {
    glActiveTexture(GL_TEXTURE0 + id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_Texture);
}

void Cubemap::BindImage(int id, int mip) {
    int format = m_Spec.InternalFormat;

    glBindImageTexture(id, m_Texture, mip, GL_TRUE, 0, GL_READ_WRITE, format);
}