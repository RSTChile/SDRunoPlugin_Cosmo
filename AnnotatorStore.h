#pragma once
#include <vector>
#include <memory>
#include <atomic>

// Contenedor de snapshot inmutable para datos iterados por el host (anotadores/overlays)
// - Los escritores construyen un std::vector<T> nuevo y llaman replace_all().
// - Los lectores llaman get_snapshot() e iteran en lock-free.
// Evita carreras/use-after-free mientras el host itera.

template <typename T>
class AnnotatorStore {
public:
    using Snapshot = std::shared_ptr<const std::vector<T>>;

    AnnotatorStore()
        : snapshot_(std::make_shared<const std::vector<T>>()) {}

    void replace_all(std::vector<T> items) {
        auto fresh = std::make_shared<const std::vector<T>>(std::move(items));
        snapshot_.store(fresh, std::memory_order_release);
    }

    Snapshot get_snapshot() const {
        return snapshot_.load(std::memory_order_acquire);
    }

private:
    std::atomic<Snapshot> snapshot_;
};
