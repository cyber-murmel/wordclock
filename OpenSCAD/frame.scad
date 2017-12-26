$fn = 180;

text_english = [
  "ITEISFTLVNE",
  "JQUARTERCKO",
  "TWENTYXFIVE",
  "HALFCTENETO",
  "PASTBSEVENL",
  "ONETWOTHREE",
  "FOURFIVESIX",
  "NINEKTWELVE",
  "EIGHTELEVEN",
  "TENDBOCLOCK",
];

text_german = [
  "ESKISTAFÜNF",
  "ZEHNZWANZIG",
  "DREIVIERTEL",
  "VORFUNKNACH",
  "HALBAELFÜNF",
  "EINSXAMZWEI",
  "DREIAUJVIER",
  "SECHSNLACHT",
  "SIEBENZWÖLF",
  "ZEHNEUNKUHR",
];

face_text = text_english;

t_mat = 3;
t_back = 2.5;

w_back = 232;
w_standoff = 217;
d_standoff = 26;

spacing = 1000/60;
nx_LEDs = 11;
ny_LEDs = 10;
y_start = (ny_LEDs)*spacing/2;
x_start = (nx_LEDs)*spacing/2;
ys = [-y_start: spacing: y_start+1];
xs = [-x_start: spacing: x_start+1];

w_pin = 5;
x_pin = -x_start-spacing/2;
y_pin = -y_start-spacing/2;

w_cable = 5;

module back() {
  difference() {
    square([1, 1]*w_back, center = true);
    // slots for horizontal pieces
    for(y = ys)
      for(x = [-x_pin, x_pin])
        translate([x, y])
          square([w_pin, t_mat], center=true);
    //  slots for vertial pieces
    for(x = xs)
      for(y = [-y_pin, y_pin])
        translate([x, y])
          square([t_mat, w_pin], center=true);
    translate([-0.5, 0.5]*w_back + [1, -1]*t_mat*1.5)
      text("top left", size=w_back/30, font="Stencilia\\-A:style=Regular", halign="left", valign="top");  
    translate(-[x_start, y_start]+spacing*[0.5, -0.5])
      square([w_cable, w_cable], center=true);
  }
}

module hori() {
  difference() {
    square([w_standoff, d_standoff], center = true);
    // slots for intersection
    for(x = xs)
      translate([x, d_standoff/2])
        square([t_mat, d_standoff], center=true);
    // slots for cables 
    for(inset = [-x_start+w_cable, x_start-w_cable])
      translate([inset, -d_standoff/2])
        square([w_cable, w_cable*2], center=true);
  }
  // pins
  for(inset = [x_pin, -x_pin])
    translate([inset, -d_standoff/2])
      square([w_pin, t_back*2], center=true);
}

module verti() {
  difference() {
    square([w_standoff, d_standoff], center = true);
    // slots for intersection
    for(x = ys)
      translate([x, -d_standoff/2])
        square([t_mat, d_standoff], center=true);
  }
  // pins
  for(inset = [y_pin, -y_pin])
    translate([inset, -d_standoff/2])
      square([w_pin, t_back*2], center=true);
}

module face() {
  difference() {
    square([1, 1]*w_back, center=true);
    for(y = [0: ny_LEDs-1])
      for(x = [0: nx_LEDs-1])
        translate(-[x_start, -y_start]+[-(x+0.5), y+0.5]*-spacing)
          text(face_text[y][x], size=spacing-2*t_mat, font="Stencilia\\-A:style=Regular", halign="center", valign="center");
  }
}

module upper_face() {
  difference() {
    face();
    translate([0, -0.5]*w_back)
      square([1, 1]*w_back, center=true);
  }
}

module lower_face() {
  difference() {
    face();
    translate([0, 0.5]*w_back)
      square([1, 1]*w_back, center=true);
  }
}

//mirror()back();
//face();
upper_face();
//lower_face();
//hori();
//verti();

//import("/tmp/wordclock-Edge.Cuts.dxf");
