#pragma once

#include "Singleton.h"
#include "pimpl.h"
#include "ClientData.h"

class FileController : public Singleton<FileController>
{
public:
    FileController() noexcept;
    ~FileController() noexcept;

   [[nodiscard]] bool save(const ClientData& data) noexcept;

private:
    DECLARE_PIMPL
};

