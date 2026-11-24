#include "core/metrics.hpp"

namespace pager {

void Metrics::record(const StepResult& step) noexcept {
    ++references;
    if (step.outcome != AccessOutcome::Hit) {
        ++faults;
    }
    if (step.eviction) {
        ++evictions;
        disk_writes += step.eviction->wrote_to_disk;
    }
}

double Metrics::fault_rate_percent() const noexcept {
    if (references == 0) {
        return 0.0;
    }
    return 100.0 * faults / references;
}

}  // namespace pager
