/* This file is part of the KDE project
   Copyright (C) 2004-2006 David Faure <faure@kde.org>
   Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KoOdfGraphicStyles.h"

#include <QBrush>
#include <QBuffer>
#include <QPen>

#include <kdebug.h>

#include <KoGenStyle.h>
#include <KoGenStyles.h>
#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoStyleStack.h>
#include <KoUnit.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>

#include "KoOasisLoadingContext.h"
#include "KoOasisStyles.h"
#include "KoPictureShared.h"

void KoOdfGraphicStyles::saveOasisFillStyle( KoGenStyle &styleFill, KoGenStyles& mainStyles, const QBrush & brush )
{
    switch( brush.style() )
    {
    case Qt::SolidPattern:
        styleFill.addProperty( "draw:fill","solid" );
        styleFill.addProperty( "draw:fill-color", brush.color().name() );
        if( ! brush.isOpaque() )
            styleFill.addProperty( "draw:opacity", QString("%1%").arg( brush.color().alphaF() * 100.0 ) );
        break;
    case Qt::Dense1Pattern:
        styleFill.addProperty( "draw:transparency", "94%" );
        styleFill.addProperty( "draw:fill","solid" );
        styleFill.addProperty( "draw:fill-color", brush.color().name() );
        break;
    case Qt::Dense2Pattern:
        styleFill.addProperty( "draw:transparency", "88%" );
        styleFill.addProperty( "draw:fill","solid" );
        styleFill.addProperty( "draw:fill-color", brush.color().name() );
        break;
    case Qt::Dense3Pattern:
        styleFill.addProperty( "draw:transparency", "63%" );
        styleFill.addProperty( "draw:fill","solid" );
        styleFill.addProperty( "draw:fill-color", brush.color().name() );
        break;
    case Qt::Dense4Pattern:
        styleFill.addProperty( "draw:transparency", "50%" );
        styleFill.addProperty( "draw:fill","solid" );
        styleFill.addProperty( "draw:fill-color", brush.color().name() );
        break;
    case Qt::Dense5Pattern:
        styleFill.addProperty( "draw:transparency", "37%" );
        styleFill.addProperty( "draw:fill","solid" );
        styleFill.addProperty( "draw:fill-color", brush.color().name() );
        break;
    case Qt::Dense6Pattern:
        styleFill.addProperty( "draw:transparency", "12%" );
        styleFill.addProperty( "draw:fill","solid" );
        styleFill.addProperty( "draw:fill-color", brush.color().name() );
        break;
    case Qt::Dense7Pattern:
        styleFill.addProperty( "draw:transparency", "6%" );
        styleFill.addProperty( "draw:fill","solid" );
        styleFill.addProperty( "draw:fill-color", brush.color().name() );
        break;
    case Qt::LinearGradientPattern:
    case Qt::RadialGradientPattern:
    case Qt::ConicalGradientPattern:
        styleFill.addProperty( "draw:fill","gradient" );
        styleFill.addProperty( "draw:fill-gradient-name", saveOasisGradientStyle( mainStyles, brush ) );
        break;
    default: //otherstyle
        styleFill.addProperty( "draw:fill","hatch" );
        styleFill.addProperty( "draw:fill-hatch-name", saveOasisHatchStyle( mainStyles, brush ) );
        break;
    }
}

QString KoOdfGraphicStyles::saveOasisHatchStyle( KoGenStyles& mainStyles, const QBrush &brush )
{
    KoGenStyle hatchStyle( KoGenStyle::StyleHatch /*no family name*/);
    hatchStyle.addAttribute( "draw:color", brush.color().name() );
    //hatchStyle.addAttribute( "draw:distance", m_distance ); not implemented into kpresenter
    switch( brush.style() )
    {
    case Qt::HorPattern:
        hatchStyle.addAttribute( "draw:style", "single" );
        hatchStyle.addAttribute( "draw:rotation", 0);
        break;
    case Qt::BDiagPattern:
        hatchStyle.addAttribute( "draw:style", "single" );
        hatchStyle.addAttribute( "draw:rotation", 450);
        break;
    case Qt::VerPattern:
        hatchStyle.addAttribute( "draw:style", "single" );
        hatchStyle.addAttribute( "draw:rotation", 900);
        break;
    case Qt::FDiagPattern:
        hatchStyle.addAttribute( "draw:style", "single" );
        hatchStyle.addAttribute( "draw:rotation", 1350);
        break;
    case Qt::CrossPattern:
        hatchStyle.addAttribute( "draw:style", "double" );
        hatchStyle.addAttribute( "draw:rotation", 0);
        break;
    case Qt::DiagCrossPattern:
        hatchStyle.addAttribute( "draw:style", "double" );
        hatchStyle.addAttribute( "draw:rotation", 450);
        break;
    default:
        break;
    }

    return mainStyles.lookup( hatchStyle, "hatch" );
}

QString KoOdfGraphicStyles::saveOasisGradientStyle( KoGenStyles &mainStyles, const QBrush &brush )
{
    KoGenStyle gradientStyle;
    if( brush.style() == Qt::RadialGradientPattern )
    {
        const QRadialGradient *gradient = static_cast<const QRadialGradient*>( brush.gradient() );
        gradientStyle = KoGenStyle( KoGenStyle::StyleGradientRadial /*no family name*/ );
        gradientStyle.addAttributePt( "svg:cx", gradient->center().x() );
        gradientStyle.addAttributePt( "svg:cy", gradient->center().y() );
        gradientStyle.addAttributePt( "svg:r",  gradient->radius() );
        gradientStyle.addAttributePt( "svg:fx", gradient->focalPoint().x() );
        gradientStyle.addAttributePt( "svg:fy", gradient->focalPoint().y() );
    }
    else if( brush.style() == Qt::LinearGradientPattern )
    {
        const QLinearGradient *gradient = static_cast<const QLinearGradient*>( brush.gradient() );
        gradientStyle = KoGenStyle( KoGenStyle::StyleGradientLinear /*no family name*/ );
        gradientStyle.addAttributePt( "svg:x1", gradient->start().x() );
        gradientStyle.addAttributePt( "svg:y1", gradient->start().y() );
        gradientStyle.addAttributePt( "svg:x2", gradient->finalStop().x() );
        gradientStyle.addAttributePt( "svg:y2", gradient->finalStop().y() );
    }
    const QGradient * gradient = brush.gradient();
    if( gradient->spread() == QGradient::RepeatSpread )
        gradientStyle.addAttribute( "svg:spreadMethod", "repeat" );
    else if( gradient->spread() == QGradient::ReflectSpread )
        gradientStyle.addAttribute( "svg:spreadMethod", "reflect" );
    else
        gradientStyle.addAttribute( "svg:spreadMethod", "pad" );

    if( ! brush.matrix().isIdentity() )
    {
        gradientStyle.addAttribute( "svg:gradientTransform", KoOasisStyles::saveTransformation( brush.matrix() ) );
    }

    QBuffer buffer;
    buffer.open( QIODevice::WriteOnly );
    KoXmlWriter elementWriter( &buffer );  // TODO pass indentation level

    // save stops
    QGradientStops stops = gradient->stops();
    foreach( QGradientStop stop, stops )
    {
        elementWriter.startElement( "svg:stop" );
        elementWriter.addAttribute( "svg:offset", QString( "%1" ).arg( stop.first ) );
        elementWriter.addAttribute( "svg:stop-color", stop.second.name() );
        if( stop.second.alphaF() < 1.0 )
            elementWriter.addAttribute( "svg:stop-opacity", QString( "%1" ).arg( stop.second.alphaF() ) );
        elementWriter.endElement();
    }

    QString elementContents = QString::fromUtf8( buffer.buffer(), buffer.buffer().size() );
    gradientStyle.addChildElement( "svg:stop", elementContents );

    return mainStyles.lookup( gradientStyle, "gradient" );
}

QBrush KoOdfGraphicStyles::loadOasisGradientStyle( const KoStyleStack &styleStack, const KoOasisStyles & oasisStyles, const QSizeF &size )
{
    QString styleName = styleStack.property( KoXmlNS::draw, "fill-gradient-name" );

    KoXmlElement* e = oasisStyles.drawStyles()[styleName];
    if( ! e )
        return QBrush();

    QGradient * gradient = 0;
    QMatrix matrix;

    if( e->namespaceURI() == KoXmlNS::draw && e->localName() == "gradient" )
    {
        // FIXME seems like oo renders the gradient start stop color at the center of the
        // radial gradient, and the start color at the radius of the radial gradient
        // whereas it is not mentioned in the spec how it should be rendered
        // note that svg defines that exactly as the opposite as oo does
        // so what should we do?
        QString type = e->attributeNS( KoXmlNS::draw, "style", QString() );
        if( type == "radial" )
        {
            QRadialGradient * rg = new QRadialGradient();
            // TODO : find out whether Oasis works with boundingBox only?
            double cx = KoUnit::parseValue( e->attributeNS( KoXmlNS::draw, "cx", QString() ).remove("%") );
            double cy = KoUnit::parseValue( e->attributeNS( KoXmlNS::draw, "cy", QString() ).remove("%") );
            rg->setCenter( QPointF( size.width() * 0.01 * cx, size.height() * 0.01 * cy ) );
            rg->setFocalPoint( rg->center() );
            double dx = 0.5 * size.width();
            double dy = 0.5 * size.height();
            rg->setRadius( sqrt( dx*dx + dy*dy ) );
            gradient = rg;
        }
        else if( type == "linear" )
        {
            QLinearGradient * lg = new QLinearGradient();
            double angle = 90 + e->attributeNS( KoXmlNS::draw, "angle", "0" ).toDouble();
            double radius = 0.5 * sqrt( size.width()*size.width() + size.height()*size.height() );
            double sx = cos( angle * M_PI / 180 ) * radius;
            double sy = sin( angle * M_PI / 180 ) * radius;
            lg->setStart( QPointF( 0.5 * size.width() + sx, 0.5 * size.height() + sy ) );
            lg->setFinalStop( QPointF( 0.5 * size.width() - sx, 0.5 * size.height() - sy ) );
            gradient = lg;
        }
        else
            return QBrush();

        QGradientStop start;
        start.first = 0.0;
        start.second = QColor( e->attributeNS( KoXmlNS::draw, "start-color", QString() ) );
        start.second.setAlphaF( 0.01 * e->attributeNS( KoXmlNS::draw, "start-intensity", "100" ).remove("%").toDouble() );

        QGradientStop end;
        end.first = 1.0;
        end.second = QColor( e->attributeNS( KoXmlNS::draw, "end-color", QString() ) );
        end.second.setAlphaF( 0.01 * e->attributeNS( KoXmlNS::draw, "end-intensity", "100" ).remove("%").toDouble() );

        QGradientStops stops;
        gradient->setStops( stops << start << end );
    }
    else if( e->namespaceURI() == KoXmlNS::svg )
    {
        if( e->localName() == "linearGradient" )
        {
            QLinearGradient * lg = new QLinearGradient();
            QPointF start, stop;
            start.setX( KoUnit::parseValue( e->attributeNS( KoXmlNS::svg, "x1", QString() ) ) );
            start.setY( KoUnit::parseValue( e->attributeNS( KoXmlNS::svg, "y1", QString() ) ) );
            stop.setX( KoUnit::parseValue( e->attributeNS( KoXmlNS::svg, "x2", QString() ) ) );
            stop.setY( KoUnit::parseValue( e->attributeNS( KoXmlNS::svg, "y2", QString() ) ) );
            lg->setStart( start );
            lg->setFinalStop( stop );
            gradient = lg;
        }
        else if( e->localName() == "radialGradient" )
        {
            QRadialGradient * rg = new QRadialGradient();
            QPointF center, focalPoint;
            center.setX( KoUnit::parseValue( e->attributeNS( KoXmlNS::svg, "cx", QString() ) ) );
            center.setY( KoUnit::parseValue( e->attributeNS( KoXmlNS::svg, "cy", QString() ) ) );
            double r = KoUnit::parseValue( e->attributeNS( KoXmlNS::svg, "r", QString() ) );
            focalPoint.setX( KoUnit::parseValue( e->attributeNS( KoXmlNS::svg, "fx", QString() ) ) );
            focalPoint.setY( KoUnit::parseValue( e->attributeNS( KoXmlNS::svg, "fy", QString() ) ) );
            rg->setCenter( center );
            rg->setFocalPoint( focalPoint );
            rg->setRadius( r );
            gradient = rg;
        }

        QString strSpread( e->attributeNS( KoXmlNS::svg, "spreadMethod", "pad" ) );
        if( strSpread == "repeat" )
            gradient->setSpread( QGradient::RepeatSpread );
        else if( strSpread == "reflect" )
            gradient->setSpread( QGradient::ReflectSpread );
        else
            gradient->setSpread( QGradient::PadSpread );

        if( e->hasAttributeNS( KoXmlNS::svg, "gradientTransform" ) )
            matrix = KoOasisStyles::loadTransformation( e->attributeNS( KoXmlNS::svg, "gradientTransform", QString() ) );

        QGradientStops stops;

        // load stops
        KoXmlElement colorstop;
        forEachElement(colorstop, (*e))
        {
            if( colorstop.namespaceURI() == KoXmlNS::svg && colorstop.localName() == "stop" )
            {
                QGradientStop stop;
                stop.second = QColor( colorstop.attributeNS( KoXmlNS::svg, "stop-color", QString() ) );
                stop.second.setAlphaF( colorstop.attributeNS( KoXmlNS::svg, "stop-opacity", "1.0" ).toDouble() );
                stop.first = colorstop.attributeNS( KoXmlNS::svg, "offset", "0.0" ).toDouble();
                stops.append( stop );
            }
        }
        // TODO should the stops be sorted?
        gradient->setStops( stops );
    }

    if( ! gradient )
        return QBrush();

    QBrush resultBrush( *gradient );
    resultBrush.setMatrix( matrix );

    delete gradient;
    return resultBrush;
}

QBrush KoOdfGraphicStyles::loadOasisPatternStyle( const KoStyleStack &styleStack, KoOasisLoadingContext & context, const QSizeF &size )
{
    QString styleName = styleStack.property( KoXmlNS::draw, "fill-image-name" );

    KoXmlElement* e = context.oasisStyles().drawStyles()[styleName];
    if( ! e )
        return QBrush();

    const QString href = e->attributeNS( KoXmlNS::xlink, "href", QString() );

    if( href.isEmpty() )
        return QBrush();

    QString strExtension;
    const int result=href.lastIndexOf(".");
    if (result>=0)
    {
        strExtension=href.mid(result+1); // As we are using KoPicture, the extension should be without the dot.
    }
    QString filename(href);

    KoPictureShared picture;
    KoStore* store = context.store();
    if ( store->open( filename ) )
    {
        KoStoreDevice dev(store);
        if ( ! picture.load( &dev, strExtension ) )
            kWarning() << "Cannot load picture: " << filename << " " << href;
        store->close();
    }

    // read the pattern repeat style
    QString style = styleStack.property( KoXmlNS::style, "repeat" );
    kDebug() <<"pattern style =" << style;

    QSize imageSize = picture.getOriginalSize();

    if( style == "stretch" )
    {
        imageSize = size.toSize();
    }
    else
    {
        // optional attributes which can override original image size
        if( styleStack.hasProperty( KoXmlNS::draw, "fill-image-height" ) && styleStack.hasProperty( KoXmlNS::draw, "fill-image-width" ) )
        {
            QString height = styleStack.property( KoXmlNS::draw, "fill-image-height" );
            double newHeight = 0.0;
            if( height.endsWith( '%' ) )
                newHeight = 0.01 * height.remove( "%" ).toDouble() * imageSize.height();
            else
                newHeight = KoUnit::parseValue( height );
            QString width = styleStack.property( KoXmlNS::draw, "fill-image-width" );
            double newWidth = 0.0;
            if( width.endsWith( '%' ) )
                newWidth = 0.01 * width.remove( "%" ).toDouble() * imageSize.width();
            else
                newWidth = KoUnit::parseValue( width );
            if( newHeight > 0.0 )
                imageSize.setHeight( static_cast<int>( newHeight ) );
            if( newWidth > 0.0 )
                imageSize.setWidth( static_cast<int>( newWidth ) );
        }
    }

    kDebug() <<"shape size =" << size;
    kDebug() <<"original image size =" << picture.getOriginalSize();
    kDebug() <<"resulting image size =" << imageSize;

    QBrush resultBrush( picture.generatePixmap( imageSize, true ) );

    if( style == "repeat" )
    {
        QMatrix matrix;
        if( styleStack.hasProperty( KoXmlNS::draw, "fill-image-ref-point" ) )
        {
            // align pattern to the given size
            QString align = styleStack.property( KoXmlNS::draw, "fill-image-ref-point" );
            kDebug() <<"pattern align =" << align;
            if( align == "top-left" )
                matrix.translate( 0, 0 );
            else if( align == "top" )
                matrix.translate( 0.5*size.width(), 0 );
            else if( align == "top-right" )
                matrix.translate( size.width(), 0 );
            else if( align == "left" )
                matrix.translate( 0, 0.5*size.height() );
            else if( align == "center" )
                matrix.translate( 0.5*size.width(), 0.5*size.height() );
            else if( align == "right" )
                matrix.translate( size.width(), 0.5*size.height() );
            else if( align == "bottom-left" )
                matrix.translate( 0, size.height() );
            else if( align == "bottom" )
                matrix.translate( 0.5*size.width(), size.height() );
            else if( align == "bottom-right" )
                matrix.translate( size.width(), size.height() );
        }
        if( styleStack.hasProperty( KoXmlNS::draw, "fill-image-ref-point-x" ) )
        {
            QString pointX = styleStack.property( KoXmlNS::draw, "fill-image-ref-point-x" );
            matrix.translate( 0.01 * pointX.remove( '%' ).toDouble() * imageSize.width(), 0 );
        }
        if( styleStack.hasProperty( KoXmlNS::draw, "fill-image-ref-point-y" ) )
        {
            QString pointY = styleStack.property( KoXmlNS::draw, "fill-image-ref-point-y" );
            matrix.translate( 0, 0.01 * pointY.remove( '%' ).toDouble() * imageSize.height() );
        }
        resultBrush.setMatrix( matrix );
    }

    return resultBrush;
}

QBrush KoOdfGraphicStyles::loadOasisFillStyle( const KoStyleStack &styleStack, const QString & fill,  const KoOasisStyles & oasisStyles )
{
    QBrush tmpBrush;
    if ( fill == "solid" )
    {
        tmpBrush.setStyle(static_cast<Qt::BrushStyle>( 1 ) );
        if ( styleStack.hasProperty( KoXmlNS::draw, "fill-color" ) )
            tmpBrush.setColor(styleStack.property( KoXmlNS::draw, "fill-color" ) );
        if( styleStack.hasProperty( KoXmlNS::draw, "opacity" ) )
        {
            QString opacity = styleStack.property( KoXmlNS::draw, "opacity" );
            if( ! opacity.isEmpty() && opacity.right( 1 ) == "%" )
            {
                float percent = opacity.left( opacity.length()-1 ).toFloat();
                QColor color = tmpBrush.color();
                color.setAlphaF( percent / 100.0 );
                tmpBrush.setColor( color );
            }
        }
        //TODO
        if ( styleStack.hasProperty( KoXmlNS::draw, "transparency" ) )
        {
            QString transparency = styleStack.property( KoXmlNS::draw, "transparency" );
            if ( transparency == "94%" )
            {
                tmpBrush.setStyle(Qt::Dense1Pattern);
            }
            else if ( transparency == "88%" )
            {
                tmpBrush.setStyle(Qt::Dense2Pattern);
            }
            else if ( transparency == "63%" )
            {
                tmpBrush.setStyle(Qt::Dense3Pattern);

            }
            else if ( transparency == "50%" )
            {
                tmpBrush.setStyle(Qt::Dense4Pattern);

            }
            else if ( transparency == "37%" )
            {
                tmpBrush.setStyle(Qt::Dense5Pattern);

            }
            else if ( transparency == "12%" )
            {
                tmpBrush.setStyle(Qt::Dense6Pattern);

            }
            else if ( transparency == "6%" )
            {
                tmpBrush.setStyle(Qt::Dense7Pattern);

            }
            else
                kDebug(30003)<<" transparency is not defined into kpresenter :"<<transparency;
        }
    }
    else if ( fill == "hatch" )
    {
        QString style = styleStack.property( KoXmlNS::draw, "fill-hatch-name" );
        kDebug(30003)<<" hatch style is  :"<<style;

        //type not defined by default
        //try to use style.
        KoXmlElement* draw = oasisStyles.drawStyles()[style];
        if ( draw)
        {
            kDebug(30003)<<"We have a style";
            int angle = 0;
            if( draw->hasAttributeNS( KoXmlNS::draw, "rotation" ))
            {
                angle = (draw->attributeNS( KoXmlNS::draw, "rotation", QString() ).toInt())/10;
                kDebug(30003)<<"angle :"<<angle;
            }
            if(draw->hasAttributeNS( KoXmlNS::draw, "color" ) )
            {
                //kDebug(30003)<<" draw:color :"<<draw->attributeNS( KoXmlNS::draw,"color", QString() );
                tmpBrush.setColor(draw->attributeNS( KoXmlNS::draw, "color", QString() ) );
            }
            if( draw->hasAttributeNS( KoXmlNS::draw, "distance" ))
            {
                //todo implemente it into kpresenter
            }
            if( draw->hasAttributeNS( KoXmlNS::draw, "display-name"))
            {
                //todo implement it into kpresenter
            }
            if( draw->hasAttributeNS( KoXmlNS::draw, "style" ))
            {
                //todo implemente it into kpresenter
                QString styleHash = draw->attributeNS( KoXmlNS::draw, "style", QString() );
                if( styleHash == "single")
                {
                    switch( angle )
                    {
                    case 0:
                    case 180:
                        tmpBrush.setStyle(Qt::HorPattern );
                        break;
                    case 45:
                    case 225:
                        tmpBrush.setStyle(Qt::BDiagPattern );
                        break;
                    case 90:
                    case 270:
                        tmpBrush.setStyle(Qt::VerPattern );
                        break;
                    case 135:
                    case 315:
                        tmpBrush.setStyle(Qt::FDiagPattern );
                        break;
                    default:
                        //todo fixme when we will have a kopaint
                        kDebug(30003)<<" draw:rotation 'angle' :"<<angle;
                        break;
                    }
                }
                else if( styleHash == "double")
                {
                    switch( angle )
                    {
                    case 0:
                    case 180:
                    case 90:
                    case 270:
                        tmpBrush.setStyle(Qt::CrossPattern );
                        break;
                    case 45:
                    case 135:
                    case 225:
                    case 315:
                        tmpBrush.setStyle(Qt::DiagCrossPattern );
                        break;
                    default:
                        //todo fixme when we will have a kopaint
                        kDebug(30003)<<" draw:rotation 'angle' :"<<angle;
                        break;
                    }

                }
                else if( styleHash == "triple")
                {
                    kDebug(30003)<<" it is not implemented :(";
                }
            }
        }
    }

    return tmpBrush;
}

QPen KoOdfGraphicStyles::loadOasisStrokeStyle( const KoStyleStack &styleStack, const QString & stroke, const KoOasisStyles & oasisStyles )
{
    QPen tmpPen;

    if( stroke == "solid" || stroke == "dash" )
    {
        if ( styleStack.hasProperty( KoXmlNS::svg, "stroke-color" ) )
            tmpPen.setColor( styleStack.property( KoXmlNS::svg, "stroke-color" ) );
        if ( styleStack.hasProperty( KoXmlNS::svg, "stroke-opacity" ) )
        {
            QColor color = tmpPen.color();
            QString opacity = styleStack.property( KoXmlNS::svg, "stroke-opacity" );
            color.setAlphaF( opacity.toDouble() );
            tmpPen.setColor( color );
        }
        if( styleStack.hasProperty( KoXmlNS::svg, "stroke-width" ) )
            tmpPen.setWidthF( KoUnit::parseValue( styleStack.property( KoXmlNS::svg, "stroke-width" ) ) );
        if( styleStack.hasProperty( KoXmlNS::draw, "stroke-linejoin" ) )
        {
            QString join = styleStack.property( KoXmlNS::draw, "stroke-linejoin" );
            if( join == "bevel" )
                tmpPen.setJoinStyle( Qt::BevelJoin );
            else if( join == "round" )
                tmpPen.setJoinStyle( Qt::RoundJoin );
            else
                tmpPen.setJoinStyle( Qt::MiterJoin );
        }

        if( stroke == "dash" && styleStack.hasProperty( KoXmlNS::draw, "stroke-dash" ) )
        {
            QString dashStyleName = styleStack.property( KoXmlNS::draw, "stroke-dash" );

            KoXmlElement * dashElement = oasisStyles.drawStyles()[ dashStyleName ];
            if( dashElement )
            {
                QVector<qreal> dashes;
                if( dashElement->hasAttributeNS( KoXmlNS::draw, "dots1" ) )
                {
                    double dotLength = KoUnit::parseValue( dashElement->attributeNS( KoXmlNS::draw, "dots1-length", QString() ) );
                    dashes.append( dotLength / tmpPen.width() );
                    double dotDistance = KoUnit::parseValue( dashElement->attributeNS( KoXmlNS::draw, "distance", QString() ) );
                    dashes.append( dotDistance / tmpPen.width() );
                    if( dashElement->hasAttributeNS( KoXmlNS::draw, "dots2" ) )
                    {
                        dotLength = KoUnit::parseValue( dashElement->attributeNS( KoXmlNS::draw, "dots2-length", QString() ) );
                        dashes.append( dotLength / tmpPen.width() );
                        dashes.append( dotDistance / tmpPen.width() );
                    }
                    tmpPen.setDashPattern( dashes );
                }
            }
        }
    }

    return tmpPen;
}
