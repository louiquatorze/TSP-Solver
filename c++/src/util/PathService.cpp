
#include "PathService.hpp"

#include <filesystem>
#include <iostream>

const PathService::path PathService::getProjectRootDir() {
    const PathService::path root = std::filesystem::current_path();
    return root;
}

const PathService::path PathService::getCXXRootDir() {
    const PathService::path cxxRoot = PathService::getProjectRootDir() / "c++";
    return cxxRoot;
}

const PathService::path PathService::getSourceDir() {
    const PathService::path source = PathService::getCXXRootDir() / "src";
    return source;
}

const PathService::path PathService::getIncludeDir() {
    const PathService::path include = PathService::getCXXRootDir() / "include";
    return include;
}

const PathService::path PathService::getShaderDir() {
    const PathService::path shader = PathService::getProjectRootDir() / "shaders";
    return shader;
}

const PathService::path PathService::getShaderGLSLDir() {
    const PathService::path shaderGLSL = PathService::getShaderDir() / "glsl";
    return shaderGLSL;
}

const PathService::path PathService::getShaderCompiledDir() {
    const PathService::path shaderCompiled = PathService::getShaderDir() / "compiled";
    return shaderCompiled;
}

const PathService::path PathService::getSPIRVFile(const std::string name) {
    const PathService::path file = PathService::getShaderCompiledDir() / name;
    return file;
}