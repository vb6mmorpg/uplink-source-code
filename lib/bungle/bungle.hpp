/*
    B U N G L E 
    
    Zip file access library
    By Christopher Delay
*/

#ifndef _included_bungle_h
#define _included_bungle_h

#include <string>
#include <vector>

#include "SDL.h"

namespace Bungle {
    bool OpenZipFile(const std::string &zip_file, const std::string &app_path, const std::string &id = std::string());
    bool OpenZipFile(SDL_RWops *zip_file, const std::string &app_path, const std::string &id = std::string());

    bool FileLoaded(const std::string &filename);
    bool ExtractFile(const std::string &filename, const std::string target = std::string());

    void CloseZipFile(const std::string &id);

    void ExtractAllFiles(const std::string &id);

    const std::vector<std::string> *ListFiles(const std::string &path, const std::string &directory = std::string(), const std::string &filter = std::string());

    void CloseAllFiles();
}

#endif
