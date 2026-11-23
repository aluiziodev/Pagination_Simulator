#ifndef PAGER_POLICIES_HPP
#define PAGER_POLICIES_HPP

#include "core/contracts.hpp"

#include <variant>

namespace pager {

struct Fifo {
    std::size_t select_victim(const Frames&, const Trace&, std::size_t position) const;
};

struct Lru {
    std::size_t select_victim(const Frames&, const Trace&, std::size_t position) const;
};

struct Opt {
    std::size_t select_victim(const Frames&, const Trace&, std::size_t position) const;
};

struct Clock {
    std::size_t select_victim(Frames&, const Trace&, std::size_t position);

private:
    std::size_t hand_ = 0;
};

struct Nru {
    std::size_t select_victim(const Frames&, const Trace&, std::size_t position) const;
    void after_access(Frames&, std::size_t position) const;
};

struct Lfu {
    std::size_t select_victim(const Frames&, const Trace&, std::size_t position) const;
};

struct Mfu {
    std::size_t select_victim(const Frames&, const Trace&, std::size_t position) const;
};

using Policy = std::variant<Fifo, Lru, Opt, Clock, Nru, Lfu, Mfu>;

}  // namespace pager

#endif
