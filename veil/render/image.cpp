#include "image.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool load_image(std::string path, image_data *img)
{
	int x, y, n;
	BYTE *data = stbi_load(path.c_str(), &x, &y, &n, 4);

	bool loaded = data != nullptr;
	if (loaded)
	{
		img->comp = n;
		img->width = x;
		img->height = y;

		img->pixels = std::vector<BYTE>(data, data + x * y * 4);
	}
	stbi_image_free(data);
	return loaded;
}