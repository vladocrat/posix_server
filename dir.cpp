#include "dir.h"

Dir::Dir(const std::string& path)
{
    m_dir = opendir(path.c_str());
}

Dir::~Dir()
{
    closedir(m_dir);
}

bool Dir::valid() const
{
    return m_dir != nullptr;
}

void Dir::print()
{
    dirent* dent;

    while ((dent = readdir(m_dir)))
    {
        puts(dent->d_name);
    }
}
