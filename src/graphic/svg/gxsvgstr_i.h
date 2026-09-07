/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021.9.6
 *
 *  Copyright (C) 2021 miragekiller
 */

#if !defined(SVG_STR_ATTRIB) && !defined(SVG_STR_TAG)
#   error "must define SVG_STR_ATTRIB2 or SVG_STR_TAG"
#endif


#ifdef SVG_STR_ATTRIB
#   define SVG_STR_ATTRIB1(x)   SVG_STR_ATTRIB2(x,x)
#   define SVG_STR_ATTRIB2      SVG_STR_ATTRIB

    SVG_STR_ATTRIB1(display)
    SVG_STR_ATTRIB1(fill)
    SVG_STR_ATTRIB1(opacity)
    SVG_STR_ATTRIB2(fill-opacity, fill_opacity)
    SVG_STR_ATTRIB1(stroke)
    SVG_STR_ATTRIB2(stroke-width, stroke_width)
    SVG_STR_ATTRIB2(stroke-dasharray, stroke_dasharray)
    SVG_STR_ATTRIB2(stroke-dashoffset, stroke_dashoffset)
    SVG_STR_ATTRIB2(stroke-opacity, stroke_opacity)
    SVG_STR_ATTRIB2(stroke-linecap, stroke_linecap)
    SVG_STR_ATTRIB2(stroke-linejoin, stroke_linejoin)
    SVG_STR_ATTRIB2(stroke-miterlimit, stroke_miterlimit)
    SVG_STR_ATTRIB2(fill-rule, fill_rule)
    SVG_STR_ATTRIB2(font-size, font_size)

    SVG_STR_ATTRIB1(transform)
    // SVG_STR_N(transform_matrix)
    // SVG_STR_N(transform_translateX)
    // SVG_STR_N(transform_translateY)
    // SVG_STR_N(transform_scale)
    // SVG_STR_N(transform_rotate)
    // SVG_STR_N(transform_skewX)
    // SVG_STR_N(transform_skewY)
    // SVG_STR_N(transform_translate)
    // SVG_STR_N(transform2_matrix)
    // SVG_STR_N(transform2_translateX)
    // SVG_STR_N(transform2_translateY)
    // SVG_STR_N(transform2_scale)
    // SVG_STR_N(transform2_rotate)
    // SVG_STR_N(transform2_skewX)
    // SVG_STR_N(transform2_skewY)

    SVG_STR_ATTRIB1(gradientTransform)
    // SVG_STR_N(gradientTransform_matrix)
    // SVG_STR_N(gradientTransform_translateX)
    // SVG_STR_N(gradientTransform_translateY)
    // SVG_STR_N(gradientTransform_scale)
    // SVG_STR_N(gradientTransform_rotate)
    // SVG_STR_N(gradientTransform_skewX)
    // SVG_STR_N(gradientTransform_skewY)
    // SVG_STR_N(gradientTransform_translate)
    // SVG_STR_N(gradientTransform2_matrix)
    // SVG_STR_N(gradientTransform2_translateX)
    // SVG_STR_N(gradientTransform2_translateY)
    // SVG_STR_N(gradientTransform2_scale)
    // SVG_STR_N(gradientTransform2_rotate)
    // SVG_STR_N(gradientTransform2_skewX)
    // SVG_STR_N(gradientTransform2_skewY)

    SVG_STR_ATTRIB2(stop-color, stop_color)
    SVG_STR_ATTRIB2(stop-opacity, stop_opacity)
    SVG_STR_ATTRIB1(offset)
    SVG_STR_ATTRIB1(id)
    SVG_STR_ATTRIB1(d)
    SVG_STR_ATTRIB1(x)
    SVG_STR_ATTRIB1(y)
    SVG_STR_ATTRIB1(width)
    SVG_STR_ATTRIB1(height)
    SVG_STR_ATTRIB1(rx)
    SVG_STR_ATTRIB1(ry)
    SVG_STR_ATTRIB1(cx)
    SVG_STR_ATTRIB1(cy)
    SVG_STR_ATTRIB1(r)
    SVG_STR_ATTRIB1(x1)
    SVG_STR_ATTRIB1(y1)
    SVG_STR_ATTRIB1(x2)
    SVG_STR_ATTRIB1(y2)
    SVG_STR_ATTRIB1(points)
    SVG_STR_ATTRIB2(xlink:href, xlink_href)
    SVG_STR_ATTRIB2(href, href)
    SVG_STR_ATTRIB1(viewBox)
    SVG_STR_ATTRIB1(preserveAspectRatio)
    SVG_STR_ATTRIB1(gradientUnits)
    SVG_STR_ATTRIB1(fx)
    SVG_STR_ATTRIB1(fy)
    SVG_STR_ATTRIB1(spreadMethod)

    SVG_STR_ATTRIB1(style)

    // animate
    SVG_STR_ATTRIB1(attributeName)
    SVG_STR_ATTRIB1(attributeType)
    SVG_STR_ATTRIB1(type)
    SVG_STR_ATTRIB1(from)
    SVG_STR_ATTRIB1(to)
    SVG_STR_ATTRIB1(dur)
    SVG_STR_ATTRIB1(repeatCount)
    SVG_STR_ATTRIB1(path)
    SVG_STR_ATTRIB1(keyTimes)
    SVG_STR_ATTRIB1(values)

    // style::animation
    SVG_STR_ATTRIB1(animation)
    SVG_STR_ATTRIB2(animation-name, animation_name)
    SVG_STR_ATTRIB2(animation-duration, animation_duration)
    SVG_STR_ATTRIB2(animation-timing-function, animation_timing_function)
    SVG_STR_ATTRIB2(animation-delay, animation_delay)
    SVG_STR_ATTRIB2(animation-iteration-count, animation_iteration_count)
    SVG_STR_ATTRIB2(animation-direction, animation_direction)
    SVG_STR_ATTRIB2(animation-fill-mode, animation_fill_mode)

#   undef SVG_STR_ATTRIB2
#   undef SVG_STR_ATTRIB1
#   undef SVG_STR_ATTRIB

#endif // SVG_STR_ATTRIB2


#ifdef SVG_STR_TAG

    SVG_STR_TAG(g)
    SVG_STR_TAG(path)
    SVG_STR_TAG(rect)
    SVG_STR_TAG(circle)
    SVG_STR_TAG(ellipse)
    SVG_STR_TAG(line)
    SVG_STR_TAG(polyline)
    SVG_STR_TAG(polygon)
    SVG_STR_TAG(linearGradient)
    SVG_STR_TAG(radialGradient)
    SVG_STR_TAG(stop)
    SVG_STR_TAG(defs)
//    SVG_STR_TAG(symbol)
    SVG_STR_TAG(use)
    SVG_STR_TAG(svg)
    SVG_STR_TAG(style)
    SVG_STR_TAG(matrix)
    SVG_STR_TAG(animate)
    SVG_STR_TAG(animateTransform)

//     SVG_STR_TAG(translate)
//     SVG_STR_TAG(translateX)
//     SVG_STR_TAG(translateY)
//     SVG_STR_TAG(scale)
//     SVG_STR_TAG(scaleX)
//     SVG_STR_TAG(scaleY)
//     SVG_STR_TAG(rotate)
//     SVG_STR_TAG(skewX)
//     SVG_STR_TAG(skewY)

#   undef SVG_STR_TAG

#endif // SVG_STR_TAG

