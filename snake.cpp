#include "snake.h"

Snake::Snake(QObject *parent) : QObject(parent)
  , direction(Left)
  , stretchMargin(0)
{
    //初始化蛇的位置(网格序号对)
    nodeX.append(15);
    nodeX.append(16);
    nodeY.append(15);
    nodeY.append(15);
}

QList<int> Snake::getHeadPos()
{
    //返回头的坐标
    QList<int> pos;
    pos.append(nodeX[0]);
    pos.append(nodeY[0]);
    return pos;
}

QList<int> Snake::getSecondPos()
{
    //返回第二个位置的坐标
    QList<int> pos;
    pos.append(nodeX[1]);
    pos.append(nodeY[1]);
    return pos;
}

QList<int> Snake::getNextHead(Direction dir)
{
    int new_X, new_Y;
    //根据dir计算下一步头的坐标
    switch(dir)
    {
        case Up:
            new_X = nodeX[0];
            new_Y = nodeY[0] - 1;
            break;
        case Down:
            new_X = nodeX[0];
            new_Y = nodeY[0] + 1;
            break;
        case Left:
            new_X = nodeX[0] - 1;
            new_Y = nodeY[0];
            break;
        case Right:
            new_X = nodeX[0] + 1;
            new_Y = nodeY[0];
            break;
    }
    //返回下一步头的坐标
    QList<int> pos;
    pos.append(new_X);
    pos.append(new_Y);
    return pos;
}

QList<int> Snake::getNodeX()
{
    return nodeX;
}

QList<int> Snake::getNodeY()
{
    return nodeY;
}

void Snake::setNodeX(QList<int> nodeX_)
{
    nodeX = nodeX_;
}

void Snake::setNodeY(QList<int> nodeY_)
{
    nodeY = nodeY_;
}

int Snake::getStretchMargin()
{
    return stretchMargin;
}

void Snake::setStretchMargin(int margin)
{
    stretchMargin = margin;
}

Snake::Direction Snake::getDirection()
{
    return Direction(direction);
}

void Snake::move()
{
    //根据direction计算下一步的头坐标
    int new_X, new_Y;
    switch(direction)
    {
        case Up:
            new_X = nodeX[0];
            new_Y = nodeY[0] - 1;
            break;
        case Down:
            new_X = nodeX[0];
            new_Y = nodeY[0] + 1;
            break;
        case Left:
            new_X = nodeX[0] - 1;
            new_Y = nodeY[0];
            break;
        case Right:
            new_X = nodeX[0] + 1;
            new_Y = nodeY[0];
            break;
    }

    //根据拉伸情况进行更新位置
    if(stretchMargin > 0)
    {
        //如果还有拉伸余量
        //则蛇直接向前伸长
        nodeX.prepend(new_X);
        nodeY.prepend(new_Y);
        stretchMargin --;
    }
    else
    {
        //如果拉伸余量为0
        //则蛇直接向前伸长,同时去掉尾部最后一个位置
        //其实下面两行可以单独抽出来,但为了思路完整性还是保留在if中
        nodeX.prepend(new_X);
        nodeY.prepend(new_Y);
        nodeX.pop_back();
        nodeY.pop_back();
    }
}

void Snake::setDirection(Snake::Direction dir)
{
    direction = dir;
}

void Snake::setStretch()
{
    //吃掉果实后会获得3个格子的拉伸余量
    stretchMargin = 3;
}
