//$fn=200;
$fa=0.5;
$fs=0.1;
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
C_dia_in=1.7;
C_thickness=2;

render=1; //1 bottom, 2 middle (1 floor), 3 top, 0 all

E_dia=51.4;
E_height=8.5;

if(render==1){
	rotate(a=[180,0,0]){base();}
}
else if(render==2){
	rotate(a=[180,0,0]){rings(1,0);}
}
else if(render==3){
	rotate(a=[180,0,0]){
		rings(1,0);
		top_plate(R_height-1.5);
	}
}
else {
	rings(R_count,T_height-R_overlap);
	top_plate(T_height+R_count*(R_height-R_overlap)-1,0);
	base();
}




module base(){
	difference(){
		union(){
			difference(){//main tube
					cylinder(r=T_dia_in/2+T_thickness,h=T_height);
					cylinder(r=T_dia_in/2,h=T_height);
			}

			difference()
			{
				for(i=[0:1:C_count-1]){//columns
					rotate(a=[0,0,i*(360/C_count)]){
						translate(v=[T_dia_in/2,0,10]){
								cylinder(r=C_dia_in/2+C_thickness,h=T_height-10);
						}
					}
				}
				translate(v=[0,0,T_height-15-E_height]){
					cylinder(r=E_dia/2,h=15);
				}
				translate(v=[0,0,10])
				{
					difference(){
						cylinder(h=15,r=T_dia_in/2+T_thickness+5);
						cylinder(h=15,r1=T_dia_in/2+T_thickness,r2=T_dia_in/2+T_thickness+5);
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
}


module rings(count,height_offset){
	translate(v=[0,0,height_offset]){
		for(i=[0:1:count-1]){
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
}

module top_plate(height_offset){
	translate(v=[0,0,height_offset]){
		cylinder(h=1.5,r2=R_dia_in/2+R_thickness,r1=R_dia_in/2+2*R_thickness);
		for(i=[0:1:C_count-1]){//columns
			rotate(a=[0,0,i*(360/C_count)]){
				translate(v=[T_dia_in/2,0,0]){
						cylinder(r=C_dia_in/2+C_thickness,h=1.5);
				}
			}
		}
	}
}
