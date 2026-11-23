#ifndef PAGER_CONTRACTS_HPP
#define PAGER_CONTRACTS_HPP

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace pager {

using PageId = std::uint64_t;

enum class AccessType { Read, Write };

struct Reference {
    PageId page;
    AccessType type = AccessType::Read;
};

using Trace = std::vector<Reference>;

struct ResidentPage {
    PageId page;
    std::size_t loaded_at;
    std::size_t last_access;
    std::size_t frequency = 1;
    bool referenced = true;
    bool modified = false;
};

using Frames = std::vector<std::optional<ResidentPage>>;

enum class AccessOutcome { Hit, Load, Replacement };

struct Eviction {
    PageId page;
    bool wrote_to_disk;
};

struct StepResult {
    std::size_t position;
    Reference reference;
    std::size_t frame;
    AccessOutcome outcome;
    std::optional<Eviction> eviction;
};

}  // namespace pager

#endif
