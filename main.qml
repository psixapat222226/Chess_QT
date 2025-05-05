import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Window {
    id: root
    width: 600
    height: 650
    visible: true
    title: "Шахматы"

    property int boardMargin: 20
    property int boardSize: Math.min(width, height - 50) - (2 * boardMargin)
    property int cellSize: boardSize / 8
    property var selectedPiece: null
    property bool inMenu: true

    // Функция для преобразования логических координат в визуальные
    function logicalToVisualPos(x, y) {
        return {
            x: x * cellSize,
            y: (7 - y) * cellSize
        }
    }

    // Функция для преобразования визуальных координат в логические
    function visualToLogicalPos(x, y) {
        return {
            x: Math.floor(x / cellSize),
            y: 7 - Math.floor(y / cellSize)
        }
    }

    // Очистка всех индикаторов и выбранных фигур
    function clearAllSelections() {
        moveIndicators.visible = false
        if (selectedPiece !== null) {
            selectedPiece.highlighted = false
        }
        selectedPiece = null
    }

    // Начальный экран с выбором режима игры
    Rectangle {
        id: menuScreen
        anchors.fill: parent
        color: "#F5F5F5"
        visible: inMenu

        Rectangle {
            width: 400
            height: 480
            color: "white"
            radius: 10
            border.color: "#CCCCCC"
            border.width: 1
            anchors.centerIn: parent

            ColumnLayout {
                anchors.centerIn: parent
                spacing: 25
                width: parent.width - 40

                Text {
                    text: "Шахматы"
                    font.pixelSize: 32
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }

                Text {
                    text: "Выберите режим игры"
                    font.pixelSize: 18
                    Layout.alignment: Qt.AlignHCenter
                }

                Button {
                    text: "Игра на двоих"
                    Layout.preferredHeight: 60
                    Layout.fillWidth: true
                    font.pixelSize: 16

                    onClicked: {
                        chessEngine.setGameMode("twoPlayers")
                        chessEngine.startNewGame()
                        inMenu = false
                    }
                }

                // Секция настроек игры против компьютера
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 220
                    color: "#F8F8F8"
                    radius: 10
                    border.color: "#E0E0E0"
                    border.width: 1

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 15
                        spacing: 15

                        Text {
                            text: "Игра против компьютера"
                            font.pixelSize: 18
                            font.bold: true
                            Layout.alignment: Qt.AlignHCenter
                        }

                        Text {
                            text: "Выберите уровень сложности:"
                            font.pixelSize: 14
                            Layout.topMargin: 5
                            Layout.alignment: Qt.AlignHCenter
                        }

                        // Кнопки выбора сложности
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 10

                            // Легкий
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 80
                                radius: 8
                                color: chessEngine.difficulty === 1 ? "#D4EDDA" : "#F8F9FA"
                                border.color: chessEngine.difficulty === 1 ? "#28A745" : "#DEE2E6"
                                border.width: 2

                                ColumnLayout {
                                    anchors.centerIn: parent
                                    spacing: 5

                                    Text {
                                        text: "🙂"
                                        font.pixelSize: 24
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    Text {
                                        text: "Легкий"
                                        font.pixelSize: 14
                                        font.bold: chessEngine.difficulty === 1
                                        color: chessEngine.difficulty === 1 ? "#155724" : "#212529"
                                        Layout.alignment: Qt.AlignHCenter
                                    }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: chessEngine.difficulty = 1
                                }
                            }

                            // Средний
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 80
                                radius: 8
                                color: chessEngine.difficulty === 2 ? "#CCE5FF" : "#F8F9FA"
                                border.color: chessEngine.difficulty === 2 ? "#0D6EFD" : "#DEE2E6"
                                border.width: 2

                                ColumnLayout {
                                    anchors.centerIn: parent
                                    spacing: 5

                                    Text {
                                        text: "😐"
                                        font.pixelSize: 24
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    Text {
                                        text: "Средний"
                                        font.pixelSize: 14
                                        font.bold: chessEngine.difficulty === 2
                                        color: chessEngine.difficulty === 2 ? "#084298" : "#212529"
                                        Layout.alignment: Qt.AlignHCenter
                                    }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: chessEngine.difficulty = 2
                                }
                            }

                            // Сложный
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 80
                                radius: 8
                                color: chessEngine.difficulty === 3 ? "#F8D7DA" : "#F8F9FA"
                                border.color: chessEngine.difficulty === 3 ? "#DC3545" : "#DEE2E6"
                                border.width: 2

                                ColumnLayout {
                                    anchors.centerIn: parent
                                    spacing: 5

                                    Text {
                                        text: "😈"
                                        font.pixelSize: 24
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    Text {
                                        text: "Сложный"
                                        font.pixelSize: 14
                                        font.bold: chessEngine.difficulty === 3
                                        color: chessEngine.difficulty === 3 ? "#842029" : "#212529"
                                        Layout.alignment: Qt.AlignHCenter
                                    }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: chessEngine.difficulty = 3
                                }
                            }
                        }



                        Button {
                            text: "Начать игру"
                            Layout.preferredHeight: 40
                            Layout.fillWidth: true
                            font.pixelSize: 15
                            highlighted: true

                            onClicked: {
                                chessEngine.setGameMode("vsComputer")
                                chessEngine.startNewGame()
                                inMenu = false
                            }
                        }
                    }
                }


            }
        }
    }

    // Игровой экран
    Item {
        anchors.fill: parent
        visible: !inMenu

        // Упрощенный компонент шахматной фигуры без перетаскивания
        component ChessPiece: Image {
            id: piece

            property int pieceX: 0  // Логическая X координата (0-7)
            property int pieceY: 0  // Логическая Y координата (0-7)
            property bool highlighted: false

            // Небольшой эффект подсветки для выбранной фигуры
            Rectangle {
                anchors.fill: parent
                color: "yellow"
                opacity: piece.highlighted ? 0.3 : 0
                z: -1
            }

            // Начальное позиционирование
            Component.onCompleted: {
                let pos = logicalToVisualPos(pieceX, pieceY)
                x = pos.x
                y = pos.y
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    // Уже выбрана эта фигура - отменяем выбор
                    if (selectedPiece === piece) {
                        clearAllSelections()
                        return
                    }

                    // Сбрасываем предыдущие выборы
                    clearAllSelections()

                    // Проверяем, может ли эта фигура ходить
                    let legalMoves = chessEngine.getLegalMovesForPiece(pieceX, pieceY)

                    if (legalMoves.length > 0) {
                        // Показываем возможные ходы
                        moveIndicators.fromX = pieceX
                        moveIndicators.fromY = pieceY
                        moveIndicators.legalMoves = legalMoves
                        moveIndicators.visible = true

                        // Выделяем выбранную фигуру
                        piece.highlighted = true
                        selectedPiece = piece
                    }
                }
            }
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 20

                Text {
                    id: statusText
                    text: chessEngine.status
                    font.pixelSize: 18
                }

                Button {
                    text: "В меню"
                    onClicked: {
                        inMenu = true
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Rectangle {
                    id: board
                    anchors.centerIn: parent
                    width: boardSize
                    height: boardSize
                    color: "#FFFFFF"

                    // MouseArea для фона доски чтобы снимать выделения по клику на пустую область
                    MouseArea {
                        anchors.fill: parent
                        z: -1  // Под всеми фигурами
                        onClicked: {
                            clearAllSelections()
                        }
                    }

                    // Рисуем шахматную доску
                    Grid {
                        anchors.fill: parent
                        rows: 8
                        columns: 8

                        Repeater {
                            model: 64

                            Rectangle {
                                width: cellSize
                                height: cellSize
                                color: {
                                    let row = Math.floor(index / 8)
                                    let col = index % 8
                                    return (row + col) % 2 === 0 ? "#F1D9B5" : "#B98863"
                                }
                            }
                        }
                    }

                    // Шахматные фигуры
                    Repeater {
                        id: piecesRepeater
                        model: chessEngine.getPieces()

                        ChessPiece {
                            width: cellSize
                            height: cellSize
                            source: resourceManager.getTexturePath(modelData.type)
                            pieceX: modelData.x
                            pieceY: modelData.y
                        }
                    }

                    // Индикаторы возможных ходов
                    Item {
                        id: moveIndicators
                        anchors.fill: parent
                        visible: false

                        property var legalMoves: []
                        property int fromX: -1
                        property int fromY: -1

                        Repeater {
                            id: movesRepeater
                            model: moveIndicators.legalMoves

                            Rectangle {
                                property var visualPos: logicalToVisualPos(modelData.x, modelData.y)

                                x: visualPos.x
                                y: visualPos.y
                                width: cellSize
                                height: cellSize
                                color: "transparent"
                                border.width: 3
                                border.color: "#32CD32"
                                radius: cellSize / 2
                                opacity: 0.7

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        if (chessEngine.processMove(
                                            moveIndicators.fromX, moveIndicators.fromY,
                                            modelData.x, modelData.y)) {
                                            clearAllSelections()
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // Кнопка новой игры
                    Button {
                        id: newGameButton
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            bottom: parent.bottom
                            bottomMargin: -35
                        }
                        text: "Новая игра"
                        onClicked: {
                            chessEngine.startNewGame()
                            clearAllSelections()
                        }
                    }
                }
            }
        }
    }

    // Обработка сигналов от движка
    Connections {
        target: chessEngine

        function onGameEnded(result) {
            resultDialog.text = result
            resultDialog.open()
        }

        function onPiecesChanged() {
            // Сбрасываем все выбранные фигуры и индикаторы при обновлении доски
            clearAllSelections()
            piecesRepeater.model = chessEngine.getPieces()
        }

        function onStatusChanged() {
            // Дополнительно убеждаемся, что индикаторы очищаются при смене хода
            clearAllSelections()
        }

        function onPawnPromotion(fromX, fromY, toX, toY) {
            promotionDialog.fromX = fromX
            promotionDialog.fromY = fromY
            promotionDialog.toX = toX
            promotionDialog.toY = toY
            promotionDialog.open()
        }

        // Добавляем обработку изменения уровня сложности
        function onDifficultyChanged() {
            // Этот метод будет вызываться при изменении сложности
            // Интерфейс будет автоматически обновляться благодаря привязкам
        }
    }

    Dialog {
        id: resultDialog
        title: "Игра окончена"
        property string text: ""

        Label {
            text: resultDialog.text
            font.pixelSize: 16
        }

        standardButtons: Dialog.Ok

        anchors.centerIn: Overlay.overlay

        onAccepted: {
            close()
        }
    }

    // Диалог выбора фигуры для превращения пешки
    Dialog {
        id: promotionDialog
        title: "Превращение пешки"
        modal: true
        closePolicy: Dialog.NoAutoClose

        property int fromX: -1
        property int fromY: -1
        property int toX: -1
        property int toY: -1

        anchors.centerIn: Overlay.overlay

        ColumnLayout {
            width: 300
            spacing: 10

            Text {
                text: "Выберите фигуру для превращения пешки:"
                font.pixelSize: 14
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 20

                Repeater {
                    model: ["queen", "rook", "bishop", "knight"]

                    delegate: Rectangle {
                        width: 60
                        height: 60
                        color: "#F0F0F0"
                        border.color: "#CCCCCC"
                        border.width: 1

                        Image {
                            anchors.fill: parent
                            anchors.margins: 5
                            source: {
                                // Определяем сторону (белые или черные) на основе положения
                                let side = promotionDialog.toY === 7 ? "white" : "black";
                                return resourceManager.getTexturePath(side + modelData.charAt(0).toUpperCase() + modelData.slice(1));
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                let pieceType = modelData;
                                chessEngine.promotePawn(
                                    promotionDialog.fromX,
                                    promotionDialog.fromY,
                                    promotionDialog.toX,
                                    promotionDialog.toY,
                                    pieceType
                                );
                                promotionDialog.close();
                            }
                        }
                    }
                }
            }
        }
    }
}
