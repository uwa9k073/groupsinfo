#pragma once

#include <stdexcept>
namespace exceptions {
class ResourceNotFound final : public std::runtime_error {
    public:
    ResourceNotFound();
};
}