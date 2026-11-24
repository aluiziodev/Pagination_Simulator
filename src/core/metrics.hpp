#ifndef PAGER_METRICS_HPP
#define PAGER_METRICS_HPP

#include "core/contracts.hpp"

namespace pager {

struct Metrics {
    std::size_t references = 0;
    std::size_t faults = 0;
    std::size_t evictions = 0;
    std::size_t disk_writes = 0;

    void record(const StepResult& step) noexcept;
    double fault_rate_percent() const noexcept;
};

}  // namespace pager

#endif
