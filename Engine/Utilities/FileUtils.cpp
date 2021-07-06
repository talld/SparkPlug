#include <cstring>
#include <stdexcept>
#include <fstream>
#include "FileUtils.h"


const char* readFileC(const char* fName, long& fSize){
    char* buffer{};
    FILE* file = fopen(fName,"r");

    if (file){

        fseek(file, 0, SEEK_END);
        fSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        buffer = static_cast<char *>(calloc(1, ++fSize));

        fread(buffer,fSize, 1,file);
        fclose(file);
    }

    return buffer;
}

std::vector<char> readFile(const std::string& filename) {

    std::vector<char> buffer;

    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (file.is_open()) {

        long fileSize = file.tellg();
        buffer.resize(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();
    }

    return buffer;
}