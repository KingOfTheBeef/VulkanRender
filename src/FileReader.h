//
// Created by jonat on 06/07/2020.
//

#ifndef DYNAMICLINK_FILEREADER_H
#define DYNAMICLINK_FILEREADER_H

#include "Structures.h"

namespace FileReader {
    int loadImage(const char *filename, ImageFile *file, int componentCount = 4);
    int loadFileBin(const char *filename, BinaryFile *file);
    int freeImage(ImageFile *file);
    void freeFileBin(BinaryFile *file);
}

#endif //DYNAMICLINK_FILEREADER_H
