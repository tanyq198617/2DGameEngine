/*
 * Copyright (C) 2008, 2009, 2010, 2014 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __WebKitAvailability__
#define __WebKitAvailability__

#if defined(__APPLE__)

#include <AvailabilityMacros.h>
#include <CoreFoundation/CoreFoundation.h>

#ifdef CF_AVAILABLE
#undef CF_AVAILABLE
#endif
#define CF_AVAILABLE(_mac, _ios)

#ifdef CF_ENUM_AVAILABLE
#undef CF_ENUM_AVAILABLE
#endif
#define CF_ENUM_AVAILABLE(_mac, _ios)

#if defined(BUILDING_GTK__)
#undef CF_AVAILABLE
#define CF_AVAILABLE(_mac, _ios)
#undef CF_ENUM_AVAILABLE
#define CF_ENUM_AVAILABLE(_mac, _ios)
#endif

#else
#define CF_AVAILABLE(_mac, _ios)
#define CF_ENUM_AVAILABLE(_mac, _ios)
#endif

#endif /* __WebKitAvailability__ */
