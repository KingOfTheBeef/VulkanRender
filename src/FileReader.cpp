//
// Created by jonat on 06/07/2020.
//

#include <iostream>
#include <fstream>
#include "FileReader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int FileReader::loadFileBin(const char* filename, BinaryFile *file) {
  std::ifstream fileStream(filename, std::ios_base::binary);
  if (fileStream.is_open()) {
    fileStream.seekg(0, fileStream.end);
    file->size = fileStream.tellg();
    file->data = new char[file->size];
    fileStream.seekg(0);
    fileStream.read(file->data, file->size);
    fileStream.close();
  } else {
    return -1;
  }
  return 0;
}

int FileReader::loadImage(const char *filename, ImageFile *file) {
    int width, height, channels;
    file->data = reinterpret_cast<char *>(stbi_load(filename, &width, &height, &channels, 0));
    std::cout << "real w: " << width << std::endl;
    std::cout << "real h: " << height << std::endl;
    file->width = width;
    file->height = height;
    file->size = width * height * channels;
    file->channels = channels;
    stbi_image_free(file->data);
    return 0;
}


void FileReader::freeFileBin(BinaryFile *file) {
  delete(file->data);
  file->data = nullptr;
}

BinaryFile::~BinaryFile() {
  delete(this->data);
  this->data = nullptr;
}
