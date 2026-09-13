#pragma once

// Include after the native OpenGL and ZzzLodTerrain declarations.
// No S21 pointers/layouts and no duplicate terrain geometry or bitmap loader.
namespace rise { namespace growlancer {
namespace detail {
class ScopedTerrainFlipV
{
public:
    ScopedTerrainFlipV()
    {
        glGetIntegerv(GL_MATRIX_MODE, &m_Mode);
        glGetFloatv(GL_TEXTURE_MATRIX, m_TextureMatrix);
        glMatrixMode(GL_TEXTURE);
        // Postmultiply: existingMatrix * (u, 1-v). This matches changing
        // submitted UVs, even when the caller's texture matrix is nonidentity.
        glTranslatef(0.f, 1.f, 0.f);
        glScalef(1.f, -1.f, 1.f);
        glMatrixMode(m_Mode);
    }

    ~ScopedTerrainFlipV()
    {
        glMatrixMode(GL_TEXTURE);
        glLoadMatrixf(m_TextureMatrix);
        glMatrixMode(m_Mode);
    }

private:
    ScopedTerrainFlipV(const ScopedTerrainFlipV&);
    ScopedTerrainFlipV& operator=(const ScopedTerrainFlipV&);
    GLint m_Mode;
    GLfloat m_TextureMatrix[16];
};
}

// Scoped opt-in only. Terrain eligibility/world mapping remains a separate
// compatibility requirement; this adapter does not claim whole-skill parity.
inline void RenderGrowLancerTerrainBitmap(int texture, float x, float y,
    float sizeX, float sizeY, vec3_t light, float rotation, float alpha,
    float height, bool flipV)
{
    if (!flipV)
    {
        RenderTerrainAlphaBitmap(texture, x, y, sizeX, sizeY, light,
            rotation, alpha, height);
        return;
    }
    detail::ScopedTerrainFlipV scope;
    RenderTerrainAlphaBitmap(texture, x, y, sizeX, sizeY, light,
        rotation, alpha, height);
}
} }
