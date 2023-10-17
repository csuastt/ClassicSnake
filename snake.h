#ifndef SNAKE_H
#define SNAKE_H

#include <QObject>

//Snake对象代表玩家操控的贪吃蛇
class Snake : public QObject
{
    Q_OBJECT
public:
    explicit Snake(QObject *parent = nullptr);

private:
    //一些成员变量
    //蛇的各个节点所在的网格位置
    QList<int> nodeX, nodeY;
    int direction; //当前移动方向
    int stretchMargin; //拉伸余量,即蛇还需要伸长几个格子

public:
    //定义枚举类型:方向
    enum Direction
    {
        Up,
        Down,
        Left,
        Right
    };
    QList<int> getHeadPos(); //返回头坐标(X,Y)
    QList<int> getSecondPos(); //返回身子第二个位置的坐标(X,Y)
    QList<int> getNextHead(Direction dir); //返回朝dir方向移动一步后的头坐标(X,Y)
    QList<int> getNodeX(); //获得nodeX
    QList<int> getNodeY(); //获得nodeY
    void setNodeX(QList<int> nodeX_); //设置nodeX
    void setNodeY(QList<int> nodeY_); //设置nodeY
    int getStretchMargin(); //获取拉伸余量
    void setStretchMargin(int margin); //设置拉伸余量
    Direction getDirection(); //获得当前移动方向
    void move(); //蛇向前移动
    void setDirection(Direction dir); //设置蛇的移动方向
    void setStretch(); //设置蛇为拉伸状态(即吃完果实的状态)
};

#endif // SNAKE_H
