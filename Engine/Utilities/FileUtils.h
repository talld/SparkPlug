#ifndef SPARKPLUG_FILEUTILS_H
#define SPARKPLUG_FILEUTILS_H

#include <cstdlib>
#include <cstdio>
#include <vector>
#include <string>

const char* readFileC(const char* fName, long& fSize);

std::vector<char> readFile(const std::string& filename);
#endif //SPARKPLUG_FILEUTILS_H
