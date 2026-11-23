/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Kerem Kilic (Ellic Studio) */
/* Shared by the application and assembly startup. Explicit build overrides win. */
#ifndef MELODEE_BUILD_OPTIONS_H
#define MELODEE_BUILD_OPTIONS_H
/* Historical themes/engines are host-reference fixtures only. By: Inputsignwave. */
#ifndef MELODEE_RETIRED_REFERENCE
#define MELODEE_RETIRED_REFERENCE 0
#endif
#ifndef MELODEE_DUAL_CORE
#define MELODEE_DUAL_CORE 1     /* paired FM6/Prophet kernels; serial fallback if CPU1 is unavailable */
#endif
#endif
