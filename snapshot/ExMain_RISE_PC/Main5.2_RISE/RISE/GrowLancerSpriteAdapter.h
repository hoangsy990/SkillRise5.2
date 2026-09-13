#pragma once

// Include after native GL, BindTexture, RenderSprite and SpriteBatch330.
namespace rise { namespace growlancer {
namespace detail {
inline void FlushBrecheSpriteBatch()
{
#ifdef jdk_shader_local330
    OGL330SPRITE::FlushForStateChange(true);
#endif
}

class ScopedBrecheClamp
{
public:
    ScopedBrecheClamp()
    {
        FlushBrecheSpriteBatch();
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &m_S);
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &m_T);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }
    ~ScopedBrecheClamp()
    {
        // Consume the queued Breche quad before restoring shared texture state.
        FlushBrecheSpriteBatch();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_S);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_T);
    }
private:
    GLint m_S, m_T;
    ScopedBrecheClamp(const ScopedBrecheClamp&);
    ScopedBrecheClamp& operator=(const ScopedBrecheClamp&);
};
}

inline void RenderBrecheFireSprite(int texture, float* position, float width,
    float height, float* light, float rotation)
{
    BindTexture(texture);
    detail::ScopedBrecheClamp scope;
    // Native renderer binds the same texture, so the scope stays on that object.
    RenderSprite(texture, position, width, height, light, rotation);
}
} }
