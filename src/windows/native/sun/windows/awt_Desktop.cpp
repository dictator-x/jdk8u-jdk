/*
 * Copyright 2005-2006 Sun Microsystems, Inc.  All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Sun designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Sun in the LICENSE file that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa Clara,
 * CA 95054 USA or visit www.sun.com if you need additional information or
 * have any questions.
 */

#include "awt.h"
#include <jni.h>
#include <shellapi.h>
#include <float.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     sun_awt_windows_WDesktopPeer
 * Method:    ShellExecute
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_sun_awt_windows_WDesktopPeer_ShellExecute
  (JNIEnv *env, jclass cls, jstring uri_j, jstring verb_j)
{
    LPCWSTR uri_c = JNU_GetStringPlatformChars(env, uri_j, JNI_FALSE);
    LPCWSTR verb_c = JNU_GetStringPlatformChars(env, verb_j, JNI_FALSE);

    // 6457572: ShellExecute possibly changes FPU control word - saving it here
    unsigned oldcontrol87 = _control87(0, 0);
    HINSTANCE retval = ::ShellExecute(NULL, verb_c, uri_c, NULL, NULL, SW_SHOWNORMAL);
    _control87(oldcontrol87, 0xffffffff);

    JNU_ReleaseStringPlatformChars(env, uri_j, uri_c);
    JNU_ReleaseStringPlatformChars(env, verb_j, verb_c);

    if ((int)retval <= 32) {
        // ShellExecute failed.
        LPTSTR buffer = NULL;
        int len = ::FormatMessage(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM  |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    GetLastError(),
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                    (LPTSTR)&buffer,
                    0,
                    NULL );

        jstring errmsg = JNU_NewStringPlatform(env, buffer, len);
        LocalFree(buffer);
        return errmsg;
    }

    return NULL;
}

#ifdef __cplusplus
}
#endif
