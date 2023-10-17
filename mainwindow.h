#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QTimer>
#include <QTime>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QDir>
#include <QLabel>
#include <snake.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    //MainWindow类的一些成员变量
    Ui::MainWindow *ui;
    QLabel *lab; //用于游戏状态提示的标签
    int blocksX[41][41], blocksY[41][41]; //40*40个格子的中心坐标
    int blocksLen; //每个格子的长度

    //记录游戏当前的状态
    //定义枚举类型
    enum Status
    {
        Ready, //游戏未开始
        Gaming, //游戏中
        Stop, //游戏暂停
        End, //游戏结束
    };
    int status; //游戏当前的状态

    //定义定时器
    //定时更新画面与系统
    QTimer *updateScreenTimer, *updateSystemTimer;
    //定时更新画面与系统的周期(毫秒)
    int updateScreenInterval, updateSystemInterval;

    //定义场景的元素
    Snake *snake; //贪吃蛇对象
    QList<int> barrierPosX; //障碍的X坐标
    QList<int> barrierPosY; //障碍的Y坐标
    int fruitPosX, fruitPosY; //果实的XY坐标

    //定义当前流逝的时间与获得的分数
    int time, score;

private:
    //重写一些事件处理函数
    void keyPressEvent(QKeyEvent *e); //对键盘事件进行处理
    void paintEvent(QPaintEvent *e); //进行绘图
    void mousePressEvent(QMouseEvent *e); //对鼠标事件进行处理
    void resizeEvent(QResizeEvent *e); //当窗口大小调整时是能自动调整游戏界面水平居中

private:
    //MainWindow类的一些成员函数
    void initUI(); //初始化界面组件的函数
    void initBlocks(int startX=35, int startY=115); //初始化网格的函数
    int getRandNum(); //产生一个[1,39]的随机数
    bool saveBinaryFile(QString &aFileName); //保存为二进制文件
    bool loadBinaryFile(QString &aFileName); //读取二进制文件

private slots:
    //MainWindow类的一些槽函数
    void on_screentimer_timeout () ; //画面定时溢出处理槽函数
    void on_systemtimer_timeout () ; //系统定时溢出处理槽函数
    //各个Action对应的槽函数
    void on_actionStart_triggered();
    void on_actionStop_triggered();
    void on_actionContinue_triggered();
    void on_actionReset_triggered();
    void on_actionSave_triggered();
    void on_actionLoad_triggered();
    void on_actionSetSpeed_triggered();
};
#endif // MAINWINDOW_H
