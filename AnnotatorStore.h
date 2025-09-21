#pragma once
#include <vector>
#include <memory>
#include <atomic>

// Contenedor de snapshots inmutables para datos que el host itera (anotadores/overlays).
// Productores: construyen un std::vector<T> nuevo y llaman replace_all().
// Lectores (invocados por el host): obtienen snapshot() y lo iteran sin locks.
// Esto elimina carreras y use-after-free mientras sdruno_plugin_host.dll itera.

template <typename T>
class AnnotatorStore {
public:
    using Snapshot = std::shared_ptr<const std::vector<T>>;

    AnnotatorStore()
        : snapshot_(std::make_shared<const std::vector<T>>()) {}

    // Reemplaza atómicamente toda la colección por una instantánea inmutable.
    void replace_all(std::vector<T> items) {
        auto fresh = std::make_shared<const std::vector<T>>(std::move(items));
        snapshot_.store(fresh, std::memory_order_release);
    }

    // Lectura lock-free: obtiene la instantánea actual para iterar sin bloquear.
    Snapshot get_snapshot() const {
        return snapshot_.load(std::memory_order_acquire);
    }

private:
    std::atomic<Snapshot> snapshot_;
};
