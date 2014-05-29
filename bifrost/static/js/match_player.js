//
// Generic replay player. Each challenge must have a javascript file that defines the object 'challenge_player',
//   implementing the following functions:
//
// - render_arena(): renders the arena on which the match is played (e.g. the grid on a tic-tac-toe match)
//                   this is called once right after the match json is loaded.
//
// - render_tick(tick): renders a specific tick (a world model) of a match (e.g. the Xs and 0s on a tic-tac-toe turn)
//                      Keep in mind that you might need to remove elements created in the previous tick.
//

var paper = Raphael("match-player", "100%", "100%");

paper.customAttributes.arc = function (centerX, centerY, startAngle, endAngle, arcEdge) {
    var radians = Math.PI / 180;
    largeArc = +(endAngle - startAngle > 180);
    // calculate the start and end points for both inner and outer edges of the arc segment
    // the -90s are about starting the angle measurement from the top get rid of these if this doesn't suit your needs
    outerX1 = centerX + arcEdge * Math.cos((startAngle - 90) * radians);
    outerY1 = centerY + arcEdge * Math.sin((startAngle - 90) * radians);
    outerX2 = centerX + arcEdge * Math.cos((endAngle - 90) * radians);
    outerY2 = centerY + arcEdge * Math.sin((endAngle - 90) * radians);

    // build the path array
    var path = [
      ["M", outerX1, outerY1], // move to the start point
      ["A", arcEdge, arcEdge, 0, largeArc, 1, outerX2, outerY2] // draw the outer edge of the arc
    ];
    return { path: path };
};



//TODO, extract magic numbers
var loading_circle = paper.circle(240, 200, 25).attr({ stroke: "#f1f2f1", "stroke-width": 10 });
var loading_circle_bar = paper.path().attr({ stroke: "#000000", "stroke-width": 10, arc: [240, 200, 0, 1, 25] });
var loading_text = paper.text(240, 250, 'Loading').attr({ 'font-size': 20, 'fill': "#d1d2d1", 'font-family': 'Helvetica Neue", Helvetica, Arial, sans-serif', 'font-weight': 'bold' });
var animRotation = Raphael.animation({ transform: 'r360,240,200' }, 2500).repeat(Infinity);
loading_circle_bar.animate(animRotation);



$.get("https://s3-us-west-2.amazonaws.com/mjollnir-matches/" + bifrost_mid, function (data) {
    // Setup replay data
    challenge_player.match_data = JSON.parse(data);

    challenge_player.current_tick = 0;
    challenge_player.total_ticks = challenge_player.match_data.wmList.length;

    // Setup replay control functions
    challenge_player.is_paused = true;

    challenge_player.play = function () {
        challenge_player.is_paused = false;
    }

    challenge_player.pause = function () {
        challenge_player.is_paused = true;
    }

    challenge_player.next_tick = function () {
        if (challenge_player.current_tick + 1 < challenge_player.total_ticks) {
            challenge_player.render_tick(++challenge_player.current_tick);
        }
    }

    challenge_player.previous_tick = function () {
        if (challenge_player.current_tick > 0) {
            challenge_player.render_tick(--challenge_player.current_tick);
        }
    }
    
    setInterval(function () {
        if (!challenge_player.is_paused) {
            challenge_player.next_tick();
        }
    }, 1000);

    // Remove the loading info
    loading_circle.remove();
    loading_circle_bar.remove();
    loading_text.remove();

    // Initial draw logic
    challenge_player.render_arena();
    challenge_player.render_tick(challenge_player.current_tick);

    // The other ticks are rendered using the buttons on match.html
})
