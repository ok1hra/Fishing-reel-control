$fn=50;


%translate([0,0,10]) rotate([20,0,0]) translate([76/2,-64/2,23]) rotate([0,180,0]) import("Ali-FM.stl", convexity=3);

//            (xx,  yy, zz, rr, rrr, wall, screw){
// MountBoxFlat(77.5, 68, 20, 6,  7,   3,     2.7);

difference(){
    hull(){
        translate([0,0,10]) rotate([20,0,0]) OutTOP(77.5, 68, 20, 6,  7,   3,   0,  0.1);
        OutBOT(77.5, 68, 20, 6,  7,   3,  0, 0.1);
    }
    translate([0,0,10]) rotate([20,0,0]) InTOP(77.5, 68, 20, 6,  7,   3,     2.7);
    InFill(77.5, 68, 20, 6,  7,   3,     2.7);
    InBOT(77.5, 68, 20, 6,  7,   3,     2.7);
}



// 76x64mm

// CUT - F6 - Exportovat jako DXF
//projection(cut = true) translate([0,0,-22]) MountBoxBot(77.5, 68, 20, 6,  7,   3,  2.7);

module InFill(xx, yy, zz, rr, rrr, wall, screw, zzz){
    translate([0,0,5]) rotate([20,0,0])
    translate([0,13.5,20]) cube([77,33,17], center=true);
    hull(){
        translate([0,0,10]) rotate([20,0,0]) hull(){

            translate([-xx/2+2.3*rr, -yy/2+2.3*rr, 20]) cylinder(h=5, r=2*rr, center=false);
            translate([xx/2-2.3*rr, -yy/2+2.3*rr, 20]) cylinder(h=5, r=2*rr, center=false);
            translate([-xx/2+2.3*rr, yy/2-2.3*rr, 20]) cylinder(h=5, r=2*rr, center=false);
            translate([xx/2-2.3*rr, yy/2-2.3*rr, 20]) cylinder(h=5, r=2*rr, center=false);
        }
        translate([-xx/2+rr, -yy/2+rr, wall]) cylinder(h=0.1, r=rrr-wall-1.5, center=false);
        translate([xx/2-rr, -yy/2+rr, wall]) cylinder(h=0.1, r=rrr-wall-1.5, center=false);
        translate([-xx/2+rr, yy/2-rr, wall]) cylinder(h=0.1, r=rrr-wall, center=false);
        translate([xx/2-rr, yy/2-rr, wall]) cylinder(h=0.1, r=rrr-wall, center=false);
    }
}

module InTOP(xx, yy, zz, rr, rrr, wall, screw, zzz){
        // SCREW IN
        translate([-76/2+1, -64/2+1, -10]) cylinder(h=zz+20, d=screw, center=false);
        translate([76/2-1, -64/2+1, -10]) cylinder(h=zz+20, d=screw, center=false);
        translate([-76/2+1, 64/2-1, -20]) cylinder(h=zz+30, d=screw, center=false);
        translate([76/2-1, 64/2-1, -20]) cylinder(h=zz+30, d=screw, center=false);
        //PCB
        translate([-76/2, -64/2, zz+rrr-5.8]) PCB();
        // Top Cut
        translate([0, 0, zz+rrr-4]) Plexi(82.4, 70.4, 2, 5);
        translate([-xx, -yy, zz+rrr-2.1]) cube([xx*2, yy*2, zz]);
        translate([0, 0, zz+rrr-4-2]) Silicone(82.4, 70.4, 4, 5);
}
module OutTOP(xx, yy, zz, rr, rrr, wall, screw, zzz){
   hull(){
            translate([-76/2+1, -64/2+1, zz]) sphere(r=rrr, center=false);
            translate([76/2-1, -64/2+1, zz]) sphere(r=rrr, center=false);
            translate([-76/2+1, 64/2-1, zz]) sphere(r=rrr, center=false);
            translate([76/2-1, 64/2-1, zz]) sphere(r=rrr, center=false);
    }
}


module InBOT(xx, yy, zz, rr, rrr, wall, screw, zzz){
//    difference(){
//        union(){
//            // OUT BOT
//            hull(){
//                translate([-xx/2+rr, -yy/2+rr, 0]) cylinder(h=wall, r=rr, center=false);
//                translate([xx/2-rr, -yy/2+rr, 0]) cylinder(h=wall, r=rr, center=false);
//                translate([-xx/2+rr, yy/2-rr, 0]) cylinder(h=wall, r=rr, center=false);
//                translate([xx/2-rr, yy/2-rr, 0]) cylinder(h=wall, r=rr, center=false);
//            }
//        }
        // RING
        difference(){
            translate([0,0,-1]) cylinder(h=3.2, d=54, center=false);
            translate([0,0,-1]) cylinder(h=3.2, d=48, center=false);
        }
        // CENTER HOLE
        translate([0,0,-1]) cylinder(h=20, d=18.5, center=false);
        // MOUNT HOLE
        for(i = [90:120:360]){
            rotate([0,0,i]) translate([21,0,-1]) cylinder(h=20, d=4, center=false);
        }
//    }
}

module OutBOT(xx, yy, zz, rr, rrr, wall, screw, zzz){
    hull(){
        translate([-xx/2+rr, -yy/2+rr, 0]) cylinder(h=zzz, r=rrr, center=false);
        translate([xx/2-rr, -yy/2+rr, 0]) cylinder(h=zzz, r=rrr, center=false);
        translate([-xx/2+rr, yy/2-rr, 0]) cylinder(h=zzz, r=rrr, center=false);
        translate([xx/2-rr, yy/2-rr, 0]) cylinder(h=zzz, r=rrr, center=false);
    }
}
module MountBoxFlat(xx, yy, zz, rr, rrr, wall, screw, zzz){
    difference(){
        union(){
            difference(){
                union(){
                    // OUT BOT
                    hull(){
                        translate([-xx/2+rr, -yy/2+rr, 0]) cylinder(h=1, r=rr, center=false);
                            translate([-76/2+1, -64/2+1, zz]) sphere(r=rrr, center=false);
                        translate([xx/2-rr, -yy/2+rr, 0]) cylinder(h=1, r=rr, center=false);
                            translate([76/2-1, -64/2+1, zz]) sphere(r=rrr, center=false);
                        translate([-xx/2+rr, yy/2-rr, 0]) cylinder(h=1, r=rr, center=false);
                            translate([-76/2+1, 64/2-1, zz]) sphere(r=rrr, center=false);
                        translate([xx/2-rr, yy/2-rr, 0]) cylinder(h=1, r=rr, center=false);
                            translate([76/2-1, 64/2-1, zz]) sphere(r=rrr, center=false);
                    }
                }
                // IN
                hull(){
                    translate([-xx/2+2.3*rr, -yy/2+2.3*rr, wall]) cylinder(h=zz+20, r=2*rr, center=false);
                    translate([xx/2-2.3*rr, -yy/2+2.3*rr, wall]) cylinder(h=zz+20, r=2*rr, center=false);
                    translate([-xx/2+2.3*rr, yy/2-2.3*rr, wall]) cylinder(h=zz+20, r=2*rr, center=false);
                    translate([xx/2-2.3*rr, yy/2-2.3*rr, wall]) cylinder(h=zz+20, r=2*rr, center=false);
                }
                // RING
                difference(){
                    translate([0,0,-1]) cylinder(h=3.2, d=54, center=false);
                    translate([0,0,-1]) cylinder(h=3.2, d=48, center=false);
                }
                // CENTER HOLE
                translate([0,0,-1]) cylinder(h=wall+2, d=18.5, center=false);
                // MOUNT HOLE
                for(i = [90:120:360]){
                    rotate([0,0,i]) translate([21,0,-1]) cylinder(h=wall+2, d=4, center=false);
                }
            }
        }
        // SCREW IN
        translate([-76/2+1, -64/2+1, -1]) cylinder(h=zz+20, d=screw, center=false);
        translate([76/2-1, -64/2+1, -1]) cylinder(h=zz+20, d=screw, center=false);
        translate([-76/2+1, 64/2-1, -1]) cylinder(h=zz+20, d=screw, center=false);
        translate([76/2-1, 64/2-1, -1]) cylinder(h=zz+20, d=screw, center=false);
        //PCB
        translate([-76/2, -64/2, zz+rrr-5.8]) PCB();
        // Top Cut
        translate([0, 0, zz+rrr-4]) Plexi(82.4, 70.4, 2, 5);
        translate([-xx, -yy, zz+rrr-2.1]) cube([xx*2, yy*2, zz]);
        translate([0, 0, zz+rrr-4-2]) Silicone(82.4, 70.4, 4, 5);

//        translate([-xx, -yy, -1]) cube([xx*2, yy*2, 10]);

        
    }    
}



module Silicone(xx, yy, zz, rr){
    difference(){
        hull(){
            translate([-xx/2+rr, -yy/2+rr, 0]) cylinder(h=zz, r=rr, center=false);
            translate([xx/2-rr, -yy/2+rr, 0]) cylinder(h=zz, r=rr, center=false);
            translate([-xx/2+rr, yy/2-rr, 0]) cylinder(h=zz, r=rr, center=false);
            translate([xx/2-rr, yy/2-rr, 0]) cylinder(h=zz, r=rr, center=false);
        }    
        hull(){
            translate([-xx/2+rr, -yy/2+rr, -1]) cylinder(h=zz+2, r=rr-1, center=false);
            translate([xx/2-rr, -yy/2+rr, -1]) cylinder(h=zz+2, r=rr-1, center=false);
            translate([-xx/2+rr, yy/2-rr, -1]) cylinder(h=zz+2, r=rr-1, center=false);
            translate([xx/2-rr, yy/2-rr, -1]) cylinder(h=zz+2, r=rr-1, center=false);
        }    
    }
}



module Plexi(xx, yy, zz, rr){
    hull(){
        translate([-xx/2+rr, -yy/2+rr, 0]) cylinder(h=zz, r=rr, center=false);
        translate([xx/2-rr, -yy/2+rr, 0]) cylinder(h=zz, r=rr, center=false);
        translate([-xx/2+rr, yy/2-rr, 0]) cylinder(h=zz, r=rr, center=false);
        translate([xx/2-rr, yy/2-rr, 0]) cylinder(h=zz, r=rr, center=false);
    }    
}

module PCB(){
        difference(){
        translate([0,0,0]) cube([76.4, 64.4, 2]);
        translate([0,0,-1]) cylinder(h=4, r=4.8, center=false);
        translate([76,0,-1]) cylinder(h=4, r=4.8, center=false);
        translate([76,64,-1]) cylinder(h=4, r=4.8, center=false);
        translate([0,64,-1]) cylinder(h=4, r=4.8, center=false);
    }
}


module MountBox(xx, yy, zz, rr, wall, xxx, yyy, zzz, wallt){
    difference(){
        union(){
            // SCREW OUT
            translate([-xx/2+rr-xxx+xx, -yy/2+rr, zz]) cylinder(h=zzz, r=rr, center=false);
            translate([xx/2-rr, -yy/2+rr, 0]) cylinder(h=zzz+zz, r=rr, center=false);
            translate([-xx/2+rr-xxx+xx, yy/2-rr+yyy-yy, zz]) cylinder(h=zzz, r=rr, center=false);
            translate([xx/2-rr, yy/2-rr+yyy-yy, zz]) cylinder(h=zzz, r=rr, center=false);
            difference(){
                union(){
                    // OUT BOT
                    hull(){
                        translate([-xx/2+rr, -yy/2+rr, 0]) cylinder(h=zz, r=rr, center=false);
                        translate([xx/2-rr, -yy/2+rr, 0]) cylinder(h=zz, r=rr, center=false);
                        translate([-xx/2+rr, yy/2-rr, 0]) cylinder(h=zz, r=rr, center=false);
                        translate([xx/2-rr, yy/2-rr, 0]) cylinder(h=zz, r=rr, center=false);
                    }
                    // OUT TOP
                    hull(){
                        translate([-xx/2+rr-xxx+xx, -yy/2+rr, zz]) cylinder(h=zzz, r=rr, center=false);
                        translate([xx/2-rr, -yy/2+rr, zz]) cylinder(h=zzz, r=rr, center=false);
                        translate([-xx/2+rr-xxx+xx, yy/2-rr+yyy-yy, zz]) cylinder(h=zzz, r=rr, center=false);
                        translate([xx/2-rr, yy/2-rr+yyy-yy, zz]) cylinder(h=zzz, r=rr, center=false);
                    }
                }
                // IN
                hull(){
                    translate([-xx/2+rr, -yy/2+rr, wall]) cylinder(h=zz+zzz, r=rr-wall, center=false);
                    translate([xx/2-rr, -yy/2+rr, wall]) cylinder(h=zz+zzz, r=rr-wall, center=false);
                    translate([-xx/2+rr, yy/2-rr, wall]) cylinder(h=zz+zzz, r=rr-wall, center=false);
                    translate([xx/2-rr, yy/2-rr, wall]) cylinder(h=zz+zzz, r=rr-wall, center=false);
                }
                // IN TOP
                hull(){
                    translate([-xx/2+rr-xxx+xx+wallt, -yy/2+rr+wallt, zz+wall]) cylinder(h=zzz, r=rr, center=false);
                    translate([xx/2-rr-wallt, -yy/2+rr+wallt, zz+wall]) cylinder(h=zz+zzz, r=rr, center=false);
                    translate([-xx/2+rr-xxx+xx+wallt, yy/2-rr+yyy-yy-wallt, zz+wall]) cylinder(h=zzz, r=rr, center=false);
                    translate([xx/2-rr-wallt, yy/2-rr+yyy-yy-wallt, zz+wall]) cylinder(h=zzz, r=rr, center=false);
                }
                // RING
                difference(){
                    translate([0,0,-1]) cylinder(h=3.2, d=54, center=false);
                    translate([0,0,-1]) cylinder(h=3.2, d=48, center=false);
                }
                // CENTER HOLE
                translate([0,0,-1]) cylinder(h=wall+2, d=18.5, center=false);
                // MOUNT HOLE
                for(i = [90:120:360]){
                    rotate([0,0,i]) translate([21,0,-1]) cylinder(h=wall+2, d=4, center=false);
                }
            }
        }
        // SCREW IN
        translate([-xx/2+rr-xxx+xx, -yy/2+rr, zz-1]) cylinder(h=zzz+2, d=4.3, center=false);
        translate([xx/2-rr, -yy/2+rr, -1]) cylinder(h=zzz+zz+2, d=4.3, center=false);
        translate([-xx/2+rr-xxx+xx, yy/2-rr+yyy-yy, zz-1]) cylinder(h=zzz+2, d=4.3, center=false);
        translate([xx/2-rr, yy/2-rr+yyy-yy, zz-1]) cylinder(h=zzz+2, d=4.3, center=false);
        //PCB
%        translate([-43.25+1, -28-1, 30-1.8]) PCB();
        
    }    
}