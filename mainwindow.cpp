#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , lab(new QLabel(this))
    , blocksX{}
    , blocksY{}
    , blocksLen(20)
    , status(Ready)
    , updateScreenInterval(120) //画面更新的默认周期，可以根据设置速度选项进行修改周期
    , updateSystemInterval(100)
    , snake(new Snake(this))
    , fruitPosX(-1)
    , fruitPosY(-1)
    , time(0)
    , score(0)
{
    ui->setupUi(this);
    //初始化主界面
    initUI();
    //初始化网格
    initBlocks();

    //初始化定时器
    //设置画面更新定时器
    updateScreenTimer = new QTimer(this);
    updateScreenTimer->stop();
    updateScreenTimer->setInterval(updateScreenInterval) ;//设置定时周期
    connect(updateScreenTimer,SIGNAL(timeout()),this,SLOT(on_screentimer_timeout()));
    //设置系统更新定时器
    updateSystemTimer = new QTimer(this);
    updateSystemTimer->stop();
    updateSystemTimer->setInterval(updateSystemInterval) ;//设置定时周期
    connect(updateSystemTimer,SIGNAL(timeout()),this,SLOT(on_systemtimer_timeout()));
    updateSystemTimer->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if(status == Gaming)
    {
        QList<int> posNext, posSec;
        switch(e->key())
        {
            case Qt::Key_Up:
                //贪吃蛇不允许掉头，下面的判断类似
                posNext = snake->getNextHead(Snake::Up);
                posSec = snake->getSecondPos();
                if(posNext[0] == posSec[0] && posNext[1] == posSec[1])
                    return;
                snake->setDirection(Snake::Up);
                break;
            case Qt::Key_Down:
                posNext = snake->getNextHead(Snake::Down);
                posSec = snake->getSecondPos();
                if(posNext[0] == posSec[0] && posNext[1] == posSec[1])
                    return;
                snake->setDirection(Snake::Down);
                break;
            case Qt::Key_Left:
                posNext = snake->getNextHead(Snake::Left);
                posSec = snake->getSecondPos();
                if(posNext[0] == posSec[0] && posNext[1] == posSec[1])
                    return;
                snake->setDirection(Snake::Left);
                break;
            case Qt::Key_Right:
                posNext = snake->getNextHead(Snake::Right);
                posSec = snake->getSecondPos();
                if(posNext[0] == posSec[0] && posNext[1] == posSec[1])
                    return;
                snake->setDirection(Snake::Right);
                break;
        }
    }
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    //设置画笔
    QPainter p(this);
    p.setPen(Qt::black);
    p.setBrush(Qt::black);
    int x,y;

    //绘制障碍
    for(int i = 0;i < barrierPosX.length();i++)
    {
        int x = barrierPosX[i];
        int y = barrierPosY[i];
        p.drawRect(blocksX[x][y] - blocksLen/2,
                   blocksY[x][y] - blocksLen/2,
                   blocksLen,
                   blocksLen);
    }

    //绘制果实
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::red);
    if(fruitPosX != -1)
    {
        int x = fruitPosX;
        int y = fruitPosY;
        p.drawRect(blocksX[x][y] - blocksLen/2 + 5,
                   blocksY[x][y] - blocksLen/2 + 5,
                   blocksLen - 10,
                   blocksLen - 10);
    }

    //绘制蛇
    p.setPen(QPen(Qt::black,2));
    //绘制蛇头
    p.setBrush(Qt::yellow);
    x = snake->getHeadPos()[0];
    y = snake->getHeadPos()[1];
    p.drawRect(blocksX[x][y] - blocksLen/2 + 2,
               blocksY[x][y] - blocksLen/2 + 2,
               blocksLen - 4,
               blocksLen - 4);
    //绘制蛇身
    p.setBrush(Qt::gray);
    QList<int> posX = snake->getNodeX();
    QList<int> posY = snake->getNodeY();
    for(int i = 1;i < posX.length();i++)
    {
        int x = posX[i];
        int y = posY[i];
        p.drawRect(blocksX[x][y] - blocksLen/2 + 2,
                   blocksY[x][y] - blocksLen/2 + 2,
                   blocksLen - 4,
                   blocksLen - 4);
    }

    //绘制网格的外边界
    p.setBrush(Qt::NoBrush);
    p.drawRect(blocksX[1][1] - blocksLen/2,
               blocksY[1][1] - blocksLen/2,
               blocksLen*40,
               blocksLen*40);
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    //只有在未开始状态才能设置障碍
    if(status == Ready)
    {
        //获取鼠标当前的坐标
        float x=e->pos().x(),y=e->pos().y();
        if(e->button()==Qt::LeftButton)
        {
            int index_X = -1;
            int index_Y = -1;
            //找出鼠标当前点击位置对应的网格序号
            for(int i = 1;i <= 40;i++)
                for(int j = 1;j <= 40;j++)
                {
                    if(x <= blocksX[i][j] + blocksLen/2 &&
                       x >= blocksX[i][j] - blocksLen/2 &&
                       y <= blocksY[i][j] + blocksLen/2 &&
                       y >= blocksY[i][j] - blocksLen/2)
                    {
                        index_X = i;
                        index_Y = j;
                    }
                }
            //如果点击在了界外那么就返回
            if(index_X == -1)
                return;
            //寻找这个位置有没有障碍
            bool empty = true;
            for(int i = 0;i < barrierPosX.length();i++)
            {
                int x = barrierPosX[i];
                int y = barrierPosY[i];
                if(x == index_X && y == index_Y)
                {
                    //如果这个位置已经有障碍了就把障碍删除
                    empty = false;
                    barrierPosX.removeAt(i);
                    barrierPosY.removeAt(i);
                    break;
                }
            }
            //如果没有障碍就在这个位置设置障碍
            if(empty)
            {
                barrierPosX.append(index_X);
                barrierPosY.append(index_Y);
            }
            //更新画面
            update();
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    //找到初始位置,当窗口变化时总是使网格水平居中
    int startX = (e->size().width() - 800)/2 + 10; //加10是为了处理外边框的宽度
    int startY = 115;
    //更新各个格子的坐标
    initBlocks(startX, startY);
    //更新画面
    update();
}

void MainWindow::initUI()
{
    //为toolButton设置对应的Action
    ui->toolButton->setDefaultAction(ui->actionStart);
    ui->toolButton_2->setDefaultAction(ui->actionStop);
    ui->toolButton_3->setDefaultAction(ui->actionContinue);
    ui->toolButton_4->setDefaultAction(ui->actionReset);
    ui->toolButton_5->setDefaultAction(ui->actionQuit);
    ui->toolButton_6->setDefaultAction(ui->actionSave);
    ui->toolButton_7->setDefaultAction(ui->actionLoad);
    ui->toolButton_8->setDefaultAction(ui->actionSetSpeed);

    //为主界面设置强焦点模式，为了避免捕获不到按键信号
    ui->centralwidget->setFocusPolicy(Qt::StrongFocus);

    //初始化状态栏
    lab->setText("当前处于未开始状态,按Enter键开始游戏");
    ui->statusbar->addWidget(lab);
}

void MainWindow::initBlocks(int startX, int startY)
{
    //为每个网格设置X坐标和Y坐标
    //startX为第一个格子左上角的X坐标
    //startY为第一个格子左上角的Y坐标
    for(int i = 1;i <= 40;i++)
        for(int j = 1;j <= 40;j++)
        {
            blocksX[i][j] = startX + blocksLen * (i - 1);
            blocksY[i][j] = startY + blocksLen * (j - 1);
        }
}

int MainWindow::getRandNum()
{
    QTime time;
    time= QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);
    int n = qrand() % 40;    //产生40以内的随机数
    //保证果实不会在边缘出现
    if(n == 0)
        n += 2;
    if(n == 1)
        n ++;
    return n;
}

bool MainWindow::saveBinaryFile(QString &aFileName)
{
    QFile aFile(aFileName);  //以文件方式读出
    if (!(aFile.open(QIODevice::WriteOnly)))
        return false;
    QDataStream out(&aFile);
    //当前时间
    out << time;
    //当前分数
    out << score;
    //果实位置
    out << fruitPosX << fruitPosY;
    //障碍位置
    out << barrierPosX << barrierPosY;
    //蛇的状态
    out << snake->getNodeX() << snake->getNodeY();
    out << snake->getDirection();
    out << snake->getStretchMargin();
    aFile.close();
    return true;
}

bool MainWindow::loadBinaryFile(QString &aFileName)
{
    QFile aFile(aFileName);  //以文件方式读入
    if (!(aFile.open(QIODevice::ReadOnly)))
        return false;
    QDataStream in(&aFile);
    //当前时间
    in >> time;
    //当前分数
    in >> score;
    //果实位置
    in >> fruitPosX >> fruitPosY;
    //障碍位置
    in >> barrierPosX >> barrierPosY;
    //蛇的状态
    QList<int> NodeX,NodeY;
    in >> NodeX >> NodeY;
    snake->setNodeX(NodeX);
    snake->setNodeY(NodeY);
    int dir;
    in >> dir;
    snake->setDirection(Snake::Direction(dir));
    int stretch;
    in >> stretch;
    snake->setStretchMargin(stretch);
    aFile.close();
    return true;
}

void MainWindow::on_screentimer_timeout()
{
    //获取蛇向前移动的坐标
    //如果这个坐标不合法就直接断定游戏失败
    //如果这个坐标合法那么就让蛇向前移动
    auto next_pos = snake->getNextHead(snake->getDirection());

    //是否出界
    if(next_pos[0] < 1 || next_pos[0] > 40 || next_pos[1] < 1 || next_pos[1] > 40 )
    {
        status = End;
        QString dlgTitle="游戏结束";
        QString strInfo="你出界了";
        QMessageBox::critical(this, dlgTitle, strInfo);
        updateScreenTimer->stop();
        return;
    }

    //是否碰到障碍
    bool fail = false;
    for(int i = 0;i < barrierPosX.length();i++)
    {
        int x = barrierPosX[i];
        int y = barrierPosY[i];
        if(next_pos[0] == x && next_pos[1] == y)
        {
            fail = true;
            break;
        }
    }
    if(fail)
    {
        status = End;
        QString dlgTitle="游戏结束";
        QString strInfo="你碰到障碍了";
        QMessageBox::critical(this, dlgTitle, strInfo);
        updateScreenTimer->stop();
        return;
    }

    //是否咬到自己
    bool bite = false;
    QList<int> self_X = snake->getNodeX();
    self_X.pop_back();
    QList<int> self_Y = snake->getNodeY();
    self_Y.pop_back();
    for(int i = 0;i < self_X.length();i++)
    {
        int x = self_X[i];
        int y = self_Y[i];
        if(next_pos[0] == x && next_pos[1] == y)
        {
            bite = true;
            break;
        }
    }
    if(bite)
    {
        status = End;
        QString dlgTitle="游戏结束";
        QString strInfo="你咬到了自己";
        QMessageBox::critical(this, dlgTitle, strInfo);
        updateScreenTimer->stop();
        return;
    }

    //到这一步说明蛇可以向前移动
    //更新蛇的位置
    snake->move();
    time++;
    ui->label->setText(QString::asprintf("当前时间：%d 当前分数：%d",time,score));
    auto now_pos = snake->getHeadPos();
    if(now_pos[0] == fruitPosX && now_pos[1] == fruitPosY)
    {
        //如果吃到了果实就让蛇长大
        snake->setStretch();
        score += 5;
        ui->label->setText(QString::asprintf("当前时间：%d 当前分数：%d",time,score));
        //被吃到的果实会自动消失
        fruitPosX = -1;
        fruitPosY = -1;
    }


    //更新果实坐标，一次只能存在一个果实
    //如果fruitPosX为-1说明还没有果实
    if(fruitPosX == -1)
    {
        //获取随机果实坐标(X,Y)
        //果实的坐标不能与蛇身子重合
        //果实的坐标不能与障碍重合
        //如果重合就再获取一次
        bool notgood = false;
        do
        {
            notgood = false;
            fruitPosX = getRandNum();
            fruitPosY = getRandNum();
            //是否与蛇身子重合
            QList<int> posX = snake->getNodeX();
            QList<int> posY = snake->getNodeY();
            for(int i = 0;i < posX.length();i++)
            {
                int x = posX[i];
                int y = posY[i];
                if(fruitPosX == x && fruitPosY == y)
                {
                    notgood = true;
                    goto end;
                }
            }
            //是否与障碍重合
            for(int i = 0;i < barrierPosX.length();i++)
            {
                int x = barrierPosX[i];
                int y = barrierPosY[i];
                if(fruitPosX == x && fruitPosY == y)
                {
                    notgood = true;
                    break;
                }
            }
            end:;
        }while(notgood);
    }
    update();
}

void MainWindow::on_systemtimer_timeout()
{
    //枚举各种状态，根据状态更新系统
    switch(status)
    {
        case Ready:
            //设置按钮状态
            ui->actionStop->setEnabled(0);
            ui->actionContinue->setEnabled(0);
            ui->actionReset->setEnabled(0);
            ui->actionSave->setEnabled(0);
            ui->actionStart->setEnabled(1);
            ui->actionQuit->setEnabled(1);
            ui->actionLoad->setEnabled(1);
            //设置状态栏信息
            lab->setText("当前处于未开始状态,按Enter键开始游戏");
            break;
        case Gaming:
            //设置按钮状态
            ui->actionStop->setEnabled(1);
            ui->actionContinue->setEnabled(0);
            ui->actionReset->setEnabled(0);
            ui->actionSave->setEnabled(0);
            ui->actionStart->setEnabled(0);
            ui->actionQuit->setEnabled(1);
            ui->actionLoad->setEnabled(0);
            //设置状态栏信息
            lab->setText("当前处于游戏状态,按P键暂停游戏");
            break;
        case Stop:
            //设置按钮状态
            ui->actionStop->setEnabled(0);
            ui->actionContinue->setEnabled(1);
            ui->actionReset->setEnabled(1);
            ui->actionSave->setEnabled(1);
            ui->actionStart->setEnabled(0);
            ui->actionQuit->setEnabled(1);
            ui->actionLoad->setEnabled(0);
            //设置状态栏信息
            lab->setText("当前处于暂停状态,按Space键继续游戏,按Backspace键重新开始");
            break;
        case End:
            //设置按钮状态
            ui->actionStop->setEnabled(0);
            ui->actionContinue->setEnabled(0);
            ui->actionReset->setEnabled(1);
            ui->actionSave->setEnabled(0);
            ui->actionStart->setEnabled(0);
            ui->actionQuit->setEnabled(1);
            ui->actionLoad->setEnabled(0);
            //设置状态栏信息
            lab->setText("当前处于终止状态,按Backspace键重新开始");
            break;
    }
}


void MainWindow::on_actionStart_triggered()
{
    //更新状态
    status = Gaming;
    //开启屏幕更新定时器
    updateScreenTimer->start();
}

void MainWindow::on_actionStop_triggered()
{
    //更新状态
    status = Stop;
    //关闭屏幕更新定时器
    updateScreenTimer->stop();
}

void MainWindow::on_actionContinue_triggered()
{
    //更新状态
    status = Gaming;
    //开启屏幕更新定时器
    updateScreenTimer->start();
}

void MainWindow::on_actionReset_triggered()
{
    //更新状态
    status = Ready;
    //初始化各个参数
    updateScreenTimer->stop();
    delete  snake;
    snake = new Snake(this);
    barrierPosX.clear();
    barrierPosY.clear();
    fruitPosX = -1;
    fruitPosY = -1;
    time = 0;
    score = 0;
    //初始化状态栏
    ui->label->setText("当前时间：未开始 当前分数：未开始");
    //更新画面
    update();
}

void MainWindow::on_actionSave_triggered()
{
    QString curPath = QDir::currentPath();
    //调用打开文件对话框选择一个文件
    QString aFileName = QFileDialog::getSaveFileName(this,"选择保存文件",curPath,"二进制数据文件(*.dat)");
    if(aFileName.isEmpty())
        return;
    if(saveBinaryFile(aFileName)) //保存为流数据文件
        QMessageBox::information(this,"提示消息","文件已经成功保存!");
}

void MainWindow::on_actionLoad_triggered()
{
    QString curPath = QDir::currentPath();
    //调用打开文件对话框选择一个文件
    QString aFileName = QFileDialog::getOpenFileName(this,"选择载入文件",curPath,"二进制数据文件(*.dat)");
    if(aFileName.isEmpty())
        return;
    if(loadBinaryFile(aFileName)) //载入流数据文件
        QMessageBox::information(this,"提示消息","文件已经成功载入!");
    //进入暂停状态
    status = Stop;
    //关闭屏幕更新定时器
    updateScreenTimer->stop();
    //更新状态栏
    ui->label->setText(QString::asprintf("当前时间：%d 当前分数：%d",time,score));
    //更新画面
    update();
}

void MainWindow::on_actionSetSpeed_triggered()
{
    QString dlgTitle="设置游戏速度";
    QString txtLabel="输入一个浮点数";
    float defaultValue=1.00;
    float minValue=0.01, maxValue=10.0;  //范围
    int decimals=2;//小数点位数
    bool ok=false;
    //弹出输入对话框
    float inputValue = QInputDialog::getDouble(this, dlgTitle,txtLabel,
                            defaultValue, minValue,maxValue,decimals,&ok);
    if (ok) //确认选择
    {
        //设置屏幕更新周期
        //k倍速就是将更新周期设置为默认周期的1/k
        updateScreenTimer->setInterval(updateScreenInterval/inputValue);
    }
}
