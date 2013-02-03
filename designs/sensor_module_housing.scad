//$fn=70;
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

render=0; //1 bottom, 2 middle (1 floor), 3 top, 4 rod mount, 0 all

E_dia=51.4;
E_height=8.5;

M_in_height=10;
M_in_thickness=1;
M_gap=0.3;
M_height=10;
M_cutoff_dia=200;
M_cutoff_angle=30;

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
else if(render==4)
{
	rod_mount(10);
}
else {
	rings(R_count,T_height-R_overlap);
	top_plate(T_height+R_count*(R_height-R_overlap)-1,0);
	base();
	rod_mount(0);
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

module rod_mount(height_offset)
{
	color("aqua"){
		translate(v=[0,0,height_offset])
		{
			difference()
			{
				cylinder(h=M_in_height,r=T_dia_in/2-M_gap);
				cylinder(h=M_in_height+1,r=T_dia_in/2-M_gap-M_in_thickness);
			}
			difference()
			{
				translate(v=[0,0,-10])
				{
					difference()
					{
						cylinder(h=M_height,r=T_dia_in/2+T_thickness);
						translate(v=[0,0,2]){cylinder(h=M_height-2,r=T_dia_in/2-M_gap-M_in_thickness);}
					}
					difference()
					{
						translate(v=[0,-T_dia_in/2,0]){cube([cos(90-M_cutoff_angle)*(T_dia_in/2+T_thickness+M_cutoff_dia/2),T_dia_in,M_height]);}
						for(i=[0,1])
						mirror([0,i,0])
						{	
							rotate([0,0,-M_cutoff_angle])
							{
								translate(v=[-20,0,0]){cube([20,70,M_height]);}
								translate(v=[0,T_dia_in/2+T_thickness+M_cutoff_dia/2,0])
								{
								
									cylinder(h=M_height,r=M_cutoff_dia/2);
								
								}
							}
						}
						cylinder(h=M_in_height+1,r=T_dia_in/2-M_gap-M_in_thickness);
						//translate(v=[cos(90-M_cutoff_angle)*(T_dia_in/2+T_thickness+M_cutoff_dia/2),-T_dia_in/2,0])
						//{//the end of mount
						//	cube([100,T_dia_in,M_height]);
							
						//}
					}
					translate(v=[cos(90-M_cutoff_angle)*(T_dia_in/2+T_thickness+M_cutoff_dia/2)-1,0,0])
					{//the end of mount
						difference()
						{
							union()
							{
								difference()
								{
									for(i=[0,1]) mirror([0,i,0]) {cube([13,5+sin(90-M_cutoff_angle)*(T_dia_in/2+T_thickness+M_cutoff_dia/2)-M_cutoff_dia/2,M_height*2+2]);}
									translate(v=[16,0,0])
									{
										difference()
										{	
											cylinder(h=22,r=30);	
											cylinder(h=22,r=19);	
										}
									}
								}
								translate(v=[-10,0,10])
								{
									cylinder(h=12,r=5);
								}
								translate(v=[-10,-5,20])
								{
									cube([15,10,2]);
								}
							}
							translate(v=[6,0,0])
								for(i=[20,-20])
								mirror([0,i,0])
								rotate([0,0,-45+i])
								{
									cube([17,17,30]);
								}
						}
			
						
					}
					//because rounded bridges are problem...
					intersection()
					{
						translate(v=[T_dia_in/2-3.5,-21,0])
						{
							cube([10,40,20]);
						}
						cylinder(h=20,r=T_dia_in/2-M_gap);
					}
				}
				translate([0,-10,-4])
				{
					rotate([0,30,0])
					{
						cube([60,20,15]);
					}
				}
				
			}
			%translate(v=[-10,0,-1])//connectors, main and wind(combined speed and dir)
			{
				translate(v=[-5,10,-8])
				{
					RJ4pMount();
				}
				translate(v=[0,-11,-8])//RJ45 mount for wind sensors
				{
					difference()
					{
						translate(v=[0,-10,1]){cube([15,20,5]);}
						#translate(v=[0,-7,1]){cube([15,15.8,5]);}
					}
				}
			}

		}
		
		
	}
	
	
}

module RJ4p()
{
	color("red"){
		translate(v=[3,-11.8/2,0])
		{
			cube([17,11.8,15]);
			
		}
		translate(v=[15.5,-14.5/2,0])
		{
			cube([1.5,14.5,15]);
		}
	}
}

module RJ4pMount()
{
	difference()
	{
		translate(v=[5,-10,1]){cube([15,20,5]);}
		RJ4p();
	}
}

