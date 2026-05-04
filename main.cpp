#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include <QRect>
#include <QPixmap>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <cstdlib>
#include <ctime>

class GameWidget : public QWidget {
public:
    const int GROUND_Y = 250;
    int gameState = 0;

    bool isDucking = false;
    bool duckSoundPlayed = false;

    const float STAND_W = 60;
    const float STAND_H = 67;
    const float DUCK_W = 65;
    const float DUCK_H = 44;

    const float BASE_JUMP = -7.0f;
    const float MAX_JUMP  = -12.0f;
    const float JUMP_INC = 0.7f;
    float currentJump = 0;
    bool isHoldingSpace = false;
    const float GRAVITY = 0.75f;

    float GAME_SPEED = 6.0f;
    const float MAX_SPEED_LIMIT = 28.0f;

    float dinoX = 100;
    float dinoY;
    float dinoVelY = 0;
    bool isJumping = false;

    int obstacleType = 0;
    float obsX = 900;
    float obsY;


    bool hasSecondObs = false;
    float obsX2;
    int obsType2;

    int score = 0;
    int highScore = 0;

    QPixmap pixDino;
    QPixmap pixDinoDuck;
    QPixmap pixCactus;
    QPixmap pixCactusSmall;
    QPixmap pixBird;

    QMediaPlayer *jumpSound;
    QAudioOutput *jumpAudio;
    QMediaPlayer *duckSound;
    QAudioOutput *duckAudio;
    QMediaPlayer *dieSound;
    QAudioOutput *dieAudio;

    QTimer *timer;

    GameWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setFixedSize(900, 300);
        setWindowTitle("凑企鹅跑酷");
        setFocusPolicy(Qt::StrongFocus);
        dinoY = GROUND_Y - STAND_H - 2;

        // 加载图片
        pixDino.load("res/dino.png");
        pixDinoDuck.load("res/dino_duck.png");
        pixCactus.load("res/cactus.png");
        pixCactusSmall.load("res/cactus.png");
        pixBird.load("res/bird.png");

        // 加载最高分
        loadHighScore();

        // 音效初始化
        jumpSound = new QMediaPlayer(this);
        jumpAudio = new QAudioOutput(this);
        jumpSound->setAudioOutput(jumpAudio);
        jumpSound->setSource(QUrl::fromLocalFile(QDir::currentPath() + "/res/jump.wav"));

        duckSound = new QMediaPlayer(this);
        duckAudio = new QAudioOutput(this);
        duckSound->setAudioOutput(duckAudio);
        duckSound->setSource(QUrl::fromLocalFile(QDir::currentPath() + "/res/duck.wav"));

        dieSound = new QMediaPlayer(this);
        dieAudio = new QAudioOutput(this);
        dieSound->setAudioOutput(dieAudio);
        dieSound->setSource(QUrl::fromLocalFile(QDir::currentPath() + "/res/die.wav"));

        srand(time(0));
        randomObstacle();

        timer = new QTimer(this);
        timer->start(16);
        connect(timer, &QTimer::timeout, this, &GameWidget::updateGame);
    }

    // 读取最高分
    void loadHighScore() {
        QFile file("score.txt");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            in >> highScore;
            file.close();
        }
    }

    // 保存最高分
    void saveHighScore() {
        QFile file("score.txt");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << highScore;
            file.close();
        }
    }

    // 生成障碍物
    void randomObstacle() {
        obsX = width() + rand() % 300 + 150;
        hasSecondObs = false;

        // 随机类型
        int rnd = rand() % 4;
        if (GAME_SPEED > 9 && rand() % 3 == 0) {
            obstacleType = 1;
            obsY = GROUND_Y - (60 + rand() % 60);
        } else {
            if(rnd == 0) { obstacleType = 0; obsY = GROUND_Y - 70; }
            if(rnd == 1) { obstacleType = 2; obsY = GROUND_Y - 50; }
            if(rnd == 2 || rnd == 3) { obstacleType = 0; obsY = GROUND_Y - 70; }
        }

        // 30% 概率生成第二个紧贴障碍物
        if(rand() % 100 < 30) {
            hasSecondObs = true;
            obsX2 = obsX + 45; // 紧贴间距
            obsType2 = rand() % 3;
        }
    }

protected:
    void keyPressEvent(QKeyEvent *e) override {
        if (e->key() == Qt::Key_Space) {
            if (gameState == 0) {
                gameState = 1;
            } else if (gameState == 1 && !isJumping) {
                isHoldingSpace = true;
                isJumping = true;
                currentJump = BASE_JUMP;
                dinoVelY = currentJump;

                jumpSound->stop();
                jumpSound->play();
            }
        }

        if (gameState == 1 && (e->key() == Qt::Key_Down || e->key() == Qt::Key_S)) {
            if (isDucking || duckSoundPlayed)
                return;

            isDucking = true;
            duckSoundPlayed = true;

            duckSound->stop();
            duckSound->play();
        }

        if (e->key() == Qt::Key_R && gameState == 2) {
            resetGame();
        }
    }

    void keyReleaseEvent(QKeyEvent *e) override {
        if (e->key() == Qt::Key_Space)
            isHoldingSpace = false;

        if (e->key() == Qt::Key_Down || e->key() == Qt::Key_S) {
            isDucking = false;
            duckSoundPlayed = false;
        }
    }

    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.fillRect(rect(), Qt::white);

        p.setPen(Qt::black);
        p.drawLine(0, GROUND_Y, width(), GROUND_Y);

        // 绘制恐龙
        if (isDucking) {
            p.drawPixmap(dinoX, dinoY + (STAND_H - DUCK_H), DUCK_W, DUCK_H, pixDinoDuck);
        } else {
            p.drawPixmap(dinoX, dinoY, STAND_W, STAND_H, pixDino);
        }

        // 绘制第一个障碍物
        drawOneObstacle(p, obstacleType, obsX, obsY);
        // 绘制第二个紧贴障碍物
        if(hasSecondObs) {
            drawOneObstacle(p, obsType2, obsX2, GROUND_Y - 70);
        }

        if (gameState == 0) {
            p.setFont(QFont("微软雅黑", 26));
            p.drawText(rect(), Qt::AlignCenter, "按空格键开始");
            return;
        }

        // 显示分数 + 最高分
        p.setFont(QFont("微软雅黑", 16));
        p.drawText(20, 30, "分数: " + QString::number(score * 100));
        p.drawText(20, 58, "最高分: " + QString::number(highScore));
        p.drawText(20, 86, "速度: " + QString::number(GAME_SPEED, 'f', 1));
        p.drawText(20, 114, "空格 跳跃  ↓/S 下蹲");

        if (gameState == 2) {
            p.setFont(QFont("微软雅黑", 32));
            p.drawText(rect(), Qt::AlignCenter, "游戏结束！按 R 重开");
        }
    }

    // 绘制单个障碍物
    void drawOneObstacle(QPainter &p, int type, float x, float y) {
        if (type == 0) {
            p.drawPixmap((int)x, (int)y, 50, 70, pixCactus);
        } else if (type == 1) {
            p.drawPixmap((int)x, (int)y, 85, 40, pixBird);
        } else if (type == 2) {
            p.drawPixmap((int)x, (int)y, 35, 50, pixCactusSmall);
        }
    }

    // 获取单个障碍物碰撞框
    QRectF getObstacleRect(int type, float x, float y) {
        if (type == 0) return QRectF(x, y, 50, 70);
        if (type == 1) return QRectF(x, y, 85, 40);
        if (type == 2) return QRectF(x, y, 35, 50);
        return QRectF(x, y, 50, 70);
    }

private:
    void resetGame() {
        gameState = 0;
        dinoY = GROUND_Y - STAND_H - 2;
        dinoVelY = 0;
        isJumping = false;
        isDucking = false;
        duckSoundPlayed = false;
        isHoldingSpace = false;
        GAME_SPEED = 6.0f;
        score = 0;
        randomObstacle();
    }

    void updateGame() {
        if (gameState != 1) {
            update();
            return;
        }

        if (isJumping && isHoldingSpace && currentJump > MAX_JUMP) {
            currentJump -= JUMP_INC;
            dinoVelY = currentJump;
        }

        if (isJumping) {
            dinoY += dinoVelY;
            dinoVelY += GRAVITY;
            if (dinoY >= GROUND_Y - STAND_H - 2) {
                dinoY = GROUND_Y - STAND_H - 2;
                dinoVelY = 0;
                isJumping = false;
                isHoldingSpace = false;
            }
        }

        // 障碍物移动
        obsX -= GAME_SPEED;
        if(hasSecondObs) obsX2 -= GAME_SPEED;

        if (obsX < -150) {
            score++;
            // 更新最高分
            if(score * 100 > highScore) {
                highScore = score * 100;
                saveHighScore();
            }
            randomObstacle();
            float remain = MAX_SPEED_LIMIT - GAME_SPEED;
            GAME_SPEED += remain * 0.028f;
        }

        // 恐龙碰撞框
        QRectF dinoRect = isDucking
                              ? QRectF(dinoX, dinoY + (STAND_H - DUCK_H), DUCK_W, DUCK_H)
                              : QRectF(dinoX, dinoY, STAND_W, STAND_H);

        // 检测第一个障碍物碰撞
        QRectF obsRect = getObstacleRect(obstacleType, obsX, obsY);
        bool hit = dinoRect.intersects(obsRect);

        // 检测第二个障碍物碰撞
        if(hasSecondObs) {
            QRectF obsRect2 = getObstacleRect(obsType2, obsX2, GROUND_Y - 70);
            if(dinoRect.intersects(obsRect2)) hit = true;
        }

        if (hit) {
            gameState = 2;
            dieSound->stop();
            dieSound->play();
        }

        update();
    }
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    GameWidget w;
    w.show();
    return a.exec();
}