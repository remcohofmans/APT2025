#ifndef OVERLAYVIEW_H
#define OVERLAYVIEW_H

#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QString>

class OverlayView : public QGraphicsPixmapItem
{
public:
    // Constructor
    explicit OverlayView(QGraphicsItem *parent = nullptr);

    // Method to set the overlay image
    void setOverlayImage(const QString &imagePath);

    void setScaledPixmap(const QPixmap &pixmap);

    void setFixedSize(const QSize &size);
    QRectF boundingRect() const ;


private:
    QPixmap overlayPixmap;  // The image to be drawn as the overlay
};

#endif // OVERLAYVIEW_H
