
#include "PathService.h"

#include <filesystem>
#include <iostream>

const PathService::path PathService::getProjectRootDir() {
    static const PathService::path root = std::filesystem::current_path();
    return root;
}

const PathService::path PathService::getCXXRootDir() {
    static const PathService::path cxxRoot = PathService::getProjectRootDir() / "c++";
    return cxxRoot;
}

const PathService::path PathService::getSourceDir() {
    static const PathService::path source = PathService::getCXXRootDir() / "src";
    return source;
}

const PathService::path PathService::getIncludeDir() {
    static const PathService::path include = PathService::getCXXRootDir() / "include";
    return include;
}

const PathService::path PathService::getShaderDir() {
    static const PathService::path shader = PathService::getProjectRootDir() / "shaders";
    return shader;
}

const PathService::path PathService::getShaderCompiledDir() {
    static const PathService::path shaderCompiled = PathService::getShaderDir() / "compiled";
    return shaderCompiled;
}