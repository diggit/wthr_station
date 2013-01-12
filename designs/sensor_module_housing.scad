//$fn=200;
T_dia_in=54;
T_thickness=1;
T_height=30;

R_dia_in=T_dia_in;
R_thickness=1;
R_dia_increase=10;
R_height=15;
R_count=5;
R_overlap=4;
R_base=4;

C_count=6;
C_dia_in=1.4;
C_thickness=2;




//translate(v=[0,0,1]){
difference(){
	union(){
		difference(){//main tube
				cylinder(r=T_dia_in/2+T_thickness,h=T_height);
				cylinder(r=T_dia_in/2,h=T_height);
		}


		for(i=[0:1:C_count-1]){//columns
			rotate(a=[0,0,i*(360/C_count)]){
				translate(v=[T_dia_in/2,0,T_height-10]){
						cylinder(r=C_dia_in/2+C_thickness,h=R_height);
				}
			}
		}

		translate(v=[0,0,T_height-1]){//ring for safe printing
			difference(){
				cylinder(h=1,r=T_dia_in/2+R_thickness);
				cylinder(h=1,r=T_dia_in/2+R_thickness-R_base);
			}
		}
	}
	for(i=[0:1:C_count-1]){
		rotate(a=[0,0,i*(360/C_count)]){
			translate(v=[T_dia_in/2,0,T_height-10+1]){
					cylinder(r=C_dia_in/2,h=R_height);
			}
		}
	}
	
}

translate(v=[0,0,T_height-R_overlap]){
for(i=[0:1:R_count-1]){
	translate(v=[0,0,(R_height-R_overlap)*i]){
		difference(){
			union(){//union of ring and columns
				difference(){
					cylinder(h=R_height,r2=R_dia_in/2+R_thickness,r1=R_dia_increase+R_dia_in/2+R_thickness);
					cylinder(h=R_height,r2=R_dia_in/2,r1=R_dia_increase+R_dia_in/2);
				}
				translate(v=[0,0,R_height-1]){//ring for safe printing
					difference(){
						cylinder(h=1,r=R_dia_in/2+R_thickness);
						cylinder(h=1,r=R_dia_in/2+R_thickness-R_base);
					}
				}
				for(i=[0:1:C_count-1]){//columns
					rotate(a=[0,0,i*(360/C_count)]){
						translate(v=[T_dia_in/2,0,R_overlap]){
								cylinder(r=C_dia_in/2+C_thickness,h=R_height-R_overlap);
						}
					}
				}
			}//cut holes in columns
			for(i=[0:1:C_count-1]){
				rotate(a=[0,0,i*(360/C_count)]){
					translate(v=[T_dia_in/2,0,R_overlap]){
							cylinder(r=C_dia_in/2,h=R_height-R_overlap);
					}
				}
			}
		}
	}
}
}
translate(v=[0,0,T_height+R_count*(R_height-R_overlap)-1]){
	cylinder(h=1,r=R_dia_in/2+R_thickness);
	for(i=[0:1:C_count-1]){//columns
		rotate(a=[0,0,i*(360/C_count)]){
			translate(v=[T_dia_in/2,0,0]){
					cylinder(r=C_dia_in/2+C_thickness,h=1);
			}
		}
	}
}

