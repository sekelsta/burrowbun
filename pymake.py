'''A python project builder.
Features:
    - Compiles object files for every c source file (.c, .cc, .cpp) in the src
    directory. No need to manually add new files as they are created.
    - Detects dependencies using compiler functionality. No need to update
    any list when adding or removing a dependency - just change the source file
    and go. The dependencies are used to avoid rebuilding object files when it
    is not necessary.
    - Supports subdirectories of sources. Subfolders are automatically
    discovered, and object and dependency files will use the same
    organizational scheme. No clashes if two files in different folders have
    the same name.
    - Directories are created if needed.
'''

import os
import subprocess
import shlex

CXX = 'g++'
# Debug version
CXX_FLAGS = '-std=c++14 -Wall -D_GLIBCXX_DEBUG -D_LIBCXX_DEBUG_PEDANTIC -Og -g'
# Optimized version (disabled)
# CXX_FLAGS = '-std=c++14 -Wall -O3'
INCLUDE_FLAGS = '-I/usr/include/libnoise -L/usr/lib'
LINKER_FLAGS = '-lSDL2 -lSDL2_image -lSDL2_ttf -lnoise -lpthread'

# Helper functions
def list_files_recursive(directory):
    '''Recursively searches the given directory for all non-directory files.
    Returns a list of files found, including the directory path.
    Parameter is a directory name with no trailing /'''
    files = []
    for name in os.listdir(directory):
        full_name = directory + '/' + name
        if os.path.isdir(full_name):
            files += list_files_recursive(full_name)
        else:
            files += [full_name]
    return files

def parse_dependencies(dep):
    '''Read the contents of a dependency file, in the form that the g++
    compiler generates, and return a list of dependencies.'''
    file_raw = dep.read()
    file_escaped = file_raw.replace('\\\n', '')
    depend_lists = filter(None, file_escaped.split('\n'))
    deps = []
    for dependency in depend_lists:
        dep_split = dependency.split(':')
        raw_deps = dep_split[1].split(' ')
        deps += filter(None, raw_deps)
    return deps

def needs_build(src_name, obj_name, dep_name):
    '''Detect whether the object file and dependency file need to be (re)built.
    This returns true if either file doesn't exist, if the source file is more
    recent than the dependency file, or if any dependency is more recent than
    the object file.'''
    # Check that the dependency file exists
    if not os.path.isfile(dep_name):
        return True
    # Check that the object file exists
    elif not os.path.isfile(obj_name):
        return True
    # Check if the source file is more recently modified than the
    # dependency file
    elif os.path.getmtime(src_name) >= os.path.getmtime(dep_name):
        return True
    # Check if any dependency is more recently modified than the object file
    # Note the source file itself is included among the dependencies.
    else:
        obj_time = os.path.getmtime(obj_name)
        f = open(dep_name, 'r')
        deps = parse_dependencies(f)
        f.close()
        for dep in deps:
            if os.path.getmtime(dep) >= obj_time:
                return True
    return False

def create_directory(path):
    if not path:
        return
    if not os.path.isdir(path):
        create_directory(os.path.dirname(path))
        print(f'Creating directory {path}')
        os.mkdir(path)

def build_object(filename, src_dir, obj_dir, dep_dir):
    '''Creates an object file for the given c++ file if needed.
    This will generate a dependency file and build the object file unless
    they both already exist and are up to date.'''
    # Get the name for the associated dependency file
    filename_base = filename[:filename.rfind('.')]
    dep_name = os.path.join(dep_dir, filename_base + '.d')
    obj_name = os.path.join(obj_dir, filename_base + '.o')
    src_name = os.path.join(src_dir, filename)
    # Create any directories needed
    create_directory(os.path.dirname(dep_name))
    create_directory(os.path.dirname(obj_name))
    # Build if needed
    if needs_build(src_name, obj_name, dep_name):
        # -MMD is to generate dependencies and also continue with compilation,
        # and to only mention user header files (not system headers) in the
        # dependencies
        # -MF {dep_name} specifies where to write the dependency file
        command = f'{CXX} -MMD -MF {dep_name} {CXX_FLAGS} {INCLUDE_FLAGS} -c' \
                   + f' -o {obj_name} {src_name}'
        print(command)
        out = subprocess.run(shlex.split(command))
        return out.returncode

def build(exec_name, src_dir='src', obj_dir='obj', dep_dir='.d', bin_dir='.'):
    # Get a list of c (.c, .cc, .cpp) files
    def is_cpp(name):
        return name.endswith('.cc') or name.endswith('.cpp') \
            or name.endswith('.c')
    l = filter(is_cpp, list_files_recursive('src'))
    # Build object files if needed
    success = True
    for s in l:
        ret = build_object(s[len('src/'):], src_dir, obj_dir, dep_dir)
        success = success and not ret
    if not success:
        print('Object compilation failed.')
        return
    # Build the executable
    objects = ' '.join(list_files_recursive(obj_dir))
    bin_name = os.path.join(bin_dir, exec_name)
    create_directory(os.path.dirname(bin_name))
    command = f'{CXX} {objects} {LINKER_FLAGS} -o {bin_name}'
    print(command)
    subprocess.run(shlex.split(command))

# Compilation script


# Name of final executable
EXEC = 'burrowbun'

if __name__ == '__main__':
    build(EXEC)
