/*
* Base64 encoding/decoding (RFC1341)
* Copyright (c) 2005-2011, Jouni Malinen <j@w1.fi>
*
* This software may be distributed under the terms of the BSD license.
* See README for more details.
*/

// Change log:
// 2016-12-12 - Gaspard Petit : Slightly modified to return a std::string 
//                              instead of a buffer allocated with malloc.
// 2020-12-08 - Jixun: Taken from StackOverflow and adapted to project.
//                     source: https://stackoverflow.com/a/41094722

#include "stdafx.h"
#include "base64.h"

static const wchar_t base64_table[] = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
* base64_encode - Base64 encode
* @src:     Data to be encoded
* @len:     Length of the data to be encoded
* @out_len: Pointer to output length variable, or %NULL if not used
* Returns:  Allocated buffer of out_len bytes of encoded data,
*           or empty string on failure
*/
wchar_t* base64_encode(const unsigned char* src, size_t len)
{
    const unsigned char* end, * in;

    size_t olen;

    olen = 4 * ((len + 2) / 3); /* 3-byte blocks to 4-byte */

    if (olen < len)
        return nullptr;

    wchar_t* out = (wchar_t*)calloc(olen + 1, sizeof(wchar_t));
    if (out == nullptr) {
        return nullptr;
    }

    end = src + len;
    in = src;
    auto pos = out;
    while (end - in >= 3) {
        *pos++ = base64_table[in[0] >> 2];
        *pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
        *pos++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
        *pos++ = base64_table[in[2] & 0x3f];
        in += 3;
    }

    if (end - in) {
        *pos++ = base64_table[in[0] >> 2];
        if (end - in == 1) {
            *pos++ = base64_table[(in[0] & 0x03) << 4];
            *pos++ = L'=';
        }
        else {
            *pos++ = base64_table[((in[0] & 0x03) << 4) |
                (in[1] >> 4)];
            *pos++ = base64_table[(in[1] & 0x0f) << 2];
        }
        *pos++ = L'=';
    }

    return out;
}
