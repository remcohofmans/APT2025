#include "OverlayView.h"
#include <QPainter>
#include <QDebug>

OverlayView::OverlayView(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)  // Inherit from QGraphicsPixmapItem
{
}


void OverlayView::setOverlayImage(const QString &imagePath) {
    if (!overlayPixmap.load(imagePath)) {
        qWarning() << "Failed to load image:" << imagePath;
    } else {
        setPixmap(overlayPixmap);  // Set the pixmap directly to the item
        setScale(1.0); // Ensure it is not scaled at the start
    }
}


void OverlayView::setScaledPixmap(const QPixmap &pixmap) {
    overlayPixmap = pixmap;
    setPixmap(overlayPixmap);
}


void OverlayView::setFixedSize(const QSize &size) {
    if (!overlayPixmap.isNull()) {
        // Scale the pixmap to the desired size
        QPixmap scaledPixmap = overlayPixmap.scaled(
            size,               // Target size
            Qt::KeepAspectRatio,  // Maintain aspect ratio
            Qt::SmoothTransformation // Use smooth scaling for better visuals
            );
        setPixmap(scaledPixmap);  // Set the scaled pixmap to the item
    }
    // Update bounding rect if needed
    prepareGeometryChange();
}

QRectF OverlayView::boundingRect() const {
    // Return the bounding rectangle of the pixmap
    return pixmap().rect();
}

