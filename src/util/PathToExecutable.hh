#ifndef PATHTOEXECUTABLE_HH
#define PATHTOEXECUTABLE_HH

#include <iostream>
#include <string>
#include <libgen.h> // For dirname
#include <unistd.h> // For readlink

inline std::string get_path_to_executable() {
    /* The path to the executable, linux-only. */
    const int PATH_MAX = 512;
    char result[ PATH_MAX ];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX - 1);
    std::string path;
    if (count != -1) {
        result[count] = '\0';
        path = dirname(result);
    }
    else {
        std::cerr << "Unable to get path to executable!\nExiting...\n";
        exit(1);
    }
    path = path + "/";
    return path;
}

const std::string PATH_TO_EXECUTABLE = get_path_to_executable();

#endif
