//
// Created by jonat on 06/07/2020.
//

#include <iostream>
#include <fstream>
#include "FileReader.h"

int FileReader::readFileBin(const char* fileName, BinaryFile *file) {
  std::ifstream fileStream(fileName, std::ios_base::binary);
  if (fileStream.is_open()) {
    fileStream.seekg(0, fileStream.end);
    file->length = fileStream.tellg();
    file->data = new char[file->length];
    fileStream.seekg(0);
    fileStream.read(file->data, file->length);
    fileStream.close();
  } else {
    return -1;
  }
  return 0;
}

void FileReader::freeFileBin(BinaryFile *file) {
  delete(file->data);
}