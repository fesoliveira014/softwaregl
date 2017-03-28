#ifndef RASTER_H
#define RASTER_H

#include "common.hpp"
#include "tga.hpp"
#include "model.hpp"
#include "renderer.hpp"


class Rasterizer : public IRenderer
{
public:
	Rasterizer(TGAImage &image);
	~Rasterizer();

	virtual void render(Model& model, TGAImage *image, bool wireframe = false);

	uint getWidth() { return m_width; }
	uint getHeight() { return m_height; }
	float * getZBuffer() { return m_zBuffer; }

protected:
	void line(const glm::ivec2 & aa, const glm::ivec2 & bb, TGAImage *image, TGAColor color);
	void getTriangleBbox2D(const glm::vec3 &a, const glm::vec3 &b,
                                const glm::vec3 &c, const glm::vec2 &clamp, bbox2d &bbox);
	glm::vec3 barycentric(const glm::vec3 &a, const glm::vec3 &b, 
                                const glm::vec3 &c, const glm::vec3 &p);
	void triangle(const glvertex &t1, const glvertex &t2, const glvertex &t3, 
                        float *zBuffer, const TGAColor &color, TGAImage *image);
	void triangle(const glvertex &t1, const glvertex &t2, const glvertex &t3,
                        float *zBuffer, Model& model, float intensity, TGAImage *image);													

	float * m_zBuffer;
	uint m_width, m_height;
};

#endif
