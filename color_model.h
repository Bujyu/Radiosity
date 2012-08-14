#ifndef _COLOR_MODEL_H
#define _COLOR_MODEL_H

#include "RGB_model.h"
#include "HSL_model.h"
#include "HSV_model.h"
#include "CMYK_model.h"

// Other color model To RGB
RGB HSLtoRGB( HSL color );
RGB HSVtoRGB( HSV color );
RGB CMYKtoRGB( CMYK color );

// RGB To other color model
HSL RGBtoHSL( RGB color );
HSV RGBtoHSV( RGB color );
CMYK RGBtoCMYK( RGB color );

#endif
