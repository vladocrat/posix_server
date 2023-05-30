#pragma once

#include <string>
#include <dirent.h>

class Dir
{
public:
    Dir(const std::string& path);
    ~Dir();

    [[nodiscard]] bool valid() const;
    void print();

private:
    DIR* m_dir { nullptr };
};


