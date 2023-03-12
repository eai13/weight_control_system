#ifndef SMARTLINE_H
#define SMARTLINE_H

#include <QLineEdit>
#include <QObject>
#include <QWidget>
#include <QKeyEvent>

class SmartLine : public QLineEdit
{
    Q_OBJECT
public:
    explicit SmartLine(QWidget * parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent * event) override;
    void keyReleaseEvent(QKeyEvent * event) override;

signals:
    void siKeyUpPressed(void);
    void siKeyUpReleased(void);
    void siKeyDownPressed(void);
    void siKeyDownReleased(void);
};

#endif // SMARTLINE_H
