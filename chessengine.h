#pragma once

#include <QObject>
#include <QString>
#include <QVariant>
#include <QVector>
#include <QMap>
#include <QPoint>
#include <QTimer>
#include <chrono>
#include "Position.h"
#include "AI.h"

#define nsecs std::chrono::high_resolution_clock::now().time_since_epoch().count()

class ChessEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(int difficulty READ difficulty WRITE setDifficulty NOTIFY difficultyChanged)

public:
    explicit ChessEngine(QObject *parent = nullptr);

    Q_INVOKABLE QVariantList getPieces() const;
    Q_INVOKABLE bool processMove(int fromX, int fromY, int toX, int toY);
    Q_INVOKABLE QVariantList getLegalMovesForPiece(int x, int y) const;
    Q_INVOKABLE void startNewGame();
    Q_INVOKABLE void setGameMode(const QString& mode);
    Q_INVOKABLE QString getTextureName(int x, int y) const;
    Q_INVOKABLE void makeAIMove();
    Q_INVOKABLE bool promotePawn(int fromX, int fromY, int toX, int toY, const QString& pieceType);

    // Новые методы для работы со сложностью
    int difficulty() const;
    void setDifficulty(int newDifficulty);
    Q_INVOKABLE QString getDifficultyName() const;

    QString status() const;

signals:
    void piecesChanged();
    void statusChanged();
    void gameEnded(const QString& result);
    void pawnPromotion(int fromX, int fromY, int toX, int toY);
    void difficultyChanged();

private:
    enum STATUS {
        WHITE_TO_MOVE,
        BLACK_TO_MOVE,
        WHITE_WON,
        BLACK_WON,
        DRAW
    };

    enum class GameMode {
        TwoPlayers,
        VsComputer
    };

    Position position;
    QPoint selectedPiece;
    STATUS currentStatus;
    GameMode gameMode;
    int m_difficulty; // 1 - легкий, 2 - средний, 3 - сложный

    void updateStatus();
    QString statusToString() const;
    uint8_t getStatus() const;
};
