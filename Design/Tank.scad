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
translate([-width/2, -80/2,0]){
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
      
      for (i=[0:2]) translate([width/2, first_screw_distance + (i*14), -5]){
        cylinder(100, 2);
        if(i == 2) {
            echo("here");
            translate([20 - 6, 10 + 5, 4]) {
                cylinder(110, 4);
                translate([-5, -5, 4.5]) cube([10, 10, 10]);
            }
            junction_size = 40;
            translate([-junction_size/2, width - 36.9, 0]) cube([junction_size,10,20]);
        } 
      }
    }
    *translate([width/2, -9, -8])
        rotate([90, 0, 90])
            battery_pack();
}

