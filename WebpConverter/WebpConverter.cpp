#pragma comment(lib, "libwebp")
#include <webp/decode.h>
#include <webp/encode.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>

using namespace std;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cerr << "Usage: " << argv[0] << " <input_webp_file1> [input_webp_file2] ... [input_webp_fileN]" << endl;
		return EXIT_FAILURE;
	}

	vector<uint8_t> webp_data;
	for (size_t i = 1; i < argc; i++)
	{
		string currentFile = argv[i];

		if (filesystem::exists(currentFile))
		{
			// Open the input webp file
			ifstream webp_file(currentFile, ios::binary | ios::ate);
			if (webp_file)
			{
				// Read the webp file into a vector
				webp_data.resize(webp_file.tellg());
				webp_file.seekg(0);
				webp_file.read(reinterpret_cast<char*>(webp_data.data()), webp_data.size());

				// Decode the webp data into a RGBA buffer
				int width, height;
				uint8_t* rgba_data = WebPDecodeRGBA(webp_data.data(), webp_data.size(), &width, &height);
				if (rgba_data)
				{
					// Encode the RGBA buffer as PNG
					uint8_t* png_data;
					size_t png_size = WebPEncodeRGBA(rgba_data, width, height, width * 4, 100, &png_data);
					if (png_size)
					{
						// Copy the PNG data into a vector
						vector<uint8_t> png_vector(png_data, png_data + png_size);
						WebPFree(png_data);

						// Write the PNG data to the output file
						ofstream png_file(currentFile + ".png", ios::binary);
						if (png_file)
						{
							png_file.write(reinterpret_cast<char*>(png_vector.data()), png_vector.size());
						}
						else
						{
							std::cerr << "Failed to open " << currentFile + ".png" << endl;
						}
					}
					else
					{
						cerr << "Failed to encode PNG" << endl;
					}
				}
				else
				{
					cerr << "Failed to decode " << currentFile << endl;
				}
			}
			else
			{
				cerr << "Failed to open " << currentFile << endl;
			}

			webp_data.clear();
		}
		else
		{
			cerr << "File: " << currentFile << " is not exists!" << endl;
		}
	}

	return EXIT_SUCCESS;
}