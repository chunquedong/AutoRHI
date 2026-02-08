#ifndef MGP_RHI_COMMON_H_
#define MGP_RHI_COMMON_H_

#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <assert.h>
#include <vector>
#include <string>
#include <map>

#define MGP_ERROR(...) do \
    { \
        fprintf(stderr, "ERROR: "); \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n"); \
        abort(); \
    } while (0)

#endif