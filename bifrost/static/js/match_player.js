//
// Generic replay player. Each challenge must be have a javascript file that implements the following functions:
// TODO
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


var loading_circle = paper.circle(250, 200, 25).attr({ stroke: "#f1f2f1", "stroke-width": 10 });
var loading_circle_bar = paper.path().attr({ stroke: "#000000", "stroke-width": 10, arc: [250, 200, 0, 1, 25] });
var loading_text = paper.text(250, 250, 'Loading').attr({ 'font-size': 20, 'fill': "#d1d2d1", 'font-family': 'Helvetica Neue", Helvetica, Arial, sans-serif', 'font-weight': 'bold' });
var animRotation = Raphael.animation({ transform: 'r360,250,200' }, 2500).repeat(Infinity);
loading_circle_bar.animate(animRotation);


$.get("https://s3-us-west-2.amazonaws.com/mjollnir-matches/" + bifrost_mid, function (data) {
    replay_data = JSON.parse(data);

    loading_circle.remove();
    loading_circle_bar.remove();
    loading_text.remove();

})
