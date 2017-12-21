l = 30;
b = 5;
h = 10;
translate([-b,-b]) %cube([30,30,h]);

difference() {
     minkowski() {
	  cube([30 - (b*2),30-(b*2),h]);
	  cylinder(h=0.1, r=5);
     }
     h_x = 6.7;
     h_y = 7.8;
     h_z = 7.5;
     translate([h_x,h_y, h_z - (h/2) + 0.2]) cube([h_x, h_y, h_z]);
     translate([l/2 - b, 100/2, h - (2 * 2)]) rotate([90,90,0]) cylinder(h=100, r=2);
}
