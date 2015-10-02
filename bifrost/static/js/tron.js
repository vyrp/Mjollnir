//
// Snake Fight (4f50c959-700b-4570-ae58-54592b4d316c)
//
challenge_player = {}

var username_font_attrs = { 'font-size': 30, 'fill': "#d1d2d1", 'font-family': 'Helvetica Neue", Helvetica, Arial, sans-serif', 'font-weight': 'bold' };
var color_player1 = "#cc0000"
var color_player2 = "#0000cc"
challenge_player.render_arena = function () {

    // Currently the grid is always 20x20
    for (var i = 0; i < 20; ++i) for (j = 0; j < 20; ++j) {
        paper.rect(90 + 15 * i, 75 + 15 * j, 13, 13).attr({ stroke: "#505050", "stroke-width": 1 });
    }

    // After the grid, we draw a text at the top of the canvas saying who is who
    var username_position_y = 10;

    bifrost_match_users.forEach(function (user) {
        var user_text = user.username;

        for (var uid in challenge_player.match_data.gameDescription) {
            if (uid == user.uid) {
                username_font_attrs.fill = challenge_player.match_data.gameDescription[uid] == "0" ? color_player1 : color_player2;
            }
        }

        paper.text(240, username_position_y, user_text).attr( username_font_attrs );
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
    
    for (var player in WM.players) {
        var player_color = player == 0 ? color_player1 : color_player2

        for (var body_idx in WM.players[player].body) {
            drawn_elements.push( paper.rect(90 + 15 * WM.players[player].body[body_idx].x, 75 + 15 * WM.players[player].body[body_idx].y, 13, 13).attr({ fill: player_color, "stroke-width": 0 }) );
        }
    }

}
