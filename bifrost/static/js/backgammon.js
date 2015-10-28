//
// Backgammon
//
challenge_player = {};

var tbase = 29;     // triangle base
var theight = 120;  // triangle height
var sbase = 15;     // left-right side bar base
var sheight = 15;   // top-bottom side bar height
var cbase = 30;     // center bar base
var cheight = 30;   // space between Up and DOWN triangles
var radius = 10;    // piece circle radius
var dside = 50;     // dice side
var dradius = 5;    // dice points radius
var bbase = tbase * 12 + cbase + 2 * sbase;          // board base
var bheight = 2 * theight + cheight + 2 * sheight;   // board height
var board_top = 10;
var board_bottom = board_top + bheight;
var board_left = 0;
var board_right = board_left + bbase;
var bcenter = board_left + sbase + 6 * tbase + cbase/2;
var bcenterx = board_left + bbase/2;  // middle coordinate X of board
var bcentery = board_top + bheight/2; // middle coordinate Y of board

var border_attr = { 'stroke-width': 0, 'fill': "#d0d0d0" };
var board_bg_attr = { 'stroke-width': 0, 'fill': "#171717" };

var red = '#ff3333';
var white = '#ffffff';

// borne off things
var boside = 60;
var box = board_right + 11; 
var boy1 = board_top + sheight; 
var boy2 = board_bottom - sheight - boside;


var elements = [];

function getPointX(idx){
    if(idx < 6){
        return board_right - sbase - tbase/2 - (tbase * idx);
    }
    if(idx < 12){
        return board_left + sbase + tbase/2 + (11 - idx) * tbase;
    } 
    if(idx < 18){
        return board_left + sbase + tbase/2 + (idx - 12) * tbase;
    } 

    return board_right - sbase - tbase/2 - (23 - idx) * tbase;
}

function getPointY(idx){
    if(idx < 12) {
        return board_bottom - sheight;
    } 
    return board_top + sheight;
}

function getTColor(idx){
    if(idx % 2 == 0) {
        return '#909090';
    }
    return '#484848';
}

function drawTriangle(idx){
    var p1x, p1y, p2x, p2y, p3x, p3y, color;

    p1x = getPointX(idx) - tbase/2;
    p1y = getPointY(idx);
    p2x = getPointX(idx);
    p3x = getPointX(idx) + tbase/2;
    p3y = getPointY(idx);   

    if( idx < 12) { // BOTTOM TRIANGLE
        p2y = getPointY(idx) - theight;
    } else { // TOP TRIANGLE
        p2y = getPointY(idx) + theight;
    }

    color = getTColor(idx);

    var tattr = { 'stroke': color, 'stroke-width': 1, 'fill': color };
    var position = "M" + p1x + "," + p1y + " L" + p2x + "," + p2y + " L" + p3x + "," + p3y + " Z"; 

    paper.path(position).attr(tattr);
}

function drawLeftBorder(){
    return paper.rect(board_left, board_top, sbase, bheight, 3).attr(border_attr);
}

function drawRightBorder(){
    return paper.rect(board_right - sbase, board_top, sbase, bheight, 3).attr(border_attr);
}

function drawTopBorder(){
    return paper.rect(board_left, board_top, bbase, sheight, 3).attr(border_attr);
}

function drawBottomBorder(){
    return paper.rect(board_left, board_bottom - sheight, bbase, sheight, 3).attr(border_attr);
}

function drawBoardCenter(){
    return paper.rect(board_left + sbase + 6 * tbase, board_top, cbase, bheight, 3).attr(border_attr);
}

function drawBoardBackground(){
    return paper.rect(board_left, board_top, bbase, bheight, 3).attr(board_bg_attr);
}

function drawAllTriangles(){

    for(var i = 0; i < 24; i++){
        drawTriangle(i);
    }
}

function drawDie(number, idx){
    var diex1 = board_right - 2*dside - 20;
    var diey1 = board_bottom + 10;
    var diex2 = board_right - dside - 10;
    var diey2 = board_bottom + 10;

    var die_bg_attr = {'fill': '#ffffff'};
    var dot_attr = {'fill': '#000000'};
    var x, y, d = 12;

    if(idx == 0){
        x = diex1;
        y = diey1;
    } else {
        x = diex2;
        y = diey2;
    }

    elements.push(paper.rect(x, y, dside, dside, 4).attr(die_bg_attr));

    if(number == 1){
        elements.push(paper.circle(x + dside/2, y + dside/2, dradius).attr(dot_attr));
    } else if(number == 2){
        elements.push(paper.circle(x + d, y + dside - d, dradius).attr(dot_attr));
        elements.push(paper.circle(x + dside - d, y + d, dradius).attr(dot_attr));
    } else if(number == 3){
        elements.push(paper.circle(x + d, y + dside - d, dradius).attr(dot_attr));
        elements.push(paper.circle(x + dside - d, y + d, dradius).attr(dot_attr));
        elements.push(paper.circle(x + dside/2, y + dside/2, dradius).attr(dot_attr));
    } else if(number == 4){
        elements.push(paper.circle(x + d, y + d, dradius).attr(dot_attr));
        elements.push(paper.circle(x + dside - d, y + d, dradius).attr(dot_attr));
        elements.push(paper.circle(x + d, y + dside - d, dradius).attr(dot_attr));
        elements.push(paper.circle(x + dside - d, y + dside - d, dradius).attr(dot_attr));
    } else if(number == 5){
        elements.push(paper.circle(x + d, y + d, dradius).attr(dot_attr));
        elements.push(paper.circle(x + dside - d, y + d, dradius).attr(dot_attr));
        elements.push(paper.circle(x + d, y + dside - d, dradius).attr(dot_attr));
        elements.push(paper.circle(x + dside - d, y + dside - d, dradius).attr(dot_attr));
        elements.push(paper.circle(x + dside/2, y + dside/2, dradius).attr(dot_attr));
    } else {
        elements.push(paper.circle(x + d, y + d, dradius).attr(dot_attr));
        elements.push(paper.circle(x + dside - d, y + d, dradius).attr(dot_attr));
        elements.push(paper.circle(x + d, y + dside - d, dradius).attr(dot_attr));
        elements.push(paper.circle(x + dside - d, y + dside - d, dradius).attr(dot_attr));
        elements.push(paper.circle(x + d, y + dside/2, dradius).attr(dot_attr));
        elements.push(paper.circle(x + dside - d, y + dside/2, dradius).attr(dot_attr));
    }
}

function drawPiece(x, y, color){
    var pattr = { 'stroke': color, 'stroke-width': 1, 'fill': color };
    return paper.circle(x, y, radius).attr(pattr);
}

function drawBarPieces(nr, nw){

    var x, y, nmax, color, extray, nextra = 0;
    x = bcenterx;
    
    if(nr > 0){ // TOP
        color = red;
        extray = getPointY(23) - 10;
        nmax = nr;
        if(nr > 5){
            nmax = 5;
            nextra = nr - nmax;
        }
        for(var i = 0; i < nmax; i++){
            y = getPointY(23) + theight - radius - i * 2 * radius;
            elements.push(drawPiece(x, y, color));
        }
        if(nextra > 0){
            var extra_piece_attr = { 'stroke': '#ffffff', 'font-size': 20, 'fill': color, 'font-family': 'Arial Black, sans-serif', 'font-weight': 'bold' };
            elements.push(paper.text(x, extray, '+' + nextra).attr(extra_piece_attr));
        }
    }

    if(nw > 0){ // BOTTOM
        color = white;
        nmax = nw;
        extray = getPointY(0) + 10;
        if(nw > 5){
            nmax = 5;
            nextra = nw - nmax;
        }
        for(var i = 0; i < nmax; i++){
            y = getPointY(0) - theight + radius + i * 2 * radius;
            elements.push(drawPiece(x, y, color));
        }

        if(nextra > 0){
            var extra_piece_attr = { 'stroke': '#171717', 'font-size': 20, 'fill': color, 'font-family': 'Arial Black, sans-serif', 'font-weight': 'bold' };
            elements.push(paper.text(x, extray, '+' + nextra).attr(extra_piece_attr));
        }
    }
}

function drawPieces(idx, nr, nw){
    var x, y, nmax, n, color, fsize, nextra = 0;
    x = getPointX(idx);

    if(nr > 0){ 
        n = nr;
        color = red;
    } else {
        n = nw;
        color = white;
    }

    nmax = n;
    if(n > 5){
        nmax = 5;
        nextra = n - nmax;
    }

    x = getPointX(idx);
    for(var i = 0; i < nmax; i++){
        if(idx < 12){
            y = getPointY(idx) - radius - i * 2 * radius;             
        } else {
            y = getPointY(idx) + radius + i * 2 * radius;
        }
        elements.push(drawPiece(x, y, color));
    }

    if(nextra > 0){

        if(idx < 12) y = getPointY(idx) - theight -2;
        else y = getPointY(idx) + theight + 2;

        fsize = 16;

        var extra_piece_attr = { 'font-size': fsize, 'fill': color, 'font-family': 'Arial Black, sans-serif', 'font-weight': 'bold' };
        elements.push(paper.text(x, y, '+' + nextra).attr(extra_piece_attr));
    }

}


function drawGameDescription(){
    var gdxx = (board_left + board_right - 2 * dside - 30)/2;
    var gdx1 = gdxx - 20;
    var gdx2 = gdxx + 20;
    var gdy = board_bottom + 10 + dside/2;
    var gd = challenge_player.match_data.gameDescription;
    var redPlayer, whitePlayer;

    bifrost_match_users.forEach(function (user) {
        user_text = user.username;

        for (uid in challenge_player.match_data.gameDescription) {
            if (uid == user.uid) {
                gd = challenge_player.match_data.gameDescription[uid];

                if(gd == "RED"){
                    redPlayer = user_text;
                } else {
                    whitePlayer = user_text;
                }                
            }
        }
    });

    var gdp1_attr = { 'font-size': 16, 'fill': red, 'font-family': 'Arial, sans-serif', 'font-weight': 'bold', 'text-anchor': 'end' };
    var gdp2_attr = { 'font-size': 16, 'fill': white, 'font-family': 'Arial, sans-serif', 'font-weight': 'bold', 'text-anchor': 'start' };
    var gdx_attr = { 'font-size': 20, 'fill': '#909090', 'font-family': 'Arial Black, sans-serif', 'font-weight': 'bold' };
    paper.text(gdx1, gdy, redPlayer).attr(gdp1_attr);
    paper.text(gdx2, gdy, whitePlayer).attr(gdp2_attr);
    paper.text(gdxx, gdy, "X").attr(gdx_attr);
}


function drawBearOffPlace(){

    var bo_attr = {'stroke-width': 2, 'stroke': '#909090', 'fill': '#484848'};

    paper.rect(box, boy1, boside, boside, 5).attr(bo_attr);
    paper.rect(box, boy2, boside, boside, 5).attr(bo_attr);
}

function drawBorneOffPieces(nr, nw){

    var bor_attr = { 'font-size': 30, 'fill': red, 'font-family': 'Helvetica Neue", Helvetica, Arial, sans-serif', 'font-weight': 'bold' };
    var bow_attr = { 'font-size': 30, 'fill': white, 'font-family': 'Helvetica Neue", Helvetica, Arial, sans-serif', 'font-weight': 'bold' };
    var x = box + boside/2;
    var y1 = boy1 + boside/2;
    var y2 = boy2 + boside/2;

    elements.push(paper.text(x, y1, "+" + nr).attr(bor_attr));
    elements.push(paper.text(x, y2, "+" + nw).attr(bow_attr));
}

challenge_player.render_arena = function() {

    drawBoardBackground();
    drawAllTriangles();
    drawLeftBorder();
    drawRightBorder();
    drawTopBorder();
    drawBottomBorder();
    drawBoardCenter();
    drawBearOffPlace();
    drawGameDescription();

}


challenge_player.render_tick = function (tick) {
    var reds, whites, WM, bar, borne_off, dice, board;

    // Cleanup
    elements.forEach(function (e) {
        e.remove();
    });

    elements = [];

    WM = challenge_player.match_data.wmList[tick];
    bar = WM.bar;
    borne_off = WM.borne_off;
    dice = WM.dice;
    board = WM.board;

    for(row in board){
        whites = parseInt(board[row].whites);
        reds = parseInt(board[row].reds);
        drawPieces(row, reds, whites);
    }


    drawBarPieces(parseInt(bar.reds), parseInt(bar.whites));
    drawBorneOffPieces(borne_off.reds, borne_off.whites);

    for(i in dice){
        console.log(parseInt(dice[i]));
        drawDie(parseInt(dice[i]), i);
    }
}

