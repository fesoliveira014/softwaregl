#ifndef RENDERER_H_
#define RENDERER_H_

#include "common.hpp"
#include "tga.hpp"
#include "model.hpp"

class IRenderer
{
public:
    virtual ~IRenderer() {}

    virtual void render(Model& model, TGAImage *image, bool wireframe) = 0;

protected:
    
};

#endif
