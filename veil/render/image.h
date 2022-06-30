#ifndef IMAGE_H
#define IMAGE_H
#include <Windows.h>
#include <string>
#include <vector>

struct image_data
{
	std::vector<BYTE> pixels;
	int width, height;
	int comp;

	image_data()
	{
		pixels = std::vector<BYTE>();
		width = 0;
		height = 0;
		comp = 0;
	}
};

bool load_image(std::string path, image_data *img);
#endif