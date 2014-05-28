//
// Tic-Tac-Toe (61dd3230-2ea1-4cc1-b521-457f91b03a9e)
//
challenge_player = {}

challenge_player.render_arena = function() {
    // The tic-tac-toe grid is 300x300, on a 480x400 canvas
    var arena = paper.path("M190,70 L190,370 M290,70 L290,370 M90,170 L390,170 M90,270 L390,270").attr({ stroke: "#aaaaaa", 'stroke-width': 5 });
}

var drawn_elements = [];
var font_attrs = { 'font-size': 70, 'fill': "#d1d2d1", 'font-family': 'Helvetica Neue", Helvetica, Arial, sans-serif', 'font-weight': 'bold' };
challenge_player.render_tick = function (tick) {

    // Cleanup
    drawn_elements.forEach(function (e) {
        e.remove();
    })

    drawn_elements = [];

    // Render logic
    var WM = challenge_player.match_data.wmList[tick];

    for (row in WM.table) {
        for (column in WM.table[row]) {
            var mark = WM.table[row][column];

            if (mark == 'X' || mark == 'O') {
                drawn_elements.push( paper.text(140 + column*100, 120 + row*100, mark).attr(font_attrs) );
            }
        }
    }

}
