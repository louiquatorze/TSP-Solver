
#pragma once

#include <string>
#include <filesystem>

class PathService {
public:
    using path = std::filesystem::path;
    
    static const path getProjectRootDir();

    static const path getCXXRootDir();
    static const path getSourceDir();
    static const path getIncludeDir();

    static const path getShaderDir();
    static const path getShaderGLSLDir();
    static const path getShaderCompiledDir();
    static const path getSPIRVFile(const std::string name);
};