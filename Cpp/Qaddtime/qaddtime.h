#ifndef QADDTIME_H
#define QADDTIME_H

#include <QDialog>
#include <QtMath>
#include <QString>

namespace Ui {
class Qaddtime;
}

class Qaddtime : public QDialog
{
    Q_OBJECT

public:
    explicit Qaddtime(QWidget *parent = 0);
    ~Qaddtime();

private slots:
    void on_runButton_clicked();

private:
    Ui::Qaddtime *ui;
};

#endif // QADDTIME_H
