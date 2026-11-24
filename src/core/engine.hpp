#ifndef PAGER_ENGINE_HPP
#define PAGER_ENGINE_HPP

#include "core/contracts.hpp"
#include "policies/policies.hpp"

#include <stdexcept>
#include <utility>

namespace pager {

class Engine {
public:
    Engine(Trace trace, std::size_t frame_count, Policy policy)
        : trace_(std::move(trace)), frames_(frame_count), policy_(std::move(policy)) {
        if (frame_count == 0) {
            throw std::invalid_argument("O motor exige pelo menos um frame.");
        }
    }

    const Frames& frames() const noexcept { return frames_; }

    std::optional<StepResult> step() {
        if (position_ == trace_.size()) {
            return std::nullopt;
        }

        const Reference reference = trace_[position_];
        const bool is_write = reference.type == AccessType::Write;
        std::size_t first_free = frames_.size();

        for (std::size_t index = 0; index < frames_.size(); ++index) {
            auto& frame = frames_[index];
            if (!frame) {
                if (first_free == frames_.size()) {
                    first_free = index;
                }
            } else if (frame->page == reference.page) {
                frame->last_access = position_;
                ++frame->frequency;
                frame->referenced = true;
                frame->modified |= is_write;
                return finish_step(reference, index, AccessOutcome::Hit);
            }
        }

        if (first_free != frames_.size()) {
            load(first_free, reference);
            return finish_step(reference, first_free, AccessOutcome::Load);
        }

        const auto victim = std::visit(
            [this](auto& policy) { return policy.select_victim(frames_, trace_, position_); },
            policy_);
        const auto& resident = frames_.at(victim).value();
        const Eviction eviction{resident.page, resident.modified};
        load(victim, reference);
        return finish_step(reference, victim, AccessOutcome::Replacement, eviction);
    }

private:
    void load(std::size_t index, Reference reference) {
        frames_[index] = ResidentPage{
            reference.page, position_, position_, 1, true, reference.type == AccessType::Write
        };
    }

    StepResult finish_step(Reference reference, std::size_t index, AccessOutcome outcome,
                           std::optional<Eviction> eviction = std::nullopt) {
        if (auto* nru = std::get_if<Nru>(&policy_)) {
            nru->after_access(frames_, position_);
        }
        return StepResult{position_++, reference, index, outcome, eviction};
    }

    Trace trace_;
    Frames frames_;
    Policy policy_;
    std::size_t position_ = 0;
};

}  // namespace pager

#endif
