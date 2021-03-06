
/*
 * Copyright 2011 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#include "SampleCode.h"
#include "SkView.h"
#include "SkCanvas.h"
#include "SkGradientShader.h"
#include "SkPath.h"
#include "SkRegion.h"
#include "SkShader.h"
#include "SkUtils.h"
#include "SkComposeShader.h"
#include "Sk1DPathEffect.h"
#include "SkCornerPathEffect.h"
#include "SkPathMeasure.h"
#include "SkRandom.h"
#include "SkColorPriv.h"
#include "SkColorFilter.h"
#include "SkLayerRasterizer.h"
#include "SkImageDecoder.h"
#include "SkStream.h"
#include "SkImageDecoder.h"

#include "SkParsePath.h"
static void testparse()
{
    SkRect r;
    r.set(0, 0, 10, 10.5f);
    SkPath p, p2;
    SkString str, str2;

    p.addRect(r);
    SkParsePath::ToSVGString(p, &str);
    SkParsePath::FromSVGString(str.c_str(), &p2);
    SkParsePath::ToSVGString(p2, &str2);
}

SkImageDecoder* sk_libpng_dfactory(SkStreamRewindable* stream);

class ArcsView : public SampleView {
public:
    ArcsView() {
        //testparse();
        fSweep = SkIntToScalar(100);
        this->setBGColor(0xFFDDDDDD);
		SkFILEStream stream("c:/test_ba.png");
		SkImageDecoder* dec = sk_libpng_dfactory(&stream);
		//delete dec;
		SkImageDecoder *coder = SkImageDecoder::Factory(&stream);
		coder->decode(&stream, &fbmp, SkColorType::kRGBA_8888_SkColorType, SkImageDecoder::kDecodePixels_Mode);
		delete dec;
    }

protected:
    // overrides from SkEventSink
    virtual bool onQuery(SkEvent* evt) {
        if (SampleCode::TitleQ(*evt)) {
            SampleCode::TitleR(evt, "Arcs");
            return true;
        }
        return this->INHERITED::onQuery(evt);
    }

    static void drawRectWithLines(SkCanvas* canvas, const SkRect& r, const SkPaint& p) {
        canvas->drawRect(r, p);
        canvas->drawLine(r.fLeft, r.fTop, r.fRight, r.fBottom, p);
        canvas->drawLine(r.fLeft, r.fBottom, r.fRight, r.fTop, p);
        canvas->drawLine(r.fLeft, r.centerY(), r.fRight, r.centerY(), p);
        canvas->drawLine(r.centerX(), r.fTop, r.centerX(), r.fBottom, p);
    }

    static void draw_label(SkCanvas* canvas, const SkRect& rect,
                            int start, int sweep) {
        SkPaint paint;

        paint.setAntiAlias(true);
        paint.setTextAlign(SkPaint::kCenter_Align);

        SkString    str;

        str.appendS32(start);
        str.append(", ");
        str.appendS32(sweep);
        canvas->drawText(str.c_str(), str.size(), rect.centerX(),
                         rect.fBottom + paint.getTextSize() * 5/4, paint);
    }

    static void drawArcs(SkCanvas* canvas) {
        SkPaint paint;
        SkRect  r;
        SkScalar w = SkIntToScalar(75);
        SkScalar h = SkIntToScalar(50);

        r.set(0, 0, w, h);
        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::kStroke_Style);

        canvas->save();
        canvas->translate(SkIntToScalar(10), SkIntToScalar(300));

        paint.setStrokeWidth(SkIntToScalar(1));

        static const int gAngles[] = {
            0, 360,
            0, 45,
            0, -45,
            720, 135,
            -90, 269,
            -90, 270,
            -90, 271,
            -180, -270,
            225, 90
        };

        for (size_t i = 0; i < SK_ARRAY_COUNT(gAngles); i += 2) {
            paint.setColor(SK_ColorBLACK);
            drawRectWithLines(canvas, r, paint);

            paint.setColor(SK_ColorRED);
            canvas->drawArc(r, SkIntToScalar(gAngles[i]),
                            SkIntToScalar(gAngles[i+1]), false, paint);

            draw_label(canvas, r, gAngles[i], gAngles[i+1]);

            canvas->translate(w * 8 / 7, 0);
        }

        canvas->restore();
    }

    virtual void onDrawContent(SkCanvas* canvas) 
	{
		static long preMillis = 0;
		static int icou = 0;
		static char szTmp[20];

		SYSTEMTIME systime;
		GetSystemTime(&systime);
		long currentMillis = systime.wSecond * 1000 + systime.wMilliseconds;
		if ( icou == 0 )
		{
			long dur = currentMillis - preMillis;
			long FPS = (1000 * 5) / (dur);
			preMillis = currentMillis;
			sprintf(szTmp, "FPS:%d", FPS);
		}
		if ( icou ++ > 4 )
		{
			icou = 0;
		}
		
		SkPaint paint;
		paint.setTextSize(20);
		paint.setColor(0xFF008000);
		canvas->drawText(szTmp, strlen( szTmp), 700, 50, paint);

		

//        fSweep = SampleCode::GetAnimScalar(SkIntToScalar(360)/24,
//                                           SkIntToScalar(360));
////        fSweep = 359.99f;
//
//        SkRect  r;
		//SkPaint paint;
//
//        paint.setAntiAlias(true);
//        paint.setStyle(SkPaint::kStroke_Style);
//
//        r.set(0, 0, 200, 200);
//        r.offset(100, 100);
//		paint.setColor(0x800000FF);
//		paint.setStrokeWidth( 10 );
//		paint.setStyle(SkPaint::kStroke_Style);
//		//canvas->drawRect(r, paint);
//		//canvas->drawCircle(200, 200, 100, paint);
//		paint.setStrokeJoin(SkPaint::kMiter_Join);
//		SkPath path;
//		path.moveTo(100, 100);
//		path.lineTo(100, 200);
//		path.lineTo(150, 200);
//		canvas->drawPath(path, paint);
		//return;
		//canvas->save();
		//paint.setColor(0x80ff0000);
		//canvas->drawLine(200, 200, 100, 200, paint);
		//canvas->restore();
		////canvas->drawLine(100, 200, 200, 200, paint);
		//canvas->drawArc(r, 0, 90, true, paint);
		//return;


        if (true )
		{
   //         const SkScalar d = SkIntToScalar(3);
   //         const SkScalar rad[] = { d, d, d, d, d, d, d, d };
   //         SkPath path;
   //         path.addRoundRect(r, rad);
   //         //canvas->drawPath(path, paint);
			//paint.setStyle(SkPaint::kStroke_Style);
			//paint.setTextSize(40);
			//canvas->drawText("test", 4, 100, 100, paint );
			for (int i = 0; i < 2000; i++ )
			{
				int fx = rand();
				int fy = rand();
				canvas->drawBitmap(fbmp, fx % 500, fy % 300, &paint);
			}
			
	
			////drawArcs(canvas);
			//paint.setStrokeWidth(0);
			//paint.setColor(SK_ColorBLUE);
			//canvas->drawArc(r, 0, fSweep, false, paint);
			//paint.setStyle(SkPaint::kStroke_Style);
			//canvas->drawCircle(250, 250, 100, paint);
			////paint.setStyle(SkPaint::kFill_Style);
			//paint.setColor(0x800000FF);
			//canvas->drawArc(r, 0, fSweep, true, paint);
			this->inval(NULL);
            return;
        }

        //drawRectWithLines(canvas, r, paint);

   //     printf("----- sweep %g %X\n", SkScalarToFloat(fSweep), SkDegreesToRadians(fSweep));


        //paint.setStyle(SkPaint::kFill_Style);
        //paint.setColor(0x800000FF);
        //canvas->drawArc(r, 0, fSweep, true, paint);

        //paint.setColor(0x800FF000);
        //canvas->drawArc(r, 0, fSweep, false, paint);

        //paint.setStyle(SkPaint::kStroke_Style);
        //paint.setColor(SK_ColorRED);
        //canvas->drawArc(r, 0, fSweep, true, paint);

        //paint.setStrokeWidth(0);
        //paint.setColor(SK_ColorBLUE);
        //canvas->drawArc(r, 0, fSweep, false, paint);

        //drawArcs(canvas);
        this->inval(NULL);
    }

    virtual SkView::Click* onFindClickHandler(SkScalar x, SkScalar y,
                                              unsigned modi) {
     //   fSweep += SK_Scalar1;
        this->inval(NULL);
        return this->INHERITED::onFindClickHandler(x, y, modi);
    }

    virtual bool onClick(Click* click) {
        return this->INHERITED::onClick(click);
    }

private:
    SkScalar fSweep;
	SkBitmap fbmp;

    typedef SampleView INHERITED;
};

//////////////////////////////////////////////////////////////////////////////

static SkView* MyFactory() { return new ArcsView; }
static SkViewRegister reg(MyFactory);
