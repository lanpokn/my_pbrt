// pbrt is Copyright(c) 1998-2020 Matt Pharr, Wenzel Jakob, and Greg Humphreys.
// The pbrt source code is licensed under the Apache License, Version 2.0.
// SPDX: Apache-2.0

#ifndef PBRT_UTIL_PARALLEL_H
#define PBRT_UTIL_PARALLEL_H

#include <pbrt/pbrt.h>

#include <pbrt/util/float.h>
#include <pbrt/util/vecmath.h>

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>
#include <vector>

namespace pbrt {

template <typename T, int maxThreads = 256>
class ThreadLocal {
public:
    ThreadLocal()
        : hashTable(maxThreads), create([]() { return T(); }) {}
    ThreadLocal(std::function<T(void)> &&c)
        : hashTable(maxThreads), create(c) {}

    T &Get() {
        std::thread::id tid = std::this_thread::get_id();
        uint32_t hash = std::hash<std::thread::id>()(tid);
        hash %= hashTable.size();
        int step = 1;

        mutex.lock_shared();
        while (true) {
            if (hashTable[hash] && hashTable[hash]->tid == tid) {
                // Found it
                T &threadLocal = hashTable[hash]->value;
                mutex.unlock_shared();
                return threadLocal;
            } else if (!hashTable[hash]) {
                mutex.unlock_shared();
                T newItem = create();
                mutex.lock();

                if (hashTable[hash]) {
                    // someone else got there first--keep looking, but now
                    // with a writer lock.
                    while (true) {
                        hash += step;
                        ++step;
                        if (hash >= hashTable.size())
                            hash %= hashTable.size();

                        if (!hashTable[hash])
                            break;
                    }
                }

                hashTable[hash] = Entry{tid, std::move(newItem)};
                T &threadLocal = hashTable[hash]->value;
                mutex.unlock();
                return threadLocal;
            }

            hash += step;
            ++step;
            if (hash >= hashTable.size())
                hash %= hashTable.size();
        }
    }

    template <typename F>
    void ForAll(F &&func) {
        mutex.lock();
        for (auto &entry : hashTable) {
            if (entry)
                func(entry->value);
        }
        mutex.unlock();
    }

private:
    struct Entry {
        std::thread::id tid;
        T value;
    };
    std::shared_mutex mutex;
    std::vector<pstd::optional<Entry>> hashTable;
    std::function<T(void)> create;
};

// AtomicFloat Definition
class AtomicFloat {
  public:
    // AtomicFloat Public Methods
    PBRT_CPU_GPU
    explicit AtomicFloat(float v = 0) {
#ifdef PBRT_IS_GPU_CODE
        value = v;
#else
        bits = FloatToBits(v);
#endif
    }

    PBRT_CPU_GPU
    operator float() const {
#ifdef PBRT_IS_GPU_CODE
        return value;
#else
        return BitsToFloat(bits);
#endif
    }
    PBRT_CPU_GPU
    Float operator=(float v) {
#ifdef PBRT_IS_GPU_CODE
        value = v;
        return value;
#else
        bits = FloatToBits(v);
        return v;
#endif
    }

    PBRT_CPU_GPU
    void Add(float v) {
#ifdef PBRT_IS_GPU_CODE
        atomicAdd(&value, v);
#else
        FloatBits oldBits = bits, newBits;
        do {
            newBits = FloatToBits(BitsToFloat(oldBits) + v);
        } while (!bits.compare_exchange_weak(oldBits, newBits));
#endif
    }

    std::string ToString() const;

  private:
    // AtomicFloat Private Members
#ifdef PBRT_IS_GPU_CODE
    float value;
#else
    std::atomic<FloatBits> bits;
#endif
};

class AtomicDouble {
  public:
    // AtomicDouble Public Methods
    PBRT_CPU_GPU
    explicit AtomicDouble(double v = 0) {
#if (defined(__CUDA_ARCH__) && __CUDA_ARCH__ >= 600)
        value = v;
#else
        bits = FloatToBits(v);
#endif
    }

    PBRT_CPU_GPU
    operator double() const {
#if (defined(__CUDA_ARCH__) && __CUDA_ARCH__ >= 600)
        return value;
#else
        return BitsToFloat(bits);
#endif
    }

    PBRT_CPU_GPU
    double operator=(double v) {
#if (defined(__CUDA_ARCH__) && __CUDA_ARCH__ >= 600)
        value = v;
        return value;
#else
        bits = FloatToBits(v);
        return v;
#endif
    }

    PBRT_CPU_GPU
    void Add(double v) {
#if (defined(__CUDA_ARCH__) && __CUDA_ARCH__ >= 600)
        atomicAdd(&value, v);
#elif defined(__CUDA_ARCH__)
        uint64_t old = bits, assumed;

        do {
            assumed = old;
            old = atomicCAS((unsigned long long int *)&bits, assumed,
                            __double_as_longlong(v + __longlong_as_double(assumed)));
        } while (assumed != old);
#else
        uint64_t oldBits = bits, newBits;
        do {
            newBits = FloatToBits(BitsToFloat(oldBits) + v);
        } while (!bits.compare_exchange_weak(oldBits, newBits));
#endif
    }

    std::string ToString() const;

  private:
    // AtomicDouble Private Data
#if (defined(__CUDA_ARCH__) && __CUDA_ARCH__ >= 600)
    double value;
#elif defined(__CUDA_ARCH__)
    uint64_t bits;
#else
    std::atomic<uint64_t> bits;
#endif
};

// Barrier Definition
class Barrier {
  public:
    explicit Barrier(int n) : numToBlock(n), numToExit(n) {}

    Barrier(const Barrier &) = delete;
    Barrier &operator=(const Barrier &) = delete;

    // All block. Returns true to only one thread (which should delete the
    // barrier).
    bool Block();

  private:
    std::mutex mutex;
    std::condition_variable cv;
    int numToBlock, numToExit;
};

void ParallelFor(int64_t start, int64_t end, std::function<void(int64_t, int64_t)> func);
void ParallelFor2D(const Bounds2i &extent, std::function<void(Bounds2i)> func);

// Parallel Inline Functions
inline void ParallelFor(int64_t start, int64_t end, std::function<void(int64_t)> func) {
    ParallelFor(start, end, [&func](int64_t start, int64_t end) {
        for (int64_t i = start; i < end; ++i)
            func(i);
    });
}

inline void ParallelFor2D(const Bounds2i &extent, std::function<void(Point2i)> func) {
    ParallelFor2D(extent, [&func](Bounds2i b) {
        for (Point2i p : b)
            func(p);
    });
}

void ForEachThread(std::function<void(void)> func);

// ThreadIndex Declaration
extern thread_local int ThreadIndex;

// ParallelFunction Declarations
void ParallelInit(int nThreads = -1);
void ParallelCleanup();

int AvailableCores();
int RunningThreads();
int MaxThreadIndex();

}  // namespace pbrt

#endif  // PBRT_UTIL_PARALLEL_H
