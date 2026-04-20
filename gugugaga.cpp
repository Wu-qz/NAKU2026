#include "gugugaga.h"
#include "ui_gugugaga.h"

gugugaga::gugugaga(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::gugugaga)
{
    ui->setupUi(this);
}

gugugaga::~gugugaga()
{
    delete ui;
}
