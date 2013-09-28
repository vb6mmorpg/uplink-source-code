#include <map>

#include "bungle.hpp"

#include "mmgr.h"

//#include "debug.h"

namespace Bungle {
    struct LocalFileHeader {
        Uint8  signature[4];
        Uint16 version;
        Uint16 bit_flag;
        Uint16 compression_method;
        Uint16 last_mod_file_time;
        Uint16 last_mod_file_date;
        Uint32 crc32;
        Uint32 compressed_size;
        Uint32 uncompressed_size;
        Uint16 filename_length;
        Uint16 extra_field_length;
        
        std::string filename;
        Uint8       *extra_field;
        Uint8       *data;
        
        LocalFileHeader() {
            this->extra_field = nullptr;
            this->data = nullptr;
        }
        
        ~LocalFileHeader() {
            if (this->extra_field)
                delete[] this->extra_field;
            if (this->data)
                delete[] this->data;
        }
    };
    
    static std::multimap<std::string, LocalFileHeader *> files;
    
    // Converts back slashes in s to forward slashes.
    // The old implementation converted the string to lower case,
    // but that messes up non-ASCII strings. Let's see how removing that bit goes...
    static void Slashify(std::string &s) {
        std::size_t pos = 0;
        while ((pos = s.find_first_of('\\')) != std::string::npos)
            s.replace(pos, 1, "/");
    }
    
    bool OpenZipFile(const std::string &zip_file, const std::string &app_path, const std::string &id) {
        SDL_RWops *file;
        if ((file = SDL_RWFromFile(zip_file.c_str(), "rb")) == nullptr)
            return false;
        bool result = OpenZipFile(file, app_path, id);
        SDL_RWclose(file);
        return result;
    }
    
    bool OpenZipFile(SDL_RWops *zip_file, const std::string &app_path, const std::string &id) {
        if (zip_file == nullptr)
            return false;
        
        bool found_valid_data = false;
        
        while (true) {
            LocalFileHeader *fh = new LocalFileHeader();
            // Signature
            if (SDL_RWread(zip_file, reinterpret_cast<void *>(&(fh->signature)), 4, 1) == 0) {
                delete fh;
                return found_valid_data;
            }
            if (!(fh->signature[0] == 'P' || fh->signature[1] == 'K')) {
                delete fh;
                return found_valid_data;
            }
            
            // Version
            if (SDL_RWread(zip_file, reinterpret_cast<void *>(&(fh->version)), 2, 1) == 0) {
                delete fh;
                return found_valid_data;
            }
            
            // Bit flag
            if (SDL_RWread(zip_file, reinterpret_cast<void *>(&(fh->bit_flag)), 2, 1) == 0) {
                delete fh;
                return found_valid_data;
            }
            
            // Compression method
            if (SDL_RWread(zip_file, reinterpret_cast<void *>(&(fh->compression_method)), 2, 1) == 0) {
                delete fh;
                return found_valid_data;
            }
            
            // "Last modified" time
            if (SDL_RWread(zip_file, reinterpret_cast<void *>(&(fh->last_mod_file_time)), 2, 1) == 0) {
                delete fh;
                return found_valid_data;
            }
            
            // "Last modified" date
            if (SDL_RWread(zip_file, reinterpret_cast<void *>(&(fh->last_mod_file_date)), 2, 1) == 0) {
                delete fh;
                return found_valid_data;
            }
            
            // CRC32
            if (SDL_RWread(zip_file, reinterpret_cast<void *>(&(fh->crc32)), 4, 1) == 0) {
                delete fh;
                return found_valid_data;
            }
            
            // Compressed size
            if (SDL_RWread(zip_file, reinterpret_cast<void *>(&(fh->compressed_size)), 4, 1) == 0) {
                delete fh;
                return found_valid_data;
            }
            
            // Uncompressed size
            if (SDL_RWread(zip_file, reinterpret_cast<void *>(&(fh->uncompressed_size)), 4, 1) == 0) {
                delete fh;
                return found_valid_data;
            }
            
            // Filename length
            if (SDL_RWread(zip_file, reinterpret_cast<void *>(&(fh->filename_length)), 2, 1) == 0) {
                delete fh;
                return found_valid_data;
            }
            
            // Extra field length
            if (SDL_RWread(zip_file, reinterpret_cast<void *>(&(fh->extra_field_length)), 2, 1) == 0) {
                delete fh;
                return found_valid_data;
            }
            
            // Filename
            if (fh->filename_length > 0 && fh->uncompressed_size == fh->compressed_size && fh->compression_method == 0) {
                Uint8 *filename = new Uint8[fh->filename_length];
                if (SDL_RWread(zip_file, reinterpret_cast<void *>(filename), fh->filename_length, 1) == 0) {
                    delete[] filename;
                    delete fh;
                    return found_valid_data;
                }
                fh->filename = std::string(app_path);
                fh->filename.append(reinterpret_cast<std::string::value_type *>(filename), fh->filename_length);
                delete[] filename;
                Slashify(fh->filename);
            } else {
                delete fh;
                return found_valid_data;
            }
            
            // Extra field
            if (fh->extra_field_length > 0) {
                fh->extra_field = new Uint8[fh->extra_field_length];
                if (SDL_RWread(zip_file, reinterpret_cast<void *>(fh->extra_field), fh->extra_field_length, 1) == 0) {
                    delete fh;
                    return found_valid_data;
                }
            }
            
            // Data
            if (fh->uncompressed_size > 0) {
                fh->data = new Uint8[fh->uncompressed_size];
                if (SDL_RWread(zip_file, reinterpret_cast<void *>(fh->data), fh->uncompressed_size, 1) == 0) {
                    delete fh;
                    return found_valid_data;
                }
            }
            
            files.insert(std::pair<std::string, LocalFileHeader *>(id, fh));
            if (found_valid_data != true)
                found_valid_data = true;
        }
    }
    
    bool FileLoaded(const std::string &filename) {
        if (filename.empty())
            return false;
        
        std::string search_term = std::string(filename);
        Slashify(search_term);
        
        for (std::multimap<std::string, LocalFileHeader *>::iterator it = files.begin(); it != files.end(); it++) {
            if (it->second->filename == search_term)
                return true;
        }
        
        return false;
    }
    
    void CloseZipFile(const std::string &id) {
        if (!id.empty()) {
            for (std::multimap<std::string, LocalFileHeader *>::iterator it = files.begin(); it != files.end(); it++) {
                if (it->first == id)
                    delete it->second;
            }
            files.erase(id);
        }
    }
    
    bool ExtractFile(const std::string &filename, const std::string &target) {
        std::string search_term = std::string(filename);
        Slashify(search_term);
        
        LocalFileHeader *lfh;
        for (std::multimap<std::string, LocalFileHeader *>::iterator it = files.begin(); it != files.end(); it++) {
            if (it->second->filename == search_term) {
                lfh = it->second;
                break;
            }
        }
        
        SDL_RWops *out_file;
        if (lfh == nullptr || (out_file = SDL_RWFromFile(target.c_str(), "wb")) == nullptr)
            return false;
        if (SDL_RWwrite(out_file, reinterpret_cast<const void *>(lfh->data), lfh->uncompressed_size, 1) < 1)
            throw std::runtime_error(std::string("Data archive extraction failed: ") + std::string(SDL_GetError()));
        
        SDL_RWclose(out_file);
        
        return true;
    }
    
    void ExtractAllFiles(const std::string &id) {
        if (files.find(id) != files.end()) {
            for (std::multimap<std::string, LocalFileHeader *>::iterator it = files.begin(); it != files.end(); it++) {
                if (it->first == id) {
                    SDL_RWops *file;
                    if ((file = SDL_RWFromFile(it->second->filename.c_str(), "wb")) == nullptr ||
                        SDL_RWwrite(file, reinterpret_cast<const void *>(it->second->data), it->second->uncompressed_size, 1) < 1)
                        throw std::runtime_error(std::string("Data archive extraction failed: ") + std::string(SDL_GetError()));
                }
            }
        }
    }
    
    const std::vector<std::string> *ListFiles(const std::string &path, const std::string &directory, const std::string &filter) {
        std::string dir = std::string(path).append(directory);
        Slashify(dir);
        
        std::vector<std::string> *list;
        if ((list = new std::vector<std::string>) == nullptr)
            throw std::runtime_error(std::string("De-archived file list allocation failed"));
        
        for (std::multimap<std::string, LocalFileHeader *>::iterator it = files.begin(); it != files.end(); it++) {
            if (it->second->filename.substr(0, dir.size()) == dir && !(it->second->filename.substr(dir.size()).find(filter)))
                list->push_back(it->second->filename);
        }
        
        return list;
    }
    
    void CloseAllFiles() {
        for (std::multimap<std::string, LocalFileHeader *>::iterator it = files.begin(); it != files.end(); it++) {
            delete it->second;
            files.erase(it);
        }
    }
}
