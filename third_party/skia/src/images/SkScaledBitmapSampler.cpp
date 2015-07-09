/*
 * Copyright 2007 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#include "SkScaledBitmapSampler.h"
#include "SkBitmap.h"
#include "SkColorPriv.h"
#include "SkDither.h"
#include "SkTypes.h"

// 8888

static bool Sample_Gray_D8888(void* SK_RESTRICT dstRow,
                              const uint8_t* SK_RESTRICT src,
                              int width, int deltaSrc, int, const SkPMColor[]) {
    SkPMColor* SK_RESTRICT dst = (SkPMColor*)dstRow;
    for (int x = 0; x < width; x++) {
        dst[x] = SkPackARGB32(0xFF, src[0], src[0], src[0]);
        src += deltaSrc;
    }
    return false;
}

static SkScaledBitmapSampler::RowProc get_gray_to_8888_proc(const SkImageDecoder& decoder) {
    // Dither, unpremul, and skipZeroes have no effect
    return Sample_Gray_D8888;
}

static bool Sample_RGBx_D8888(void* SK_RESTRICT dstRow,
                              const uint8_t* SK_RESTRICT src,
                              int width, int deltaSrc, int, const SkPMColor[]) {
    SkPMColor* SK_RESTRICT dst = (SkPMColor*)dstRow;
    for (int x = 0; x < width; x++) {
        dst[x] = SkPackARGB32(0xFF, src[0], src[1], src[2]);
        src += deltaSrc;
    }
    return false;
}

static SkScaledBitmapSampler::RowProc get_RGBx_to_8888_proc(const SkImageDecoder& decoder) {
    // Dither, unpremul, and skipZeroes have no effect
    return Sample_RGBx_D8888;
}

static bool Sample_RGBA_D8888(void* SK_RESTRICT dstRow,
                              const uint8_t* SK_RESTRICT src,
                              int width, int deltaSrc, int, const SkPMColor[]) {
    SkPMColor* SK_RESTRICT dst = (SkPMColor*)dstRow;
    unsigned alphaMask = 0xFF;
    for (int x = 0; x < width; x++) {
        unsigned alpha = src[3];
        dst[x] = SkPreMultiplyARGB(alpha, src[0], src[1], src[2]);
        src += deltaSrc;
        alphaMask &= alpha;
    }
    return alphaMask != 0xFF;
}

static bool Sample_RGBA_D8888_Unpremul(void* SK_RESTRICT dstRow,
                                       const uint8_t* SK_RESTRICT src,
                                       int width, int deltaSrc, int,
                                       const SkPMColor[]) {
    uint32_t* SK_RESTRICT dst = reinterpret_cast<uint32_t*>(dstRow);
    unsigned alphaMask = 0xFF;
    for (int x = 0; x < width; x++) {
        unsigned alpha = src[3];
        dst[x] = SkPackARGB32NoCheck(alpha, src[0], src[1], src[2]);
        src += deltaSrc;
        alphaMask &= alpha;
    }
    return alphaMask != 0xFF;
}

static bool Sample_RGBA_D8888_SkipZ(void* SK_RESTRICT dstRow,
                                    const uint8_t* SK_RESTRICT src,
                                    int width, int deltaSrc, int,
                                    const SkPMColor[]) {
    SkPMColor* SK_RESTRICT dst = (SkPMColor*)dstRow;
    unsigned alphaMask = 0xFF;
    for (int x = 0; x < width; x++) {
        unsigned alpha = src[3];
        if (0 != alpha) {
            dst[x] = SkPreMultiplyARGB(alpha, src[0], src[1], src[2]);
        }
        src += deltaSrc;
        alphaMask &= alpha;
    }
    return alphaMask != 0xFF;
}

static SkScaledBitmapSampler::RowProc get_RGBA_to_8888_proc(const SkImageDecoder& decoder) {
    // Dither has no effect.
    if (decoder.getRequireUnpremultipliedColors()) {
        // We could check each component for a zero, at the expense of extra checks.
        // For now, just return unpremul.
        return Sample_RGBA_D8888_Unpremul;
    }
    // Supply the versions that premultiply the colors
    if (decoder.getSkipWritingZeroes()) {
        return Sample_RGBA_D8888_SkipZ;
    }
    return Sample_RGBA_D8888;
}

// 565

static bool Sample_Gray_D565(void* SK_RESTRICT dstRow,
                             const uint8_t* SK_RESTRICT src,
                             int width, int deltaSrc, int, const SkPMColor[]) {
    uint16_t* SK_RESTRICT dst = (uint16_t*)dstRow;
    for (int x = 0; x < width; x++) {
        dst[x] = SkPack888ToRGB16(src[0], src[0], src[0]);
        src += deltaSrc;
    }
    return false;
}

static bool Sample_Gray_D565_D(void* SK_RESTRICT dstRow,
                               const uint8_t* SK_RESTRICT src,
                           int width, int deltaSrc, int y, const SkPMColor[]) {
    uint16_t* SK_RESTRICT dst = (uint16_t*)dstRow;
    DITHER_565_SCAN(y);
    for (int x = 0; x < width; x++) {
        dst[x] = SkDitherRGBTo565(src[0], src[0], src[0], DITHER_VALUE(x));
        src += deltaSrc;
    }
    return false;
}

static SkScaledBitmapSampler::RowProc get_gray_to_565_proc(const SkImageDecoder& decoder) {
    // Unpremul and skip zeroes make no difference
    if (decoder.getDitherImage()) {
        return Sample_Gray_D565_D;
    }
    return Sample_Gray_D565;
}

static bool Sample_RGBx_D565(void* SK_RESTRICT dstRow,
                             const uint8_t* SK_RESTRICT src,
                             int width, int deltaSrc, int, const SkPMColor[]) {
    uint16_t* SK_RESTRICT dst = (uint16_t*)dstRow;
    for (int x = 0; x < width; x++) {
        dst[x] = SkPack888ToRGB16(src[0], src[1], src[2]);
        src += deltaSrc;
    }
    return false;
}

static bool Sample_RGBx_D565_D(void* SK_RESTRICT dstRow,
                               const uint8_t* SK_RESTRICT src,
                               int width, int deltaSrc, int y,
                               const SkPMColor[]) {
    uint16_t* SK_RESTRICT dst = (uint16_t*)dstRow;
    DITHER_565_SCAN(y);
    for (int x = 0; x < width; x++) {
        dst[x] = SkDitherRGBTo565(src[0], src[1], src[2], DITHER_VALUE(x));
        src += deltaSrc;
    }
    return false;
}

static SkScaledBitmapSampler::RowProc get_RGBx_to_565_proc(const SkImageDecoder& decoder) {
    // Unpremul and skip zeroes make no difference
    if (decoder.getDitherImage()) {
        return Sample_RGBx_D565_D;
    }
    return Sample_RGBx_D565;
}


static bool Sample_D565_D565(void* SK_RESTRICT dstRow,
                             const uint8_t* SK_RESTRICT src,
                             int width, int deltaSrc, int, const SkPMColor[]) {
    uint16_t* SK_RESTRICT dst = (uint16_t*)dstRow;
    uint16_t* SK_RESTRICT castedSrc = (uint16_t*) src;
    for (int x = 0; x < width; x++) {
        dst[x] = castedSrc[0];
        castedSrc += deltaSrc >> 1;
    }
    return false;
}

static SkScaledBitmapSampler::RowProc get_565_to_565_proc(const SkImageDecoder& decoder) {
    // Unpremul, dither, and skip zeroes have no effect
    return Sample_D565_D565;
}

// 4444

static bool Sample_Gray_D4444(void* SK_RESTRICT dstRow,
                              const uint8_t* SK_RESTRICT src,
                              int width, int deltaSrc, int, const SkPMColor[]) {
    SkPMColor16* SK_RESTRICT dst = (SkPMColor16*)dstRow;
    for (int x = 0; x < width; x++) {
        unsigned gray = src[0] >> 4;
        dst[x] = SkPackARGB4444(0xF, gray, gray, gray);
        src += deltaSrc;
    }
    return false;
}

static bool Sample_Gray_D4444_D(void* SK_RESTRICT dstRow,
                                const uint8_t* SK_RESTRICT src,
                            int width, int deltaSrc, int y, const SkPMColor[]) {
    SkPMColor16* SK_RESTRICT dst = (SkPMColor16*)dstRow;
    DITHER_4444_SCAN(y);
    for (int x = 0; x < width; x++) {
        dst[x] = SkDitherARGB32To4444(0xFF, src[0], src[0], src[0],
                                      DITHER_VALUE(x));
        src += deltaSrc;
    }
    return false;
}

static SkScaledBitmapSampler::RowProc get_gray_to_4444_proc(const SkImageDecoder& decoder) {
    // Skip zeroes and unpremul make no difference
    if (decoder.getDitherImage()) {
        return Sample_Gray_D4444_D;
    }
    return Sample_Gray_D4444;
}

static bool Sample_RGBx_D4444(void* SK_RESTRICT dstRow,
                              const uint8_t* SK_RESTRICT src,
                              int width, int deltaSrc, int, const SkPMColor[]) {
    SkPMColor16* SK_RESTRICT dst = (SkPMColor16*)dstRow;
    for (int x = 0; x < width; x++) {
        dst[x] = SkPackARGB4444(0xF, src[0] >> 4, src[1] >> 4, src[2] >> 4);
        src += deltaSrc;
    }
    return false;
}

static bool Sample_RGBx_D4444_D(void* SK_RESTRICT dstRow,
                                const uint8_t* SK_RESTRICT src,
                            int width, int deltaSrc, int y, const SkPMColor[]) {
    SkPMColor16* dst = (SkPMColor16*)dstRow;
    DITHER_4444_SCAN(y);

    for (int x = 0; x < width; x++) {
        dst[x] = SkDitherARGB32To4444(0xFF, src[0], src[1], src[2],
                                      DITHER_VALUE(x));
        src += deltaSrc;
    }
    return false;
}

static SkScaledBitmapSampler::RowProc get_RGBx_to_4444_proc(const SkImageDecoder& decoder) {
    // Skip zeroes and unpremul make no difference
    if (decoder.getDitherImage()) {
        return Sample_RGBx_D4444_D;
    }
    return Sample_RGBx_D4444;
}

static bool Sample_RGBA_D4444(void* SK_RESTRICT dstRow,
                              const uint8_t* SK_RESTRICT src,
                              int width, int deltaSrc, int, const SkPMColor[]) {
    SkPMColor16* SK_RESTRICT dst = (SkPMColor16*)dstRow;
    unsigned alphaMask = 0xFF;

    for (int x = 0; x < width; x++) {
        unsigned alpha = src[3];
        SkPMColor c = SkPreMultiplyARGB(alpha, src[0], src[1], src[2]);
        dst[x] = SkPixel32ToPixel4444(c);
        src += deltaSrc;
        alphaMask &= alpha;
    }
    return alphaMask != 0xFF;
}

static bool Sample_RGBA_D4444_SkipZ(void* SK_RESTRICT dstRow,
                                    const uint8_t* SK_RESTRICT src,
                                    int width, int deltaSrc, int,
                                    const SkPMColor[]) {
    SkPMColor16* SK_RESTRICT dst = (SkPMColor16*)dstRow;
    unsigned alphaMask = 0xFF;

    for (int x = 0; x < width; x++) {
        unsigned alpha = src[3];
        if (alpha != 0) {
            SkPMColor c = SkPreMultiplyARGB(alpha, src[0], src[1], src[2]);
            dst[x] = SkPixel32ToPixel4444(c);
        }
        src += deltaSrc;
        alphaMask &= alpha;
    }
    return alphaMask != 0xFF;
}


static bool Sample_RGBA_D4444_D(void* SK_RESTRICT dstRow,
                                const uint8_t* SK_RESTRICT src,
                                int width, int deltaSrc, int y,
                                const SkPMColor[]) {
    SkPMColor16* SK_RESTRICT dst = (SkPMColor16*)dstRow;
    unsigned alphaMask = 0xFF;
    DITHER_4444_SCAN(y);

    for (int x = 0; x < width; x++) {
        unsigned alpha = src[3];
        SkPMColor c = SkPreMultiplyARGB(alpha, src[0], src[1], src[2]);
        dst[x] = SkDitherARGB32To4444(c, DITHER_VALUE(x));
        src += deltaSrc;
        alphaMask &= alpha;
    }
    return alphaMask != 0xFF;
}

static bool Sample_RGBA_D4444_D_SkipZ(void* SK_RESTRICT dstRow,
                                      const uint8_t* SK_RESTRICT src,
                                      int width, int deltaSrc, int y,
                                      const SkPMColor[]) {
    SkPMColor16* SK_RESTRICT dst = (SkPMColor16*)dstRow;
    unsigned alphaMask = 0xFF;
    DITHER_4444_SCAN(y);

    for (int x = 0; x < width; x++) {
        unsigned alpha = src[3];
        if (alpha != 0) {
            SkPMColor c = SkPreMultiplyARGB(alpha, src[0], src[1], src[2]);
            dst[x] = SkDitherARGB32To4444(c, DITHER_VALUE(x));
        }
        src += deltaSrc;
        alphaMask &= alpha;
    }
    return alphaMask != 0xFF;
}

static SkScaledBitmapSampler::RowProc get_RGBA_to_4444_proc(const SkImageDecoder& decoder) {
    if (decoder.getRequireUnpremultipliedColors()) {
        // Unpremultiplied is not supported for 4444
        return NULL;
    }
    const bool dither = decoder.getDitherImage();
    if (decoder.getSkipWritingZeroes()) {
        if (dither) {
            return Sample_RGBA_D4444_D_SkipZ;
        }
        return Sample_RGBA_D4444_SkipZ;
    }
    if (dither) {
        return Sample_RGBA_D4444_D;
    }
    return Sample_RGBA_D4444;
}

// Index

#define A32_MASK_IN_PLACE   (SkPMColor)(SK_A32_MASK << SK_A32_SHIFT)

static bool Sample_Index_D8888(void* SK_RESTRICT dstRow,
                               const uint8_t* SK_RESTRICT src,
                       int width, int deltaSrc, int, const SkPMColor ctable[]) {

    SkPMColor* SK_RESTRICT dst = (SkPMColor*)dstRow;
    SkPMColor cc = A32_MASK_IN_PLACE;
    for (int x = 0; x < width; x++) {
        SkPMColor c = ctable[*src];
        cc &= c;
        dst[x] = c;
        src += deltaSrc;
    }
    return cc != A32_MASK_IN_PLACE;
}

static bool Sample_Index_D8888_SkipZ(void* SK_RESTRICT dstRow,
                                     const uint8_t* SK_RESTRICT src,
                                     int width, int deltaSrc, int,
                                     const SkPMColor ctable[]) {

    SkPMColor* SK_RESTRICT dst = (SkPMColor*)dstRow;
    SkPMColor cc = A32_MASK_IN_PLACE;
    for (int x = 0; x < width; x++) {
        SkPMColor c = ctable[*src];
        cc &= c;
        if (c != 0) {
            dst[x] = c;
        }
        src += deltaSrc;
    }
    return cc != A32_MASK_IN_PLACE;
}

static SkScaledBitmapSampler::RowProc get_index_to_8888_proc(const SkImageDecoder& decoder) {
    if (decoder.getRequireUnpremultipliedColors()) {
        // Unpremultiplied is not supported for an index source.
        return NULL;
    }
    // Dither makes no difference
    if (decoder.getSkipWritingZeroes()) {
        return Sample_Index_D8888_SkipZ;
    }
    return Sample_Index_D8888;
}

static bool Sample_Index_D565(void* SK_RESTRICT dstRow,
                               const uint8_t* SK_RESTRICT src,
                       int width, int deltaSrc, int, const SkPMColor ctable[]) {

    uint16_t* SK_RESTRICT dst = (uint16_t*)dstRow;
    for (int x = 0; x < width; x++) {
        dst[x] = SkPixel32ToPixel16(ctable[*src]);
        src += deltaSrc;
    }
    return false;
}

static bool Sample_Index_D565_D(void* SK_RESTRICT dstRow,
                                const uint8_t* SK_RESTRICT src, int width,
                                int deltaSrc, int y, const SkPMColor ctable[]) {

    uint16_t* SK_RESTRICT dst = (uint16_t*)dstRow;
    DITHER_565_SCAN(y);

    for (int x = 0; x < width; x++) {
        SkPMColor c = ctable[*src];
        dst[x] = SkDitherRGBTo565(SkGetPackedR32(c), SkGetPackedG32(c),
                                  SkGetPackedB32(c), DITHER_VALUE(x));
        src += deltaSrc;
    }
    return false;
}

static SkScaledBitmapSampler::RowProc get_index_to_565_proc(const SkImageDecoder& decoder) {
    // Unpremultiplied and skip zeroes make no difference
    if (decoder.getDitherImage()) {
        return Sample_Index_D565_D;
    }
    return Sample_Index_D565;
}

static bool Sample_Index_D4444(void* SK_RESTRICT dstRow,
                               const uint8_t* SK_RESTRICT src, int width,
                               int deltaSrc, int y, const SkPMColor ctable[]) {

    SkPMColor16* SK_RESTRICT dst = (SkPMColor16*)dstRow;
    SkPMColor cc = A32_MASK_IN_PLACE;
    for (int x = 0; x < width; x++) {
        SkPMColor c = ctable[*src];
        cc &= c;
        dst[x] = SkPixel32ToPixel4444(c);
        src += deltaSrc;
    }
    return cc != A32_MASK_IN_PLACE;
}

static bool Sample_Index_D4444_D(void* SK_RESTRICT dstRow,
                                 const uint8_t* SK_RESTRICT src, int width,
                                int deltaSrc, int y, const SkPMColor ctable[]) {

    SkPMColor16* SK_RESTRICT dst = (SkPMColor16*)dstRow;
    SkPMColor cc = A32_MASK_IN_PLACE;
    DITHER_4444_SCAN(y);

    for (int x = 0; x < width; x++) {
        SkPMColor c = ctable[*src];
        cc &= c;
        dst[x] = SkDitherARGB32To4444(c, DITHER_VALUE(x));
        src += deltaSrc;
    }
    return cc != A32_MASK_IN_PLACE;
}

static bool Sample_Index_D4444_SkipZ(void* SK_RESTRICT dstRow,
                                     const uint8_t* SK_RESTRICT src, int width,
                                     int deltaSrc, int y, const SkPMColor ctable[]) {

    SkPMColor16* SK_RESTRICT dst = (SkPMColor16*)dstRow;
    SkPMColor cc = A32_MASK_IN_PLACE;
    for (int x = 0; x < width; x++) {
        SkPMColor c = ctable[*src];
        cc &= c;
        if (c != 0) {
            dst[x] = SkPixel32ToPixel4444(c);
        }
        src += deltaSrc;
    }
    return cc != A32_MASK_IN_PLACE;
}

static bool Sample_Index_D4444_D_SkipZ(void* SK_RESTRICT dstRow,
                                       const uint8_t* SK_RESTRICT src, int width,
                                       int deltaSrc, int y, const SkPMColor ctable[]) {

    SkPMColor16* SK_RESTRICT dst = (SkPMColor16*)dstRow;
    SkPMColor cc = A32_MASK_IN_PLACE;
    DITHER_4444_SCAN(y);

    for (int x = 0; x < width; x++) {
        SkPMColor c = ctable[*src];
        cc &= c;
        if (c != 0) {
            dst[x] = SkDitherARGB32To4444(c, DITHER_VALUE(x));
        }
        src += deltaSrc;
    }
    return cc != A32_MASK_IN_PLACE;
}

static SkScaledBitmapSampler::RowProc get_index_to_4444_proc(const SkImageDecoder& decoder) {
    // Unpremul not allowed
    if (decoder.getRequireUnpremultipliedColors()) {
        return NULL;
    }
    const bool dither = decoder.getDitherImage();
    if (decoder.getSkipWritingZeroes()) {
        if (dither) {
            return Sample_Index_D4444_D_SkipZ;
        }
        return Sample_Index_D4444_SkipZ;
    }
    if (dither) {
        return Sample_Index_D4444_D;
    }
    return Sample_Index_D4444;
}

static bool Sample_Index_DI(void* SK_RESTRICT dstRow,
                            const uint8_t* SK_RESTRICT src,
                            int width, int deltaSrc, int, const SkPMColor[]) {
    if (1 == deltaSrc) {
        memcpy(dstRow, src, width);
    } else {
        uint8_t* SK_RESTRICT dst = (uint8_t*)dstRow;
        for (int x = 0; x < width; x++) {
            dst[x] = src[0];
            src += deltaSrc;
        }
    }
    return false;
}

static SkScaledBitmapSampler::RowProc get_index_to_index_proc(const SkImageDecoder& decoder) {
    // Unpremul not allowed
    if (decoder.getRequireUnpremultipliedColors()) {
        return NULL;
    }
    // Ignore dither and skip zeroes
    return Sample_Index_DI;
}

// A8
static bool Sample_Gray_DA8(void* SK_RESTRICT dstRow,
                            const uint8_t* SK_RESTRICT src,
                            int width, int deltaSrc, int,
                            const SkPMColor[]) {
    // Sampling Gray to A8 uses the same function as Index to Index8,
    // except we assume that there is alpha for speed, since an A8
    // bitmap with no alpha is not interesting.
    (void) Sample_Index_DI(dstRow, src, width, deltaSrc, /* y unused */ 0,
                           /* ctable unused */ NULL);
    return true;
}

static SkScaledBitmapSampler::RowProc get_gray_to_A8_proc(const SkImageDecoder& decoder) {
    if (decoder.getRequireUnpremultipliedColors()) {
        return NULL;
    }
    // Ignore skip and dither.
    return Sample_Gray_DA8;
}

typedef SkScaledBitmapSampler::RowProc (*RowProcChooser)(const SkImageDecoder& decoder);
///////////////////////////////////////////////////////////////////////////////

#include "SkScaledBitmapSampler.h"

SkScaledBitmapSampler::SkScaledBitmapSampler(int width, int height,
                                             int sampleSize) {
    fCTable = NULL;
    fDstRow = NULL;
    fRowProc = NULL;

    if (width <= 0 || height <= 0) {
		abort();
    }

    SkDEBUGCODE(fSampleMode = kUninitialized_SampleMode);

    if (sampleSize <= 1) {
        fScaledWidth = width;
        fScaledHeight = height;
        fX0 = fY0 = 0;
        fDX = fDY = 1;
        return;
    }

    int dx = SkMin32(sampleSize, width);
    int dy = SkMin32(sampleSize, height);

    fScaledWidth = width / dx;
    fScaledHeight = height / dy;

    fX0 = dx >> 1;
    fY0 = dy >> 1;

    fDX = dx;
    fDY = dy;

}

bool SkScaledBitmapSampler::begin(SkBitmap* dst, SrcConfig sc,
                                  const SkImageDecoder& decoder,
                                  const SkPMColor ctable[]) {
    static const RowProcChooser gProcChoosers[] = {
        get_gray_to_8888_proc,
        get_RGBx_to_8888_proc,
        get_RGBA_to_8888_proc,
        get_index_to_8888_proc,
        NULL, // 565 to 8888

        get_gray_to_565_proc,
        get_RGBx_to_565_proc,
        get_RGBx_to_565_proc, // The source alpha will be ignored.
        get_index_to_565_proc,
        get_565_to_565_proc,

        get_gray_to_4444_proc,
        get_RGBx_to_4444_proc,
        get_RGBA_to_4444_proc,
        get_index_to_4444_proc,
        NULL, // 565 to 4444

        NULL, // gray to index
        NULL, // rgbx to index
        NULL, // rgba to index
        get_index_to_index_proc,
        NULL, // 565 to index

        get_gray_to_A8_proc,
        NULL, // rgbx to a8
        NULL, // rgba to a8
        NULL, // index to a8
        NULL, // 565 to a8
    };

    // The jump between dst configs in the table
    static const int gProcDstConfigSpan = 5;
    SK_COMPILE_ASSERT(SK_ARRAY_COUNT(gProcChoosers) == 5 * gProcDstConfigSpan,
                      gProcs_has_the_wrong_number_of_entries);

    fCTable = ctable;

    int index = 0;
    switch (sc) {
        case SkScaledBitmapSampler::kGray:
            fSrcPixelSize = 1;
            index += 0;
            break;
        case SkScaledBitmapSampler::kRGB:
            fSrcPixelSize = 3;
            index += 1;
            break;
        case SkScaledBitmapSampler::kRGBX:
            fSrcPixelSize = 4;
            index += 1;
            break;
        case SkScaledBitmapSampler::kRGBA:
            fSrcPixelSize = 4;
            index += 2;
            break;
        case SkScaledBitmapSampler::kIndex:
            fSrcPixelSize = 1;
            index += 3;
            break;
        case SkScaledBitmapSampler::kRGB_565:
            fSrcPixelSize = 2;
            index += 4;
            break;
        default:
            return false;
    }

    switch (dst->config()) {
        case SkBitmap::kARGB_8888_Config:
            index += 0 * gProcDstConfigSpan;
            break;
        case SkBitmap::kRGB_565_Config:
            index += 1 * gProcDstConfigSpan;
            break;
        case SkBitmap::kARGB_4444_Config:
            index += 2 * gProcDstConfigSpan;
            break;
        case SkBitmap::kIndex8_Config:
            index += 3 * gProcDstConfigSpan;
            break;
        case SkBitmap::kA8_Config:
            index += 4 * gProcDstConfigSpan;
            break;
        default:
            return false;
    }

    RowProcChooser chooser = gProcChoosers[index];
    if (NULL == chooser) {
        fRowProc = NULL;
    } else {
        fRowProc = chooser(decoder);
    }
    fDstRow = (char*)dst->getPixels();
    fDstRowBytes = dst->rowBytes();
    fCurrY = 0;
    return fRowProc != NULL;
}

bool SkScaledBitmapSampler::next(const uint8_t* SK_RESTRICT src) {
    SkDEBUGCODE(fSampleMode = kConsecutive_SampleMode);

    bool hadAlpha = fRowProc(fDstRow, src + fX0 * fSrcPixelSize, fScaledWidth,
                             fDX * fSrcPixelSize, fCurrY, fCTable);
    fDstRow += fDstRowBytes;
    fCurrY += 1;
    return hadAlpha;
}

bool SkScaledBitmapSampler::sampleInterlaced(const uint8_t* SK_RESTRICT src, int srcY) {
    SkDEBUGCODE(fSampleMode = kInterlaced_SampleMode);
    // Any line that should be a part of the destination can be created by the formula:
    // fY0 + (some multiplier) * fDY
    // so if srcY - fY0 is not an integer multiple of fDY that srcY will be skipped.
    const int srcYMinusY0 = srcY - fY0;
    if (srcYMinusY0 % fDY != 0) {
        // This line is not part of the output, so return false for alpha, since we have
        // not added an alpha to the output.
        return false;
    }
    // Unlike in next(), where the data is used sequentially, this function skips around,
    // so fDstRow and fCurrY are never updated. fDstRow must always be the starting point
    // of the destination bitmap's pixels, which is used to calculate the destination row
    // each time this function is called.
    const int dstY = srcYMinusY0 / fDY;
    char* dstRow = fDstRow + dstY * fDstRowBytes;
    return fRowProc(dstRow, src + fX0 * fSrcPixelSize, fScaledWidth,
                    fDX * fSrcPixelSize, dstY, fCTable);
}

