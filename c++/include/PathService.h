
#pragma once

#include <string>
#include <filesystem>

class PathService {
public:
    using path = std::filesystem::path;
    
    static const path getRootDir();
    static const path getSourceDir();
    static const path getIncludeDir();
    static const path getShaderDir();
};