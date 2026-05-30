
#include "PathService.h"

#include <filesystem>

const PathService::path PathService::getRootDir() {
    return std::filesystem::current_path().parent_path();
}

const PathService::path PathService::getSourceDir() {
    return PathService::getRootDir() / "src";
}

const PathService::path PathService::getIncludeDir() {
    return PathService::getRootDir() / "include";
}

const PathService::path PathService::getShaderDir() {
    return PathService::getRootDir() / "shaders";
}