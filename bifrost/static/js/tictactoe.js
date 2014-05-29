//
// Tic-Tac-Toe (61dd3230-2ea1-4cc1-b521-457f91b03a9e)
//
challenge_player = {}

var username_font_attrs = { 'font-size': 30, 'fill': "#d1d2d1", 'font-family': 'Helvetica Neue", Helvetica, Arial, sans-serif', 'font-weight': 'bold' };
challenge_player.render_arena = function() {
    // The tic-tac-toe grid is 300x300, on a 480x400 canvas
    var grid = paper.path("M190,80 L190,380 M290,80 L290,380 M90,180 L390,180 M90,280 L390,280").attr({ stroke: "#aaaaaa", 'stroke-width': 5 });

    // After the grid, we draw a text at the top of the canvas saying who is X and who is O
    var username_position_y = 10;

    bifrost_match_users.forEach(function (user) {
        var user_text = user.username;

        for (uid in challenge_player.match_data.gameDescription) {
            if (uid == user.uid) {
                user_text += ": " + challenge_player.match_data.gameDescription[uid]
            }
        }

        paper.text(240, username_position_y, user_text).attr(username_font_attrs);
        username_position_y += 35;
    })
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
                drawn_elements.push( paper.text(140 + column*100, 125 + row*100, mark).attr(font_attrs) );
            }
        }
    }

}
