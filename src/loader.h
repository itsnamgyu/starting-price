#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "estab.h"
#include "memory.h"
#include "utility.h"


bool load(FILE **files, int n_files, Block *block);


#if defined(TEST) && !defined(LOADER_C)
#undef TEST
#include "loader.c"
#define TEST
#endif
