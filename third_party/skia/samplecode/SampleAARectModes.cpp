/*
 * Copyright 2011 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SampleCode.h"
#include "SkView.h"
#include "SkCanvas.h"
#include "SkDevice.h"
#include "SkColorPriv.h"
#include "SkShader.h"
#include "CanvasRenderingContext2D.h"
using namespace WebCore;
using namespace WTF;


static const struct 
{
    SkXfermode::Mode  fMode;
    const char*         fLabel;
} 
gModes[] =
{
    { SkXfermode::kClear_Mode,    "Clear"     },
    { SkXfermode::kSrc_Mode,      "Src"       },
    { SkXfermode::kDst_Mode,      "Dst"       },
    { SkXfermode::kSrcOver_Mode,  "SrcOver"   },
    { SkXfermode::kDstOver_Mode,  "DstOver"   },
    { SkXfermode::kSrcIn_Mode,    "SrcIn"     },
    { SkXfermode::kDstIn_Mode,    "DstIn"     },
    { SkXfermode::kSrcOut_Mode,   "SrcOut"    },
    { SkXfermode::kDstOut_Mode,   "DstOut"    },
    { SkXfermode::kSrcATop_Mode,  "SrcATop"   },
    { SkXfermode::kDstATop_Mode,  "DstATop"   },
    { SkXfermode::kXor_Mode,      "Xor"       },
};

const int gWidth = 64;
const int gHeight = 64;
const float W = SkIntToScalar(gWidth);
const float H = SkIntToScalar(gHeight);

static float drawCell(SkCanvas* canvas, SkXfermode* mode,
                         SkAlpha a0, SkAlpha a1) {

    SkPaint paint;
	paint.setColor(0xFF0000ff);

	paint.setAntiAlias(true);

	SkRect r = SkRect::MakeWH(W, H);
	r.inset(W / 10, H / 10);

	paint.setColor(SK_ColorBLUE);
	paint.setAlpha(a0);
	canvas->drawOval(r, paint);

	paint.setColor(SK_ColorRED);
	paint.setAlpha(a1);
	paint.setXfermode(mode);

    float offset = SK_Scalar1 / 3;
	SkRect rect = SkRect::MakeXYWH(W / 4 + offset,
		H / 4 + offset,
		W / 2, H / 2);
    canvas->drawRect(rect, paint);

    return H;
}

static SkShader* make_bg_shader() 
{
    SkBitmap bm;
    bm.setConfig(SkBitmap::kARGB_8888_Config, 2, 2);
    bm.allocPixels();
    *bm.getAddr32(0, 0) = *bm.getAddr32(1, 1) = 0xFFFFffff;
    *bm.getAddr32(1, 0) = *bm.getAddr32(0, 1) = SkPackARGB32(0xFF, 0xCC,
                                                             0xCC, 0xCC);

    SkShader* s = SkShader::CreateBitmapShader(bm,
                                               SkShader::kRepeat_TileMode,
                                               SkShader::kRepeat_TileMode);

    SkMatrix m;
    m.setScale(SkIntToScalar(6), SkIntToScalar(6));
    s->setLocalMatrix(m);
    return s;
}

class AARectsModesView : public SampleView
{
    SkPaint fBGPaint;
public:
    AARectsModesView () {
        fBGPaint.setShader(make_bg_shader())->unref();
    }

protected:
    // overrides from SkEventSink
    virtual bool onQuery(SkEvent* evt)
	{
        if (SampleCode::TitleQ(*evt)) 
		{
            SampleCode::TitleR(evt, "AARectsModes");
            return true;
        }
        return this->INHERITED::onQuery(evt);
    }

    virtual void onDrawContent(SkCanvas* canvas)
	{
        const SkRect bounds = SkRect::MakeWH(W, H);
        static const SkAlpha gAlphaValue[] = { 0xFF, 0x88, 0x88 };

        canvas->translate(SkIntToScalar(4), SkIntToScalar(4));

        for (int alpha = 0; alpha < 4; ++alpha) {
            canvas->save();
            canvas->save();
            for (size_t i = 0; i < SK_ARRAY_COUNT(gModes); ++i) {
                if (6 == i) {
                    canvas->restore();
                    canvas->translate(W * 5, 0);
                    canvas->save();
                }
                SkXfermode* mode = SkXfermode::Create(gModes[i].fMode);

			
                canvas->drawRect(bounds, fBGPaint);
                canvas->saveLayer(&bounds, NULL);
                float dy = drawCell(canvas, mode, gAlphaValue[alpha & 1], gAlphaValue[alpha & 2]);
			
				SkPaint paint;
				paint.setColor(0xFFff0000);
				SkRect rect = SkRect::MakeXYWH(W / 4, H / 4, W / 2, H / 2);
				canvas->drawRect(rect, paint);
                canvas->restore();

                canvas->translate(0, dy * 5 / 4);
                SkSafeUnref(mode);
            }
            canvas->restore();
            canvas->restore();
            canvas->translate(W * 5 / 4, 0);
			
        }

		SkPaint paint;
		paint.setColor(0xFF0000ff);
		paint.setStyle(SkPaint::kFill_Style);
		canvas->translate(0, 0);
		SkPath path;
		path.moveTo(10, 10);
		path.lineTo(300, 300);
		path.lineTo(100, 10);
		canvas->drawPath(path, paint);
		//canvas->drawLine(0, 0, 100, 200, paint);

		
	
		PassOwnPtr<CanvasRenderingContext2D> ctx = CanvasRenderingContext2D::create(canvas, NULL, false);
		Color color(255, 102, 234);
		ctx->setStrokeColor(1.0f, 1.0f, 0.0, 1.0f);
		ctx->moveTo(10, 10);
		ctx->lineTo(100, 100);
		ctx->stroke();
    }

private:
    typedef SampleView INHERITED;
};

///////////////////////////////////////////////////////////////////////////////

static SkView* MyFactory() { return new AARectsModesView; }
static SkViewRegister reg(MyFactory);
