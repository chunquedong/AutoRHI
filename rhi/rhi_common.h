/*
 * Copyright (C) 2026 chunquedong
 *
 * Licensed under the Mozilla Public License Version 2.0
 */
#ifndef MGP_RHI_COMMON_H_
#define MGP_RHI_COMMON_H_

#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <assert.h>
#include <vector>
#include <string>
#include <map>
#include <string.h>
#include <memory>
#include <functional>

#ifdef SRIC
    #include "sc_runtime.h"
#endif

#define ARHI_ERROR(...) do \
    { \
        fprintf(stderr, "ERROR: "); \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n"); \
        abort(); \
    } while (0)

namespace arhi {

#ifdef SRIC
    template<typename T>
    using APtr = sric::OwnPtr<T>;

    template<typename T>
    APtr<T> share(APtr<T>& p) {
        return sric::share(p);
    }

    template<typename T, typename... Args>
    APtr<T> makeAPtr(Args&&... args) {
        return sric::new_<T>(std::forward<Args>(args)...);
    }

    template <typename T, typename U>
    APtr<T> dynamicCast(APtr<U>&& ptr) noexcept {
        return sric::dynamicCast<T>(std::move(ptr));
    }

    template <typename T, typename U>
    APtr<T> cast(APtr<U>&& ptr) noexcept {
        return sric::cast<T>(std::move(ptr));
    }
#else
    template<typename T>
    using APtr = std::shared_ptr<T>;

    template<typename T>
    APtr<T> share(APtr<T>& p) {
        return p;
    }

    template<typename T, typename... Args>
    APtr<T> makeAPtr(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template <typename T, typename U>
    APtr<T> dynamicCast(APtr<U>&& ptr) noexcept {
        return std::dynamic_pointer_cast<T>(std::move(ptr));
    }

    template <typename T, typename U>
    APtr<T> cast(APtr<U>&& ptr) noexcept {
        return std::static_pointer_cast<T>(std::move(ptr));
    }
#endif
}

#endif