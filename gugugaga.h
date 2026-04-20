#ifndef GUGUGAGA_H
#define GUGUGAGA_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class gugugaga;
}
QT_END_NAMESPACE

class gugugaga : public QMainWindow
{
    Q_OBJECT

public:
    explicit gugugaga(QWidget *parent = nullptr);
    ~gugugaga() override;

private:
    Ui::gugugaga *ui;
};
#endif // GUGUGAGA_H
