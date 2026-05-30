
#include "PathService.h"

#include <filesystem>
#include <iostream>

const PathService::path PathService::getProjectRootDir() {
    return std::filesystem::current_path();
}

const PathService::path PathService::getCXXRootDir() {
    return PathService::getProjectRootDir() / "c++";
}

const PathService::path PathService::getSourceDir() {
    return PathService::getCXXRootDir() / "src";
}

const PathService::path PathService::getIncludeDir() {
    return PathService::getCXXRootDir() / "include";
}

const PathService::path PathService::getShaderDir() {
    return PathService::getProjectRootDir() / "shaders";
}

const PathService::path PathService::getShaderCompiledDir() {
    return PathService::getShaderDir() / "compiled";
}

void PathService::printAll() {
    std::cout << "Project root: "       << PathService::getProjectRootDir()     << std::endl;
    std::cout << "C++ root: "           << PathService::getCXXRootDir()         << std::endl;
    std::cout << "Source: "             << PathService::getSourceDir()          << std::endl;
    std::cout << "Include: "            << PathService::getIncludeDir()         << std::endl;
    std::cout << "Shaders: "            << PathService::getShaderDir()          << std::endl;
    std::cout << "Compiled Shaders: "   << PathService::getShaderCompiledDir()  << std::endl;
}