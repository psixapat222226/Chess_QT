import QtQuick 2.15

Rectangle {
    id: chessBoard

    property int cellSize: Math.min(width, height) / 8

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
                    let row = Math.floor(index / 8);
                    let col = index % 8;
                    return (row + col) % 2 === 0 ? "#F1D9B5" : "#B98863";
                }
            }
        }
    }
}
