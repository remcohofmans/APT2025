#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include <QWidget>
#include <memory>
#include "GraphicalView.h"
#include "TextBasedView.h"

class ViewController : public QWidget
{
    Q_OBJECT

public:
    explicit ViewController(QWidget *parent = nullptr);
    void render();
    std::shared_ptr<View> getCurrentView() const;
    void setView(View *cView);

public slots:
    void ProtagonistMoved(int x, int y);

private:
    std::shared_ptr<GraphicalView> graphicalView;
    std::shared_ptr<TextBasedView> textBasedView;
    std::shared_ptr<View> currentView;
};

#endif // VIEWCONTROLLER_H
