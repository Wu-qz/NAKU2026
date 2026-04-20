#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include <QRect>
#include <QPixmap>
#include <cstdlib>
#include <ctime>

class GameWidget : public QWidget {
public:
    const int GROUND_Y = 250;
    int gameState = 0;

    bool isDucking = false;
    const float STAND_W = 60;
    const float STAND_H = 67;
    const float DUCK_W = 65;
    const float DUCK_H = 44;

    const float BASE_JUMP = -7.0f;
    const float MAX_JUMP  = -14.0f;
    const float JUMP_INC = 0.7f;
    float currentJump = 0;
    bool isHoldingSpace = false;
    const float GRAVITY = 0.75f;

    float GAME_SPEED = 6.0f;
    const float MAX_SPEED_LIMIT = 28.0f;

    float dinoX = 50;
    float dinoY;
    float dinoVelY = 0;
    bool isJumping = false;

    int obstacleType = 0;
    float obsX = 900;
    float obsY;

    int score = 0;

    QPixmap pixDino;
    QPixmap pixDinoDuck;
    QPixmap pixCactus;
    QPixmap pixBird;

    QTimer *timer;

    GameWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setFixedSize(900, 300);
        setWindowTitle("凑企鹅跑酷");
        setFocusPolicy(Qt::StrongFocus);
        dinoY = GROUND_Y - STAND_H;

        pixDino.load("res/dino.png");
        pixDinoDuck.load("res/dino_duck.png");
        pixCactus.load("res/cactus.png");
        pixBird.load("res/bird.png");

        srand(time(0));
        randomObstacle();

        timer = new QTimer(this);
        timer->start(16);
        connect(timer, &QTimer::timeout, this, &GameWidget::updateGame);
    }

    void randomObstacle() {
        obsX = width() + rand() % 300 + 200;
        if (GAME_SPEED > 9 && rand() % 3 == 0) {
            obstacleType = 1;
            obsY = GROUND_Y - 70;
        } else {
            obstacleType = 0;
            obsY = GROUND_Y - 70;
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
            }
        }

        if (gameState == 1 && (e->key() == Qt::Key_Down || e->key() == Qt::Key_S)) {
            isDucking = true;
        }

        if (e->key() == Qt::Key_R && gameState == 2) {
            resetGame();
        }
    }

    void keyReleaseEvent(QKeyEvent *e) override {
        if (e->key() == Qt::Key_Space)
            isHoldingSpace = false;
        if (e->key() == Qt::Key_Down || e->key() == Qt::Key_S)
            isDucking = false;
    }

    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.fillRect(rect(), Qt::white);

        p.setPen(Qt::black);
        p.drawLine(0, GROUND_Y, width(), GROUND_Y);

        if (isDucking) {
            p.drawPixmap(dinoX, dinoY + (STAND_H - DUCK_H), DUCK_W, DUCK_H, pixDinoDuck);
        } else {
            p.drawPixmap(dinoX, dinoY, STAND_W, STAND_H, pixDino);
        }

        if (obstacleType == 0) {
            p.drawPixmap((int)obsX, (int)obsY, 50, 70, pixCactus);
        } else {
            p.drawPixmap((int)obsX, (int)obsY, 85, 40, pixBird);
        }

        if (gameState == 0) {
            p.setFont(QFont("微软雅黑", 26));
            p.drawText(rect(), Qt::AlignCenter, "你是谁啊\n空格键");
            return;
        }

        p.setFont(QFont("微软雅黑", 16));
        p.drawText(20, 30, "分数: " + QString::number(score * 100));
        p.drawText(20, 58, "速度: " + QString::number(GAME_SPEED, 'f', 1));
        p.drawText(20, 86, "空格跳跃  ↓下蹲");

        if (gameState == 2) {
            p.setFont(QFont("微软雅黑", 32));
            p.drawText(rect(), Qt::AlignCenter, "游戏结束！按 R 重开");
        }
    }

private:
    void resetGame() {
        gameState = 0;
        dinoY = GROUND_Y - STAND_H;
        dinoVelY = 0;
        isJumping = false;
        isDucking = false;
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
            if (dinoY >= GROUND_Y - STAND_H) {
                dinoY = GROUND_Y - STAND_H;
                dinoVelY = 0;
                isJumping = false;
                isHoldingSpace = false;
            }
        }

        obsX -= GAME_SPEED;
        if (obsX < -150) {
            score++;
            randomObstacle();
            float remain = MAX_SPEED_LIMIT - GAME_SPEED;
            GAME_SPEED += remain * 0.028f;
        }

        QRectF dinoRect = isDucking
                              ? QRectF(dinoX, dinoY + (STAND_H - DUCK_H), DUCK_W, DUCK_H)
                              : QRectF(dinoX, dinoY, STAND_W, STAND_H);

        QRectF obsRect = obstacleType == 0
                             ? QRectF(obsX, obsY, 50, 70)
                             : QRectF(obsX, obsY, 85, 40);

        if (dinoRect.intersects(obsRect)) {
            gameState = 2;
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