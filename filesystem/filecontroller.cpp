#include "filecontroller.h"

#include <fstream>

#include "utils.h"

namespace Internal
{
    std::string formPath(const ClientData& data)
    {
        return std::string()
                .append(data.machineName)
                .append("_")
                .append(data.timestamp)
                .append(".png");
    }
} // Internal

struct FileController::impl_t
{
    std::fstream stream;
};

FileController::FileController() noexcept
{
    createImpl();
}

FileController::~FileController() noexcept
{

}

bool FileController::save(const ClientData& data) noexcept
{
    std::ofstream file(Internal::formPath(data),
                       std::ios::out    |
                       std::ios::binary |
                       std::ios::trunc);

    if (!file)
    {
        LOGE("Error opening file for writing");
        return false;
    }

    file.write(data.imgData, data.imgDataLength);

    if (!file)
    {
        LOGE("Error writing data to file");
        return false;
    }

    file.flush();
    file.close();

    return true;
}
