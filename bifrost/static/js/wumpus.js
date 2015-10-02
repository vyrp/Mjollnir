//
// Wumpus (add real ID here)
//
challenge_player = {}
var side = 90;

function drawPlayer(direction, x, y){
    var p1x, p1y, p2x, p2y, p3x, p3y;
    var tbase = 5;
    var theight = 10;
    switch(direction){
        case "UP":
            p1x = x + side/2 - tbase;
            p1y = y + side/2 + theight;
            p2x = x + side/2 + tbase;
            p2y = y + side/2 + theight;
            p3x = x + side/2;
            p3y = y + side/2 - theight;
            break;
        case "RIGHT":
            p1x = x + side/2 - theight;
            p1y = y + side/2 - tbase;
            p2x = x + side/2 - theight;
            p2y = y + side/2 + tbase;
            p3x = x + side/2 + theight;
            p3y = y + side/2;
            break;
        case "DOWN":
            p1x = x + side/2 - tbase;
            p1y = y + side/2 - theight;
            p2x = x + side/2 + tbase;
            p2y = y + side/2 - theight;
            p3x = x + side/2;
            p3y = y + side/2 + theight;
            break;
        case "LEFT":
            p1x = x + side/2 + theight;
            p1y = y + side/2 - tbase;
            p2x = x + side/2 + theight;
            p2y = y + side/2 + tbase;
            p3x = x + side/2 - theight;
            p3y = y + side/2;
            break;
    }
    var adventurer_attr = { 'stroke': "#cc0000", 'stroke-width': 1, 'fill': "#cc0000" };
    var position = "M" + p1x + "," + p1y + " L" + p2x + "," + p2y + " L" + p3x + "," + p3y + " z"; 
    return paper.path(position).attr(adventurer_attr);
}


function drawGold(x, y){
    var gold_attrs = { 'font-size': 12, 'fill': "#ffcc66", 'font-family': 'Helvetica Neue", Helvetica, Arial, sans-serif', 'font-weight': 'bold' };
    var marginx = 0;
    var marginy = 10;
    var px, py;
    px = x + side/2 - marginx;
    py = y + marginy;

    return paper.text(px, py, "gold").attr(gold_attrs);
}


function drawPit(x, y){
    var gold_attrs = { 'font-size': 12, 'fill': "#ff6347", 'font-family': 'Helvetica Neue", Helvetica, Arial, sans-serif', 'font-weight': 'bold' };
    var marginx = 0;
    var marginy = 25;
    var px, py;
    px = x + side/2 - marginx;
    py = y + marginy;

    return paper.text(px, py, "pit").attr(gold_attrs);
}


function drawBreeze(x, y){
    var gold_attrs = { 'font-size': 12, 'fill': "#00ccff", 'font-family': 'Helvetica Neue", Helvetica, Arial, sans-serif', 'font-weight': 'bold' };
    var marginx = 0;
    var marginy = 40;
    var px, py;
    px = x + side/2 - marginx;
    py = y + marginy;

    return paper.text(px, py, "breeze").attr(gold_attrs);
}


function drawWumpus(x, y){
    var gold_attrs = { 'font-size': 12, 'fill': "#ba55d3", 'font-family': 'Helvetica Neue", Helvetica, Arial, sans-serif', 'font-weight': 'bold' };
    var marginx = 0;
    var marginy = 55;
    var px, py;
    px = x + side/2 - marginx;
    py = y + marginy;

    return paper.text(px, py, "wumpus").attr(gold_attrs);
}


function drawStench(x, y){
    var gold_attrs = { 'font-size': 12, 'fill': "#98fb98", 'font-family': 'Helvetica Neue", Helvetica, Arial, sans-serif', 'font-weight': 'bold' };
    var marginx = 0;
    var marginy = 70;
    var px, py;
    px = x + side/2 - marginx;
    py = y + marginy;

    return paper.text(px, py, "stench").attr(gold_attrs);
}


challenge_player.render_arena = function() {
    for (var i = 0; i < 4; ++i) for (j = 0; j < 4; ++j) {
        paper.rect(58 + side * i, 10 + side * j, side, side).attr({ stroke: "#505050", "stroke-width": 4 });
    }
}


var drawn_elements = [];
challenge_player.render_tick = function (tick) {

    // Cleanup
    drawn_elements.forEach(function (e) {
        e.remove();
    })

    drawn_elements = [];

    // Render logic
    var WM = challenge_player.match_data.wmList[tick];
    var direction = WM.direction;
    for (row in WM.table) {
        for (column in WM.table[row]) {
            var square = WM.table[column][row];

            if (square.indexOf("G") != -1) {
                drawn_elements.push(drawGold(58 + side * row, 10 + side * column));
            }
            if (square.indexOf("P") != -1) {
                drawn_elements.push(drawPit(58 + side * row, 10 + side * column));
            }
            if (square.indexOf("B") != -1) {
                drawn_elements.push(drawBreeze(58 + side * row, 10 + side * column));
            }
            if (square.indexOf("W") != -1) {
                drawn_elements.push(drawWumpus(58 + side * row, 10 + side * column));
            }
            if (square.indexOf("S") != -1) {
                drawn_elements.push(drawStench(58 + side * row, 10 + side * column));
            }
            if (square.indexOf("A") != -1){
                drawn_elements.push(drawPlayer(direction, 58 + side * row, 10 + side * column));
            } 

        }
    }
}