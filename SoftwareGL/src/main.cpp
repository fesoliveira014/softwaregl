#include "common.hpp"
#include "tga.hpp"
#include "raster.hpp"
#include "model.hpp"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(0, 0, 255, 255);

int main(int argc, char * argv[])
{
    TGAImage frame(200, 200, TGAImage::RGB);
    glm::ivec2 points[3] = {glm::ivec2(10,10), glm::ivec2(100, 30), glm::ivec2(190, 160)}; 

    raster::triangle(points[0], points[1], points[2], red, &frame);

    frame.FlipVertically();
    frame.WriteFile("triangle.tga", false);


    // TGAImage image(1024, 1024, TGAImage::RGB);
    // Model model(std::string("../obj/diablo3_pose/diablo3_pose.obj"));

    // raster::render(model, &image, true);
    
    // image.FlipVertically();
    // image.WriteFile("output.tga", false);

    return 0;
}