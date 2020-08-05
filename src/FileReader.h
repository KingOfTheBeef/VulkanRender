//
// Created by jonat on 06/07/2020.
//

#ifndef DYNAMICLINK_FILEREADER_H
#define DYNAMICLINK_FILEREADER_H

struct BinaryFile {
    uint32_t length;
    char *data;
    ~BinaryFile();
};

struct ImageFile : BinaryFile {
    uint32_t width;
    uint32_t height;
    uint32_t channels;
};

namespace FileReader {
    int loadImage(const char *filename, ImageFile *file);
    int loadFileBin(const char *filename, BinaryFile *file);
    void freeFileBin(BinaryFile *file);
}

#endif //DYNAMICLINK_FILEREADER_H
