#include "ChessEngine.h"
#include <QDebug>
#include <QTimer>

ChessEngine::ChessEngine(QObject *parent)
    : QObject(parent),
    selectedPiece(QPoint(-1, -1)),
    currentStatus(STATUS::WHITE_TO_MOVE),
    gameMode(GameMode::TwoPlayers),
    m_difficulty(2) // По умолчанию средняя сложность
{
    startNewGame();
}

void ChessEngine::startNewGame()
{
    // Инициализация позиции на шахматной доске
    position = Position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR",
                        Position::NONE, true, true, true, true, 1);
    selectedPiece = QPoint(-1, -1);
    currentStatus = STATUS::WHITE_TO_MOVE;
    emit piecesChanged();
    emit statusChanged();
}

void ChessEngine::setGameMode(const QString& mode)
{
    if (mode == "twoPlayers") {
        gameMode = GameMode::TwoPlayers;
    } else if (mode == "vsComputer") {
        gameMode = GameMode::VsComputer;
    }
}

QVariantList ChessEngine::getPieces() const
{
    QVariantList piecesList;
    Pieces pieces = position.getPieces();

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            int index = y * 8 + x;
            QString pieceName;

            if (BOp::getBit(pieces.getPieceBitboard(SIDE::WHITE, PIECE::PAWN), index))
                pieceName = "whitePawn";
            else if (BOp::getBit(pieces.getPieceBitboard(SIDE::WHITE, PIECE::KNIGHT), index))
                pieceName = "whiteKnight";
            else if (BOp::getBit(pieces.getPieceBitboard(SIDE::WHITE, PIECE::BISHOP), index))
                pieceName = "whiteBishop";
            else if (BOp::getBit(pieces.getPieceBitboard(SIDE::WHITE, PIECE::ROOK), index))
                pieceName = "whiteRook";
            else if (BOp::getBit(pieces.getPieceBitboard(SIDE::WHITE, PIECE::QUEEN), index))
                pieceName = "whiteQueen";
            else if (BOp::getBit(pieces.getPieceBitboard(SIDE::WHITE, PIECE::KING), index))
                pieceName = "whiteKing";
            else if (BOp::getBit(pieces.getPieceBitboard(SIDE::BLACK, PIECE::PAWN), index))
                pieceName = "blackPawn";
            else if (BOp::getBit(pieces.getPieceBitboard(SIDE::BLACK, PIECE::KNIGHT), index))
                pieceName = "blackKnight";
            else if (BOp::getBit(pieces.getPieceBitboard(SIDE::BLACK, PIECE::BISHOP), index))
                pieceName = "blackBishop";
            else if (BOp::getBit(pieces.getPieceBitboard(SIDE::BLACK, PIECE::ROOK), index))
                pieceName = "blackRook";
            else if (BOp::getBit(pieces.getPieceBitboard(SIDE::BLACK, PIECE::QUEEN), index))
                pieceName = "blackQueen";
            else if (BOp::getBit(pieces.getPieceBitboard(SIDE::BLACK, PIECE::KING), index))
                pieceName = "blackKing";
            else
                continue;

            QVariantMap piece;
            piece["x"] = x;
            piece["y"] = y;
            piece["type"] = pieceName;
            piecesList.append(piece);
        }
    }

    return piecesList;
}

QString ChessEngine::getTextureName(int x, int y) const
{
    int index = y * 8 + x;
    Pieces pieces = position.getPieces();

    if (BOp::getBit(pieces.getPieceBitboard(SIDE::WHITE, PIECE::PAWN), index))
        return "whitePawn";
    else if (BOp::getBit(pieces.getPieceBitboard(SIDE::WHITE, PIECE::KNIGHT), index))
        return "whiteKnight";
    else if (BOp::getBit(pieces.getPieceBitboard(SIDE::WHITE, PIECE::BISHOP), index))
        return "whiteBishop";
    else if (BOp::getBit(pieces.getPieceBitboard(SIDE::WHITE, PIECE::ROOK), index))
        return "whiteRook";
    else if (BOp::getBit(pieces.getPieceBitboard(SIDE::WHITE, PIECE::QUEEN), index))
        return "whiteQueen";
    else if (BOp::getBit(pieces.getPieceBitboard(SIDE::WHITE, PIECE::KING), index))
        return "whiteKing";
    else if (BOp::getBit(pieces.getPieceBitboard(SIDE::BLACK, PIECE::PAWN), index))
        return "blackPawn";
    else if (BOp::getBit(pieces.getPieceBitboard(SIDE::BLACK, PIECE::KNIGHT), index))
        return "blackKnight";
    else if (BOp::getBit(pieces.getPieceBitboard(SIDE::BLACK, PIECE::BISHOP), index))
        return "blackBishop";
    else if (BOp::getBit(pieces.getPieceBitboard(SIDE::BLACK, PIECE::ROOK), index))
        return "blackRook";
    else if (BOp::getBit(pieces.getPieceBitboard(SIDE::BLACK, PIECE::QUEEN), index))
        return "blackQueen";
    else if (BOp::getBit(pieces.getPieceBitboard(SIDE::BLACK, PIECE::KING), index))
        return "blackKing";

    return "";
}

QVariantList ChessEngine::getLegalMovesForPiece(int x, int y) const
{
    QVariantList moves;
    int from = y * 8 + x;

    // Получаем ходы только для фигур текущего игрока
    Pieces pieces = position.getPieces();
    if ((currentStatus == STATUS::WHITE_TO_MOVE &&
         !BOp::getBit(pieces.getSideBitboard(SIDE::WHITE), from)) ||
        (currentStatus == STATUS::BLACK_TO_MOVE &&
         !BOp::getBit(pieces.getSideBitboard(SIDE::BLACK), from))) {
        return moves;
    }

    uint8_t side = (currentStatus == STATUS::WHITE_TO_MOVE) ? SIDE::WHITE : SIDE::BLACK;
    MoveList legalMoves = LegalMoveGen::generate(position, side);

    for (uint8_t i = 0; i < legalMoves.getSize(); i++) {
        Move move = legalMoves[i];
        if (move.getFrom() == from) {
            int toX = move.getTo() % 8;
            int toY = move.getTo() / 8;
            QVariantMap moveMap;
            moveMap["x"] = toX;
            moveMap["y"] = toY;
            moves.append(moveMap);
        }
    }

    return moves;
}

bool ChessEngine::processMove(int fromX, int fromY, int toX, int toY)
{
    if (currentStatus != STATUS::WHITE_TO_MOVE &&
        currentStatus != STATUS::BLACK_TO_MOVE) {
        return false;
    }

    // В режиме игры с компьютером, игрок может ходить только белыми
    if (gameMode == GameMode::VsComputer && currentStatus == STATUS::BLACK_TO_MOVE) {
        return false;
    }

    uint8_t side = (currentStatus == STATUS::WHITE_TO_MOVE) ? SIDE::WHITE : SIDE::BLACK;
    uint8_t from = fromY * 8 + fromX;
    uint8_t to = toY * 8 + toX;

    MoveList moves = LegalMoveGen::generate(position, side);
    int moveIndex = -1;
    bool isPawnPromotion = false;

    // Проверяем, является ли это ходом пешки на последнюю горизонталь
    bool isWhitePawnPromotion = (side == SIDE::WHITE && toY == 7 &&
                                 BOp::getBit(position.getPieces().getPieceBitboard(SIDE::WHITE, PIECE::PAWN), from));
    bool isBlackPawnPromotion = (side == SIDE::BLACK && toY == 0 &&
                                 BOp::getBit(position.getPieces().getPieceBitboard(SIDE::BLACK, PIECE::PAWN), from));

    // Если это ход пешки на последнюю горизонталь, запускаем диалог выбора фигуры
    if (isWhitePawnPromotion || isBlackPawnPromotion) {
        // Просто проверяем, что такой ход возможен (без учета типа превращения)
        for (uint8_t i = 0; i < moves.getSize(); i++) {
            Move move = moves[i];
            if (move.getFrom() == from && move.getTo() == to &&
                (move.getFlag() == Move::FLAG::PROMOTE_TO_QUEEN ||
                 move.getFlag() == Move::FLAG::PROMOTE_TO_ROOK ||
                 move.getFlag() == Move::FLAG::PROMOTE_TO_BISHOP ||
                 move.getFlag() == Move::FLAG::PROMOTE_TO_KNIGHT)) {
                isPawnPromotion = true;
                // Мы нашли допустимый ход превращения пешки, но не выполняем его сейчас
                // Вместо этого запрашиваем у пользователя тип фигуры
                emit pawnPromotion(fromX, fromY, toX, toY);
                return true; // Возвращаем true, чтобы показать, что ход будет обработан позже
            }
        }
    }

    // Если это не превращение пешки, продолжаем как обычно
    if (!isPawnPromotion) {
        for (uint8_t i = 0; i < moves.getSize(); i++) {
            Move move = moves[i];
            if (move.getFrom() == from && move.getTo() == to) {
                moveIndex = i;
                break;
            }
        }
    }

    if (moveIndex != -1) {
        Move selectedMove = moves[moveIndex];

        position.move(selectedMove);
        updateStatus();
        emit piecesChanged();
        emit statusChanged();

        if (currentStatus == STATUS::WHITE_WON ||
            currentStatus == STATUS::BLACK_WON ||
            currentStatus == STATUS::DRAW) {
            QString result;
            if (currentStatus == STATUS::WHITE_WON) {
                result = "Белые победили!";
            } else if (currentStatus == STATUS::BLACK_WON) {
                result = "Чёрные победили!";
            } else {
                result = "Ничья!";
            }
            emit gameEnded(result);
        }
        // Если режим игры с компьютером и сейчас ход черных, компьютер делает ход
        else if (gameMode == GameMode::VsComputer && currentStatus == STATUS::BLACK_TO_MOVE) {
            QTimer::singleShot(500, this, &ChessEngine::makeAIMove);
        }

        return true;
    }

    return false;
}

bool ChessEngine::promotePawn(int fromX, int fromY, int toX, int toY, const QString& pieceType)
{
    uint8_t side = (currentStatus == STATUS::WHITE_TO_MOVE) ? SIDE::WHITE : SIDE::BLACK;
    uint8_t from = fromY * 8 + fromX;
    uint8_t to = toY * 8 + toX;

    MoveList moves = LegalMoveGen::generate(position, side);
    int moveIndex = -1;

    // Определяем флаг превращения в зависимости от выбранного типа фигуры
    Move::FLAG promotionFlag;
    if (pieceType == "queen") {
        promotionFlag = Move::FLAG::PROMOTE_TO_QUEEN;
    } else if (pieceType == "rook") {
        promotionFlag = Move::FLAG::PROMOTE_TO_ROOK;
    } else if (pieceType == "bishop") {
        promotionFlag = Move::FLAG::PROMOTE_TO_BISHOP;
    } else if (pieceType == "knight") {
        promotionFlag = Move::FLAG::PROMOTE_TO_KNIGHT;
    } else {
        // По умолчанию превращаем в ферзя
        promotionFlag = Move::FLAG::PROMOTE_TO_QUEEN;
    }

    // Находим ход с соответствующим флагом
    for (uint8_t i = 0; i < moves.getSize(); i++) {
        Move move = moves[i];
        if (move.getFrom() == from && move.getTo() == to && move.getFlag() == promotionFlag) {
            moveIndex = i;
            break;
        }
    }

    if (moveIndex != -1) {
        Move selectedMove = moves[moveIndex];

        position.move(selectedMove);
        updateStatus();
        emit piecesChanged();
        emit statusChanged();

        if (currentStatus == STATUS::WHITE_WON ||
            currentStatus == STATUS::BLACK_WON ||
            currentStatus == STATUS::DRAW) {
            QString result;
            if (currentStatus == STATUS::WHITE_WON) {
                result = "Белые победили!";
            } else if (currentStatus == STATUS::BLACK_WON) {
                result = "Чёрные победили!";
            } else {
                result = "Ничья!";
            }
            emit gameEnded(result);
        }
        // Если режим игры с компьютером и сейчас ход черных, компьютер делает ход
        else if (gameMode == GameMode::VsComputer && currentStatus == STATUS::BLACK_TO_MOVE) {
            QTimer::singleShot(500, this, &ChessEngine::makeAIMove);
        }

        return true;
    }

    return false;
}



void ChessEngine::updateStatus()
{
    currentStatus = static_cast<STATUS>(getStatus());
}

QString ChessEngine::status() const
{
    return statusToString();
}

QString ChessEngine::statusToString() const
{
    switch (currentStatus) {
    case STATUS::WHITE_TO_MOVE: return "Ход белых";
    case STATUS::BLACK_TO_MOVE: return "Ход чёрных";
    case STATUS::WHITE_WON: return "Белые выиграли";
    case STATUS::BLACK_WON: return "Чёрные выиграли";
    case STATUS::DRAW: return "Ничья";
    }
    return "Неизвестно";
}

uint8_t ChessEngine::getStatus() const
{
    if (position.fiftyMovesRuleDraw() || position.threefoldRepetitionDraw()) {
        return STATUS::DRAW;
    }

    if (!position.getPieces().getPieceBitboard(SIDE::WHITE, PIECE::PAWN) &&
        !position.getPieces().getPieceBitboard(SIDE::BLACK, PIECE::PAWN) &&
        !position.getPieces().getPieceBitboard(SIDE::WHITE, PIECE::ROOK) &&
        !position.getPieces().getPieceBitboard(SIDE::BLACK, PIECE::ROOK) &&
        !position.getPieces().getPieceBitboard(SIDE::WHITE, PIECE::QUEEN) &&
        !position.getPieces().getPieceBitboard(SIDE::BLACK, PIECE::QUEEN) &&
        BOp::count1(position.getPieces().getPieceBitboard(SIDE::WHITE, PIECE::KNIGHT) |
                    position.getPieces().getPieceBitboard(SIDE::WHITE, PIECE::BISHOP)) < 2 &&
        BOp::count1(position.getPieces().getPieceBitboard(SIDE::BLACK, PIECE::KNIGHT) |
                    position.getPieces().getPieceBitboard(SIDE::BLACK, PIECE::BISHOP)) < 2) {
        return STATUS::DRAW;
    }

    if (position.whiteToMove()) {
        MoveList whiteMoves = LegalMoveGen::generate(position, SIDE::WHITE);
        if (whiteMoves.getSize() == 0) {
            bool whiteInCheck = PsLegalMoveMaskGen::inDanger(
                position.getPieces(),
                BOp::bsf(position.getPieces().getPieceBitboard(SIDE::WHITE, PIECE::KING)),
                SIDE::WHITE
                );
            return whiteInCheck ? STATUS::BLACK_WON : STATUS::DRAW;
        }
        return STATUS::WHITE_TO_MOVE;
    } else {
        MoveList blackMoves = LegalMoveGen::generate(position, SIDE::BLACK);
        if (blackMoves.getSize() == 0) {
            bool blackInCheck = PsLegalMoveMaskGen::inDanger(
                position.getPieces(),
                BOp::bsf(position.getPieces().getPieceBitboard(SIDE::BLACK, PIECE::KING)),
                SIDE::BLACK
                );
            return blackInCheck ? STATUS::WHITE_WON : STATUS::DRAW;
        }
        return STATUS::BLACK_TO_MOVE;
    }
}

int ChessEngine::difficulty() const
{
    return m_difficulty;
}

void ChessEngine::setDifficulty(int newDifficulty)
{
    if (newDifficulty < 1)
        newDifficulty = 1;
    else if (newDifficulty > 3)
        newDifficulty = 3;

    if (m_difficulty != newDifficulty) {
        m_difficulty = newDifficulty;
        emit difficultyChanged();
    }
}

QString ChessEngine::getDifficultyName() const
{
    switch (m_difficulty) {
    case 1: return "Легкий";
    case 2: return "Средний";
    case 3: return "Сложный";
    default: return "Средний";
    }
}

// Изменение метода makeAIMove для учета сложности
void ChessEngine::makeAIMove()
{
    if (currentStatus != STATUS::BLACK_TO_MOVE) {
        return;
    }

    // Время на размышление в зависимости от сложности (в миллисекундах)
    int thinkingTime;
    switch (m_difficulty) {
    case 1: // Легкий
        thinkingTime = 500; // 0.5 секунд
        break;
    case 2: // Средний
        thinkingTime = 1000; // 1 секунда
        break;
    case 3: // Сложный
        thinkingTime = 2000; // 2 секунды
        break;
    default:
        thinkingTime = 1000; // По умолчанию 1 секунда
    }

    // AI делает ход с учетом сложности
    Move aiMove = AI::getBestMove(position, SIDE::BLACK, thinkingTime);

    position.move(aiMove);
    updateStatus();
    emit piecesChanged();
    emit statusChanged();

    if (currentStatus == STATUS::WHITE_WON ||
        currentStatus == STATUS::BLACK_WON ||
        currentStatus == STATUS::DRAW) {
        QString result;
        if (currentStatus == STATUS::WHITE_WON) {
            result = "Белые победили!";
        } else if (currentStatus == STATUS::BLACK_WON) {
            result = "Чёрные победили!";
        } else {
            result = "Ничья!";
        }
        emit gameEnded(result);
    }
}
