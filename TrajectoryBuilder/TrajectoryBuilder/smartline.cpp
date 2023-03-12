#include "smartline.h"

SmartLine::SmartLine(QWidget * parent)
    : QLineEdit(parent)
{

}

void SmartLine::keyPressEvent(QKeyEvent * event){
    switch(event->key()){
    case(Qt::Key_Up):{
        emit this->siKeyUpPressed();
        break;
    }
    case(Qt::Key_Down):{
        emit this->siKeyDownPressed();
        break;
    }
    default:{
        QLineEdit::keyPressEvent(event);
        break;
    }
    }
}

void SmartLine::keyReleaseEvent(QKeyEvent * event){
    switch(event->key()){
    case(Qt::Key_Up):{
        emit this->siKeyUpReleased();
        break;
    }
    case(Qt::Key_Down):{
        emit this->siKeyDownReleased();
        break;
    }
    default:{
        QLineEdit::keyReleaseEvent(event);
        break;
    }
    }
}
