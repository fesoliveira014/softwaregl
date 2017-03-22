#include "common.hpp"
#include "tga.hpp"
#include "raster.hpp"
#include "model.hpp"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(0, 0, 255, 255);

int main(int argc, char * argv[])
{
    TGAImage image(1024, 1024, TGAImage::RGB);
    Model model(std::string("../obj/diablo3_pose/diablo3_pose.obj"));

    raster::render(model, &image, true);
    
    image.FlipVertically();
    image.WriteFile("output.tga", false);

    return 0;
}