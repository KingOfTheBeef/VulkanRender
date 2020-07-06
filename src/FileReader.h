//
// Created by jonat on 06/07/2020.
//

#ifndef DYNAMICLINK_FILEREADER_H
#define DYNAMICLINK_FILEREADER_H

struct BinaryFile {
    uint32_t length;
    char *data;
};

namespace FileReader {
    int readFileBin(const char *fileName, BinaryFile *file);
    void freeFileBin(BinaryFile *file);
}

#endif //DYNAMICLINK_FILEREADER_H
