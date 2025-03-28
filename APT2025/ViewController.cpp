#include "ViewController.h"
#include <qdebug.h>

ViewController::ViewController(QWidget *parent)
    : QWidget(parent)
{
    //qDebug() << "ViewController Ctor called";
}

void ViewController::render()
{
    currentView->visualizeWorld();
}

std::shared_ptr<View> ViewController::getCurrentView() const
{
    return currentView;
}

void ViewController::setView(View *cView)
{
    // Safely convert the raw pointer to a shared_ptr
    this->currentView = std::shared_ptr<View>(cView, [](View*) {});

    // If you want to debug which type of view is being set
    if (dynamic_cast<GraphicalView*>(cView)) {
        qDebug() << "Graphical View Set";
    } else if (dynamic_cast<TextBasedView*>(cView)) {
        qDebug() << "Text-Based View Set";
    }

    // Render the new view
    render();
}

void ViewController::ProtagonistMoved(int x, int y) {
    currentView->visualizeWorld();
}
