/* ***** BEGIN LICENSE BLOCK *****
 * This file is part of Natron <http://www.natron.fr/>,
 * Copyright (C) 2013-2017 INRIA and Alexandre Gauthier-Foichat
 *
 * Natron is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Natron is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Natron.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>
 * ***** END LICENSE BLOCK ***** */

#ifndef ROTOCONTEXTPRIVATE_H
#define ROTOCONTEXTPRIVATE_H

// ***** BEGIN PYTHON BLOCK *****
// from <https://docs.python.org/3/c-api/intro.html#include-files>:
// "Since Python may define some pre-processor definitions which affect the standard headers on some systems, you must include Python.h before any standard headers are included."
#include <Python.h>
// ***** END PYTHON BLOCK *****

#include "Global/Macros.h"

#include <list>
#include <map>
#include <string>
#include <vector>
#include <limits>
#include <sstream> // stringstream
#include <stdexcept>

#if !defined(Q_MOC_RUN) && !defined(SBK_RUN)
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#endif

CLANG_DIAG_OFF(deprecated)
CLANG_DIAG_OFF(uninitialized)
#include <QtCore/QMutex>
#include <QtCore/QReadWriteLock>
#include <QtCore/QCoreApplication>
#include <QtCore/QThread>
#include <QtCore/QReadWriteLock>
#include <QtCore/QWaitCondition>
CLANG_DIAG_ON(deprecated)
CLANG_DIAG_ON(uninitialized)

#include <cairo/cairo.h>

#include "Global/GlobalDefines.h"

#include "Engine/AppManager.h"
#include "Engine/BezierCP.h"
#include "Engine/Curve.h"
#include "Engine/EffectInstance.h"
#include "Engine/Image.h"
#include "Engine/KnobTypes.h"
#include "Engine/MergingEnum.h"
#include "Engine/Node.h"
#include "Engine/RotoContext.h"
#include "Engine/RotoPaint.h"
#include "Engine/Transform.h"
#include "Engine/ViewIdx.h"
#include "Engine/EngineFwd.h"

#define ROTO_DEFAULT_OPACITY 1.
#define ROTO_DEFAULT_FEATHER 1.5
#define ROTO_DEFAULT_FEATHERFALLOFF 1.
#define ROTO_DEFAULT_COLOR_R 1.
#define ROTO_DEFAULT_COLOR_G 1.
#define ROTO_DEFAULT_COLOR_B 1.


#define kRotoScriptNameHint "Script-name of the item for Python scripts. It cannot be edited."

#define kRotoLabelHint "Label of the layer or curve"

#define kRotoNameHint "Name of the layer or curve."


#define kRotoOpacityParam "opacity"
#define kRotoOpacityParamLabel "Opacity"
#define kRotoOpacityHint \
    "Controls the opacity of the selected shape(s)."

#define kRotoFeatherParam "feather"
#define kRotoFeatherParamLabel "Feather"
#define kRotoFeatherHint \
    "Controls the distance of feather (in pixels) to add around the selected shape(s)"

#define kRotoFeatherFallOffParam "featherFallOff"
#define kRotoFeatherFallOffParamLabel "Feather fall-off"
#define kRotoFeatherFallOffHint \
    "Controls the rate at which the feather is applied on the selected shape(s)."

#define kRotoActivatedParam "activated"
#define kRotoActivatedParamLabel "Activated"
#define kRotoActivatedHint \
    "Controls whether the selected shape(s) should be rendered or not." \
    "Note that you can animate this parameter so you can activate/deactive the shape " \
    "throughout the time."

#define kRotoLockedHint \
    "Control whether the layer/curve is editable or locked."

#define kRotoInvertedParam "inverted"
#define kRotoInvertedParamLabel "Inverted"

#define kRotoInvertedHint \
    "Controls whether the selected shape(s) should be inverted. When inverted everything " \
    "outside the shape will be set to 1 and everything inside the shape will be set to 0."

#define kRotoOverlayHint "Color of the display overlay for this curve. Doesn't affect output."

#define kRotoColorParam "color"
#define kRotoColorParamLabel "Color"
#define kRotoColorHint \
    "The color of the shape. This parameter is used when the output components are set to RGBA."

#define kRotoCompOperatorParam "operator"
#define kRotoCompOperatorParamLabel "Operator"
#define kRotoCompOperatorHint \
    "The compositing operator controls how this shape is merged with the shapes that have already been rendered.\n" \
    "The roto mask is initialised as black and transparent, then each shape is drawn in the selected order, with the selected color and operator.\n" \
    "Finally, the mask is composed with the source image, if connected, using the 'over' operator.\n" \
    "See http://cairographics.org/operators/ for a full description of available operators."

#define kRotoBrushSourceColor "sourceType"
#define kRotoBrushSourceColorLabel "Source"
#define kRotoBrushSourceColorHint "Source color used for painting the stroke when the Reveal/Clone tools are used:\n" \
    "- foreground: the painted result at this point in the hierarchy\n" \
    "- background: the original image unpainted connected to bg\n" \
    "- backgroundN: the original image unpainted connected to bgN\n"

#define kRotoBrushSizeParam "brushSize"
#define kRotoBrushSizeParamLabel "Brush Size"
#define kRotoBrushSizeParamHint "This is the diameter of the brush in pixels. Shift + drag on the viewer to modify this value"

#define kRotoBrushSpacingParam "brushSpacing"
#define kRotoBrushSpacingParamLabel "Brush Spacing"
#define kRotoBrushSpacingParamHint "Spacing between stamps of the paint brush"

#define kRotoBrushHardnessParam "brushHardness"
#define kRotoBrushHardnessParamLabel "Brush Hardness"
#define kRotoBrushHardnessParamHint "Fall off of the brush effect from the center to the edge"

#define kRotoBrushEffectParam "brushEffect"
#define kRotoBrushEffectParamLabel "Brush effect"
#define kRotoBrushEffectParamHint "The strength of the effect"

#define kRotoBrushVisiblePortionParam "strokeVisiblePortion"
#define kRotoBrushVisiblePortionParamLabel "Visible portion"
#define kRotoBrushVisiblePortionParamHint "Defines the range of the stroke that should be visible: 0 is the start of the stroke and 1 the end."

#define kRotoBrushPressureLabelParam "pressureAlters"
#define kRotoBrushPressureLabelParamLabel "Pressure alters"
#define kRotoBrushPressureLabelParamHint ""

#define kRotoBrushPressureOpacityParam "pressureOpacity"
#define kRotoBrushPressureOpacityParamLabel "Opacity"
#define kRotoBrushPressureOpacityParamHint "Alters the opacity of the paint brush proportionate to changes in pen pressure"

#define kRotoBrushPressureSizeParam "pressureSize"
#define kRotoBrushPressureSizeParamLabel "Size"
#define kRotoBrushPressureSizeParamHint "Alters the size of the paint brush proportionate to changes in pen pressure"

#define kRotoBrushPressureHardnessParam "pressureHardness"
#define kRotoBrushPressureHardnessParamLabel "Hardness"
#define kRotoBrushPressureHardnessParamHint "Alters the hardness of the paint brush proportionate to changes in pen pressure"

#define kRotoBrushBuildupParam "buildUp"
#define kRotoBrushBuildupParamLabel "Build-up"
#define kRotoBrushBuildupParamHint "When checked, the paint stroke builds up when painted over itself"

#define kRotoBrushTimeOffsetParam "timeOffset"
#define kRotoBrushTimeOffsetParamLabel "Clone time offset"
#define kRotoBrushTimeOffsetParamHint "When the Clone tool is used, this determines depending on the time offset mode the source frame to " \
    "clone. When in absolute mode, this is the frame number of the source, when in relative mode, this is an offset relative to the current frame."

#define kRotoBrushTimeOffsetModeParam "timeOffsetMode"
#define kRotoBrushTimeOffsetModeParamLabel "Mode"
#define kRotoBrushTimeOffsetModeParamHint "Time offset mode: when in absolute mode, this is the frame number of the source, when in relative mode, this is an offset relative to the current frame."

#define kRotoBrushTranslateParam "cloneTranslate"
#define kRotoBrushTranslateParamLabel "Translate"
#define kRotoBrushTranslateParamHint ""

#define kRotoBrushRotateParam "cloneRotate"
#define kRotoBrushRotateParamLabel "Rotate"
#define kRotoBrushRotateParamHint ""

#define kRotoBrushScaleParam "cloneScale"
#define kRotoBrushScaleParamLabel "Scale"
#define kRotoBrushScaleParamHint ""

#define kRotoBrushScaleUniformParam "cloneUniform"
#define kRotoBrushScaleUniformParamLabel "Uniform"
#define kRotoBrushScaleUniformParamHint ""

#define kRotoBrushSkewXParam "cloneSkewx"
#define kRotoBrushSkewXParamLabel "Skew X"
#define kRotoBrushSkewXParamHint ""

#define kRotoBrushSkewYParam "cloneSkewy"
#define kRotoBrushSkewYParamLabel "Skew Y"
#define kRotoBrushSkewYParamHint ""

#define kRotoBrushSkewOrderParam "cloneSkewOrder"
#define kRotoBrushSkewOrderParamLabel "Skew Order"
#define kRotoBrushSkewOrderParamHint ""

#define kRotoBrushCenterParam "cloneCenter"
#define kRotoBrushCenterParamLabel "Center"
#define kRotoBrushCenterParamHint ""

#define kRotoBrushFilterParam "cloneFilter"
#define kRotoBrushFilterParamLabel "Filter"
#define kRotoBrushFilterParamHint "Filtering algorithm - some filters may produce values outside of the initial range (*) or modify the values even if there is no movement (+)."

#define kRotoBrushBlackOutsideParam "blackOutside"
#define kRotoBrushBlackOutsideParamLabel "Black Outside"
#define kRotoBrushBlackOutsideParamHint "Fill the area outside the source image with black"

#define kFilterImpulse "Impulse"
#define kFilterImpulseHint "(nearest neighbor / box) Use original values"
#define kFilterBilinear "Bilinear"
#define kFilterBilinearHint "(tent / triangle) Bilinear interpolation between original values"
#define kFilterCubic "Cubic"
#define kFilterCubicHint "(cubic spline) Some smoothing"
#define kFilterKeys "Keys"
#define kFilterKeysHint "(Catmull-Rom / Hermite spline) Some smoothing, plus minor sharpening (*)"
#define kFilterSimon "Simon"
#define kFilterSimonHint "Some smoothing, plus medium sharpening (*)"
#define kFilterRifman "Rifman"
#define kFilterRifmanHint "Some smoothing, plus significant sharpening (*)"
#define kFilterMitchell "Mitchell"
#define kFilterMitchellHint "Some smoothing, plus blurring to hide pixelation (*+)"
#define kFilterParzen "Parzen"
#define kFilterParzenHint "(cubic B-spline) Greatest smoothing of all filters (+)"
#define kFilterNotch "Notch"
#define kFilterNotchHint "Flat smoothing (which tends to hide moire' patterns) (+)"


#define kRotoDrawableItemTranslateParam "translate"
#define kRotoDrawableItemTranslateParamLabel "Translate"
#define kRotoDrawableItemTranslateParamHint ""

#define kRotoDrawableItemRotateParam "rotate"
#define kRotoDrawableItemRotateParamLabel "Rotate"
#define kRotoDrawableItemRotateParamHint ""

#define kRotoDrawableItemScaleParam "scale"
#define kRotoDrawableItemScaleParamLabel "Scale"
#define kRotoDrawableItemScaleParamHint ""

#define kRotoDrawableItemScaleUniformParam "uniform"
#define kRotoDrawableItemScaleUniformParamLabel "Uniform"
#define kRotoDrawableItemScaleUniformParamHint ""

#define kRotoDrawableItemSkewXParam "skewx"
#define kRotoDrawableItemSkewXParamLabel "Skew X"
#define kRotoDrawableItemSkewXParamHint ""

#define kRotoDrawableItemSkewYParam "skewy"
#define kRotoDrawableItemSkewYParamLabel "Skew Y"
#define kRotoDrawableItemSkewYParamHint ""

#define kRotoDrawableItemSkewOrderParam "skewOrder"
#define kRotoDrawableItemSkewOrderParamLabel "Skew Order"
#define kRotoDrawableItemSkewOrderParamHint ""

#define kRotoDrawableItemCenterParam "center"
#define kRotoDrawableItemCenterParamLabel "Center"
#define kRotoDrawableItemCenterParamHint ""

#define kRotoDrawableItemExtraMatrixParam "extraMatrix"
#define kRotoDrawableItemExtraMatrixParamLabel "Extra Matrix"
#define kRotoDrawableItemExtraMatrixParamHint "This matrix gets concatenated to the transform resulting from the parameter above."

#define kRotoDrawableItemLifeTimeParam "lifeTime"
#define kRotoDrawableItemLifeTimeParamLabel "Life Time"
#define kRotoDrawableItemLifeTimeParamHint "Controls the life-time of the shape/stroke"

#define kRotoDrawableItemLifeTimeAll "All"
#define kRotoDrawableItemLifeTimeAllHelp "All frames"

#define kRotoDrawableItemLifeTimeSingle "Single"
#define kRotoDrawableItemLifeTimeSingleHelp "Only for the specified frame"

#define kRotoDrawableItemLifeTimeFromStart "From start"
#define kRotoDrawableItemLifeTimeFromStartHelp "From the start of the sequence up to the specified frame"

#define kRotoDrawableItemLifeTimeToEnd "To end"
#define kRotoDrawableItemLifeTimeToEndHelp "From the specified frame to the end of the sequence"

#define kRotoDrawableItemLifeTimeCustom "Custom"
#define kRotoDrawableItemLifeTimeCustomHelp "Use the Activated parameter animation to control the life-time of the shape/stroke using keyframes"

#define kRotoDrawableItemLifeTimeFrameParam "lifeTimeFrame"
#define kRotoDrawableItemLifeTimeFrameParamLabel "Frame"
#define kRotoDrawableItemLifeTimeFrameParamHint "Use this to specify the frame when in mode Single/From start/To end"

#define kRotoResetCloneTransformParam "resetCloneTransform"
#define kRotoResetCloneTransformParamLabel "Reset Transform"
#define kRotoResetCloneTransformParamHint "Reset the clone transform to an identity"

#define kRotoResetTransformParam "resetTransform"
#define kRotoResetTransformParamLabel "Reset Transform"
#define kRotoResetTransformParamHint "Reset the transform to an identity"

#define kRotoResetCloneCenterParam "resetCloneCenter"
#define kRotoResetCloneCenterParamLabel "Reset Center"
#define kRotoResetCloneCenterParamHint "Reset the clone transform center"

#define kRotoResetCenterParam "resetTransformCenter"
#define kRotoResetCenterParamLabel "Reset Center"
#define kRotoResetCenterParamHint "Reset the transform center"

#define kRotoTransformInteractive "RotoTransformInteractive"
#define kRotoTransformInteractiveLabel "Interactive"
#define kRotoTransformInteractiveHint "When check, modifying the transform will directly render the shape in the viewer. When unchecked, modifications are applied when releasing the mouse button."

#define kRotoMotionBlurModeParam "motionBlurMode"
#define kRotoMotionBlurModeParamLabel "Mode"
#define kRotoMotionBlurModeParamHint "Per-shape motion blurs applies motion blur independently to each shape and then blends them together." \
    " This may produce artifacts when shapes blur over the same portion of the image, but might be more efficient than global motion-blur." \
    " Global motion-blur takes into account the interaction between shapes and will not create artifacts at the expense of being slightly " \
    "more expensive than the per-shape motion blur. Note that when using the global motion-blur, all shapes will have the same motion-blur " \
    "settings applied to them."

#define kRotoPerShapeMotionBlurParam "motionBlur"
#define kRotoGlobalMotionBlurParam "globalMotionBlur"
#define kRotoMotionBlurParamLabel "Motion Blur"
#define kRotoMotionBlurParamHint "The number of Motion-Blur samples used for blurring. Increase for better quality but slower rendering."

#define kRotoPerShapeShutterParam "motionBlurShutter"
#define kRotoGlobalShutterParam "globalMotionBlurShutter"
#define kRotoShutterParamLabel "Shutter"
#define kRotoShutterParamHint "The number of frames during which the shutter should be opened when motion blurring."

#define kRotoPerShapeShutterOffsetTypeParam "motionBlurShutterOffset"
#define kRotoGlobalShutterOffsetTypeParam "gobalMotionBlurShutterOffset"
#define kRotoShutterOffsetTypeParamLabel "Shutter Offset"
#define kRotoShutterOffsetTypeParamHint "This controls how the shutter operates in respect to the current frame value."

#define kRotoShutterOffsetCenteredHint "Centers the shutter around the current frame, that is the shutter will be opened from f - shutter/2 to " \
    "f + shutter/2"
#define kRotoShutterOffsetStartHint "The shutter will open at the current frame and stay open until f + shutter"
#define kRotoShutterOffsetEndHint "The shutter will open at f - shutter until the current frame"
#define kRotoShutterOffsetCustomHint "The shutter will open at the time indicated by the shutter offset parameter"

#define kRotoPerShapeShutterCustomOffsetParam "motionBlurCustomShutterOffset"
#define kRotoGlobalShutterCustomOffsetParam "globalMotionBlurCustomShutterOffset"
#define kRotoShutterCustomOffsetParamLabel "Custom Offset"
#define kRotoShutterCustomOffsetParamHint "If the Shutter Offset parameter is set to Custom then this parameter controls the frame at " \
    "which the shutter opens. The value is an offset in frames to the current frame, e.g: -1  would open the shutter 1 frame before the current frame."


NATRON_NAMESPACE_ENTER;

struct RotoFeatherVertex
{
    double x,y;
    bool isInner;
};

struct RotoTriangleStrips
{
    std::list<Point> vertices;
};

struct RotoTriangleFans
{
    std::list<Point> vertices;
};

struct RotoTriangles
{
    std::list<Point> vertices;
};

struct BezierPrivate
{
    BezierCPs points; //< the control points of the curve
    BezierCPs featherPoints; //< the feather points, the number of feather points must equal the number of cp.

    //updated whenever the Bezier is edited, this is used to determine if a point lies inside the bezier or not
    //it has a value for each keyframe
    mutable std::map<double, bool> isClockwiseOriented;
    mutable bool isClockwiseOrientedStatic; //< used when the bezier has no keyframes
    mutable std::map<double, bool> guiIsClockwiseOriented;
    mutable bool guiIsClockwiseOrientedStatic; //< used when the bezier has no keyframes
    bool autoRecomputeOrientation; // when true, orientation will be computed automatically on editing
    bool finished; //< when finished is true, the last point of the list is connected to the first point of the list.
    bool isOpenBezier;
    mutable QMutex guiCopyMutex;
    bool mustCopyGui;

    BezierPrivate(bool isOpenBezier)
        : points()
        , featherPoints()
        , isClockwiseOriented()
        , isClockwiseOrientedStatic(false)
        , guiIsClockwiseOriented()
        , guiIsClockwiseOrientedStatic(false)
        , autoRecomputeOrientation(true)
        , finished(false)
        , isOpenBezier(isOpenBezier)
        , guiCopyMutex()
        , mustCopyGui(false)
    {
    }

    void setMustCopyGuiBezier(bool copy)
    {
        QMutexLocker k(&guiCopyMutex);

        mustCopyGui = copy;
    }

    bool hasKeyframeAtTime(bool useGuiCurves,
                           double time) const
    {
        // PRIVATE - should not lock

        if ( points.empty() ) {
            return false;
        } else {
            KeyFrame k;

            return points.front()->hasKeyFrameAtTime(useGuiCurves, time);
        }
    }

    void getKeyframeTimes(bool useGuiCurves,
                          std::set<double>* times) const
    {
        // PRIVATE - should not lock

        if ( points.empty() ) {
            return;
        }
        points.front()->getKeyframeTimes(useGuiCurves, times);
    }

    BezierCPs::const_iterator atIndex(int index) const
    {
        // PRIVATE - should not lock

        if ( ( index >= (int)points.size() ) || (index < 0) ) {
            throw std::out_of_range("RotoSpline::atIndex: non-existent control point");
        }

        BezierCPs::const_iterator it = points.begin();
        std::advance(it, index);

        return it;
    }

    BezierCPs::iterator atIndex(int index)
    {
        // PRIVATE - should not lock

        if ( ( index >= (int)points.size() ) || (index < 0) ) {
            throw std::out_of_range("RotoSpline::atIndex: non-existent control point");
        }

        BezierCPs::iterator it = points.begin();
        std::advance(it, index);

        return it;
    }

    BezierCPs::const_iterator findControlPointNearby(double x,
                                                     double y,
                                                     double acceptance,
                                                     double time,
                                                     ViewIdx view,
                                                     const Transform::Matrix3x3& transform,
                                                     int* index) const
    {
        // PRIVATE - should not lock
        int i = 0;

        for (BezierCPs::const_iterator it = points.begin(); it != points.end(); ++it, ++i) {
            Transform::Point3D p;
            p.z = 1;
            (*it)->getPositionAtTime(true, time, view, &p.x, &p.y);
            p = Transform::matApply(transform, p);
            if ( ( p.x >= (x - acceptance) ) && ( p.x <= (x + acceptance) ) && ( p.y >= (y - acceptance) ) && ( p.y <= (y + acceptance) ) ) {
                *index = i;

                return it;
            }
        }

        return points.end();
    }

    BezierCPs::const_iterator findFeatherPointNearby(double x,
                                                     double y,
                                                     double acceptance,
                                                     double time,
                                                     ViewIdx view,
                                                     const Transform::Matrix3x3& transform,
                                                     int* index) const
    {
        // PRIVATE - should not lock
        int i = 0;

        for (BezierCPs::const_iterator it = featherPoints.begin(); it != featherPoints.end(); ++it, ++i) {
            Transform::Point3D p;
            p.z = 1;
            (*it)->getPositionAtTime(true, time, view, &p.x, &p.y);
            p = Transform::matApply(transform, p);
            if ( ( p.x >= (x - acceptance) ) && ( p.x <= (x + acceptance) ) && ( p.y >= (y - acceptance) ) && ( p.y <= (y + acceptance) ) ) {
                *index = i;

                return it;
            }
        }

        return featherPoints.end();
    }
};

class RotoLayer;
struct RotoItemPrivate
{
    boost::weak_ptr<RotoContext> context;
    std::string scriptName, label;
    boost::weak_ptr<RotoLayer> parentLayer;

    ////This controls whether the item (and all its children if it is a layer)
    ////should be visible/rendered or not at any time.
    ////This is different from the "activated" knob for RotoDrawableItem's which in that
    ////case allows to define a life-time
    bool globallyActivated;

    ////A locked item should not be modifiable by the GUI
    bool locked;

    RotoItemPrivate(const boost::shared_ptr<RotoContext> context,
                    const std::string & n,
                    const boost::shared_ptr<RotoLayer>& parent)
        : context(context)
        , scriptName(n)
        , label(n)
        , parentLayer(parent)
        , globallyActivated(true)
        , locked(false)
    {
    }
};

typedef std::list< boost::shared_ptr<RotoItem> > RotoItems;

struct RotoLayerPrivate
{
    RotoItems items;

    RotoLayerPrivate()
        : items()
    {
    }
};


///Keep this in synch with the cairo_operator_t enum !
///We are not going to create a similar enum just to represent the same thing
inline void
getCairoCompositingOperators(std::vector<std::string>* operators,
                             std::vector<std::string>* toolTips)
{
    assert(operators->size() == CAIRO_OPERATOR_CLEAR);
    operators->push_back("clear");
    toolTips->push_back("clear destination layer");

    assert(operators->size() == CAIRO_OPERATOR_SOURCE);
    operators->push_back("source");
    toolTips->push_back("replace destination layer");

    assert(operators->size() == CAIRO_OPERATOR_OVER);
    operators->push_back("over");
    toolTips->push_back("draw source layer on top of destination layer ");

    assert(operators->size() == CAIRO_OPERATOR_IN);
    operators->push_back("in");
    toolTips->push_back("draw source where there was destination content");

    assert(operators->size() == CAIRO_OPERATOR_OUT);
    operators->push_back("out");
    toolTips->push_back("draw source where there was no destination content");

    assert(operators->size() == CAIRO_OPERATOR_ATOP);
    operators->push_back("atop");
    toolTips->push_back("draw source on top of destination content and only there");

    assert(operators->size() == CAIRO_OPERATOR_DEST);
    operators->push_back("dest");
    toolTips->push_back("ignore the source");

    assert(operators->size() == CAIRO_OPERATOR_DEST_OVER);
    operators->push_back("dest-over");
    toolTips->push_back("draw destination on top of source");

    assert(operators->size() == CAIRO_OPERATOR_DEST_IN);
    operators->push_back("dest-in");
    toolTips->push_back("leave destination only where there was source content");

    assert(operators->size() == CAIRO_OPERATOR_DEST_OUT);
    operators->push_back("dest-out");
    toolTips->push_back("leave destination only where there was no source content");

    assert(operators->size() == CAIRO_OPERATOR_DEST_ATOP);
    operators->push_back("dest-atop");
    toolTips->push_back("leave destination on top of source content and only there ");

    assert(operators->size() == CAIRO_OPERATOR_XOR);
    operators->push_back("xor");
    toolTips->push_back("source and destination are shown where there is only one of them");

    assert(operators->size() == CAIRO_OPERATOR_ADD);
    operators->push_back("add");
    toolTips->push_back("source and destination layers are accumulated");

    assert(operators->size() == CAIRO_OPERATOR_SATURATE);
    operators->push_back("saturate");
    toolTips->push_back("like over, but assuming source and dest are disjoint geometries ");

    assert(operators->size() == CAIRO_OPERATOR_MULTIPLY);
    operators->push_back("multiply");
    toolTips->push_back("source and destination layers are multiplied. This causes the result to be at least as dark as the darker inputs.");

    assert(operators->size() == CAIRO_OPERATOR_SCREEN);
    operators->push_back("screen");
    toolTips->push_back("source and destination are complemented and multiplied. This causes the result to be at least as "
                        "light as the lighter inputs.");

    assert(operators->size() == CAIRO_OPERATOR_OVERLAY);
    operators->push_back("overlay");
    toolTips->push_back("multiplies or screens, depending on the lightness of the destination color. ");

    assert(operators->size() == CAIRO_OPERATOR_DARKEN);
    operators->push_back("darken");
    toolTips->push_back("replaces the destination with the source if it is darker, otherwise keeps the source");

    assert(operators->size() == CAIRO_OPERATOR_LIGHTEN);
    operators->push_back("lighten");
    toolTips->push_back("replaces the destination with the source if it is lighter, otherwise keeps the source.");

    assert(operators->size() == CAIRO_OPERATOR_COLOR_DODGE);
    operators->push_back("color-dodge");
    toolTips->push_back("brightens the destination color to reflect the source color. ");

    assert(operators->size() == CAIRO_OPERATOR_COLOR_BURN);
    operators->push_back("color-burn");
    toolTips->push_back("darkens the destination color to reflect the source color.");

    assert(operators->size() == CAIRO_OPERATOR_HARD_LIGHT);
    operators->push_back("hard-light");
    toolTips->push_back("Multiplies or screens, dependent on source color.");

    assert(operators->size() == CAIRO_OPERATOR_SOFT_LIGHT);
    operators->push_back("soft-light");
    toolTips->push_back("Darkens or lightens, dependent on source color.");

    assert(operators->size() == CAIRO_OPERATOR_DIFFERENCE);
    operators->push_back("difference");
    toolTips->push_back("Takes the difference of the source and destination color. ");

    assert(operators->size() == CAIRO_OPERATOR_EXCLUSION);
    operators->push_back("exclusion");
    toolTips->push_back("Produces an effect similar to difference, but with lower contrast. ");

    assert(operators->size() == CAIRO_OPERATOR_HSL_HUE);
    operators->push_back("HSL-hue");
    toolTips->push_back("Creates a color with the hue of the source and the saturation and luminosity of the target.");

    assert(operators->size() == CAIRO_OPERATOR_HSL_SATURATION);
    operators->push_back("HSL-saturation");
    toolTips->push_back("Creates a color with the saturation of the source and the hue and luminosity of the target."
                        " Painting with this mode onto a gray area produces no change.");

    assert(operators->size() == CAIRO_OPERATOR_HSL_COLOR);
    operators->push_back("HSL-color");
    toolTips->push_back("Creates a color with the hue and saturation of the source and the luminosity of the target."
                        " This preserves the gray levels of the target and is useful for coloring monochrome"
                        " images or tinting color images");

    assert(operators->size() == CAIRO_OPERATOR_HSL_LUMINOSITY);
    operators->push_back("HSL-luminosity");
    toolTips->push_back("Creates a color with the luminosity of the source and the hue and saturation of the target."
                        " This produces an inverse effect to HSL-color.");
} // getCompositingOperators

struct RotoDrawableItemPrivate
{
    Q_DECLARE_TR_FUNCTIONS(RotoDrawableItem)

public:
    /*
     * The effect node corresponds to the following given the selected tool:
     * Stroke= RotoOFX
     * Blur = BlurCImg
     * Clone = TransformOFX
     * Sharpen = SharpenCImg
     * Smear = hand made tool
     * Reveal = Merge(over) with A being color type and B the tree upstream
     * Dodge/Burn = Merge(color-dodge/color-burn) with A being the tree upstream and B the color type
     *
     * Each effect is followed by a merge (except for the ones that use a merge) with the user given operator
     * onto the previous tree upstream of the effectNode.
     */
    NodePtr effectNode;
    NodePtr mergeNode;
    NodePtr timeOffsetNode, frameHoldNode;
    double overlayColor[4]; //< the color the shape overlay should be drawn with, defaults to smooth red
    boost::shared_ptr<KnobDouble> opacity; //< opacity of the rendered shape between 0 and 1
    boost::shared_ptr<KnobDouble> feather; //< number of pixels to add to the feather distance (from the feather point), between -100 and 100
    boost::shared_ptr<KnobDouble> featherFallOff; //< the rate of fall-off for the feather, between 0 and 1,  0.5 meaning the
                                                  //alpha value is half the original value when at half distance from the feather distance
    boost::shared_ptr<KnobChoice> lifeTime;
    boost::shared_ptr<KnobBool> activated; //< should the curve be visible/rendered ? (animable)
    boost::shared_ptr<KnobInt> lifeTimeFrame;
#ifdef NATRON_ROTO_INVERTIBLE
    boost::shared_ptr<KnobBool> inverted; //< invert the rendering
#endif
    boost::shared_ptr<KnobColor> color;
    boost::shared_ptr<KnobChoice> compOperator;
    boost::shared_ptr<KnobDouble> translate;
    boost::shared_ptr<KnobDouble> rotate;
    boost::shared_ptr<KnobDouble> scale;
    boost::shared_ptr<KnobBool> scaleUniform;
    boost::shared_ptr<KnobDouble> skewX;
    boost::shared_ptr<KnobDouble> skewY;
    boost::shared_ptr<KnobChoice> skewOrder;
    boost::shared_ptr<KnobDouble> center;
    boost::shared_ptr<KnobDouble> extraMatrix;
    boost::shared_ptr<KnobDouble> brushSize;
    boost::shared_ptr<KnobDouble> brushSpacing;
    boost::shared_ptr<KnobDouble> brushHardness;
    boost::shared_ptr<KnobDouble> effectStrength;
    boost::shared_ptr<KnobBool> pressureOpacity, pressureSize, pressureHardness, buildUp;
    boost::shared_ptr<KnobDouble> visiblePortion; // [0,1] by default
    boost::shared_ptr<KnobDouble> cloneTranslate;
    boost::shared_ptr<KnobDouble> cloneRotate;
    boost::shared_ptr<KnobDouble> cloneScale;
    boost::shared_ptr<KnobBool> cloneScaleUniform;
    boost::shared_ptr<KnobDouble> cloneSkewX;
    boost::shared_ptr<KnobDouble> cloneSkewY;
    boost::shared_ptr<KnobChoice> cloneSkewOrder;
    boost::shared_ptr<KnobDouble> cloneCenter;
    boost::shared_ptr<KnobChoice> cloneFilter;
    boost::shared_ptr<KnobBool> cloneBlackOutside;
    boost::shared_ptr<KnobChoice> sourceColor;
    boost::shared_ptr<KnobInt> timeOffset;
    boost::shared_ptr<KnobChoice> timeOffsetMode;

#ifdef NATRON_ROTO_ENABLE_MOTION_BLUR
    boost::shared_ptr<KnobDouble> motionBlur;
    boost::shared_ptr<KnobDouble> shutter;
    boost::shared_ptr<KnobChoice> shutterType;
    boost::shared_ptr<KnobDouble> customOffset;
#endif
    std::list<KnobPtr > knobs; //< list for easy access to all knobs

    //Used to prevent 2 threads from writing the same image in the rotocontext
    mutable QReadWriteLock cacheAccessMutex;

    RotoDrawableItemPrivate(bool isPaintingNode)
        : effectNode()
        , mergeNode()
        , timeOffsetNode()
        , frameHoldNode()
        , opacity()
        , feather()
        , featherFallOff()
        , lifeTime()
        , activated()
        , lifeTimeFrame()
#ifdef NATRON_ROTO_INVERTIBLE
        , inverted()
#endif
        , color()
        , compOperator()
        , translate()
        , rotate()
        , scale()
        , scaleUniform()
        , skewX()
        , skewY()
        , skewOrder()
        , center()
        , brushSize( new KnobDouble(NULL, tr(kRotoBrushSizeParamLabel), 1, true) )
        , brushSpacing( new KnobDouble(NULL, tr(kRotoBrushSpacingParamLabel), 1, true) )
        , brushHardness( new KnobDouble(NULL, tr(kRotoBrushHardnessParamLabel), 1, true) )
        , effectStrength( new KnobDouble(NULL, tr(kRotoBrushEffectParamLabel), 1, true) )
        , pressureOpacity( new KnobBool(NULL, tr(kRotoBrushPressureOpacityParamLabel), 1, true) )
        , pressureSize( new KnobBool(NULL, tr(kRotoBrushPressureSizeParamLabel), 1, true) )
        , pressureHardness( new KnobBool(NULL, tr(kRotoBrushPressureHardnessParamLabel), 1, true) )
        , buildUp( new KnobBool(NULL, tr(kRotoBrushBuildupParamLabel), 1, true) )
        , visiblePortion( new KnobDouble(NULL, tr(kRotoBrushVisiblePortionParamLabel), 2, true) )
        , cloneTranslate( new KnobDouble(NULL, tr(kRotoBrushTranslateParamLabel), 2, true) )
        , cloneRotate( new KnobDouble(NULL, tr(kRotoBrushRotateParamLabel), 1, true) )
        , cloneScale( new KnobDouble(NULL, tr(kRotoBrushScaleParamLabel), 2, true) )
        , cloneScaleUniform( new KnobBool(NULL, tr(kRotoBrushScaleUniformParamLabel), 1, true) )
        , cloneSkewX( new KnobDouble(NULL, tr(kRotoBrushSkewXParamLabel), 1, true) )
        , cloneSkewY( new KnobDouble(NULL, tr(kRotoBrushSkewYParamLabel), 1, true) )
        , cloneSkewOrder( new KnobChoice(NULL, tr(kRotoBrushSkewOrderParamLabel), 1, true) )
        , cloneCenter( new KnobDouble(NULL, tr(kRotoBrushCenterParamLabel), 2, true) )
        , cloneFilter( new KnobChoice(NULL, tr(kRotoBrushFilterParamLabel), 1, true) )
        , cloneBlackOutside( new KnobBool(NULL, tr(kRotoBrushBlackOutsideParamLabel), 1, true) )
        , sourceColor( new KnobChoice(NULL, tr(kRotoBrushSourceColorLabel), 1, true) )
        , timeOffset( new KnobInt(NULL, tr(kRotoBrushTimeOffsetParamLabel), 1, true) )
        , timeOffsetMode( new KnobChoice(NULL, tr(kRotoBrushTimeOffsetModeParamLabel), 1, true) )
        , knobs()
        , cacheAccessMutex()
    {
        opacity.reset( new KnobDouble(NULL, tr(kRotoOpacityParamLabel), 1, true) );
        opacity->setHintToolTip( tr(kRotoOpacityHint) );
        opacity->setName(kRotoOpacityParam);
        opacity->populate();
        opacity->setMinimum(0.);
        opacity->setMaximum(1.);
        opacity->setDisplayMinimum(0.);
        opacity->setDisplayMaximum(1.);
        opacity->setDefaultValue(ROTO_DEFAULT_OPACITY);
        knobs.push_back(opacity);

        feather.reset( new KnobDouble(NULL, tr(kRotoFeatherParamLabel), 1, true) );
        feather->setHintToolTip( tr(kRotoFeatherHint) );
        feather->setName(kRotoFeatherParam);
        feather->populate();
        feather->setMinimum(0);
        feather->setDisplayMinimum(0);
        feather->setDisplayMaximum(500);
        feather->setDefaultValue(ROTO_DEFAULT_FEATHER);
        knobs.push_back(feather);

        featherFallOff.reset( new KnobDouble(NULL, tr(kRotoFeatherFallOffParamLabel), 1, true) );
        featherFallOff->setHintToolTip( tr(kRotoFeatherFallOffHint) );
        featherFallOff->setName(kRotoFeatherFallOffParam);
        featherFallOff->populate();
        featherFallOff->setMinimum(0.001);
        featherFallOff->setMaximum(5.);
        featherFallOff->setDisplayMinimum(0.2);
        featherFallOff->setDisplayMaximum(5.);
        featherFallOff->setDefaultValue(ROTO_DEFAULT_FEATHERFALLOFF);
        knobs.push_back(featherFallOff);


        lifeTime.reset( new KnobChoice(NULL, tr(kRotoDrawableItemLifeTimeParamLabel), 1, true) );
        lifeTime->setHintToolTip( tr(kRotoDrawableItemLifeTimeParamHint) );
        lifeTime->populate();
        lifeTime->setName(kRotoDrawableItemLifeTimeParam);
        {
            std::vector<ChoiceOption> choices;
            choices.push_back(ChoiceOption(kRotoDrawableItemLifeTimeSingle));
            choices.push_back(ChoiceOption(kRotoDrawableItemLifeTimeFromStart));
            choices.push_back(ChoiceOption(kRotoDrawableItemLifeTimeToEnd));
            choices.push_back(ChoiceOption(kRotoDrawableItemLifeTimeCustom));

            lifeTime->populateChoices(choices);
        }
        lifeTime->setDefaultValue(isPaintingNode ? 0 : 3);
        knobs.push_back(lifeTime);

        lifeTimeFrame.reset( new KnobInt(NULL, tr(kRotoDrawableItemLifeTimeFrameParamLabel), 1, true) );
        lifeTimeFrame->setHintToolTip( tr(kRotoDrawableItemLifeTimeFrameParamHint) );
        lifeTimeFrame->setName(kRotoDrawableItemLifeTimeFrameParam);
        lifeTimeFrame->populate();
        knobs.push_back(lifeTimeFrame);

        activated.reset( new KnobBool(NULL, tr(kRotoActivatedParamLabel), 1, true) );
        activated->setHintToolTip( tr(kRotoActivatedHint) );
        activated->setName(kRotoActivatedParam);
        activated->populate();
        activated->setDefaultValue(true);
        knobs.push_back(activated);

#ifdef NATRON_ROTO_INVERTIBLE
        inverted.reset( new KnobBool(NULL, tr(kRotoInvertedParamLabel), 1, true) );
        inverted->setHintToolTip( tr(kRotoInvertedHint) );
        inverted->setName(kRotoInvertedParam);
        inverted->populate();
        inverted->setDefaultValue(false);
        knobs.push_back(inverted);
#endif

        color.reset( new KnobColor(NULL, tr(kRotoColorParamLabel), 3, true) );
        color->setHintToolTip( tr(kRotoColorHint) );
        color->setName(kRotoColorParam);
        color->populate();
        color->setDefaultValue(ROTO_DEFAULT_COLOR_R, 0);
        color->setDefaultValue(ROTO_DEFAULT_COLOR_G, 1);
        color->setDefaultValue(ROTO_DEFAULT_COLOR_B, 2);
        knobs.push_back(color);

        compOperator.reset( new KnobChoice(NULL, tr(kRotoCompOperatorParamLabel), 1, true) );
        compOperator->setHintToolTip( tr(kRotoCompOperatorHint) );
        compOperator->setName(kRotoCompOperatorParam);
        compOperator->populate();
        knobs.push_back(compOperator);


        translate.reset( new KnobDouble(NULL, tr(kRotoDrawableItemTranslateParamLabel), 2, true) );
        translate->setName(kRotoDrawableItemTranslateParam);
        translate->setHintToolTip( tr(kRotoDrawableItemTranslateParamHint) );
        translate->populate();
        knobs.push_back(translate);

        rotate.reset( new KnobDouble(NULL, tr(kRotoDrawableItemRotateParamLabel), 1, true) );
        rotate->setName(kRotoDrawableItemRotateParam);
        rotate->setHintToolTip( tr(kRotoDrawableItemRotateParamHint) );
        rotate->populate();
        knobs.push_back(rotate);

        scale.reset( new KnobDouble(NULL, tr(kRotoDrawableItemScaleParamLabel), 2, true) );
        scale->setName(kRotoDrawableItemScaleParam);
        scale->setHintToolTip( tr(kRotoDrawableItemScaleParamHint) );
        scale->populate();
        scale->setDefaultValue(1, 0);
        scale->setDefaultValue(1, 1);
        knobs.push_back(scale);

        scaleUniform.reset( new KnobBool(NULL, tr(kRotoDrawableItemScaleUniformParamLabel), 1, true) );
        scaleUniform->setName(kRotoDrawableItemScaleUniformParam);
        scaleUniform->setHintToolTip( tr(kRotoDrawableItemScaleUniformParamHint) );
        scaleUniform->populate();
        scaleUniform->setDefaultValue(true);
        knobs.push_back(scaleUniform);

        skewX.reset( new KnobDouble(NULL, tr(kRotoDrawableItemSkewXParamLabel), 1, true) );
        skewX->setName(kRotoDrawableItemSkewXParam);
        skewX->setHintToolTip( tr(kRotoDrawableItemSkewXParamHint) );
        skewX->populate();
        knobs.push_back(skewX);

        skewY.reset( new KnobDouble(NULL, tr(kRotoDrawableItemSkewYParamLabel), 1, true) );
        skewY->setName(kRotoDrawableItemSkewYParam);
        skewY->setHintToolTip( tr(kRotoDrawableItemSkewYParamHint) );
        skewY->populate();
        knobs.push_back(skewY);

        skewOrder.reset( new KnobChoice(NULL, tr(kRotoDrawableItemSkewOrderParamLabel), 1, true) );
        skewOrder->setName(kRotoDrawableItemSkewOrderParam);
        skewOrder->setHintToolTip( tr(kRotoDrawableItemSkewOrderParamHint) );
        skewOrder->populate();
        knobs.push_back(skewOrder);
        {
            std::vector<ChoiceOption> choices;
            choices.push_back(ChoiceOption("XY"));
            choices.push_back(ChoiceOption("YX"));
            skewOrder->populateChoices(choices);
        }

        center.reset( new KnobDouble(NULL, tr(kRotoDrawableItemCenterParamLabel), 2, true) );
        center->setName(kRotoDrawableItemCenterParam);
        center->setHintToolTip( tr(kRotoDrawableItemCenterParamHint) );
        center->populate();
        knobs.push_back(center);

        extraMatrix.reset( new KnobDouble(NULL, tr(kRotoDrawableItemExtraMatrixParamLabel), 9, true) );
        extraMatrix->setName(kRotoDrawableItemExtraMatrixParam);
        extraMatrix->setHintToolTip( tr(kRotoDrawableItemExtraMatrixParamHint) );
        extraMatrix->populate();
        extraMatrix->setDefaultValue(1, 0);
        extraMatrix->setDefaultValue(1, 4);
        extraMatrix->setDefaultValue(1, 8);
        knobs.push_back(extraMatrix);


        brushSize->setName(kRotoBrushSizeParam);
        brushSize->setHintToolTip( tr(kRotoBrushSizeParamHint) );
        brushSize->populate();
        brushSize->setDefaultValue(25);
        brushSize->setMinimum(1);
        brushSize->setMaximum(1000);
        knobs.push_back(brushSize);

        brushSpacing->setName(kRotoBrushSpacingParam);
        brushSpacing->setHintToolTip( tr(kRotoBrushSpacingParamHint) );
        brushSpacing->populate();
        brushSpacing->setDefaultValue(0.1);
        brushSpacing->setMinimum(0);
        brushSpacing->setMaximum(1);
        knobs.push_back(brushSpacing);

        brushHardness->setName(kRotoBrushHardnessParam);
        brushHardness->setHintToolTip( tr(kRotoBrushHardnessParamHint) );
        brushHardness->populate();
        brushHardness->setDefaultValue(0.2);
        brushHardness->setMinimum(0);
        brushHardness->setMaximum(1);
        knobs.push_back(brushHardness);

        effectStrength->setName(kRotoBrushEffectParam);
        effectStrength->setHintToolTip( tr(kRotoBrushEffectParamHint) );
        effectStrength->populate();
        effectStrength->setDefaultValue(15);
        effectStrength->setMinimum(0);
        effectStrength->setMaximum(100);
        knobs.push_back(effectStrength);

        pressureOpacity->setName(kRotoBrushPressureOpacityParam);
        pressureOpacity->setHintToolTip( tr(kRotoBrushPressureOpacityParamHint) );
        pressureOpacity->populate();
        pressureOpacity->setAnimationEnabled(false);
        pressureOpacity->setDefaultValue(true);
        knobs.push_back(pressureOpacity);

        pressureSize->setName(kRotoBrushPressureSizeParam);
        pressureSize->populate();
        pressureSize->setHintToolTip( tr(kRotoBrushPressureSizeParamHint) );
        pressureSize->setAnimationEnabled(false);
        pressureSize->setDefaultValue(false);
        knobs.push_back(pressureSize);


        pressureHardness->setName(kRotoBrushPressureHardnessParam);
        pressureHardness->populate();
        pressureHardness->setHintToolTip( tr(kRotoBrushPressureHardnessParamHint) );
        pressureHardness->setAnimationEnabled(false);
        pressureHardness->setDefaultValue(false);
        knobs.push_back(pressureHardness);

        buildUp->setName(kRotoBrushBuildupParam);
        buildUp->populate();
        buildUp->setHintToolTip( tr(kRotoBrushBuildupParamHint) );
        buildUp->setDefaultValue(false);
        buildUp->setAnimationEnabled(false);
        buildUp->setDefaultValue(true);
        knobs.push_back(buildUp);


        visiblePortion->setName(kRotoBrushVisiblePortionParam);
        visiblePortion->setHintToolTip( tr(kRotoBrushVisiblePortionParamHint) );
        visiblePortion->populate();
        visiblePortion->setDefaultValue(0, 0);
        visiblePortion->setDefaultValue(1, 1);
        std::vector<double> mins, maxs;
        mins.push_back(0);
        mins.push_back(0);
        maxs.push_back(1);
        maxs.push_back(1);
        visiblePortion->setMinimumsAndMaximums(mins, maxs);
        knobs.push_back(visiblePortion);

        cloneTranslate->setName(kRotoBrushTranslateParam);
        cloneTranslate->setHintToolTip( tr(kRotoBrushTranslateParamHint) );
        cloneTranslate->populate();
        knobs.push_back(cloneTranslate);

        cloneRotate->setName(kRotoBrushRotateParam);
        cloneRotate->setHintToolTip( tr(kRotoBrushRotateParamHint) );
        cloneRotate->populate();
        knobs.push_back(cloneRotate);

        cloneScale->setName(kRotoBrushScaleParam);
        cloneScale->setHintToolTip( tr(kRotoBrushScaleParamHint) );
        cloneScale->populate();
        cloneScale->setDefaultValue(1, 0);
        cloneScale->setDefaultValue(1, 1);
        knobs.push_back(cloneScale);

        cloneScaleUniform->setName(kRotoBrushScaleUniformParam);
        cloneScaleUniform->setHintToolTip( tr(kRotoBrushScaleUniformParamHint) );
        cloneScaleUniform->populate();
        cloneScaleUniform->setDefaultValue(true);
        knobs.push_back(cloneScaleUniform);

        cloneSkewX->setName(kRotoBrushSkewXParam);
        cloneSkewX->setHintToolTip( tr(kRotoBrushSkewXParamHint) );
        cloneSkewX->populate();
        knobs.push_back(cloneSkewX);

        cloneSkewY->setName(kRotoBrushSkewYParam);
        cloneSkewY->setHintToolTip( tr(kRotoBrushSkewYParamHint) );
        cloneSkewY->populate();
        knobs.push_back(cloneSkewY);

        cloneSkewOrder->setName(kRotoBrushSkewOrderParam);
        cloneSkewOrder->setHintToolTip( tr(kRotoBrushSkewOrderParamHint) );
        cloneSkewOrder->populate();
        knobs.push_back(cloneSkewOrder);
        {
            std::vector<ChoiceOption> choices;
            choices.push_back(ChoiceOption("XY"));
            choices.push_back(ChoiceOption("YX"));
            cloneSkewOrder->populateChoices(choices);
        }

        cloneCenter->setName(kRotoBrushCenterParam);
        cloneCenter->setHintToolTip( tr(kRotoBrushCenterParamHint) );
        cloneCenter->populate();
        knobs.push_back(cloneCenter);


        cloneFilter->setName(kRotoBrushFilterParam);
        cloneFilter->setHintToolTip( tr(kRotoBrushFilterParamHint) );
        cloneFilter->populate();
        {
            std::vector<ChoiceOption> choices;
            choices.push_back(ChoiceOption(kFilterImpulse, "", tr(kFilterImpulseHint).toStdString()));
            choices.push_back(ChoiceOption(kFilterBilinear, "", tr(kFilterBilinearHint).toStdString()));
            choices.push_back(ChoiceOption(kFilterCubic, "", tr(kFilterCubicHint).toStdString()));
            choices.push_back(ChoiceOption(kFilterKeys, "", tr(kFilterKeysHint).toStdString()));
            choices.push_back(ChoiceOption(kFilterSimon, "", tr(kFilterSimonHint).toStdString()));
            choices.push_back(ChoiceOption(kFilterRifman, "", tr(kFilterRifmanHint).toStdString()));
            choices.push_back(ChoiceOption(kFilterMitchell, "", tr(kFilterMitchellHint).toStdString()));
            choices.push_back(ChoiceOption(kFilterParzen, "", tr(kFilterParzenHint).toStdString()));
            choices.push_back(ChoiceOption(kFilterNotch, "", tr(kFilterNotchHint).toStdString()));
            cloneFilter->populateChoices(choices);
        }
        cloneFilter->setDefaultValue(2);
        knobs.push_back(cloneFilter);


        cloneBlackOutside->setName(kRotoBrushBlackOutsideParam);
        cloneBlackOutside->setHintToolTip( tr(kRotoBrushBlackOutsideParamHint) );
        cloneBlackOutside->populate();
        cloneBlackOutside->setDefaultValue(true);
        knobs.push_back(cloneBlackOutside);

        sourceColor->setName(kRotoBrushSourceColor);
        sourceColor->setHintToolTip( tr(kRotoBrushSizeParamHint) );
        sourceColor->populate();
        sourceColor->setDefaultValue(1);
        {
            std::vector<ChoiceOption> choices;
            choices.push_back(ChoiceOption("foreground"));
            choices.push_back(ChoiceOption("background"));
            for (int i = 1; i < 10; ++i) {
                std::stringstream ss;
                ss << "background " << i + 1;
                choices.push_back( ChoiceOption(ss.str()) );
            }
            sourceColor->populateChoices(choices);
        }
        knobs.push_back(sourceColor);

        timeOffset->setName(kRotoBrushTimeOffsetParam);
        timeOffset->setHintToolTip( tr(kRotoBrushTimeOffsetParamHint) );
        timeOffset->populate();
        timeOffset->setDisplayMinimum(-100);
        timeOffset->setDisplayMaximum(100);
        knobs.push_back(timeOffset);

        timeOffsetMode->setName(kRotoBrushTimeOffsetModeParam);
        timeOffsetMode->setHintToolTip( tr(kRotoBrushTimeOffsetModeParamHint) );
        timeOffsetMode->populate();
        {
            std::vector<ChoiceOption> modes;
            modes.push_back(ChoiceOption("Relative"));
            modes.push_back(ChoiceOption("Absolute"));
            timeOffsetMode->populateChoices(modes);
        }
        knobs.push_back(timeOffsetMode);

#ifdef NATRON_ROTO_ENABLE_MOTION_BLUR
        motionBlur.reset( new KnobDouble(NULL, tr(kRotoMotionBlurParamLabel), 1, true) );
        motionBlur->setName(kRotoPerShapeMotionBlurParam);
        motionBlur->setHintToolTip( tr(kRotoMotionBlurParamHint) );
        motionBlur->populate();
        motionBlur->setDefaultValue(0);
        motionBlur->setMinimum(0);
        motionBlur->setDisplayMinimum(0);
        motionBlur->setDisplayMaximum(4);
        motionBlur->setMaximum(4);
        knobs.push_back(motionBlur);

        shutter.reset( new KnobDouble(NULL, tr(kRotoShutterParamLabel), 1, true) );
        shutter->setName(kRotoPerShapeShutterParam);
        shutter->setHintToolTip( tr(kRotoShutterParamHint) );
        shutter->populate();
        shutter->setDefaultValue(0.5);
        shutter->setMinimum(0);
        shutter->setDisplayMinimum(0);
        shutter->setDisplayMaximum(2);
        shutter->setMaximum(2);
        knobs.push_back(shutter);

        shutterType.reset( new KnobChoice(NULL, tr(kRotoShutterOffsetTypeParamLabel), 1, true) );
        shutterType->setName(kRotoPerShapeShutterOffsetTypeParam);
        shutterType->setHintToolTip( tr(kRotoShutterOffsetTypeParamHint) );
        shutterType->populate();
        shutterType->setDefaultValue(0);
        {
            std::vector<std::string> options, helps;
            options.push_back("Centered");
            helps.push_back(kRotoShutterOffsetCenteredHint);
            options.push_back("Start");
            helps.push_back(kRotoShutterOffsetStartHint);
            options.push_back("End");
            helps.push_back(kRotoShutterOffsetEndHint);
            options.push_back("Custom");
            helps.push_back(kRotoShutterOffsetCustomHint);
            shutterType->populateChoices(options, helps);
        }
        knobs.push_back(shutterType);

        customOffset.reset( new KnobDouble(NULL, tr(kRotoShutterCustomOffsetParamLabel), 1, true) );
        customOffset->setName(kRotoPerShapeShutterCustomOffsetParam);
        customOffset->setHintToolTip( tr(kRotoShutterCustomOffsetParamHint) );
        customOffset->populate();
        customOffset->setDefaultValue(0);
        knobs.push_back(customOffset);
#endif

        overlayColor[0] = 0.85164;
        overlayColor[1] = 0.196936;
        overlayColor[2] = 0.196936;
        overlayColor[3] = 1.;
    }

    ~RotoDrawableItemPrivate()
    {
    }
};

struct RotoStrokeItemPrivate
{
    RotoStrokeType type;
    bool finished;
    struct StrokeCurves
    {
        boost::shared_ptr<Curve> xCurve, yCurve, pressureCurve;
    };

    /**
     * @brief A list of all storkes contained in this item. Basically each time penUp() is called it makes a new stroke
     **/
    std::vector<StrokeCurves> strokes;
    double curveT0; // timestamp of the first point in curve
    double lastTimestamp;
    RectD bbox;
    RectD wholeStrokeBboxWhilePainting;
    mutable QMutex strokeDotPatternsMutex;
    std::vector<cairo_pattern_t*> strokeDotPatterns;

    RotoStrokeItemPrivate(RotoStrokeType type)
        : type(type)
        , finished(false)
        , strokes()
        , curveT0(0)
        , lastTimestamp(0)
        , bbox()
        , wholeStrokeBboxWhilePainting()
        , strokeDotPatternsMutex()
        , strokeDotPatterns()
    {
        bbox.x1 = std::numeric_limits<double>::infinity();
        bbox.x2 = -std::numeric_limits<double>::infinity();
        bbox.y1 = std::numeric_limits<double>::infinity();
        bbox.y2 = -std::numeric_limits<double>::infinity();
    }
};

struct RotoContextPrivate
{
    Q_DECLARE_TR_FUNCTIONS(RotoContext)

public:
    mutable QMutex rotoContextMutex;

    /*
     * We have chosen to disable rotopainting and roto shapes from the same RotoContext because the rendering techniques are
     * very much differents. The rotopainting systems requires an entire compositing tree held inside whereas the rotoshapes
     * are rendered and optimized by Cairo internally.
     */
    bool isPaintNode;
    std::list< boost::shared_ptr<RotoLayer> > layers;
    bool autoKeying;
    bool rippleEdit;
    bool featherLink;
    bool isCurrentlyLoading;
    NodeWPtr node;
    U64 age;

    ///These are knobs that take the value of the selected splines info.
    ///Their value changes when selection changes.
    boost::weak_ptr<KnobDouble> opacity;
    boost::weak_ptr<KnobDouble> feather;
    boost::weak_ptr<KnobDouble> featherFallOff;
    boost::weak_ptr<KnobChoice> lifeTime;
    boost::weak_ptr<KnobBool> activated; //<allows to disable a shape on a specific frame range
    boost::weak_ptr<KnobInt> lifeTimeFrame;

#ifdef NATRON_ROTO_INVERTIBLE
    boost::weak_ptr<KnobBool> inverted;
#endif
    boost::weak_ptr<KnobColor> colorKnob;
    boost::weak_ptr<KnobDouble> brushSizeKnob;
    boost::weak_ptr<KnobDouble> brushSpacingKnob;
    boost::weak_ptr<KnobDouble> brushHardnessKnob;
    boost::weak_ptr<KnobDouble> brushEffectKnob;
    boost::weak_ptr<KnobSeparator> pressureLabelKnob;
    boost::weak_ptr<KnobBool> pressureOpacityKnob;
    boost::weak_ptr<KnobBool> pressureSizeKnob;
    boost::weak_ptr<KnobBool> pressureHardnessKnob;
    boost::weak_ptr<KnobBool> buildUpKnob;
    boost::weak_ptr<KnobDouble> brushVisiblePortionKnob;
    boost::weak_ptr<KnobDouble> cloneTranslateKnob;
    boost::weak_ptr<KnobDouble> cloneRotateKnob;
    boost::weak_ptr<KnobDouble> cloneScaleKnob;
    boost::weak_ptr<KnobBool> cloneUniformKnob;
    boost::weak_ptr<KnobDouble> cloneSkewXKnob;
    boost::weak_ptr<KnobDouble> cloneSkewYKnob;
    boost::weak_ptr<KnobChoice> cloneSkewOrderKnob;
    boost::weak_ptr<KnobDouble> cloneCenterKnob;
    boost::weak_ptr<KnobButton> resetCloneCenterKnob;
    boost::weak_ptr<KnobChoice> cloneFilterKnob;
    boost::weak_ptr<KnobBool> cloneBlackOutsideKnob;
    boost::weak_ptr<KnobButton> resetCloneTransformKnob;
    boost::weak_ptr<KnobDouble> translateKnob;
    boost::weak_ptr<KnobDouble> rotateKnob;
    boost::weak_ptr<KnobDouble> scaleKnob;
    boost::weak_ptr<KnobBool> scaleUniformKnob;
    boost::weak_ptr<KnobBool> transformInteractiveKnob;
    boost::weak_ptr<KnobDouble> skewXKnob;
    boost::weak_ptr<KnobDouble> skewYKnob;
    boost::weak_ptr<KnobChoice> skewOrderKnob;
    boost::weak_ptr<KnobDouble> centerKnob;
    boost::weak_ptr<KnobButton> resetCenterKnob;
    boost::weak_ptr<KnobDouble> extraMatrixKnob;
    boost::weak_ptr<KnobButton> resetTransformKnob;
    boost::weak_ptr<KnobChoice> sourceTypeKnob;
    boost::weak_ptr<KnobInt> timeOffsetKnob;
    boost::weak_ptr<KnobChoice> timeOffsetModeKnob;

#ifdef NATRON_ROTO_ENABLE_MOTION_BLUR
    boost::weak_ptr<KnobChoice> motionBlurTypeKnob;
    boost::weak_ptr<KnobDouble> motionBlurKnob, globalMotionBlurKnob;
    boost::weak_ptr<KnobDouble> shutterKnob, globalShutterKnob;
    boost::weak_ptr<KnobChoice> shutterTypeKnob, globalShutterTypeKnob;
    boost::weak_ptr<KnobDouble> customOffsetKnob, globalCustomOffsetKnob;
#endif

    std::list<boost::weak_ptr<KnobI> > knobs; //< list for easy access to all knobs
    std::list<boost::weak_ptr<KnobI> > cloneKnobs;
    std::list<boost::weak_ptr<KnobI> > strokeKnobs;
    std::list<boost::weak_ptr<KnobI> > shapeKnobs;

    ///This keeps track  of the items linked to the context knobs
    std::list<boost::shared_ptr<RotoItem> > selectedItems;
    boost::shared_ptr<RotoItem> lastInsertedItem;
    boost::shared_ptr<RotoItem> lastLockedItem;
    mutable QMutex doingNeatRenderMutex;
    QWaitCondition doingNeatRenderCond;
    bool doingNeatRender;
    bool mustDoNeatRender;

    /*
     * A merge node (or more if there are more than 64 items) used when all items share the same compositing operator to make the rotopaint tree shallow
     */
    NodesList globalMergeNodes;

    RotoContextPrivate(const NodePtr& n )
        : rotoContextMutex()
        , isPaintNode(false)
        , layers()
        , autoKeying(true)
        , rippleEdit(false)
        , featherLink(true)
        , isCurrentlyLoading(false)
        , node(n)
        , age(0)
        , doingNeatRender(false)
        , mustDoNeatRender(false)
        , globalMergeNodes()
    {
        EffectInstPtr effect = n->getEffectInstance();
        RotoPaint* isRotoNode = dynamic_cast<RotoPaint*>( effect.get() );

        if (isRotoNode) {
            isPaintNode = isRotoNode->isDefaultBehaviourPaintContext();
        } else {
            isPaintNode = false;
        }

        assert( n && n->getEffectInstance() );

        boost::shared_ptr<KnobPage> shapePage, strokePage, generalPage, clonePage, transformPage;

        generalPage = AppManager::createKnob<KnobPage>(effect.get(), tr("General"), 1, true);
        shapePage = AppManager::createKnob<KnobPage>(effect.get(), tr("Shape"), 1, true);
        strokePage = AppManager::createKnob<KnobPage>(effect.get(), tr("Stroke"), 1, true);
        clonePage = AppManager::createKnob<KnobPage>(effect.get(), tr("Clone"), 1, true);
        transformPage = AppManager::createKnob<KnobPage>(effect.get(), tr("Transform"), 1, true);

        boost::shared_ptr<KnobDouble> opacityKnob = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoOpacityParamLabel), 1, true);
        opacityKnob->setHintToolTip( tr(kRotoOpacityHint) );
        opacityKnob->setName(kRotoOpacityParam);
        opacityKnob->setMinimum(0.);
        opacityKnob->setMaximum(1.);
        opacityKnob->setDisplayMinimum(0.);
        opacityKnob->setDisplayMaximum(1.);
        opacityKnob->setDefaultValue(ROTO_DEFAULT_OPACITY);
        opacityKnob->setDefaultAllDimensionsEnabled(false);
        opacityKnob->setIsPersistent(false);
        generalPage->addKnob(opacityKnob);
        knobs.push_back(opacityKnob);
        opacity = opacityKnob;

        boost::shared_ptr<KnobColor> ck = AppManager::createKnob<KnobColor>(effect.get(), tr(kRotoColorParamLabel), 3, true);
        ck->setHintToolTip( tr(kRotoColorHint) );
        ck->setName(kRotoColorParam);
        ck->setDefaultValue(ROTO_DEFAULT_COLOR_R, 0);
        ck->setDefaultValue(ROTO_DEFAULT_COLOR_G, 1);
        ck->setDefaultValue(ROTO_DEFAULT_COLOR_B, 2);
        ck->setDefaultAllDimensionsEnabled(false);
        generalPage->addKnob(ck);
        ck->setIsPersistent(false);
        knobs.push_back(ck);
        colorKnob = ck;

        boost::shared_ptr<KnobChoice> lifeTimeKnob = AppManager::createKnob<KnobChoice>(effect.get(), tr(kRotoDrawableItemLifeTimeParamLabel), 1, true);
        lifeTimeKnob->setHintToolTip( tr(kRotoDrawableItemLifeTimeParamHint) );
        lifeTimeKnob->setName(kRotoDrawableItemLifeTimeParam);
        lifeTimeKnob->setAddNewLine(false);
        lifeTimeKnob->setIsPersistent(false);
        lifeTimeKnob->setDefaultAllDimensionsEnabled(false);
        lifeTimeKnob->setAnimationEnabled(false);
        {
            std::vector<ChoiceOption> choices;
            choices.push_back(ChoiceOption(kRotoDrawableItemLifeTimeAll, "",tr(kRotoDrawableItemLifeTimeAllHelp).toStdString() ));
            choices.push_back(ChoiceOption(kRotoDrawableItemLifeTimeSingle, "", tr(kRotoDrawableItemLifeTimeSingleHelp).toStdString()));
            choices.push_back(ChoiceOption(kRotoDrawableItemLifeTimeFromStart, "", tr(kRotoDrawableItemLifeTimeFromStartHelp).toStdString()));
            choices.push_back(ChoiceOption(kRotoDrawableItemLifeTimeToEnd, "" ,tr(kRotoDrawableItemLifeTimeToEndHelp).toStdString()));
            choices.push_back(ChoiceOption(kRotoDrawableItemLifeTimeCustom, "", tr(kRotoDrawableItemLifeTimeCustomHelp).toStdString()));
            lifeTimeKnob->populateChoices(choices);

        }
        lifeTimeKnob->setDefaultValue(isPaintNode ? 0 : 3);
        generalPage->addKnob(lifeTimeKnob);
        knobs.push_back(lifeTimeKnob);
        lifeTime = lifeTimeKnob;

        boost::shared_ptr<KnobInt> lifeTimeFrameKnob = AppManager::createKnob<KnobInt>(effect.get(), tr(kRotoDrawableItemLifeTimeFrameParamLabel), 1, true);
        lifeTimeFrameKnob->setHintToolTip( tr(kRotoDrawableItemLifeTimeFrameParamHint) );
        lifeTimeFrameKnob->setName(kRotoDrawableItemLifeTimeFrameParam);
        lifeTimeFrameKnob->setSecretByDefault(!isPaintNode);
        lifeTimeFrameKnob->setDefaultAllDimensionsEnabled(false);
        lifeTimeFrameKnob->setAddNewLine(false);
        lifeTimeFrameKnob->setAnimationEnabled(false);
        generalPage->addKnob(lifeTimeFrameKnob);
        knobs.push_back(lifeTimeFrameKnob);
        lifeTimeFrame = lifeTimeFrameKnob;

        boost::shared_ptr<KnobBool> activatedKnob = AppManager::createKnob<KnobBool>(effect.get(), tr(kRotoActivatedParamLabel), 1, true);
        activatedKnob->setHintToolTip( tr(kRotoActivatedHint) );
        activatedKnob->setName(kRotoActivatedParam);
        activatedKnob->setAddNewLine(true);
        activatedKnob->setSecretByDefault(isPaintNode);
        activatedKnob->setDefaultValue(true);
        activatedKnob->setDefaultAllDimensionsEnabled(false);
        generalPage->addKnob(activatedKnob);
        activatedKnob->setIsPersistent(false);
        knobs.push_back(activatedKnob);
        activated = activatedKnob;

#ifdef NATRON_ROTO_INVERTIBLE
        boost::shared_ptr<KnobBool> invertedKnob = AppManager::createKnob<KnobBool>(effect.get(), tr(kRotoInvertedParamLabel), 1, true);
        invertedKnob->setHintToolTip( tr(kRotoInvertedHint) );
        invertedKnob->setName(kRotoInvertedParam);
        invertedKnob->setDefaultValue(false);
        invertedKnob->setDefaultAllDimensionsEnabled(false);
        invertedKnob->setIsPersistent(false);
        generalPage->addKnob(invertedKnob);
        knobs.push_back(invertedKnob);
        inverted = invertedKnob;
#endif

        boost::shared_ptr<KnobDouble> featherKnob = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoFeatherParamLabel), 1, true);
        featherKnob->setHintToolTip( tr(kRotoFeatherHint) );
        featherKnob->setName(kRotoFeatherParam);
        featherKnob->setMinimum(0);
        featherKnob->setDisplayMinimum(0);
        featherKnob->setDisplayMaximum(500);
        featherKnob->setDefaultValue(ROTO_DEFAULT_FEATHER);
        featherKnob->setDefaultAllDimensionsEnabled(false);
        featherKnob->setIsPersistent(false);
        shapePage->addKnob(featherKnob);
        knobs.push_back(featherKnob);
        shapeKnobs.push_back(featherKnob);
        feather = featherKnob;

        boost::shared_ptr<KnobDouble> featherFallOffKnob = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoFeatherFallOffParamLabel), 1, true);
        featherFallOffKnob->setHintToolTip( tr(kRotoFeatherFallOffHint) );
        featherFallOffKnob->setName(kRotoFeatherFallOffParam);
        featherFallOffKnob->setMinimum(0.001);
        featherFallOffKnob->setMaximum(5.);
        featherFallOffKnob->setDisplayMinimum(0.2);
        featherFallOffKnob->setDisplayMaximum(5.);
        featherFallOffKnob->setDefaultValue(ROTO_DEFAULT_FEATHERFALLOFF);
        featherFallOffKnob->setDefaultAllDimensionsEnabled(false);
        featherFallOffKnob->setIsPersistent(false);
        shapePage->addKnob(featherFallOffKnob);
        knobs.push_back(featherFallOffKnob);
        shapeKnobs.push_back(featherFallOffKnob);
        featherFallOff = featherFallOffKnob;

        {
            boost::shared_ptr<KnobChoice> sourceType = AppManager::createKnob<KnobChoice>(effect.get(), tr(kRotoBrushSourceColorLabel), 1, true);
            sourceType->setName(kRotoBrushSourceColor);
            sourceType->setHintToolTip( tr(kRotoBrushSourceColorHint) );
            sourceType->setDefaultValue(1);
            {
                std::vector<ChoiceOption> choices;
                choices.push_back(ChoiceOption("foreground"));
                choices.push_back(ChoiceOption("background"));
                for (int i = 1; i < 10; ++i) {
                    std::stringstream ss;
                    ss << "background " << i + 1;
                    choices.push_back( ChoiceOption(ss.str()) );
                }
                sourceType->populateChoices(choices);
            }
            sourceType->setDefaultAllDimensionsEnabled(false);
            clonePage->addKnob(sourceType);
            knobs.push_back(sourceType);
            cloneKnobs.push_back(sourceType);
            sourceTypeKnob = sourceType;

            boost::shared_ptr<KnobDouble> translate = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoBrushTranslateParamLabel), 2, true);
            translate->setName(kRotoBrushTranslateParam);
            translate->setHintToolTip( tr(kRotoBrushTranslateParamHint) );
            translate->setDefaultAllDimensionsEnabled(false);
            translate->setIncrement(10);
            clonePage->addKnob(translate);
            knobs.push_back(translate);
            cloneKnobs.push_back(translate);
            cloneTranslateKnob = translate;

            boost::shared_ptr<KnobDouble> rotate = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoBrushRotateParamLabel), 1, true);
            rotate->setName(kRotoBrushRotateParam);
            rotate->setHintToolTip( tr(kRotoBrushRotateParamHint) );
            rotate->setDefaultAllDimensionsEnabled(false);
            rotate->setDisplayMinimum(-180);
            rotate->setDisplayMaximum(180);
            clonePage->addKnob(rotate);
            knobs.push_back(rotate);
            cloneKnobs.push_back(rotate);
            cloneRotateKnob = rotate;

            boost::shared_ptr<KnobDouble> scale = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoBrushScaleParamLabel), 2, true);
            scale->setName(kRotoBrushScaleParam);
            scale->setHintToolTip( tr(kRotoBrushScaleParamHint) );
            scale->setDefaultValue(1, 0);
            scale->setDefaultValue(1, 1);
            scale->setDisplayMinimum(0.1, 0);
            scale->setDisplayMinimum(0.1, 1);
            scale->setDisplayMaximum(10, 0);
            scale->setDisplayMaximum(10, 1);
            scale->setAddNewLine(false);
            scale->setDefaultAllDimensionsEnabled(false);
            clonePage->addKnob(scale);
            cloneKnobs.push_back(scale);
            knobs.push_back(scale);
            cloneScaleKnob = scale;

            boost::shared_ptr<KnobBool> scaleUniform = AppManager::createKnob<KnobBool>(effect.get(), tr(kRotoBrushScaleUniformParamLabel), 1, true);
            scaleUniform->setName(kRotoBrushScaleUniformParam);
            scaleUniform->setHintToolTip( tr(kRotoBrushScaleUniformParamHint) );
            scaleUniform->setDefaultValue(true);
            scaleUniform->setDefaultAllDimensionsEnabled(false);
            scaleUniform->setAnimationEnabled(false);
            clonePage->addKnob(scaleUniform);
            cloneKnobs.push_back(scaleUniform);
            knobs.push_back(scaleUniform);
            cloneUniformKnob = scaleUniform;

            boost::shared_ptr<KnobDouble> skewX = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoBrushSkewXParamLabel), 1, true);
            skewX->setName(kRotoBrushSkewXParam);
            skewX->setHintToolTip( tr(kRotoBrushSkewXParamHint) );
            skewX->setDefaultAllDimensionsEnabled(false);
            skewX->setDisplayMinimum(-1, 0);
            skewX->setDisplayMaximum(1, 0);
            cloneKnobs.push_back(skewX);
            clonePage->addKnob(skewX);
            knobs.push_back(skewX);
            cloneSkewXKnob = skewX;

            boost::shared_ptr<KnobDouble> skewY = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoBrushSkewYParamLabel), 1, true);
            skewY->setName(kRotoBrushSkewYParam);
            skewY->setHintToolTip( tr(kRotoBrushSkewYParamHint) );
            skewY->setDefaultAllDimensionsEnabled(false);
            skewY->setDisplayMinimum(-1, 0);
            skewY->setDisplayMaximum(1, 0);
            clonePage->addKnob(skewY);
            cloneKnobs.push_back(skewY);
            knobs.push_back(skewY);
            cloneSkewYKnob = skewY;

            boost::shared_ptr<KnobChoice> skewOrder = AppManager::createKnob<KnobChoice>(effect.get(), tr(kRotoBrushSkewOrderParamLabel), 1, true);
            skewOrder->setName(kRotoBrushSkewOrderParam);
            skewOrder->setHintToolTip( tr(kRotoBrushSkewOrderParamHint) );
            skewOrder->setDefaultValue(0);
            {
                std::vector<ChoiceOption> choices;
                choices.push_back(ChoiceOption("XY"));
                choices.push_back(ChoiceOption("YX"));
                skewOrder->populateChoices(choices);
            }
            skewOrder->setDefaultAllDimensionsEnabled(false);
            skewOrder->setAnimationEnabled(false);
            clonePage->addKnob(skewOrder);
            cloneKnobs.push_back(skewOrder);
            knobs.push_back(skewOrder);
            cloneSkewOrderKnob = skewOrder;

            boost::shared_ptr<KnobDouble> center = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoBrushCenterParamLabel), 2, true);
            center->setName(kRotoBrushCenterParam);
            center->setHintToolTip( tr(kRotoBrushCenterParamHint) );
            center->setDefaultAllDimensionsEnabled(false);
            center->setDefaultValuesAreNormalized(true);
            center->setAddNewLine(false);
            center->setDefaultValue(0.5, 0);
            center->setDefaultValue(0.5, 1);
            clonePage->addKnob(center);
            cloneKnobs.push_back(center);
            knobs.push_back(center);
            cloneCenterKnob = center;

            boost::shared_ptr<KnobButton> resetCloneCenter = AppManager::createKnob<KnobButton>(effect.get(), tr(kRotoResetCloneCenterParamLabel), 1, true);
            resetCloneCenter->setName(kRotoResetCloneCenterParam);
            resetCloneCenter->setHintToolTip( tr(kRotoResetCloneCenterParamHint) );
            resetCloneCenter->setAllDimensionsEnabled(false);
            clonePage->addKnob(resetCloneCenter);
            cloneKnobs.push_back(resetCloneCenter);
            knobs.push_back(resetCloneCenter);
            resetCloneCenterKnob = resetCloneCenter;


            boost::shared_ptr<KnobButton> resetCloneTransform = AppManager::createKnob<KnobButton>(effect.get(), tr(kRotoResetCloneTransformParamLabel), 1, true);
            resetCloneTransform->setName(kRotoResetCloneTransformParam);
            resetCloneTransform->setHintToolTip( tr(kRotoResetCloneTransformParamHint) );
            resetCloneTransform->setAllDimensionsEnabled(false);
            clonePage->addKnob(resetCloneTransform);
            cloneKnobs.push_back(resetCloneTransform);
            knobs.push_back(resetCloneTransform);
            resetCloneTransformKnob = resetCloneTransform;

            node.lock()->addTransformInteract(translate, scale, scaleUniform, rotate, skewX, skewY, skewOrder, center,
                                              boost::shared_ptr<KnobBool>() /*invert*/,
                                              boost::shared_ptr<KnobBool>() /*interactive*/);

            boost::shared_ptr<KnobChoice> filter = AppManager::createKnob<KnobChoice>(effect.get(), tr(kRotoBrushFilterParamLabel), 1, true);
            filter->setName(kRotoBrushFilterParam);
            filter->setHintToolTip( tr(kRotoBrushFilterParamHint) );
            {
                std::vector<ChoiceOption> choices;
                choices.push_back(ChoiceOption(kFilterImpulse, "", tr(kFilterImpulseHint).toStdString()));
                choices.push_back(ChoiceOption(kFilterBilinear, "", tr(kFilterBilinearHint).toStdString()));
                choices.push_back(ChoiceOption(kFilterCubic, "", tr(kFilterCubicHint).toStdString()));
                choices.push_back(ChoiceOption(kFilterKeys, "", tr(kFilterKeysHint).toStdString()));
                choices.push_back(ChoiceOption(kFilterSimon, "", tr(kFilterSimonHint).toStdString()));
                choices.push_back(ChoiceOption(kFilterRifman, "", tr(kFilterRifmanHint).toStdString()));
                choices.push_back(ChoiceOption(kFilterMitchell, "", tr(kFilterMitchellHint).toStdString()));
                choices.push_back(ChoiceOption(kFilterParzen, "", tr(kFilterParzenHint).toStdString()));
                choices.push_back(ChoiceOption(kFilterNotch, "", tr(kFilterNotchHint).toStdString()));
                filter->populateChoices(choices);
            }
            filter->setDefaultValue(2);
            filter->setDefaultAllDimensionsEnabled(false);
            filter->setAddNewLine(false);
            clonePage->addKnob(filter);
            cloneKnobs.push_back(filter);
            knobs.push_back(filter);
            cloneFilterKnob = filter;

            boost::shared_ptr<KnobBool> blackOutside = AppManager::createKnob<KnobBool>(effect.get(), tr(kRotoBrushBlackOutsideParamLabel), 1, true);
            blackOutside->setName(kRotoBrushBlackOutsideParam);
            blackOutside->setHintToolTip( tr(kRotoBrushBlackOutsideParamHint) );
            blackOutside->setDefaultValue(true);
            blackOutside->setDefaultAllDimensionsEnabled(false);
            clonePage->addKnob(blackOutside);
            knobs.push_back(blackOutside);
            cloneKnobs.push_back(blackOutside);
            cloneBlackOutsideKnob = blackOutside;

            boost::shared_ptr<KnobInt> timeOffset = AppManager::createKnob<KnobInt>(effect.get(), tr(kRotoBrushTimeOffsetParamLabel), 1, true);
            timeOffset->setName(kRotoBrushTimeOffsetParam);
            timeOffset->setHintToolTip( tr(kRotoBrushTimeOffsetParamHint) );
            timeOffset->setDisplayMinimum(-100);
            timeOffset->setDisplayMaximum(100);
            timeOffset->setDefaultAllDimensionsEnabled(false);
            timeOffset->setIsPersistent(false);
            timeOffset->setAddNewLine(false);
            clonePage->addKnob(timeOffset);
            cloneKnobs.push_back(timeOffset);
            knobs.push_back(timeOffset);
            timeOffsetKnob = timeOffset;

            boost::shared_ptr<KnobChoice> timeOffsetMode = AppManager::createKnob<KnobChoice>(effect.get(), tr(kRotoBrushTimeOffsetModeParamLabel), 1, true);
            timeOffsetMode->setName(kRotoBrushTimeOffsetModeParam);
            timeOffsetMode->setHintToolTip( tr(kRotoBrushTimeOffsetModeParamHint) );
            {
                std::vector<ChoiceOption> modes;
                modes.push_back(ChoiceOption("Relative"));
                modes.push_back(ChoiceOption("Absolute"));
                timeOffsetMode->populateChoices(modes);
            }
            timeOffsetMode->setDefaultAllDimensionsEnabled(false);
            timeOffsetMode->setIsPersistent(false);
            clonePage->addKnob(timeOffsetMode);
            knobs.push_back(timeOffsetMode);
            cloneKnobs.push_back(timeOffsetMode);
            timeOffsetModeKnob = timeOffsetMode;

            boost::shared_ptr<KnobDouble> brushSize = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoBrushSizeParamLabel), 1, true);
            brushSize->setName(kRotoBrushSizeParam);
            brushSize->setHintToolTip( tr(kRotoBrushSizeParamHint) );
            brushSize->setDefaultValue(25);
            brushSize->setMinimum(1.);
            brushSize->setMaximum(1000);
            brushSize->setDefaultAllDimensionsEnabled(false);
            brushSize->setIsPersistent(false);
            strokePage->addKnob(brushSize);
            knobs.push_back(brushSize);
            strokeKnobs.push_back(brushSize);
            brushSizeKnob = brushSize;

            boost::shared_ptr<KnobDouble> brushSpacing = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoBrushSpacingParamLabel), 1, true);
            brushSpacing->setName(kRotoBrushSpacingParam);
            brushSpacing->setHintToolTip( tr(kRotoBrushSpacingParamHint) );
            brushSpacing->setDefaultValue(0.1);
            brushSpacing->setMinimum(0.);
            brushSpacing->setMaximum(1.);
            brushSpacing->setDefaultAllDimensionsEnabled(false);
            brushSpacing->setIsPersistent(false);
            strokePage->addKnob(brushSpacing);
            knobs.push_back(brushSpacing);
            strokeKnobs.push_back(brushSpacing);
            brushSpacingKnob = brushSpacing;

            boost::shared_ptr<KnobDouble> brushHardness = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoBrushHardnessParamLabel), 1, true);
            brushHardness->setName(kRotoBrushHardnessParam);
            brushHardness->setHintToolTip( tr(kRotoBrushHardnessParamHint) );
            brushHardness->setDefaultValue(0.2);
            brushHardness->setMinimum(0.);
            brushHardness->setMaximum(1.);
            brushHardness->setDefaultAllDimensionsEnabled(false);
            brushHardness->setIsPersistent(false);
            strokePage->addKnob(brushHardness);
            knobs.push_back(brushHardness);
            strokeKnobs.push_back(brushHardness);
            brushHardnessKnob = brushHardness;

            boost::shared_ptr<KnobDouble> effectStrength = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoBrushEffectParamLabel), 1, true);
            effectStrength->setName(kRotoBrushEffectParam);
            effectStrength->setHintToolTip( tr(kRotoBrushEffectParamHint) );
            effectStrength->setDefaultValue(15);
            effectStrength->setMinimum(0.);
            effectStrength->setMaximum(100.);
            effectStrength->setDefaultAllDimensionsEnabled(false);
            effectStrength->setIsPersistent(false);
            strokePage->addKnob(effectStrength);
            knobs.push_back(effectStrength);
            strokeKnobs.push_back(effectStrength);
            brushEffectKnob = effectStrength;

            boost::shared_ptr<KnobSeparator> pressureLabel = AppManager::createKnob<KnobSeparator>( effect.get(), tr(kRotoBrushPressureLabelParamLabel) );
            pressureLabel->setName(kRotoBrushPressureLabelParam);
            pressureLabel->setHintToolTip( tr(kRotoBrushPressureLabelParamHint) );
            strokePage->addKnob(pressureLabel);
            knobs.push_back(pressureLabel);
            strokeKnobs.push_back(pressureLabel);
            pressureLabelKnob = pressureLabel;

            boost::shared_ptr<KnobBool> pressureOpacity = AppManager::createKnob<KnobBool>( effect.get(), tr(kRotoBrushPressureOpacityParamLabel) );
            pressureOpacity->setName(kRotoBrushPressureOpacityParam);
            pressureOpacity->setHintToolTip( tr(kRotoBrushPressureOpacityParamHint) );
            pressureOpacity->setAnimationEnabled(false);
            pressureOpacity->setDefaultValue(true);
            pressureOpacity->setAddNewLine(false);
            pressureOpacity->setDefaultAllDimensionsEnabled(false);
            pressureOpacity->setIsPersistent(false);
            strokePage->addKnob(pressureOpacity);
            knobs.push_back(pressureOpacity);
            strokeKnobs.push_back(pressureOpacity);
            pressureOpacityKnob = pressureOpacity;

            boost::shared_ptr<KnobBool> pressureSize = AppManager::createKnob<KnobBool>( effect.get(), tr(kRotoBrushPressureSizeParamLabel) );
            pressureSize->setName(kRotoBrushPressureSizeParam);
            pressureSize->setHintToolTip( tr(kRotoBrushPressureSizeParamHint) );
            pressureSize->setAnimationEnabled(false);
            pressureSize->setDefaultValue(false);
            pressureSize->setAddNewLine(false);
            pressureSize->setDefaultAllDimensionsEnabled(false);
            pressureSize->setIsPersistent(false);
            knobs.push_back(pressureSize);
            strokeKnobs.push_back(pressureSize);
            strokePage->addKnob(pressureSize);
            pressureSizeKnob = pressureSize;

            boost::shared_ptr<KnobBool> pressureHardness = AppManager::createKnob<KnobBool>( effect.get(), tr(kRotoBrushPressureHardnessParamLabel) );
            pressureHardness->setName(kRotoBrushPressureHardnessParam);
            pressureHardness->setHintToolTip( tr(kRotoBrushPressureHardnessParamHint) );
            pressureHardness->setAnimationEnabled(false);
            pressureHardness->setDefaultValue(false);
            pressureHardness->setAddNewLine(true);
            pressureHardness->setDefaultAllDimensionsEnabled(false);
            pressureHardness->setIsPersistent(false);
            knobs.push_back(pressureHardness);
            strokeKnobs.push_back(pressureHardness);
            strokePage->addKnob(pressureHardness);
            pressureHardnessKnob = pressureHardness;

            boost::shared_ptr<KnobBool> buildUp = AppManager::createKnob<KnobBool>( effect.get(), tr(kRotoBrushBuildupParamLabel) );
            buildUp->setName(kRotoBrushBuildupParam);
            buildUp->setHintToolTip( tr(kRotoBrushBuildupParamHint) );
            buildUp->setAnimationEnabled(false);
            buildUp->setDefaultValue(false);
            buildUp->setAddNewLine(true);
            buildUp->setDefaultAllDimensionsEnabled(false);
            buildUp->setIsPersistent(false);
            knobs.push_back(buildUp);
            strokeKnobs.push_back(buildUp);
            strokePage->addKnob(buildUp);
            buildUpKnob = buildUp;

            boost::shared_ptr<KnobDouble> visiblePortion = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoBrushVisiblePortionParamLabel), 2, true);
            visiblePortion->setName(kRotoBrushVisiblePortionParam);
            visiblePortion->setHintToolTip( tr(kRotoBrushVisiblePortionParamHint) );
            visiblePortion->setDefaultValue(0, 0);
            visiblePortion->setDefaultValue(1, 1);
            std::vector<double> mins, maxs;
            mins.push_back(0);
            mins.push_back(0);
            maxs.push_back(1);
            maxs.push_back(1);
            visiblePortion->setMinimumsAndMaximums(mins, maxs);
            visiblePortion->setDefaultAllDimensionsEnabled(false);
            visiblePortion->setIsPersistent(false);
            strokePage->addKnob(visiblePortion);
            visiblePortion->setDimensionName(0, "start");
            visiblePortion->setDimensionName(1, "end");
            knobs.push_back(visiblePortion);
            strokeKnobs.push_back(visiblePortion);
            brushVisiblePortionKnob = visiblePortion;
        }

        boost::shared_ptr<KnobDouble> translate = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoDrawableItemTranslateParamLabel), 2, true);
        translate->setName(kRotoDrawableItemTranslateParam);
        translate->setHintToolTip( tr(kRotoDrawableItemTranslateParamHint) );
        translate->setDefaultAllDimensionsEnabled(false);
        translate->setIncrement(10);
        transformPage->addKnob(translate);
        knobs.push_back(translate);
        translateKnob = translate;

        boost::shared_ptr<KnobDouble> rotate = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoDrawableItemRotateParamLabel), 1, true);
        rotate->setName(kRotoDrawableItemRotateParam);
        rotate->setHintToolTip( tr(kRotoDrawableItemRotateParamHint) );
        rotate->setDefaultAllDimensionsEnabled(false);
        rotate->setDisplayMinimum(-180);
        rotate->setDisplayMaximum(180);
        transformPage->addKnob(rotate);
        knobs.push_back(rotate);
        rotateKnob = rotate;

        boost::shared_ptr<KnobDouble> scale = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoDrawableItemScaleParamLabel), 2, true);
        scale->setName(kRotoDrawableItemScaleParam);
        scale->setHintToolTip( tr(kRotoDrawableItemScaleParamHint) );
        scale->setDefaultValue(1, 0);
        scale->setDefaultValue(1, 1);
        scale->setDisplayMinimum(0.1, 0);
        scale->setDisplayMinimum(0.1, 1);
        scale->setDisplayMaximum(10, 0);
        scale->setDisplayMaximum(10, 1);
        scale->setAddNewLine(false);
        scale->setDefaultAllDimensionsEnabled(false);
        transformPage->addKnob(scale);
        knobs.push_back(scale);
        scaleKnob = scale;

        boost::shared_ptr<KnobBool> scaleUniform = AppManager::createKnob<KnobBool>(effect.get(), tr(kRotoDrawableItemScaleUniformParamLabel), 1, true);
        scaleUniform->setName(kRotoDrawableItemScaleUniformParam);
        scaleUniform->setHintToolTip( tr(kRotoDrawableItemScaleUniformParamHint) );
        scaleUniform->setDefaultValue(true);
        scaleUniform->setDefaultAllDimensionsEnabled(false);
        scaleUniform->setAnimationEnabled(false);
        transformPage->addKnob(scaleUniform);
        knobs.push_back(scaleUniform);
        scaleUniformKnob = scaleUniform;

        boost::shared_ptr<KnobDouble> skewX = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoDrawableItemSkewXParamLabel), 1, true);
        skewX->setName(kRotoDrawableItemSkewXParam);
        skewX->setHintToolTip( tr(kRotoDrawableItemSkewXParamHint) );
        skewX->setDefaultAllDimensionsEnabled(false);
        skewX->setDisplayMinimum(-1, 0);
        skewX->setDisplayMaximum(1, 0);
        transformPage->addKnob(skewX);
        knobs.push_back(skewX);
        skewXKnob = skewX;

        boost::shared_ptr<KnobDouble> skewY = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoDrawableItemSkewYParamLabel), 1, true);
        skewY->setName(kRotoDrawableItemSkewYParam);
        skewY->setHintToolTip( tr(kRotoDrawableItemSkewYParamHint) );
        skewY->setDefaultAllDimensionsEnabled(false);
        skewY->setDisplayMinimum(-1, 0);
        skewY->setDisplayMaximum(1, 0);
        transformPage->addKnob(skewY);
        knobs.push_back(skewY);
        skewYKnob = skewY;

        boost::shared_ptr<KnobChoice> skewOrder = AppManager::createKnob<KnobChoice>(effect.get(), tr(kRotoDrawableItemSkewOrderParamLabel), 1, true);
        skewOrder->setName(kRotoDrawableItemSkewOrderParam);
        skewOrder->setHintToolTip( tr(kRotoDrawableItemSkewOrderParamHint) );
        skewOrder->setDefaultValue(0);
        {
            std::vector<ChoiceOption> choices;
            choices.push_back(ChoiceOption("XY"));
            choices.push_back(ChoiceOption("YX"));
            skewOrder->populateChoices(choices);
        }
        skewOrder->setDefaultAllDimensionsEnabled(false);
        skewOrder->setAnimationEnabled(false);
        transformPage->addKnob(skewOrder);
        knobs.push_back(skewOrder);
        skewOrderKnob = skewOrder;

        boost::shared_ptr<KnobDouble> center = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoDrawableItemCenterParamLabel), 2, true);
        center->setName(kRotoDrawableItemCenterParam);
        center->setHintToolTip( tr(kRotoDrawableItemCenterParamHint) );
        center->setDefaultAllDimensionsEnabled(false);
        center->setDefaultValuesAreNormalized(true);
        center->setAddNewLine(false);
        center->setDefaultValue(0.5, 0);
        center->setDefaultValue(0.5, 1);
        transformPage->addKnob(center);
        knobs.push_back(center);
        centerKnob = center;

        boost::shared_ptr<KnobButton> resetCenter = AppManager::createKnob<KnobButton>(effect.get(), tr(kRotoResetCenterParamLabel), 1, true);
        resetCenter->setName(kRotoResetCenterParam);
        resetCenter->setHintToolTip( tr(kRotoResetCenterParamHint) );
        resetCenter->setAllDimensionsEnabled(false);
        transformPage->addKnob(resetCenter);
        knobs.push_back(resetCenter);
        resetCenterKnob = resetCenter;

        boost::shared_ptr<KnobBool> transformInteractive = AppManager::createKnob<KnobBool>(effect.get(), tr(kRotoTransformInteractiveLabel), 1, true);
        transformInteractive->setName(kRotoTransformInteractive);
        transformInteractive->setHintToolTip(tr(kRotoTransformInteractiveHint));
        transformInteractive->setDefaultValue(true);
        transformInteractive->setAllDimensionsEnabled(false);
        transformPage->addKnob(transformInteractive);
        knobs.push_back(transformInteractive);
        transformInteractiveKnob = transformInteractive;



        boost::shared_ptr<KnobDouble> extraMatrix = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoDrawableItemExtraMatrixParamLabel), 9, true);
        extraMatrix->setName(kRotoDrawableItemExtraMatrixParam);
        extraMatrix->setHintToolTip( tr(kRotoDrawableItemExtraMatrixParamHint) );
        extraMatrix->setDefaultAllDimensionsEnabled(false);
        // Set to identity
        extraMatrix->setDefaultValue(1, 0);
        extraMatrix->setDefaultValue(1, 4);
        extraMatrix->setDefaultValue(1, 8);
        transformPage->addKnob(extraMatrix);
        knobs.push_back(extraMatrix);
        extraMatrixKnob = extraMatrix;

        boost::shared_ptr<KnobButton> resetTransform = AppManager::createKnob<KnobButton>(effect.get(), tr(kRotoResetTransformParamLabel), 1, true);
        resetTransform->setName(kRotoResetTransformParam);
        resetTransform->setHintToolTip( tr(kRotoResetTransformParamHint) );
        resetTransform->setAllDimensionsEnabled(false);
        transformPage->addKnob(resetTransform);
        knobs.push_back(resetTransform);
        resetTransformKnob = resetTransform;

        node.lock()->addTransformInteract(translate, scale, scaleUniform, rotate, skewX, skewY, skewOrder, center,
                                          boost::shared_ptr<KnobBool>() /*invert*/,
                                          transformInteractive /*interactive*/);


#ifdef NATRON_ROTO_ENABLE_MOTION_BLUR
        boost::shared_ptr<KnobPage> mbPage = AppManager::createKnob<KnobPage>(effect.get(), tr("Motion Blur"), 1, true);
        boost::shared_ptr<KnobChoice> motionBlurType = AppManager::createKnob<KnobChoice>(effect.get(), tr(kRotoMotionBlurModeParamLabel), 1, true);
        motionBlurType->setName(kRotoMotionBlurModeParam);
        motionBlurType->setHintToolTip( tr(kRotoMotionBlurModeParamHint) );
        motionBlurType->setAnimationEnabled(false);
        {
            std::vector<std::string> entries;
            entries.push_back("Per-Shape");
            entries.push_back("Global");
            motionBlurType->populateChoices(entries);
        }
        mbPage->addKnob(motionBlurType);
        motionBlurTypeKnob = motionBlurType;
        knobs.push_back(motionBlurType);


        //////Per shape motion blur parameters
        boost::shared_ptr<KnobDouble> motionBlur = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoMotionBlurParamLabel), 1, true);
        motionBlur->setName(kRotoPerShapeMotionBlurParam);
        motionBlur->setHintToolTip( tr(kRotoMotionBlurParamHint) );
        motionBlur->setDefaultValue(0);
        motionBlur->setMinimum(0);
        motionBlur->setDisplayMinimum(0);
        motionBlur->setDisplayMaximum(4);
        motionBlur->setAllDimensionsEnabled(false);
        motionBlur->setIsPersistent(false);
        motionBlur->setMaximum(4);
        shapeKnobs.push_back(motionBlur);
        mbPage->addKnob(motionBlur);
        motionBlurKnob = motionBlur;
        knobs.push_back(motionBlur);

        boost::shared_ptr<KnobDouble> shutter = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoShutterParamLabel), 1, true);
        shutter->setName(kRotoPerShapeShutterParam);
        shutter->setHintToolTip( tr(kRotoShutterParamHint) );
        shutter->setDefaultValue(0.5);
        shutter->setMinimum(0);
        shutter->setDisplayMinimum(0);
        shutter->setDisplayMaximum(2);
        shutter->setMaximum(2);
        shutter->setAllDimensionsEnabled(false);
        shutter->setIsPersistent(false);
        shapeKnobs.push_back(shutter);
        mbPage->addKnob(shutter);
        shutterKnob = shutter;
        knobs.push_back(shutter);

        boost::shared_ptr<KnobChoice> shutterType = AppManager::createKnob<KnobChoice>(effect.get(), tr(kRotoShutterOffsetTypeParamLabel), 1, true);
        shutterType->setName(kRotoPerShapeShutterOffsetTypeParam);
        shutterType->setHintToolTip( tr(kRotoShutterOffsetTypeParamHint) );
        shutterType->setDefaultValue(0);
        {
            std::vector<ChoiceOption> options;
            options.push_back(ChoiceOption("Centered", "", tr(kRotoShutterOffsetCenteredHint).toStdString()));
            options.push_back(ChoiceOption("Start", "", tr(kRotoShutterOffsetStartHint).toStdString()));
            options.push_back(ChoiceOption("End", "", tr(kRotoShutterOffsetEndHint).toStdString()));
            options.push_back(ChoiceOption("Custom", "", tr(kRotoShutterOffsetCustomHint).toStdString()));
            shutterType->populateChoices(options);
        }
        shutterType->setAllDimensionsEnabled(false);
        shutterType->setAddNewLine(false);
        shutterType->setIsPersistent(false);
        mbPage->addKnob(shutterType);
        shutterTypeKnob = shutterType;
        shapeKnobs.push_back(shutterType);
        knobs.push_back(shutterType);

        boost::shared_ptr<KnobDouble> customOffset = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoShutterCustomOffsetParamLabel), 1, true);
        customOffset->setName(kRotoPerShapeShutterCustomOffsetParam);
        customOffset->setHintToolTip( tr(kRotoShutterCustomOffsetParamHint) );
        customOffset->setDefaultValue(0);
        customOffset->setAllDimensionsEnabled(false);
        customOffset->setIsPersistent(false);
        mbPage->addKnob(customOffset);
        customOffsetKnob = customOffset;
        shapeKnobs.push_back(customOffset);
        knobs.push_back(customOffset);

        //////Global motion blur parameters
        boost::shared_ptr<KnobDouble> globalMotionBlur = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoMotionBlurParamLabel), 1, true);
        globalMotionBlur->setName(kRotoGlobalMotionBlurParam);
        globalMotionBlur->setHintToolTip( tr(kRotoMotionBlurParamHint) );
        globalMotionBlur->setDefaultValue(0);
        globalMotionBlur->setMinimum(0);
        globalMotionBlur->setDisplayMinimum(0);
        globalMotionBlur->setDisplayMaximum(4);
        globalMotionBlur->setMaximum(4);
        globalMotionBlur->setSecretByDefault(true);
        mbPage->addKnob(globalMotionBlur);
        globalMotionBlurKnob = globalMotionBlur;
        knobs.push_back(globalMotionBlur);

        boost::shared_ptr<KnobDouble> globalShutter = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoShutterParamLabel), 1, true);
        globalShutter->setName(kRotoGlobalShutterParam);
        globalShutter->setHintToolTip( tr(kRotoShutterParamHint) );
        globalShutter->setDefaultValue(0.5);
        globalShutter->setMinimum(0);
        globalShutter->setDisplayMinimum(0);
        globalShutter->setDisplayMaximum(2);
        globalShutter->setMaximum(2);
        globalShutter->setSecretByDefault(true);
        mbPage->addKnob(globalShutter);
        globalShutterKnob = globalShutter;
        knobs.push_back(globalShutter);

        boost::shared_ptr<KnobChoice> globalShutterType = AppManager::createKnob<KnobChoice>(effect.get(), tr(kRotoShutterOffsetTypeParamLabel), 1, true);
        globalShutterType->setName(kRotoGlobalShutterOffsetTypeParam);
        globalShutterType->setHintToolTip( tr(kRotoShutterOffsetTypeParamHint) );
        globalShutterType->setDefaultValue(0);
        {
            std::vector<std::string> options, helps;
            options.push_back("Centered");
            helps.push_back(kRotoShutterOffsetCenteredHint);
            options.push_back("Start");
            helps.push_back(kRotoShutterOffsetStartHint);
            options.push_back("End");
            helps.push_back(kRotoShutterOffsetEndHint);
            options.push_back("Custom");
            helps.push_back(kRotoShutterOffsetCustomHint);
            globalShutterType->populateChoices(options, helps);
        }
        globalShutterType->setAddNewLine(false);
        globalShutterType->setSecretByDefault(true);
        mbPage->addKnob(globalShutterType);
        globalShutterTypeKnob = globalShutterType;
        knobs.push_back(globalShutterType);

        boost::shared_ptr<KnobDouble> globalCustomOffset = AppManager::createKnob<KnobDouble>(effect.get(), tr(kRotoShutterCustomOffsetParamLabel), 1, true);
        globalCustomOffset->setName(kRotoPerShapeShutterCustomOffsetParam);
        globalCustomOffset->setHintToolTip( tr(kRotoShutterCustomOffsetParamHint) );
        globalCustomOffset->setDefaultValue(0);
        globalCustomOffset->setSecretByDefault(true);
        mbPage->addKnob(globalCustomOffset);
        globalCustomOffsetKnob = globalCustomOffset;
        knobs.push_back(globalCustomOffset);

#endif // ifdef NATRON_ROTO_ENABLE_MOTION_BLUR
    }

    /**
     * @brief Call this after any change to notify the mask has changed for the cache.
     **/
    void incrementRotoAge()
    {
        ///MT-safe: only called on the main-thread
        assert( QThread::currentThread() == qApp->thread() );

        QMutexLocker l(&rotoContextMutex);
        ++age;
    }

    boost::shared_ptr<RotoLayer>findDeepestSelectedLayer() const
    {
        assert( !rotoContextMutex.tryLock() );

        int minLevel = -1;
        boost::shared_ptr<RotoLayer> minLayer;
        for (std::list< boost::shared_ptr<RotoItem> >::const_iterator it = selectedItems.begin();
             it != selectedItems.end(); ++it) {
            int lvl = (*it)->getHierarchyLevel();
            if (lvl > minLevel) {
                boost::shared_ptr<RotoLayer> isLayer = boost::dynamic_pointer_cast<RotoLayer>(*it);
                if (isLayer) {
                    minLayer = isLayer;
                } else {
                    minLayer = (*it)->getParentLayer();
                }
                minLevel = lvl;
            }
        }

        return minLayer;
    }

    static void renderDot(cairo_t* cr,
                          std::vector<cairo_pattern_t*>* dotPatterns,
                          const Point &center,
                          double internalDotRadius,
                          double externalDotRadius,
                          double pressure,
                          bool doBuildUp,
                          const std::vector<std::pair<double, double> >& opacityStops,
                          double opacity);
    static double renderStroke(cairo_t* cr,
                               std::vector<cairo_pattern_t*>& dotPatterns,
                               const std::list<std::list<std::pair<Point, double> > >& strokes,
                               double distToNext,
                               const RotoDrawableItem* stroke,
                               bool doBuildup,
                               double opacity,
                               double time,
                               unsigned int mipmapLevel);
    static void renderBezier(cairo_t* cr, const Bezier* bezier, double opacity, double time, double startTime, double endTime, double mbFrameStep, unsigned int mipmapLevel);
    static void renderFeather(const Bezier * bezier, double time, unsigned int mipmapLevel, double shapeColor[3], double opacity, double featherDist, double fallOff, cairo_pattern_t * mesh);
    static void renderFeather_cairo(const std::list<RotoFeatherVertex>& vertices, double shapeColor[3],  double fallOff, cairo_pattern_t * mesh);
    static void renderInternalShape_cairo(const std::list<RotoTriangles>& triangles,
                                          const std::list<RotoTriangleFans>& fans,
                                          const std::list<RotoTriangleStrips>& strips,
                                          double shapeColor[3],  cairo_pattern_t * mesh);
    static void computeTriangles(const Bezier * bezier, double time, unsigned int mipmapLevel,  double featherDist, std::list<RotoFeatherVertex>* featherMesh, std::list<RotoTriangleFans>* internalFans, std::list<RotoTriangles>* internalTriangles,std::list<RotoTriangleStrips>* internalStrips);
    static void renderInternalShape(double time, unsigned int mipmapLevel, double shapeColor[3], double opacity, const Transform::Matrix3x3 & transform, cairo_t * cr, cairo_pattern_t * mesh, const BezierCPs &cps);
    static void bezulate(double time, const BezierCPs& cps, std::list<BezierCPs>* patches);
    static void applyAndDestroyMask(cairo_t* cr, cairo_pattern_t* mesh);
};

NATRON_NAMESPACE_EXIT;

#endif // ROTOCONTEXTPRIVATE_H
