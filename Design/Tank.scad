border= 3;
width= 55 + (2 * border);
first_screw_distance=20 - 12;
support_width = width - (border * 2);
module support(length) {
  translate([border,0,border]) {
    cube([support_width,length,border]);
  };
}

module base(length) {
  union(){
    cube([width,length,border]);
    support(length);
  };
}
module battery_pack() {
  b_length = 57;
  union(){
     for (i=[0:3])
       translate([i*(b_length)/4,0,0])
         cylinder(b_length, 8, 8, true);
    }
 }
// battery_pack();
translate([0, -70]){
    difference() {
      angle = 35;
      translation = [0, -14, sin(angle) * 19];
      union() {
        base(70);
        difference() {
          translate(translation) {
            rotate([-angle, 0, 0]) {
              base(19);
            }
          }
          # echo(sin(40) * 19);
          translate(translation + [-1, 0, 2.1]) {
              scale(1.1) {
                cube([width, 10, 10]);
              }

              translate([border + 0.9 , -border + 5.3, 0.0])
                    rotate([-angle, 0, 0])
                        cube([support_width + 0.2, border/2 + .7, border/2]);
          }
        }
        translate(translation + [0, 2.2, border + 2]) {
            rotate([-90, 0, 0]) {
              scale([1,1,1/sin(angle) * .5]) {
                  difference() {
                    support(border);
                    rotate([40, 0, 0]) {
                      translate([-1, 2, 0.5]) {
                        scale(1.1) {
                          support(border);
                         }
                      }
                    }
                  }

              }
            }
        };
      }

      for (i=[0:2]) translate([width/2, first_screw_distance + (i*16), -5]){
        cylinder(100, 2);
        if(i == 2) {
            translate([20 - 4, 10, 4]) {
                cylinder(110, 3.6);
                translate([-5, -5, 4.5]) cube([10, 10, 10]);
            }
        }
      }
    }
    *translate([width/2, -9, -8])
        rotate([90, 0, 90])
            battery_pack();
}

function sum(v,i) = (i==len(v) ? 0 : v[i] + sum(v,i+1));

s_0 = 70;
s_1 = 6.5;
s_2 = 16;
s_3 = 11;
s_4 = 10.5;
difference() {
  union() {
    base(s_0);
    translate([0,s_0,0]) {
      base(s_1);
      cube([width, s_1, s_1 + 2]);
      translate([0, s_1]) base(s_2);
      translate([0, s_1 + s_2]) rotate([45,0,0]) base(s_3);
      translate([0, s_1 + s_2 + s_3 * cos(45), s_3 * cos(45)])
      rotate([90, 0, 0]) base(s_4);
    }
  }
  motor_w = 17.5;
  motor_l = 20.0;
  translate([0,s_0,0]) {
    translate([width - 17.5,s_1 + s_2, 1 + s_3 * cos(45)]) cube([10,10,10]);
    translate([8, -0.1, 3.1]) cube([motor_w, s_1 + 0.2,motor_w]);
    translate([width - 8 - motor_w, -0.1, 3.1]) cube([motor_w,s_1 + 0.2,motor_w]);

    translate([8, -motor_l, -0.1]) cube([motor_w, motor_l + 0.2,motor_w]);
    translate([width - 8 - motor_w, -motor_l, -0.1]) cube([motor_w, motor_l + 0.2,motor_w]);
    hole_l = 34;
    hole_w = 50;
    translate([width - hole_w - 2.9, -hole_l - motor_l + 0.1, -0.1]) cube([hole_w + 0.5, hole_l + 0.2,motor_w]);
    screw_1_r = 2.8;
    translate([width-12-screw_1_r, -56, -0.1]) cylinder(110,screw_1_r, screw_1_r, true);
    screw_2_r = 2.7;
    translate([width - 40 - screw_2_r, -58, -0.1]) cylinder(20,screw_2_r, screw_2_r, true);
  }
}
