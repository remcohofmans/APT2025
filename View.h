#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>
#include <QWidget>
#include "GameModel.h"

class View : public QGraphicsView {
    Q_OBJECT

public:
    explicit View(QWidget *parent = nullptr)
        : QGraphicsView(parent),
        model(GameModel::getInstance()),
        scene(new QGraphicsScene(this))
    {
        setScene(scene);
    }

    virtual ~View() {}

    virtual void visualizeWorld() = 0;

protected:
    GameModel &model;
    QGraphicsScene *scene;
};

#endif // VIEW_H
