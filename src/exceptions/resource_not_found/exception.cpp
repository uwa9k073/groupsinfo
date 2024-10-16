#include "exception.hpp"

namespace exceptions {
ResourceNotFound::ResourceNotFound()
    : std::runtime_error("Resource not found") {}
}  // namespace exceptions