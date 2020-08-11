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


// Load an image using stb
int FileReader::loadImage(const char *filename, ImageFile *file, int componentCount) {
    int width, height, channels;
    file->data = reinterpret_cast<char *>(stbi_load(filename, &width, &height, &channels, componentCount));
    file->width = width;
    file->height = height;
    file->size = width * height * (componentCount == 0 ? channels : componentCount);
    file->channels = channels;
    return 0;
}

int FileReader::freeImage(ImageFile *file) {
    if (file->data != nullptr) {
        stbi_image_free(file->data);
        file->data = nullptr;
    }
    return 0;
}

void FileReader::freeFileBin(BinaryFile *file) {
  delete(file->data);
  file->data = nullptr;
}