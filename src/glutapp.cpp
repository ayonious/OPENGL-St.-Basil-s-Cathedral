/*
example:
1.roating
2.scaling
3.translate
*/


#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<stdlib.h>
#include<iostream>
#include<vector>

#if defined __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

using namespace std;

#define BLACK 0, 0, 0
#define asdf exit(0);

#define PI 2*acos(0.0)
//make a global variable -- for tracking the anglular position of camera
double cameraAngle;			//in radian
double cameraAngleDelta;

double cameraHeight;	
double cameraRadius;
double cameraX,cameraY,cameraZ;
double lookX,lookY,lookZ;
int birdy;
int rotate_YES;

double rectAngle;	//in degree

bool canDrawGrid;
GLUquadric *quad;

int debug;

struct PT
{
	double x,y,z;
	PT()
	{
		x=0;
		y=0;
		z=0;
	}
	PT(double X,double Y,double Z)
	{
		x=X;
		y=Y;
		z=Z;
	}
};





double GLO_COL[3];
void COL(double R,double G,double B)
{
	GLO_COL[0]=R;
	GLO_COL[1]=G;
	GLO_COL[2]=B;

	glColor3f(R,G, B);
}



void swap(int &a,int &b)
{
	int t=a;
	a=b;
	b=t;
}





void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
{
	
	glPushMatrix ();
	{
		glTranslatef (X, Y, Z);
		//glRotatef(30, 1.0f, 0.0f, 0.0f);
		gluCylinder(quad, R_high,R_low, Height, slice, 1);//	quad,base,top,height,slices,stacks
	}
	glPopMatrix ();
}


void line_black(double x1,double y1,double z1,double x2,double y2,double z2,double w)
{
	double a,b,c;
	a=GLO_COL[0];
	b=GLO_COL[1];
	c=GLO_COL[2];
	
	COL(0,0,0);
	glPushMatrix ();
	{
		glLineWidth(w); 
		glBegin(GL_LINES);
		glVertex3f(x1, y1, z1);
		glVertex3f(x2, y2, z2);
		glEnd();
	}
	glPopMatrix ();
	
	COL(a,b,c);	
}

void Cylinder_Cage(double X,double Y,double Z,double R_low,double R_high,double Height,double slice)
{
	vector<PT> P1,P2;
	int i;
	glPushMatrix ();
	{
		glTranslatef (X, Y, Z);
		
		double ang=360.0/slice;
		

		for(i=0;i<slice;i++)
		{
			glPushMatrix ();
			{
				P1.push_back(PT(R_low*cos((ang*i)*(PI/180)),R_low*sin(ang*i*(PI/180)),0));
				P2.push_back(PT(R_high*cos(ang*i*(PI/180)),R_high*sin(ang*i*(PI/180)),Height));
			}
			glPopMatrix ();
		}
		

		for(i=0;i<P1.size();i++)
		{
			line_black(P1[i].x,P1[i].y,P1[i].z,P2[i].x,P2[i].y,P2[i].z,.8);			
		}

	}
	glPopMatrix ();
}




void Solid_Cylinder(double X,double Y,double Z,double R_low,double R_high,double Height,int slice)
{	
	
	glPushMatrix ();
	{
		glTranslatef (X, Y, Z);
		
		gluCylinder (quad, R_high,R_low, Height, slice, 1);
		gluCylinder (quad, 0,R_low, 0, slice, 1);


		glPushMatrix ();
		{
			glTranslatef (0, 0, Height);
			gluCylinder (quad, 0,R_low, 0, slice, 1);//	quad,base,top,height,slices,stacks
		}
		glPopMatrix ();
		
		
	}
	glPopMatrix ();
}





void Solid_Cylinder2(double X,double Y,double Z,double R_out,double R_in,double out_Height,double in_Height,int slice)
{	
	double db;

	glPushMatrix ();
	{
		glTranslatef (X, Y, Z);
		

		gluCylinder (quad, R_out,R_out, out_Height, slice, 1);//	quad,base,top,height,slices,stacks
		gluCylinder (quad, R_in,R_in, in_Height, slice, 1);//	quad,base,top,height,slices,stacks
		gluCylinder (quad, R_in,R_out, 0, slice, 1);//	quad,base,top,height,slices,stacks	


		glPushMatrix ();
		{
			glTranslatef (0, 0, in_Height);

			db=GLO_COL[0];
			COL(GLO_COL[0]+db*.12,GLO_COL[1],GLO_COL[2]);
			

			gluCylinder (quad, R_in,R_out, out_Height-in_Height, slice, 1);//	quad,base,top,height,slices,stacks	
			

			COL(GLO_COL[0]-db*.12,GLO_COL[1],GLO_COL[2]);
			
		}
		glPopMatrix ();	
	}
	glPopMatrix ();
}







void plane(PT p1,PT p2,PT p3,PT p4)
{
	glPushMatrix();
	{	glBegin(GL_QUADS);
		{
			glVertex3f(p1.x,p1.y,p1.z);
			glVertex3f(p2.x,p2.y,p2.z);
			glVertex3f(p3.x,p3.y,p3.z);
			glVertex3f(p4.x,p4.y,p4.z);
		}glEnd();
	}glPopMatrix();		//the effect of rotation is not there now.
}




PT glo_P_house[4];
void house(int type,double dz)
{
	int i;
	PT P[4];
	for(i=0;i<4;i++)
	{
		P[i]=glo_P_house[i];
		
		if(type==0) P[i].z=0;
		else
		{
			P[i].z+=dz;
		}	
	}

	if(type==1)	COL(.2, .3, .3);//dark green
	else if(type==2) COL(.2, .3, .35);//dark green2


	plane(glo_P_house[0],glo_P_house[1],glo_P_house[2],glo_P_house[3]);
	plane(P[0],P[1],P[2],P[3]);
	
	for(i=0;i<4;i++)
	{
		if(type)	COL(.2, .1, .1);//brown
		plane(glo_P_house[i],glo_P_house[(i+1)%4],P[(i+1)%4],P[i]);
		

		if(debug)
		{
			line_black(glo_P_house[i].x,glo_P_house[i].y,glo_P_house[i].z,P[i].x,P[i].y,P[i].z,1);			
			line_black(P[i].x,P[i].y,P[i].z,P[(i+1)%4].x,P[(i+1)%4].y,P[(i+1)%4].z,1);
		}

		//line0(double x1,double y1,double z1,double x2,double y2,double z2,double w)
	}	
}





void house0(int type)
{
	house(0,0);
	house(type,3);
}




void Sphere(double X,double Y,double Z,double R,int slice)
{
	glPushMatrix ();
	{
		glTranslatef (X, Y, Z);
		glutSolidSphere(R,slice,20);//quad,radius,slice,stacks
    }
	glPopMatrix ();
}





void box(double X,double Y,double Z,double A,double B,double C)
{

	
	glPushMatrix ();
	{
		glTranslatef (X, Y, Z);
		glScalef(A,B,C);
		glutSolidCube(1);//quad,radius,slice,stacks
	}
	glPopMatrix ();
}

void kata(double X,double Y,double Z,double size,double ang)
{
	glPushMatrix ();
	{
		//void Solid_Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		glTranslatef(X,Y,Z);
		glRotatef(ang,1,0,0); 
		
		glRotatef(22,0,0,1); 
		Solid_Cylinder(0,0,0,0,size,3*size,2);
		glRotatef(90,0,0,1); 
		Solid_Cylinder(0,0,0,0,size,3*size,2);
	}	
	glPopMatrix ();
}





void line(double x1,double y1,double z1,double x2,double y2,double z2,double w)
{
	glPushMatrix ();
	{
		glLineWidth(w); 
		glBegin(GL_LINES);
		glVertex3f(x1, y1, z1);
		glVertex3f(x2, y2, z2);
		glEnd();
	}
	glPopMatrix ();
}





//					(x,y,z)				,					height,											rotate_about_z_axis
void cave(double X0,double Y0,double Z0,double R_out,double R_in,double Height,double Width,double ratate_ang)
{
	//FOUR spheres

	//draw first two : as half spheres: bottom half.
	/// the cutting plane equation: z = 30
	/// we will keep the points with
	//		z <= 30
	//OR	0.x + 0.y + 1.z - 30 <= 0
	//OR	0.x + 0.y - 1.z + 30 >= 0	//// standard format:: ax + by + cz + d >= 0
	

	
	double equ[4];
	equ[0] = 0;	//0.x
	equ[1] = 0;	//0.y
	equ[2] = 1;//-1.z
	equ[3] = 0;//0
	
	glPushMatrix ();
	{
		glTranslatef (X0, Y0, Z0 );
		glRotatef(ratate_ang,0,0,1); 
		glClipPlane(GL_CLIP_PLANE0,equ);//now we enable the clip plane
		glEnable(GL_CLIP_PLANE0);
		{
			

			
			double equ1[4];
			equ1[0] = 0;	//0.x
			equ1[1] = 0;	//0.y
			equ1[2] = -1;//-1.z
			equ1[3] = Height;//0
			
			
			double equ2[4];
			equ2[0] = 0;	//0.x
			equ2[1] = 1;	//0.y
			equ2[2] = 0;//-1.z
			equ2[3] = R_out;//0


			double equ3[4];
			equ3[0] = 0;	//0.x
			equ3[1] = -1;	//0.y
			equ3[2] = 0;//-1.z
			equ3[3] = R_out;//0
		
			

			glClipPlane(GL_CLIP_PLANE1,equ1);//now we enable the clip plane
			glClipPlane(GL_CLIP_PLANE2,equ2);//now we enable the clip plane
			glClipPlane(GL_CLIP_PLANE3,equ3);//now we enable the clip plane
			
			
			
			glEnable(GL_CLIP_PLANE1);
			glEnable(GL_CLIP_PLANE2);
			glEnable(GL_CLIP_PLANE3);
			{	




				glPushMatrix ();
				{
					glRotatef(90, 0, 1, 0); // Rotate our object around the y axis  

					//eikhane jeta print korbo seta 4 ta copy hoye 4 dike jabe
					Solid_Cylinder2(0,0,-Width/2,R_in+50,R_in, Width,Width,20);//X,Y,Z,R_out,R_in, out_Height,in_Height,int slice)
					
					
					//PT P[4];
					//COL(0,0,0);
					


				
					

					glo_P_house[0]=PT(-.1,-R_out,-Width/2);
					glo_P_house[1]=PT(-.1,-R_in,-Width/2);
					glo_P_house[2]=PT(-.1,-R_in,Width/2);
					glo_P_house[3]=PT(-.1,-R_out,Width/2);

					plane(glo_P_house[0],glo_P_house[1],glo_P_house[2],glo_P_house[3]);
					
					
					glo_P_house[0]=PT(-.1,R_out,-Width/2);
					glo_P_house[1]=PT(-.1,R_in,-Width/2);
					glo_P_house[2]=PT(-.1,R_in,Width/2);
					glo_P_house[3]=PT(-.1,R_out,Width/2);

					plane(glo_P_house[0],glo_P_house[1],glo_P_house[2],glo_P_house[3]);
					


				}
				glPopMatrix ();	
			
					
				
			}
			glDisable(GL_CLIP_PLANE1);
			glDisable(GL_CLIP_PLANE2);
			glDisable(GL_CLIP_PLANE3);
			


			glRotatef(45,0,0,1); 
			
			


	
	}
		glDisable(GL_CLIP_PLANE0);

















	}
	glPopMatrix ();
}





//					(x,y,z)  							radius,	 size_of_kata	scale_qntt		rotate_about_z_axis
void ribbon_type_kata(double X0,double Y0,	double Z0,		double R,double size,double scale_val,double ratate_ang,double kata_angle)
{
	COL(0,0,0);
	glPushMatrix ();
	{
		glTranslatef (X0, Y0, Z0);
		glScalef(1,1,scale_val);
		glRotatef(ratate_ang,0,0,1);  
		
		
		int i;
		for(i=0;i<16;i++)
		{
			
			glPushMatrix ();
			{
				glRotatef(90, 1, 0, 0);
				//void kata(double X,double Y,double Z,double size,double ang)
				kata(0,0,R,size,kata_angle);
			}
			glPopMatrix ();
			glRotatef(22.5, 0, 0, 1);
		}
		
	}
	glPopMatrix ();
}



void wall_post(double X,double Y,double Z,double R)
{
	glPushMatrix();	
	{
		glTranslatef(X,Y,Z);
		glRotatef(45,0,0,1);

		
		

		double temp_Z=0;	
		
		COL(.73, .6, .4);//main color(mati color)
		//Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		Cylinder(0,0,0,R,R,R,4);
		if(debug) Cylinder_Cage(0,0,0,R,R,R,4);		
		temp_Z+=R;
		Cylinder(0,0,temp_Z,R,0,0,4);
		
		
		COL(.2, .1, .1);//brown
		Cylinder(0,0,temp_Z,R*.8,R*.76,R*.6,4);
		if(debug) Cylinder_Cage(0,0,temp_Z,R*.8,R*.76,R*.6,4);		
		temp_Z+=R*.6;
		
		
		
		
		COL(.73, .6, .4);//main color(mati color)
		Cylinder(0,0,temp_Z,R*.8,R*.76,R*1.25,4);
		if(debug) Cylinder_Cage(0,0,temp_Z,R*.8,R*.76,R*1.25,4);	
		temp_Z+=R*1.25;
		
		Cylinder(0,0,temp_Z,R*.76,0,0,4);
		
		


		COL(.2, .1, .1);//brown
		Cylinder(0,0,temp_Z,R*.6,R*.6,R*.50,4);
		if(debug) Cylinder_Cage(0,0,temp_Z,R*.6,R*.6,R*.50,4);	
		temp_Z+=R*.5;
		Cylinder(0,0,temp_Z,R*.6,0,0,4);
		
		
	
	}
	glPopMatrix();	
}



//globals for gombuj
double glo_angs[2][6]={{40*(PI/180),50*(PI/180),60*(PI/180),70*(PI/180),80*(PI/180),87*(PI/180)},{40*(PI/180),50*(PI/180),60*(PI/180),70*(PI/180),80*(PI/180),87*(PI/180)}};
double glo_angs2[2][7]={ {36*(PI/180), 42*(PI/180),52*(PI/180),56*(PI/180),67*(PI/180),78*(PI/180),87*(PI/180)}, {25*(PI/180), 35*(PI/180),42*(PI/180),60*(PI/180),70*(PI/180),80*(PI/180),-87*(PI/180)} };
double glo_angs3[2][7]={ {38*(PI/180), 42*(PI/180),48*(PI/180),60*(PI/180),64*(PI/180),70*(PI/180),90*(PI/180)}, {35*(PI/180), 50*(PI/180),65*(PI/180),78*(PI/180),80*(PI/180),80*(PI/180),80*(PI/180)} };
	


double kata_R_mult[8]={.13,.33,.33,.13,.13,.13,.13};
double kata_rad_mult[8]={.53,.63,.53,.53,.53,.33,.23};

double kata_size_mult[8]={.07,.10,.08,.08,.07,.05,.05};
double kata_angle_mult[8]={40,0,-30,-33,-49,-54,-60};
double kata_rotate_mult[8]={0,4.5,8.5,12.15,15.435,18.391,21.331};



void gombuj1(double X,double Y,double Z,double R)
{
	
	int i;
	double per_seg_dz=( .7*R )/(6);

	
	
	double temp_Z=Z;
	double temp_R=R;
	double ang=3*(PI/18);
	int kata_I=0;
	
	
	for(i=0;i<6;i++)
	{
		COL(.2, .3, .3);//dark green
		Cylinder(X,Y,temp_Z,temp_R,temp_R+per_seg_dz*(1/tan(glo_angs[0][i])),per_seg_dz,50);		
		
		//COL(0, 0, 0);//black
		//Cylinder(X,Y,temp_Z,temp_R+.1,temp_R+.1,1,50);	

		
		temp_R+=per_seg_dz*(1/tan(glo_angs[0][i]));
		temp_Z+=per_seg_dz;
				
	}
	

	

	
	int type=0;

	
	COL(0,.32,.30);
	double per_seg_dz2=( .85*R )/(7);
	double dy2[2];
	double temp_Z2;
	

	
	COL(0,.34,.20);
	for(i=6;i>=0;i--)
	{
		COL(0,.32,.30);
		Cylinder(X,Y,temp_Z,temp_R,temp_R-per_seg_dz2*(1/tan(glo_angs2[type][i])),per_seg_dz2,50);		
		
		if(type==1 && (i<=3))
		{
			ribbon_type_kata(0,0,temp_Z,	temp_R,		 R*kata_size_mult[kata_I],				1		,		kata_rotate_mult[kata_I],			kata_angle_mult[kata_I] );
			kata_I++;	
		}

		temp_R-=per_seg_dz2*(1/tan(glo_angs2[type][i]));
		temp_Z+=per_seg_dz2;
		
		
		if(i==2)
		{
			dy2[0]=temp_R*.5;
			dy2[1]=-temp_R*.5;
			temp_Z2=temp_Z;
		}

		
		
	}

	
	

	

	double per_seg_dz3=( 1.1*R )/(7);
	COL(.2,.14,.21);
	for(i=0;i<7;i++)
	{
		COL(0,.32,.30);
		
		Cylinder(X,Y,temp_Z,temp_R,temp_R-per_seg_dz3*(1/tan(glo_angs3[type][i])),per_seg_dz3,50);		
		
		if(type==1 && (i<=0))
		{
			ribbon_type_kata(0,0,temp_Z,	temp_R,		 R*kata_size_mult[kata_I],				1		,		kata_rotate_mult[kata_I],			kata_angle_mult[kata_I] );
			kata_I++;	
		}

		temp_R-=per_seg_dz3*(1/tan(glo_angs3[type][i]));
		temp_Z+=per_seg_dz3;	
	}
	
	




	//sphere
	double r=(temp_Z-Z)*(.08);
	
	
	COL(.1,.3,1);
	Sphere(X,Y,temp_Z+.80*r,r,15);
	temp_Z+=2*r;
	

	
	//cross
	COL(.9, .8, .7);//cream
	box(X,Y,temp_Z,r*.5,r*.5,3*R);
	temp_Z+=R*.35;
	
	

	COL(.9, .8, .7);//cream
	box(X,Y,temp_Z,r*1.5,r*0.5,r*.5);
	temp_Z+=R*.85;
	
	


	COL(.9, .8, .7);//cream
	box(X,Y,temp_Z,r*5.7,r*.5,r*.5);
	
	
	COL(.9, .8, .7);//cream
	box(X,Y,temp_Z+R*.20,r,r*.5,r*.5);
	
	
	//4wires
	COL(.9, 1, .7);//cream
	double dy[2]={r*2.3,-r*2.3};
	for(i=0;i<2;i++)
	{
		line(X+dy[i],Y,temp_Z,X+dy2[i],Y+dy2[i],temp_Z2,1);
		line(X+dy[i],Y,temp_Z,X+dy2[i],Y-dy2[i],temp_Z2,1);
	}
}


void gombuj(double X,double Y,double Z,double R,int type,int wired)
{
	
	int i;
	double per_seg_dz=( .7*R )/(6);

	COL(0,.32,.30);
	
	double temp_Z=Z;
	double temp_R=R;
	double ang=3*(PI/18);
	int kata_I=0;
	
	
	

	for(i=0;i<6;i++)
	{
		Cylinder(X,Y,temp_Z,temp_R,temp_R+per_seg_dz*(1/tan(glo_angs[type][i])),per_seg_dz,50);		
		
		
		if(i==2 && type==1)
		{
			//				X0,Y0,Z0,		R,					size,					scale_val,	ratate_ang,		kata_angle)
			ribbon_type_kata(0,0,temp_Z,	temp_R,		 R*kata_size_mult[kata_I],				1		,		kata_rotate_mult[kata_I],			kata_angle_mult[kata_I] );
			kata_I++;
		}


		temp_R+=per_seg_dz*(1/tan(glo_angs[type][i]));
		temp_Z+=per_seg_dz;
		

	}
	
	
	
	if(type==1)
	{
		ribbon_type_kata(0,0,temp_Z,	temp_R,		 R*kata_size_mult[kata_I],				1		,		kata_rotate_mult[kata_I],			kata_angle_mult[kata_I] );
		kata_I++;
	}

	


	
	COL(0,.32,.30);
	double per_seg_dz2=( .85*R )/(7);
	double dy2[2];
	double temp_Z2;
	

	
	COL(0,.34,.20);
	for(i=6;i>=0;i--)
	{
		COL(0,.32,.30);
		Cylinder(X,Y,temp_Z,temp_R,temp_R-per_seg_dz2*(1/tan(glo_angs2[type][i])),per_seg_dz2,50);		
		
		if(type==1 && (i<=3))
		{
			ribbon_type_kata(0,0,temp_Z,	temp_R,		 R*kata_size_mult[kata_I],				1		,		kata_rotate_mult[kata_I],			kata_angle_mult[kata_I] );
			kata_I++;	
		}

		temp_R-=per_seg_dz2*(1/tan(glo_angs2[type][i]));
		temp_Z+=per_seg_dz2;
		
		
		if(i==2)
		{
			dy2[0]=temp_R*.5;
			dy2[1]=-temp_R*.5;
			temp_Z2=temp_Z;
		}

		
		
	}

	
	

	

	double per_seg_dz3=( 1.1*R )/(7);
	COL(.2,.14,.21);
	for(i=0;i<7;i++)
	{
		COL(0,.32,.30);
		
		Cylinder(X,Y,temp_Z,temp_R,temp_R-per_seg_dz3*(1/tan(glo_angs3[type][i])),per_seg_dz3,50);		
		
		if(type==1 && (i<=0))
		{
			ribbon_type_kata(0,0,temp_Z,	temp_R,		 R*kata_size_mult[kata_I],				1		,		kata_rotate_mult[kata_I],			kata_angle_mult[kata_I] );
			kata_I++;	
		}

		temp_R-=per_seg_dz3*(1/tan(glo_angs3[type][i]));
		temp_Z+=per_seg_dz3;	
	}
	
	




	//sphere
	double r=(temp_Z-Z)*(.08);
	
	
	COL(.1,.3,1);
	Sphere(X,Y,temp_Z+.80*r,r,15);
	temp_Z+=2*r;
	

	
	//cross
	COL(.9, .8, .7);//cream
	box(X,Y,temp_Z,r*.5,r*.5,3*R);
	temp_Z+=R*.35;
	
	

	COL(.9, .8, .7);//cream
	box(X,Y,temp_Z,r*1.5,r*0.5,r*.5);
	temp_Z+=R*.85;
	
	


	COL(.9, .8, .7);//cream
	box(X,Y,temp_Z,r*5.7,r*.5,r*.5);
	
	
	COL(.9, .8, .7);//cream
	box(X,Y,temp_Z+R*.20,r,r*.5,r*.5);
	
	

	if(wired)
	{	
		//4wires
		COL(.9, 1, .7);//cream
		double dy[2]={r*2.3,-r*2.3};
		for(i=0;i<2;i++)
		{
			line(X+dy[i],Y,temp_Z,X+dy2[i],Y+dy2[i],temp_Z2,1);
			line(X+dy[i],Y,temp_Z,X+dy2[i],Y-dy2[i],temp_Z2,1);
		}
	}	
}










//					(x,y,z)  mainrad,					height,											rotate_about_z_axis
void ribbon_type2(double X0,double Y0,double Z0,double R,double R_out,double R_in,double out_Height,double in_Height,double ratate_ang,int num_of_cyls,double inter_dist_cyls)
{
	//FOUR spheres

	//draw first two : as half spheres: bottom half.
	/// the cutting plane equation: z = 30
	/// we will keep the points with
	//		z <= 30
	//OR	0.x + 0.y + 1.z - 30 <= 0
	//OR	0.x + 0.y - 1.z + 30 >= 0	//// standard format:: ax + by + cz + d >= 0
	
	double equ[4];
	equ[0] = 0;	//0.x
	equ[1] = 0;	//0.y
	equ[2] = 1;//-1.z
	equ[3] = 0;//0
	
	


	int k;
	glPushMatrix ();
	{
		glTranslatef (X0, Y0, Z0 );
		glRotatef(ratate_ang,0,0,1); 
		
	
		glClipPlane(GL_CLIP_PLANE0,equ);//now we enable the clip plane
		
		int l;
		glEnable(GL_CLIP_PLANE0);
		{

			
			//now whatever we draw it will be cut in half

			//ei loop tar jonno jai korbo seta copy hoye 45 degree z axis borabor rotate korbe
			//comment this out to see the result
			
			//k=0;
			for(k=0;k<8;k++)//push_matrix dilam na eikhane coz total rotation 360 hoye ager jayga te firot astese
			{
				glPushMatrix ();
				{
					glRotatef(90, 0, 1, 0); // Rotate our object around the y axis  

					//eikhane jeta print korbo seta 4 ta copy hoye 4 dike jabe
					double dr=-(2*R_out*num_of_cyls+(num_of_cyls-1)*inter_dist_cyls)/2+R_out;
					Solid_Cylinder2(0,dr,R,R_out,R_in, out_Height,in_Height,20);//X,Y,Z,R_out,R_in, out_Height,in_Height,int slice)
					for(l=1;l<num_of_cyls;l++)
					{
						dr+=(2*R_out+inter_dist_cyls);
						Solid_Cylinder2(0,dr,R,R_out,R_in, out_Height,in_Height,20);//X,Y,Z,R_out,R_in, out_Height,in_Height,int slice)
					}
				}
				glPopMatrix ();	
				
				glRotatef(45,0,0,1); 
			}
		}
		glDisable(GL_CLIP_PLANE0);
	}
	glPopMatrix ();
}








//					(x,y,z)  							radius,	 size_of_cube	scale_qntt		rotate_about_z_axis
void ribbon_type3(double X0,double Y0,	double Z0,		double R,double size,double scale_val,double ratate_ang,int num_of_cyls,double inter_dist_cyls)
{
	//FOUR spheres

	//draw first two : as half spheres: bottom half.
	/// the cutting plane equation: z = 30
	/// we will keep the points with
	//		z <= 30
	//OR	0.x + 0.y + 1.z - 30 <= 0
	//OR	0.x + 0.y - 1.z + 30 >= 0	//// standard format:: ax + by + cz + d >= 0
	
	glPushMatrix ();
	{
		glTranslatef (X0, Y0, Z0);
		glScalef(1,1,scale_val);
		glRotatef(ratate_ang,0,0,1);  
		
		
		int k,l;
		


		for(k=0;k<8;k++)
		{
			glPushMatrix ();
			{
				glRotatef(90, 1, 1, 0); // Rotate our object around the y axis  

				

				double R_out=size/2;
				double dr=-(2*R_out*num_of_cyls+(num_of_cyls-1)*inter_dist_cyls)/2+R_out;
				box(dr,dr,R,size,size, size);
				for(l=1;l<num_of_cyls;l++)
				{
					dr+=(2*R_out+inter_dist_cyls);
					box(dr,dr,R,size,size, size);
				}



				
				glPopMatrix ();	
			}
			glRotatef(45,0,0,1); 
		}
		

	}
	glPopMatrix ();
}




//					(x,y,z)  							radius,	 size_of_cube	scale_qntt		rotate_about_z_axis
void ribbon_type4(double X0,double Y0,	double Z0,		double R,double size,double scale_val,double ratate_ang)
{
	glPushMatrix ();
	{
		glTranslatef (X0, Y0, Z0);
		glScalef(1,1,scale_val);
		glRotatef(ratate_ang,0,0,1);  
				
		int i;
		for(i=0;i<8;i++)
		{
			
			glPushMatrix ();
			{
				glRotatef(90, 1, 0, 0);
				box(0,0,R,size,size, size);
			}
			glPopMatrix ();
			glRotatef(45, 0, 0, 1);
		}
		
	}
	glPopMatrix ();
}






void ribbon_type5(double X0,double Y0,	double Z0,		double R,double size,double scale_val,double ratate_ang)
{
	int i;
	
	//			   X,Y,Z,	R_low,	R_high,		Height,		slice
	//Cylinder(0,0,R,	size,		size,		size,		4);
	//return;



	glPushMatrix ();
	{
		glTranslatef (X0, Y0, Z0);
		glScalef(1,1,scale_val);
		glRotatef(ratate_ang,0,0,1);  
		
		
		
		
		for(i=0;i<8;i++)
		{
			
			glPushMatrix ();
			{
				glRotatef(90, 1, 0, 0);
				Cylinder(0,0,R,size,	0,		size,		4);
				//			   X,Y,Z,R_low,R_high,	Height,slice)
			}
			glPopMatrix ();
			glRotatef(45, 0, 0, 1);
		}

	}
	glPopMatrix ();
}






//					(x,y,z)  							radius,	 size_of_cube	scale_qntt	  rotate_about_z_axis
void ribbon_type6(double X0,double Y0,	double Z0,		double R,double size,double scale_val,double ratate_ang)
{
	glPushMatrix ();
	{
		glTranslatef (X0, Y0, Z0);
		glScalef(1,1,scale_val);
		
		glRotatef(ratate_ang,0,0,1);  
				
		double dr;
		dr=R*tan((22.5)*PI/180)-size/3;
		
		double X[2]={0,1};
		double Y[2]={1,0};
		double deg[2]={90,-90};
		int i,j,k;	
		

		double multx[2]={0,1};
		double multy[2]={1,0};
		

		for(k=0;k<2;k++)
		{
			if(k)
			{
				glPushMatrix ();
				glRotatef(45,0,0,1); 
			}
			for(i=0;i<2;i++)
			{
				for(j=0;j<2;j++)
				{
					glPushMatrix ();
					glRotatef(deg[j], X[i], Y[i], 0);
					box(multx[i]*dr,multy[i]*dr,R,size,size, size);
					box(multx[i]*(-dr),multy[i]*(-dr),R,size,size, size);	
					glPopMatrix ();						
				}
			}
			if(k)glPopMatrix ();
		}
	}
	glPopMatrix ();
}




//					(x,y,z)  mainrad,					height,											rotate_about_z_axis
void ribbon_type7(double X0,double Y0,double Z0,double R,double R_out,double R_in,double out_Height,double in_Height,double ratate_ang,int num_of_cyls,double inter_dist_cyls)
{
	//FOUR spheres

	//draw first two : as half spheres: bottom half.
	/// the cutting plane equation: z = 30
	/// we will keep the points with
	//		z <= 30
	//OR	0.x + 0.y + 1.z - 30 <= 0
	//OR	0.x + 0.y - 1.z + 30 >= 0	//// standard format:: ax + by + cz + d >= 0
	
	double equ[4];
	equ[0] = 0;	//0.x
	equ[1] = 0;	//0.y
	equ[2] = 1;//-1.z
	equ[3] = 0;//0
	
	

	int k;
	glPushMatrix ();
	{
		glTranslatef (X0, Y0, Z0 );
		glRotatef(ratate_ang,0,0,1); 
		
	
		glClipPlane(GL_CLIP_PLANE0,equ);//now we enable the clip plane
		
		int l;
		glEnable(GL_CLIP_PLANE0);
		{
			
			
			//now whatever we draw it will be cut in half

			//ei loop tar jonno jai korbo seta copy hoye 45 degree z axis borabor rotate korbe
			//comment this out to see the result
			
			//k=0;
			for(k=0;k<4;k++)//push_matrix dilam na eikhane coz total rotation 360 hoye ager jayga te firot astese
			{
				glPushMatrix ();
				{
					glRotatef(90, 0, 1, 0); // Rotate our object around the y axis  

					//eikhane jeta print korbo seta 4 ta copy hoye 4 dike jabe
					double dr=-(2*R_out*num_of_cyls+(num_of_cyls-1)*inter_dist_cyls)/2+R_out;
					Solid_Cylinder2(0,dr,R,R_out,R_in, out_Height,in_Height,20);//X,Y,Z,R_out,R_in, out_Height,in_Height,int slice)
					for(l=1;l<num_of_cyls;l++)
					{
						dr+=(2*R_out+inter_dist_cyls);
						Solid_Cylinder2(0,dr,R,R_out,R_in, out_Height,in_Height,20);//X,Y,Z,R_out,R_in, out_Height,in_Height,int slice)
					}
				}
				glPopMatrix ();	
				
				glRotatef(90,0,0,1); 
			}
		}
		glDisable(GL_CLIP_PLANE0);
	}
	glPopMatrix ();
}



//					(x,y,z)  							radius,	 size_of_cube	scale_qntt		rotate_about_z_axis
void ribbon_type8(double X0,double Y0,	double Z0,		double R,double size,double scale_val,double ratate_ang)
{
	glPushMatrix ();
	{
		glTranslatef (X0, Y0, Z0);
		glScalef(1,1,scale_val);
		glRotatef(ratate_ang,0,0,1);  
				
		int i;
		for(i=0;i<4;i++)
		{
			glPushMatrix ();
			{
				glRotatef(90, 1, 0, 0);
				box(0,0,R,size,size, size);
			}
			glPopMatrix ();
			glRotatef(90, 0, 0, 1);
		}
		
	}
	glPopMatrix ();
}


//					(x,y,z)  							radius,	 size_of_cube	scale_qntt		rotate_about_z_axis
void ribbon_type9(double X0,double Y0,	double Z0,		double R,double size,double scale_val,double ratate_ang,int num_of_cyls,double inter_dist_cyls)
{
	//FOUR spheres

	//draw first two : as half spheres: bottom half.
	/// the cutting plane equation: z = 30
	/// we will keep the points with
	//		z <= 30
	//OR	0.x + 0.y + 1.z - 30 <= 0
	//OR	0.x + 0.y - 1.z + 30 >= 0	//// standard format:: ax + by + cz + d >= 0
	
	glPushMatrix ();
	{
		glTranslatef (X0, Y0, Z0);
		glScalef(1,1,scale_val);
		glRotatef(ratate_ang,0,0,1);  
		
		
		int k,l;
		


		for(k=0;k<8;k++)
		{
			glPushMatrix ();
			{
				glRotatef(90, 1, 0, 0); // Rotate our object around the y axis  

				

				double R_out=size/2;
				double dr=-(2*R_out*num_of_cyls+(num_of_cyls-1)*inter_dist_cyls)/2+R_out;
				box(dr,0,R,size,size, size);
				for(l=1;l<num_of_cyls;l++)
				{
					dr+=(2*R_out+inter_dist_cyls);
					box(dr,0,R,size,size, size);
				}



				
				glPopMatrix ();	
			}
			glRotatef(45,0,0,1); 
		}
		

	}
	glPopMatrix ();
}








void tower_type1(double X,double Y,double Z,double R)
{
	
	double temp_R=R;
	double temp_Z=Z;
	int i;

	for(i=0;i<3;i++)
	{
		COL(.8, .8, .7);//cream
		Cylinder(X,Y,temp_Z,temp_R,temp_R,R*.3,20);
		temp_Z+=R*.3;
		

		COL(.3, .2, .2);//brown
		Cylinder(X,Y,temp_Z,temp_R,temp_R*.87,R*.1,20);
		temp_Z+=R*.1;
		temp_R*=.87;
	}
	
	
	COL(.8, .78, .67);//cream
	Cylinder(X,Y,temp_Z,temp_R,temp_R,R,20);
	temp_Z+=R;
	
	
	
	COL(.2, .3, .3);//dark green
	Cylinder(X,Y,temp_Z,temp_R*1.1,temp_R*1.1,R*.1,20);
	temp_Z+=R*.1;
	
	
	COL(.2, .3, .3);//dark green
	Cylinder(X,Y,temp_Z,temp_R*1.1,0.0,0,20);
	

	gombuj1(X,Y,temp_Z,R*.66);
}





void tower_type2(double X,double Y,double Z,double R)
{
	
	
	double temp_Z=0;
	
	
	glPushMatrix ();
	{
		glTranslatef(X,Y,Z); 
		glRotatef(22.5,0,0,1); 
			
		X=0;
		Y=0;
		Z=0;
		

		COL(.73, .6, .4);//main color
		Cylinder(X,Y,temp_Z,R,0,1.0,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		

		
		COL(.73, .6, .4);//main color
		Cylinder(X,Y,temp_Z,R,R,1.9*R,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		if(debug) Cylinder_Cage(X,Y,temp_Z,R,R,1.9*R,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
		

		temp_Z+=R;


		
		
		COL(.7, .6, .4);
		Cylinder(X,Y,temp_Z,R*1.02,R*1.02,5*(R/60),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=5*(R/60);
		
		
		
		COL(.7, .5, .4);
		Cylinder(X,Y,temp_Z,R*1.02,R*1.04,10*(R/60),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=10*(R/60);
		
		



		
		
		COL(.2, .1, .1);//brown
		Cylinder(X,Y,temp_Z,R*1.04,R*1.04,2*(R/60),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=2*(R/60);
		
		COL(.2, .1, .1);//brown
		Cylinder(X,Y,temp_Z,R*1.04,0,0,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=2*(R/60);
		

		temp_Z=Z+1.9*R;
		

		Cylinder(X,Y,temp_Z,R*1.04,0,0,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
		



		
		COL(.2, .3, .3);//dark green
		Cylinder(X,Y,temp_Z,R*1.04,R*1.04,4*(R/60),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=4*(R/60);
		


		COL(.17, .25, .3);
		Cylinder(X,Y,temp_Z,R*1.04,R*.93,4.5*(R/60),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=4.5*(R/60);
			
		
		
		
		COL(.2, .2, .1);
		Cylinder(X,Y,temp_Z,R*.93,R*.89,5*(R/60),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=5*(R/60);
		
		
		
		COL(.1, .2, .1);//khaista 
		Cylinder(X,Y,temp_Z,R*.89,R*.95,7.5*(R/60),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=7.5*(R/60);
		
		

		COL(.1, .2, .5);
		Cylinder(X,Y,temp_Z,R*.95,1.04*R,2.5*(R/60),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=2.5*(R/60);
		
		
		COL(.2, .3, .3);//dark green
		Cylinder(X,Y,temp_Z,1.04*R,R*.87,4*(R/60),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=4*(R/60);
		

		
		
		

		COL(.9, .8, .7);//cream
		Cylinder(X,Y,temp_Z,R*.87,R*.83,3*(R/60),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=3*(R/60);
		

		
		
		COL(.9, .8, .7);//cream
		Cylinder(X,Y,temp_Z,R*.83,0,0,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			
		
		
		COL(.73, .6, .4);//main color
		Cylinder(X,Y,temp_Z,R*.76,R*.76,5*((temp_Z-Z)/9),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		if(debug) Cylinder_Cage(X,Y,temp_Z,R*.76,R*.76,5*((temp_Z-Z)/9),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
		



	
		temp_Z+=5*((temp_Z-Z)/9);
		

			
		
		
		COL(.1, .2, .1);
		Cylinder(X,Y,temp_Z,R*.76,R*.86,7*(R/60),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=7*(R/60);

		
		
		COL(.1, .4, .4);
		Cylinder(X,Y,temp_Z,R*.86,R*.02,0,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
		
		
		COL(.2, .1, .1);//brown
		//							R	,	R_out,		R_in, out_Height,	in_Height,	ratate_ang,		quantity_of_cyl_per_side	,	interdist_of_cyls)
		ribbon_type2(X,Y,temp_Z,  R*.605,   R*.29,		0,		R*.1,			R*.1,	       22.5,			1,								0);
		
		COL(.2, .3, .3);//dark green
		ribbon_type5(X,Y,temp_Z+R*.06,	R*.43,		 R*.32,		4,			0);
		

		
		



		COL(.73, .6, .4);//main color(mati color)
		Cylinder(X,Y,temp_Z,R*.63,R*.63,1.1*R,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		if(debug)Cylinder_Cage(X,Y,temp_Z,R*.63,R*.63,1.1*R,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
		

		temp_Z+=1.1*R;

		
		
		COL(.2, .3, .3);//dark green
		Cylinder(X,Y,temp_Z,R*.63,R*.68,0.05*R,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=0.05*R;
			
		
		
		COL(.2, .3, .34);//dark green
		Cylinder(X,Y,temp_Z,R*.68,R*.68,0.1*R,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=0.1*R;
			
		Cylinder(X,Y,temp_Z,R*.68,0,0,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
		
		glRotatef(90+45+22.5,0,0,1);
		gombuj(X,Y,temp_Z,R*.66,0,1);
		
	}
	glPopMatrix ();
		
}


void tower_type3(double X,double Y,double Z,double R)
{
	
	double temp_Z=Z;
	

	
	
	glPushMatrix ();
	
	
	glTranslatef(X,Y,Z); 
	glRotatef(22.5,0,0,1); 
	X=0;
	Y=0;
	Z=0;
	
	/*
	glPushMatrix ();
	{
		glRotatef(22.5,0,0,1); 
		COL(0, 0, 0);//main color
		if(debug)ribbon_type6(0,0,0,		 .92*R,		 .5,		7.5*R			,	0);
	}
	glPopMatrix ();
	*/


	COL(.73, .6, .4);//main color
	Cylinder(X,Y,temp_Z,R,0,1.0,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	

	
	COL(.73, .6, .4);//main color
	Cylinder(X,Y,temp_Z,R,R,1.9*R,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	if(debug)Cylinder_Cage(X,Y,temp_Z,R,R,1.9*R,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	
	temp_Z+=R;


	
	
	COL(.7, .6, .4);
	Cylinder(X,Y,temp_Z,R*1.02,R*1.02,5*(R/60),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	temp_Z+=5*(R/60);
	
	
	
	COL(.7, .5, .4);
	Cylinder(X,Y,temp_Z,R*1.02,R*1.04,10*(R/60),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	temp_Z+=10*(R/60);
	
	



	
	
	COL(.2, .1, .1);//brown
	Cylinder(X,Y,temp_Z,R*1.04,R*1.04,2*(R/60),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	temp_Z+=2*(R/60);
	
	COL(.2, .1, .1);//brown
	Cylinder(X,Y,temp_Z,R*1.04,0,0,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	temp_Z+=2*(R/60);
	

	temp_Z=Z+1.9*R;
	
	
	Cylinder(X,Y,temp_Z,R*1.04,0,0,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	
	
	
	COL(.2, .3, .3);//dark green
	Cylinder(X,Y,temp_Z,R*1.04,R*1.04,4*(R/60),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	temp_Z+=4*(R/60);
	


	COL(.17, .25, .3);
	Cylinder(X,Y,temp_Z,R*1.04,R*.93,4.5*(R/60),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	temp_Z+=4.5*(R/60);
		
	
	
	
	COL(.2, .2, .1);
	Cylinder(X,Y,temp_Z,R*.93,R*.89,5*(R/60),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	temp_Z+=5*(R/60);
	
	
	
	COL(.1, .2, .1);//khaista 
	Cylinder(X,Y,temp_Z,R*.89,R*.95,7.5*(R/60),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	temp_Z+=7.5*(R/60);
	
	

	COL(.1, .2, .5);
	Cylinder(X,Y,temp_Z,R*.95,1.04*R,2.5*(R/60),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	temp_Z+=2.5*(R/60);
	
	
	COL(.2, .3, .3);//dark green
	Cylinder(X,Y,temp_Z,1.04*R,R*.87,4*(R/60),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	temp_Z+=4*(R/60);
	

	
	
	

	COL(.9, .8, .7);//cream
	Cylinder(X,Y,temp_Z,R*.87,R*.83,3*(R/60),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	temp_Z+=3*(R/60);
	
	
	
	
	COL(.9, .8, .7);//cream
	Cylinder(X,Y,temp_Z,R*.83,0,0,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
	
	
	COL(.73, .6, .4);//main color
	Cylinder(X,Y,temp_Z,R*.76,R*.76,5*((temp_Z-Z)/9),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	if(debug)Cylinder_Cage(X,Y,temp_Z,R*.76,R*.76,5*((temp_Z-Z)/9),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	
	

	temp_Z+=5*((temp_Z-Z)/9);
	
	


	
	
	COL(.1, .2, .1);
	Cylinder(X,Y,temp_Z,R*.76,R*.86,7*(R/60),8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	temp_Z+=7*(R/60);

	
	
	COL(.1, .4, .4);
	Cylinder(X,Y,temp_Z,R*.86,R*.02,0,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	
	
	
	COL(.2, .1, .1);//brown
	//							R	,	R_out,		R_in, out_Height,	in_Height,	ratate_ang,		quantity_of_cyl_per_side	,	interdist_of_cyls)
	ribbon_type2(X,Y,temp_Z,  R*.605,   R*.29,		R*.22,		R*.1,		R*.05,	       22.5,			1,								0);
	COL(.24, .1, .1);//brown
	ribbon_type2(X,Y,temp_Z,  R*.605,   R*.22,		0,			R*.05,		R*.05,	       22.5,			1,								0);
	

	COL(.2, .3, .3);//dark green
	Cylinder(X,Y,temp_Z,R*.705,R*.705,R*.29,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	temp_Z+=R*.29;
	
	COL(.2, .3, .3);//dark green
	Cylinder(X,Y,temp_Z,R*.705,0,0,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	
	

	COL(.2, .1, .1);//brown
	//							R	,	R_out,		R_in, out_Height,	in_Height,	ratate_ang,		quantity_of_cyl_per_side	,	interdist_of_cyls)
	ribbon_type2(X,Y,temp_Z,  R*.57,   R*.23,		R*.15,		R*.1,		R*.1,	       0,			1,								0);
	COL(.23, .1, .1);//brown
	ribbon_type2(X,Y,temp_Z,  R*.57,   R*.15,		R*.12,		R*.1,		R*.05,	       0,			1,								0);
	ribbon_type2(X,Y,temp_Z,  R*.57,   R*.13,		0,		R*.07,		R*.07,	       0,			1,								0);
	
	
	//X0 Y0 Z0,							R,		size,		scale_val	,ratate_ang,	discs_per_side,		inter_dist_of_disc)
	COL(.2, .1, .1);//brown
	ribbon_type3(X,Y,temp_Z+R*.22,	R*.610,		R*.1,			    1,				0,			1,				0);
	
	

	

	
	COL(.73, .6, .4);//main color(mati color)
	Cylinder(X,Y,temp_Z,R*.63,R*.63,1.1*R,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	if(debug)Cylinder_Cage(X,Y,temp_Z,R*.63,R*.63,1.1*R,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	
	

	temp_Z+=1.1*R;
	
	
	
	COL(.2, .3, .3);//dark green
	Cylinder(X,Y,temp_Z,R*.63,R*.68,0.05*R,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	temp_Z+=0.05*R;
		
	
	
	COL(.2, .3, .34);//dark green
	Cylinder(X,Y,temp_Z,R*.68,R*.68,0.1*R,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	temp_Z+=0.1*R;
		
	Cylinder(X,Y,temp_Z,R*.68,0,0.1*R,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
	


	glRotatef(90+45+22.5,0,0,1);
	gombuj(X,Y,temp_Z,R*.66,0,1);
	

	glPopMatrix ();
	
}




void tower_type_kata(double X,double Y,double Z,double R)
{
	

	double temp_R=R;
	double temp_Z=Z;
	int i;
	

	glPushMatrix ();
	{
		glTranslatef(X,Y,Z);
		
		for(i=0;i<2;i++) temp_R*=.87;
		
		X=0;
		Y=0;
		Z=0;
	
		COL(.73, .6, .4);//main color(mati color)
		Cylinder(X,Y,temp_Z,temp_R,temp_R,R*.42,20);
		temp_Z+=R*.42;
		

		COL(.2, .1, .1);//brown
		Cylinder(X,Y,temp_Z,temp_R,temp_R*.93,R*.1,20);
		temp_Z+=R*.1;
		temp_R*=.93;


		
		COL(.73, .6, .4);//main color(mati color)
		Cylinder(X,Y,temp_Z,temp_R,temp_R,1.3*R,20);
		temp_Z+=1.3*R;
		

		COL(0, .4, .2);
		Cylinder(X,Y,temp_Z,temp_R*1.1,temp_R*1.1,R*.1,20);
		temp_Z+=R*.1;
		

		
		COL(0, .4, .2);
		Cylinder(X,Y,temp_Z,temp_R*1.1,0.0,0,20);
		

		
		gombuj(X,Y,temp_Z,R*.66,1,1);

	}
	glPopMatrix ();

	

}

void tower_type4(double X,double Y,double Z,double R)
{
	
	double temp_Z=Z;
	int i;
	double savex=X,savey=Y,savez=Z;
	
	
	glPushMatrix ();
	{
		glTranslatef(X,Y,0); 
		glRotatef(45,0,0,1); 
		
		X=0;
		Y=0;


		

		COL(.73, .6, .4);//main color
		Cylinder(X,Y,temp_Z,R,R,R*1.4,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		if(debug) Cylinder_Cage(X,Y,temp_Z,R,R,R*1.4,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
		temp_Z+=R*1.4;

		
		
		
		COL(.2, .1, .1);//brown
		Cylinder(X,Y,temp_Z,R*1.05,0,0,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
		
		
		
		COL(.2, .1, .1);
		Cylinder(X,Y,temp_Z,R*1.05,R*1.05,R*0.05,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*0.05;
		
		

		COL(.2, .1, .1);
		Cylinder(X,Y,temp_Z,R*1.05,0,0,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
		
		


		COL(.2, .1, .1);
		Cylinder(X,Y,temp_Z,R*1.05,R*1.05,R*0.05,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*0.05;
		
		
		
		COL(.2, .3, .3);//dark green
		Cylinder(X,Y,temp_Z,R*1.05,0,R*0.25,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		if(debug) Cylinder_Cage(X,Y,temp_Z,R*1.05,0,R*0.25,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*0.03;
		
		
		//				X,Y,Z,			R_out,		R_in,			out_Height,		in_Height,	slice
		
		
		
		
		
		glPushMatrix ();
		{

			glRotatef(22.5,0,0,1);
			
			//void Solid_Cylinder2(double X,double Y,double Z,double R_out,double R_in,double out_Height,double in_Height,int slice)


			COL(.67, .6, .4);
			
			Solid_Cylinder2(X,Y,temp_Z,		R*.68,		R*.54,			R*0.25,			R*0.25,		8);
			temp_Z+=R*0.25;
			
			
		}
		glPopMatrix ();
		
	
		
		COL(.64, .6, .4);
		//		(x,y,z)				radius,	 size_of_cube	scaled_up  rotate_about_z_axis
		ribbon_type6(X,Y,temp_Z,	 R*.56,		 R*.13,		6			,	0);
		
		
		temp_Z+=R*.38;

	
		COL(.60, .6, .4);//cream
		//							R	,	R_out,		R_in, out_Height,	in_Height,	ratate_ang,		quantity_of_cyl_per_side	,	interdist_of_cyls)
		ribbon_type2(X,Y,temp_Z,  R*.504,   R*.24,		R*.13,		R*.12,		R*.12,	       0,			1,								0);
		temp_Z+=R*.13;
		

		double save_Z=temp_Z;
		
		
		glRotatef(22.5,0,0,1);
		COL(.73, .6, .4);//main color
		Cylinder(X,Y,temp_Z,R*.604,R*.05,R*1.8,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		if(debug)Cylinder_Cage(X,Y,temp_Z,R*.604,R*.05,R*1.8,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
		temp_Z+=R*0.25;
		glRotatef(-22.5,0,0,1);
		
		

		
	
		COL(.3, .1, .1);//brown

		//rib1
		
		//		(x,y,z)				radius,	 size_of_cube	scaled_up	rotate_about_z_axis
		ribbon_type4(X,Y,temp_Z,		 R*.45,		 R*.15,			1.3,			0);
		temp_Z+=R*.096;
		
		
		//							R	,	R_out,		R_in, out_Height,	in_Height,	ratate_ang,		quantity_of_cyl_per_side	,	interdist_of_cyls)
		ribbon_type2(X,Y,temp_Z,  R*.37,   R*.08,		0,			R*.15,		R*.15,	       0,			1,								0);
		temp_Z+=R*.060;
		
					//X0 Y0 Z0,		R,			size,		scale_val		,ratate_ang,	discs_per_side,		inter_dist_of_disc)
		ribbon_type3(X,Y,temp_Z,	R*.48,		R*.07,			    1,				0,			1,				0);
		


		
		COL(.2, .3, .3);//dark green
		

		//rib2
		
		
		temp_Z+=R*.20;

		//		(x,y,z)				radius,	 size_of_cube	scaled_up	rotate_about_z_axis
		ribbon_type4(X,Y,temp_Z,		 R*.35,		 R*.15,			1.3,			0);
		temp_Z+=R*.10;
		
		
		ribbon_type4(X,Y,temp_Z,		 R*.34,		 R*.17,			.2,			0);
		temp_Z+=R*.02;
		
		
		
					//X0 Y0 Z0,		R,			size,		scale_val		,ratate_ang,	discs_per_side,		inter_dist_of_disc)
		ribbon_type3(X,Y,temp_Z,	R*.36,		R*.12,			    1,				0,			1,				0);
		
		


		



		COL(.3, .1, .1);//brown

		
		//rib3
		
		
		temp_Z+=R*.20;

		//			(x,y,z)				radius,	 size_of_cube	scaled_up	rotate_about_z_axis
		ribbon_type4(X,Y,temp_Z,		 R*.255,		 R*.15,			1.3,			0);
		temp_Z+=R*.10;
		
		
		ribbon_type4(X,Y,temp_Z,		 R*.245,		 R*.17,			.2,			0);
		temp_Z+=R*.02;
		
		
		
					//X0 Y0 Z0,		R,			size,		scale_val		,ratate_ang,	discs_per_side,		inter_dist_of_disc)
		ribbon_type3(X,Y,temp_Z,	R*.26,		R*.12,			    .7,				0,			1,				0);
		
		
					//X0 Y0 Z0,		R,			size,		scale_val		,ratate_ang,	discs_per_side,		inter_dist_of_disc)
		ribbon_type3(X,Y,temp_Z,	R*.27,		R*.105,			    1.1,				0,			1,				0);
		
		








		temp_Z=save_Z+1.8*R;
		
		
		glPushMatrix ();
		{
			COL(.1, .6, .8);	
			glRotatef(22.5,0,0,1);
			
			Cylinder(X,Y,temp_Z,R*.05,R*.048,R*.03,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			temp_Z+=R*.03;
		


			COL(.1, .2, .1);	
			Cylinder(X,Y,temp_Z,R*.048,R*.07,R*.05,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			temp_Z+=R*.05;

			
			COL(.5, .1, .8);	
			Cylinder(X,Y,temp_Z,R*.07,R*.075,R*.25,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			temp_Z+=R*.25;
			
			
			COL(0, .6, 0);	
			Cylinder(X,Y,temp_Z,R*.075,R*.085,R*.05,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			temp_Z+=R*.05;
			
			
			COL(0, .6, 1);	
			Cylinder(X,Y,temp_Z,R*.085,R*.065,R*.03,8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			temp_Z+=R*.03;

		}
		glPopMatrix ();
		gombuj(X,Y,temp_Z,R*.12,0,0);

		
	}
	glPopMatrix ();
	
	
	X=savex;
	Y=savey;
	Z=savez;
		
	//shade
	glPushMatrix ();
	{
		glTranslatef(X+(R/2)+(R*.3),Y,0); 
		temp_Z=Z+R*1.25;
		
		
		COL(.2, .3, .3);//dark green
		glo_P_house[0]=PT(-(R*.3),-(R*.3),temp_Z);
		glo_P_house[1]=PT(-(R*.3),(R*.3),temp_Z);
		glo_P_house[2]=PT((R*.3),(R*.3),temp_Z);
		glo_P_house[3]=PT((R*.3),-(R*.3),temp_Z);
		house(1,1);
		
		
		
		COL(0, .34, .35);//dark green
		glo_P_house[0]=PT(-(R*.3),-(R*.3),temp_Z);
		glo_P_house[1]=PT((R*.3),-(R*.3),temp_Z);
		glo_P_house[2]=PT((R*.3),-(R*.3)-(R*.15),temp_Z-(R*.15));
		glo_P_house[3]=PT(-(R*.3),-(R*.3)-(R*.15),temp_Z-(R*.15));
		house(1,1);
		

		COL(0, .34, .5);//dark green
		glo_P_house[2]=PT((R*.3),-(R*.3)-(R*.15),temp_Z-(R*.15));
		glo_P_house[3]=PT(-(R*.3),-(R*.3)-(R*.15),temp_Z-(R*.15));
		glo_P_house[0]=PT(-(R*.3),-(R*.3)-(R*.15),temp_Z-(R*.5));
		glo_P_house[1]=PT((R*.3),-(R*.3)-(R*.15),temp_Z-(R*.2));
		plane(glo_P_house[0],glo_P_house[1],glo_P_house[2],glo_P_house[3]);
		


		
		COL(0, .34, .35);//dark green
		glo_P_house[0]=PT(-(R*.3),(R*.3),temp_Z);
		glo_P_house[1]=PT((R*.3),(R*.3),temp_Z);
		glo_P_house[2]=PT((R*.3),(R*.3)+(R*.15),temp_Z-(R*.15));
		glo_P_house[3]=PT(-(R*.3),(R*.3)+(R*.15),temp_Z-(R*.15));
		house(1,1);
		

		COL(0, .34, .5);//dark green
		glo_P_house[0]=PT(-(R*.3),(R*.3)+(R*.15),temp_Z-(R*.5));
		glo_P_house[1]=PT((R*.3),(R*.3)+(R*.15),temp_Z-(R*.2));
		glo_P_house[2]=PT((R*.3),(R*.3)+(R*.15),temp_Z-(R*.15));
		glo_P_house[3]=PT(-(R*.3),(R*.3)+(R*.15),temp_Z-(R*.15));
		plane(glo_P_house[0],glo_P_house[1],glo_P_house[2],glo_P_house[3]);
		
	}
	glPopMatrix ();

		

	
	X=savex;
	Y=savey;
	Z=savez;
	glPushMatrix ();
	{
		glTranslatef(X,Y-(R/2)-(R*.4),0); 
		temp_Z=Z+R*1.20;
		
		
		
		COL(.6, .6, .4);//main color
		glo_P_house[0]=PT((R*.6),-(R*.3),temp_Z-(R*.2)-1);
		glo_P_house[1]=PT((R*.6),(R*.3),temp_Z-1);
		glo_P_house[2]=PT(-(R*.6),(R*.3),temp_Z-1);
		glo_P_house[3]=PT(-(R*.6),-(R*.3),temp_Z-(R*.2)-1);
		house0(1);
			
		

		COL(.73, .6, .4);//main color(mati color)
		glo_P_house[0]=PT((R*.5),-(R*.3),(temp_Z-(R*.2)-1)/2);
		glo_P_house[1]=PT((R*.5),-2.2*(R*.3),(temp_Z-(R*.2)-1)/2);
		glo_P_house[2]=PT((R*.34),-2.2*(R*.3),(temp_Z-(R*.2)-1)/2);
		glo_P_house[3]=PT((R*.34),-(R*.3),(temp_Z-(R*.2)-1)/2);
		house0(0);
		
		
	}
	glPopMatrix ();
	


	
}





void stand_type_5(double X,double Y,double Z,double R)
{
	
	double temp_Z=Z;
	
	
	
	
	glPushMatrix ();
	{
		glTranslatef(X,Y,0); 
		glRotatef(45,0,0,1); 
		
		
		


		COL(.73, .6, .4);
		Cylinder(0,0,temp_Z,R,R,R*.15,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*0.15;
		
		COL(.63, .5, .4);
		Cylinder(0,0,temp_Z,R,0,0,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
	

		
		COL(.73, .6, .4);
		Cylinder(0,0,temp_Z,R*.9,R*.9,R*.27,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*0.27;
		
		COL(.63, .5, .4);
		Cylinder(0,0,temp_Z,R*.9,0,0,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
		

		COL(.73, .6, .4);
		Cylinder(0,0,temp_Z,R*.83,R*.83,R*.27,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*0.27;
		
		COL(.63, .5, .4);
		Cylinder(0,0,temp_Z,R*.83,0,0,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		

		
		COL(.73, .6, .4);
		Cylinder(0,0,temp_Z,R*.76,R*.76,R*.40,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*0.40;
		
		COL(.63, .5, .4);
		Cylinder(0,0,temp_Z,R*.76,0,0,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		

		
		COL(.8, .6, .5);
		Cylinder(0,0,temp_Z,R*.72,R*.68,R*.07,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*0.07;
		
		COL(.63, .5, .4);
		Cylinder(0,0,temp_Z,R*.68,0,0,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		

		
	
		COL(.73, .6, .4);
		Cylinder(0,0,temp_Z,R*.68,R*.68,R*.8,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*0.8;
		
		COL(.63, .5, .4);
		Cylinder(0,0,temp_Z,R*.68,0,0,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			
		

		COL(.8, .6, .5);
		Cylinder(0,0,temp_Z,R*.68,R*.76,R*.1,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*0.1;
		
		COL(.63, .5, .4);
		Cylinder(0,0,temp_Z,R*.76,0,0,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
		
		COL(.73, .6, .4);
		Cylinder(0,0,temp_Z,R*.68,R*.68,R*.8,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*0.8;
		
		
		COL(.63, .5, .4);
		Cylinder(0,0,temp_Z,R*.76,0,0,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
		
		COL(.63, .6, .4);
		Cylinder(0,0,temp_Z,R*.76,R*.85,R*.1,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*0.1;
		
		
		
		//COL(.73, .6, .4);
		COL(.8, .6, .5);//pink
		Cylinder(0,0,temp_Z,R*.85,R*.68,R*.14,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*0.14;

		
		COL(.8, .6, .5);//pink
		COL(.63, .5, .4);//brown
		Cylinder(0,0,temp_Z,R*.68,0,0,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		

		

		COL(.73, .6, .4);
		Cylinder(0,0,temp_Z,R*.68,R*.68,R*.14,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*0.14;

		


		//COL(.8, .6, .5);//pink
		//COL(.63, .5, .4);//brown
		

		COL(.63, .6, .4);
		Cylinder(0,0,temp_Z,R*.68,R*.85,R*.15,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*0.15;

		//COL(.73, .6, .4);
		COL(.8, .6, .5);//pink
		Cylinder(0,0,temp_Z,R*.85,R*.68,R*.17,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*0.17;

		
		COL(.8, .6, .5);//pink
		COL(.63, .5, .4);//brown
		Cylinder(0,0,temp_Z,R*.68,0,0,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		


		//COL(.8, .6, .5);//pink
		COL(.63, .5, .4);//brown
		Cylinder(0,0,temp_Z,R*.68,R*.68,0,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		

		
		COL(.73, .6, .4);
		Cylinder(0,0,temp_Z,R*.68,R*.68,R,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R;


	}
	glPopMatrix ();
	

}



void tower_type5(double X,double Y,double Z,double R)
{
	
	double temp_Z=Z;
	int i;
	

	
	
	glPushMatrix ();
	{
		glTranslatef(X,Y,0); 
		glScalef(.95,.95,1.1);
		
		double dx[4]={1,-1,1,-1};
		double dy[4]={1,-1,-1,1};
		

		
		for(i=0;i<4;i++)
		{
			stand_type_5(dx[i]*R*.53,dy[i]*R*.53,temp_Z,R*.3);
		}
	

		glRotatef(45,0,0,1); 
		
			
		
		
		for(i=0;i<3;i++)
		{
			
			COL(.2, .1, .1);//brown
			Cylinder(0,0,temp_Z,R*(1.05-i*(.2)),R*(1.05-i*(.2)),R*0.06,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			temp_Z+=R*0.06;
			
			COL(.9, .8, .7);//cream
			Cylinder(0,0,temp_Z,R*(1.05-i*(.2)),0,0,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		}
		
		
		
		temp_Z+=.89*R;

	
		
		
		//				X,Y,Z,			R				R_out,		R_in,	out_Height,	in_Height,slice
		ribbon_type7(0,0,temp_Z,		R*.38,			R*.25,		R*.12,		R*.3,		R*.3,        45,	2,	1);
		
	
			
		temp_Z+=R*.185;

		//COL(.63, .6, .4);
		//COL(.8, .6, .5);//gu calar
		//COL(.73, .5, .4);//brown
		COL(.73, .6, .4);
		Cylinder(0,0,temp_Z,R*.95,R*.95,R*.4,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*.4;

		
		

		
		COL(.2, .1, .1);
		Cylinder(0,0,temp_Z,R*1.05,0,0,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
		
		COL(.73, .6, .4);//main color
		glRotatef(45,0,0,1);
		ribbon_type8(0,0,temp_Z-R*.44,		 R*.535,		 R*.28,		1,			0);
		glRotatef(-45,0,0,1);


		
		

		COL(.2, .1, .1);
		Cylinder(0,0,temp_Z,R*1.05,R*1.05,R*0.05,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*0.05;


		COL(.2, .1, .1);
		Cylinder(0,0,temp_Z,R*1.05,0,0,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
		
		

		
		COL(.2, .1, .1);
		Cylinder(0,0,temp_Z,R*1.05,R*1.05,R*0.05,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*0.05;
		
		
		
		COL(.2, .3, .3);
		Cylinder(0,0,temp_Z,R*1.05,0,R*0.25,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		if(debug)Cylinder_Cage(0,0,temp_Z,R*1.05,0,R*0.25,4);
		temp_Z+=R*0.03;
		
		

		
		
		
		glPushMatrix ();
		{
			glRotatef(22.5,0,0,1);
			//void Solid_Cylinder2(double X,double Y,double Z,double R_out,double R_in,double out_Height,double in_Height,int slice)
			COL(.2, .1, .1);//dark brown
			Solid_Cylinder2(0,0,temp_Z,		R*.63,		R*.54,			R*0.15,			R*0.15,		8);
			temp_Z+=R*0.15;
			
			COL(.2, .3, .3);//dark green
			Cylinder(0,0,temp_Z,	R*.63,		0,		R*1.3,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			if(debug)Cylinder_Cage(0,0,temp_Z,	R*.63,		0,		R*1.3,		8);
			temp_Z+=R*1.28;
		
		}
		glPopMatrix ();
		
		
		
		COL(.2, .1, 0);//dark brown
		Cylinder(0,0,temp_Z,	R*.05,		R*.05,		R*.3,		4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*.3;

		COL(.2, .1, 0);//dark brown
		Cylinder(0,0,temp_Z,	R*.05,		0,		0,		4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*.3;
		
	
	}
	glPopMatrix ();


	
	
	
	
	glPushMatrix ();
	{
		X+=1.05*R;
		glTranslatef(X,Y,Z);
		glScalef(.95,.95,1.1);

		temp_Z=0;
		
		COL(.73, .6, .4);//main color
		box(0,0,0,R,1.3*R,2*R);

		
		glPushMatrix ();
		{
			glTranslatef(0,0,1.2*R);
			glRotatef(-25,0,1,0);
			
			box(0,0,0,R,1.3*R,R);
			
			glRotatef(45,0,0,1);

			COL(.2, .1, .1);
			Cylinder(0,0,temp_Z+R/2,R*.9,R*.9,R*0.05,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			temp_Z+=R*0.05;
			
			

			COL(.2, .3, .3);
			Cylinder(0,0,temp_Z+R/2,R*.9,0,0,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			temp_Z+=R*0.03;



			//now print the shed
		}
		glPopMatrix ();
	
		
		

		//COL(.2, .1, .1);//brown
		COL(.73, .6, .4);//mai
		glTranslatef(R*.8,0,0);
		box(0,0,R,R,1.3*R,2*R);
		
		
		glPushMatrix();
		{
			glRotatef(45,0,0,1);
			temp_Z=2*R;
		
			
			COL(.2, .1, .1);
			Cylinder(0,0,temp_Z,R*.92,R*.92,R*0.05,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			temp_Z+=R*0.05;
			
			
			
			COL(.2, .3, .3);
			Cylinder(0,0,temp_Z,R*.92,0,R*0.25,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			if(debug)Cylinder_Cage(0,0,temp_Z,R*.92,0,R*0.25,4);
			temp_Z+=R*0.03;
			
			
			
			glPushMatrix ();
			{
				glRotatef(22.5,0,0,1);
				//void Solid_Cylinder2(double X,double Y,double Z,double R_out,double R_in,double out_Height,double in_Height,int slice)
				COL(.2, .1, .1);//dark brown
				Solid_Cylinder2(0,0,temp_Z,		R*.63,		R*.54,			R*0.15,			R*0.15,		8);
				temp_Z+=R*0.15;
				
				COL(.2, .3, .3);//dark green
				Cylinder(0,0,temp_Z,	R*.63,		0,		R*1.3,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
				if(debug)Cylinder_Cage(0,0,temp_Z,	R*.63,		0,		R*1.3,		8);
				temp_Z+=R*1.28;
			
			}
			glPopMatrix ();
			
			COL(.2, .1, 0);//dark brown
			Cylinder(0,0,temp_Z,	R*.05,		R*.05,		R*.3,		4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			temp_Z+=R*.3;

			COL(.2, .1, 0);//dark brown
			Cylinder(0,0,temp_Z,	R*.05,		0,		0,		4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			temp_Z+=R*.3;

			

		}	
		glPopMatrix ();
		
		

		
	}
	glPopMatrix ();
	

	
}	





void tower_type6(double X,double Y,double Z,double R)
{
	
	double temp_Z=0;
	int i;
		
	

	glPushMatrix ();
	{
		glTranslatef(X,Y,Z); 
		glRotatef(22.5,0,0,1); 
		

		COL(.35, .25, .2);//dark brown
		//COL(.7, .6, .4);//main color
		Cylinder(0,0,0,	R,		R,		R*1.3,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		if(debug)Cylinder_Cage(0,0,0,	R,		R,		R*1.3,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*1.3;
		

		
		
		COL(.2, .1, 0);//dark brown
		Cylinder(0,0,temp_Z,	R,		0,		0,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
		
		COL(.73, .6, .4);//main color
		Cylinder(0,0,temp_Z,	R*.95,		R*.95,		R,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		if(debug) Cylinder_Cage(0,0,temp_Z,	R*.95,		R*.95,		R,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R;
		
		
		COL(.8, .6, .5);//pink
		Cylinder(0,0,temp_Z,	R*.95,		R*.98,		R*.05,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*.05;
		
		COL(.7, .6, .5);//pink
		Cylinder(0,0,temp_Z,	R*.98,		R*.98,		R*.05,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*.05;
		
		
		COL(.2, .1, 0);//dark brown
		Cylinder(0,0,temp_Z,	R*.98,		R,		0,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
		
		COL(.73, .6, .4);//main color
		Cylinder(0,0,temp_Z,	R*.98,		R*1.03,		R*.05,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*.05;
		
	
		COL(.73, .6, .5);//main color
		Cylinder(0,0,temp_Z,	0,		R*1.03,		0,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
		
		
		double sz=temp_Z;
		double temp_Z1=temp_Z;

		
		for(i=0;i<3;i++)
		{
			
			COL(.2, .1+i*.05, 0);//dark brown
			glRotatef(22.5,0,0,1);
			//							R,			R_out,		R_in, out_Height,		in_Height,ratate_ang,	quantity_of_cyl_per_side	,	interdist_of_cyls)
			ribbon_type2(0,0,temp_Z,  R*.8-i*R*.07,   R*.27,			0,		R*.1,			R*.1,        0,			1,								0);
			glRotatef(-22.5,0,0,1);
			
				
			
			COL(.2+i*.05, .3, .3);//dark green
			//							R,			R_out,		R_in,	out_Height,		in_Height,ratate_ang,	quantity_of_cyl_per_side	,	interdist_of_cyls)
			ribbon_type2(0,0,temp_Z1,  R*.87-i*R*.07,   R*.15,			0,		R*.1,			R*.1,        0,			1,								0);
			

			if(i!=2)
			{
				COL(.73-i*.15, .6, .4);//main color
				Cylinder(0,0,temp_Z,	R*.8-i*R*.09,		R*.8-i*R*.09,		R*.2,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			}
			temp_Z+=R*.2;
			temp_Z1+=R*.17;
			
			if(i!=2)
			{
				COL(.73, .6, .5);//main color
				Cylinder(0,0,temp_Z,	0,	R*.8-i*R*.09,		0,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			}
			
		}
		temp_Z-=R*.2;

	
		glPushMatrix ();
		for(i=0;i<2;i++)
		{
			COL(.73-i*.05, .6, .4);//main color
			Cylinder(0,0,temp_Z,	R*.72,		R*.72,		R*.7,		4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			Cylinder(0,0,temp_Z+R*.6,	R*.72,		0,		0,		4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			glRotatef(45,0,0,1);
			
		}
		glPopMatrix ();
		
		
		temp_Z+=R*.2;
			


		
		
		
		for(i=0;i<2;i++)
		{
			COL(.2, .3-i*.2, .3);//dark green


							//X0 Y0 Z0,		R,		size,	scale_val	,ratate_ang,	discs_per_side,		inter_dist_of_disc)
			ribbon_type9(0,0,temp_Z,	.6*R-i*R*.05,		R*.2,        1,				22.5,			2-i,				R*.1);
			temp_Z+=R*.1;
			
			//COL(.2, .1, 1);//dark brown
			//							R,		R_out,		R_in,	out_Height,	in_Height,	ratate_ang,	quantity_of_cyl_per_side	,	interdist_of_cyls)
			ribbon_type2(0,0,temp_Z,  .5*R-i*R*.05,		R*.1,			0,		R*.2,		R*.2,        22.5,			2-i,								R*.1);
			

			temp_Z+=R*.07;
			//COL(.2, .1, .5);//dark brown
						//X0 Y0 Z0,				R,				size,	scale_val	,ratate_ang,	discs_per_side,		inter_dist_of_disc)
			ribbon_type3(0,0,temp_Z,		.649*R-i*R*.05,		R*.1,        .75,			22.5,			2-i,				R*.11);
			

			temp_Z-=R*.07;
		}


		temp_Z+=R*.25;


		
		glPushMatrix ();

		double	 sv=temp_Z;
		for(i=0;i<2;i++)
		{
			temp_Z=sv;


			COL(.2, .3-i*.1, .3);//dark green
			Cylinder(0,0,temp_Z,		R*.72,		R*.78,		0,		4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			Cylinder(0,0,temp_Z,	R*.78,		R*.82,		R*.05,		4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			temp_Z+=R*.05;
			Cylinder(0,0,temp_Z,	R*.82,		R*.81,		R*.02,		4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			temp_Z+=R*.02;
			Cylinder(0,0,temp_Z,	R*.81,		R*.78,		R*.03,		4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			temp_Z+=R*.03;
			Cylinder(0,0,temp_Z,	R*.78,		0,		0,		4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			
			glRotatef(45,0,0,1);
		}
		glPopMatrix ();
		

		

		COL(.2, .25, .3);//dark green
		Cylinder(0,0,temp_Z,	R*.66,		R*.25,		1.1*R,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		if(debug) Cylinder_Cage(0,0,temp_Z,	R*.66,		R*.25,		1.1*R,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		

		double sv2=temp_Z+1.1*R;
		

		glPushMatrix ();
		{
			COL(.2, .1, 0);//dark brown
			glTranslatef(0,0,temp_Z); 
			glScalef(1,1,1.5);
			temp_Z=0;
			//							R,		R_out,			R_in,	out_Height,	in_Height,	ratate_ang,	quantity_of_cyl_per_side	,	interdist_of_cyls)
			ribbon_type2(0,0,temp_Z,  .40*R,		R*.085,			0,		R*.2,		R*.2,        22.5,			3,								0);
			
			ribbon_type2(0,0,temp_Z,  .40*R,		R*.085,			0,		R*.2,		R*.2,        22.5,			2,								0);
			temp_Z+=R*.07;
			ribbon_type2(0,0,temp_Z,  .40*R,		R*.085,			0,		R*.2,		R*.2,        22.5,			2,								0);
			ribbon_type2(0,0,temp_Z,  .40*R,		R*.085,			0,		R*.2,		R*.2,        22.5,			1,								0);
			
			temp_Z+=R*.075;
			ribbon_type2(0,0,temp_Z,  .40*R,		R*.10,			0,		R*.2,		R*.2,        22.5,			1,								0);
			temp_Z+=R*.05;
			

			ribbon_type3(0,0,temp_Z,		.53*R,		R*.12,        1.2,			22.5,			1,				0);		
		}
		glPopMatrix ();

		






		temp_Z=sv2;
		COL(.18, .25, .3);//dark green
		Cylinder(0,0,temp_Z,	R*.25,		R*.25,		.2*R,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		if(debug) Cylinder_Cage(0,0,temp_Z,	R*.25,		R*.25,		.2*R,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
		temp_Z+=.2*R;
		
		
		COL(.16, .25, .3);//dark green
		Cylinder(0,0,temp_Z,	R*.25,		R*.30,		.2*R,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		if(debug) Cylinder_Cage(0,0,temp_Z,	R*.25,		R*.30,		.2*R,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
		temp_Z+=.2*R;

		

		COL(.16, .25, .3);//dark green
		Cylinder(0,0,temp_Z,	R*.30,		0,		0,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		

	


		

		COL(.2, .1, 0);//dark brown
		//							R,		R_out,			R_in,	out_Height,	in_Height,	ratate_ang,	quantity_of_cyl_per_side	,	interdist_of_cyls)
		
		glPushMatrix ();
		{
			glTranslatef(0,0,temp_Z); 
			glScalef(1,1,1.4);
			ribbon_type2(0,0,0,  .2*R,		R*.095,			0,		R*.07,		R*.07,        22.5,			1,								0);
		}
		glPopMatrix ();

		
		
		COL(.16, .25, .3);//dark green
		Cylinder(0,0,temp_Z,	R*.23,		R*.26,		.45*R,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		if(debug)Cylinder_Cage(0,0,temp_Z,	R*.23,		R*.26,		.45*R,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
		temp_Z+=.45*R;
		

		COL(.2, .1, 0);//dark brown
		Cylinder(0,0,temp_Z,	R*.26,		R*.28,		.07*R,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=.07*R;
		
		COL(.1, .25, .3);//dark green
		Cylinder(0,0,temp_Z,	R*.28,		0,		0,		8);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
		
		gombuj(0,0,temp_Z,R*.27,0,0);


	}
	glPopMatrix ();

}












void stand_type_7(double X,double Y,double Z,double R)
{
	
	double temp_Z=Z;
	
	
	
	
	glPushMatrix ();
	{
		glTranslatef(X,Y,0); 
		glRotatef(45,0,0,1);
		glScalef(1,1,1.2);
		
		
		
		COL(.8, .6, .5);//pink
		Cylinder(0,0,temp_Z,R*.68,R*.68,R*2.5,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*2.5;
		
		
		
		COL(.63, .6, .1);
		Cylinder(0,0,temp_Z,R*.68,R*.85,R*.15,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*0.15;

		//COL(.73, .6, .4);
		COL(.8, .6, .5);//pink
		Cylinder(0,0,temp_Z,R*.85,R*.68,0,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
			
		
		
		COL(.7, .6, .5);
		Cylinder(0,0,temp_Z,R*.68,R*.68,2*R,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=2*R;


	}
	glPopMatrix ();
	

}







void tower_type7(double X,double Y,double Z,double R)
{
	
	double temp_Z=Z;
	int i;
	
	

	double dx[4]={1,-1,1,-1};
	double dy[4]={1,-1,-1,1};
	

	glPushMatrix ();
	{
		glScalef(1,1,1.25);
		for(i=0;i<4;i++)
		{
			stand_type_7(dx[i]*R*.94,dy[i]*R*.62,temp_Z,R*.15);
		}
	}
	glPopMatrix ();
		
	

	glPushMatrix ();
	{
		glTranslatef(X,Y,0); 
		glScalef(1.37,.95,1.1);
		
		
		

		glRotatef(45,0,0,1); 
		
		
		
		temp_Z+=.13*R;

		
		//				X,Y,Z,			R				R_out,		R_in,	out_Height,	in_Height,slice
		//ribbon_type7(0,0,temp_Z+10,		R*.38,			R*.25,		R*.12,		R*.3,		R*.3,        45,	2,	1);
		
		temp_Z+=R*.4;

		COL(.73, .6, .5);
		//Cylinder(0,0,temp_Z,R*.95,R*.95,R*.4,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*.4;

		
		

		
		COL(.2, .1, .1);
		Cylinder(0,0,temp_Z,R*1.05,0,0,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		

		
		COL(.73, .6, .4);//main color
		glRotatef(45,0,0,1);
		//ribbon_type8(0,0,temp_Z-R*.44,		 R*.535,		 R*.28,		1,			0);
		glRotatef(-45,0,0,1);

		
		
		

		COL(.2, .1, .1);
		Cylinder(0,0,temp_Z,R*1.05,R*1.05,R*0.05,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*0.05;

	
	
		COL(.2, .1, .1);
		Cylinder(0,0,temp_Z,R*1.05,0,0,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		
		
		

		
		
		
		COL(.2, .3, .3);
		Cylinder(0,0,temp_Z,R*1.05,0,R*0.25,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		if(debug)Cylinder_Cage(0,0,temp_Z,R*1.05,0,R*0.25,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		temp_Z+=R*0.03;
		
		
	
	}
	glPopMatrix ();


	
	
	

	
}





//tower 1,6 only depends on size
//tower 3 depends on Z_sz
void base1(double X,double Y,double Z_sz,double size)
{
	
	glPushMatrix();
	{
		glTranslatef(X,Y,0);
		COL(.6, .6, .4);//main color
		box(0,0,Z_sz/2,size,size,Z_sz);
	
		double temp_Z=Z_sz;
		
		X=0;
		Y=0;
		
		
		
		double R=Z_sz*.507;
		
		size/=2;

		glPushMatrix ();
		{
			glRotatef(45,0,0,1);
			
			COL(.17, .25, .3);
			Cylinder(X,Y,temp_Z,size+R*1.04,0,0,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			

			COL(.17, .25, .3);
			Cylinder(X,Y,temp_Z,size+R*1.04,size+R*.93,4.5*(R/60),4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			temp_Z+=4.5*(R/60);
				
			
			
			
			COL(.2, .2, .1);
			Cylinder(X,Y,temp_Z,size+R*.93,size+R*.89,5*(R/60),4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			temp_Z+=5*(R/60);


			COL(.1, .2, .1);//khaista 
			Cylinder(X,Y,temp_Z,size+R*.89,size+R*.95,7.5*(R/60),4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			temp_Z+=7.5*(R/60);
			
			

			COL(.1, .2, .5);
			Cylinder(X,Y,temp_Z,size+R*.95,size+1.04*R,2.5*(R/60),4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			temp_Z+=2.5*(R/60);
			
			
			COL(.2, .3, .3);//dark green
			Cylinder(X,Y,temp_Z,size+1.04*R,size+R*.87,4*(R/60),4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
			temp_Z+=4*(R/60);
			
			COL(.9, .8, .7);//cream
			Cylinder(X,Y,temp_Z,size+R*.87,0,0,4);//void Cylinder(double X,double Y,double Z,double R_high,double R_low,double Height,int slice)
		}
		size*=2;
		glPopMatrix();
		double slab=temp_Z-Z_sz;
		
		



		glPushMatrix ();
		{
			glRotatef(22.5+45+90,0,0,1);
			tower_type6(0,0,temp_Z,size*.29);
		}
		glPopMatrix();
		
		
		

		double dx[4]={1,1,-1,-1};
		double dy[4]={1,-1,1,-1};
		int i;

		for(i=0;i<4;i++)
		{
			tower_type1(dx[i]*size*.32,dy[i]*size*.32,temp_Z,size*.17);	
		}



		double dx1[4]={1,-1,0,0};
		double dy1[4]={0,0,1,-1};
		
		

		
		for(i=0;i<4;i++)
		{
			if(i==2) tower_type3(dx1[i]*size*.58,dy1[i]*size*.58,0,R);
			else tower_type2(dx1[i]*size*.58,dy1[i]*size*.58,0,R);	
		}
		

		double cc=size*.49;
		
		
	
	
		glPushMatrix ();
		{
			glTranslatef(-cc,-cc,0);

			double red=.3*slab;
			
			
			COL(.73, .6, .3);//main color
			glo_P_house[0]=PT(0,0,Z_sz+slab*.4);
			glo_P_house[1]=PT(size*.4,0,Z_sz+slab*.4);
			glo_P_house[2]=PT(size*.4,-size*.06,Z_sz-red);
			glo_P_house[3]=PT(-size*.07,-size*.06,Z_sz-red);
			house0(1);
			
			
			COL(.73, .6, .4);//main color
			glo_P_house[0]=PT(0,0,Z_sz+slab*.7-red);
			glo_P_house[1]=PT(0,size*.4,Z_sz+slab*.7-red);
			glo_P_house[2]=PT(-size*.06,size*.4,Z_sz-red);
			glo_P_house[3]=PT(-size*.07,-size*.06,Z_sz-red);
			house0(2);
			
			
			COL(.73, .6, .4);//main color
			glo_P_house[0]=PT(0,size*.18,Z_sz+slab*.7-red);
			glo_P_house[1]=PT(0,size*.93,Z_sz+slab*.7-red);
			glo_P_house[2]=PT(-size*.18,size*.93,Z_sz-red);
			glo_P_house[3]=PT(-size*.18,size*.1,Z_sz-red);
			house0(2);
				
			
			
			COL(.73, .6, .3);//main color
			glo_P_house[0]=PT(0,size*.18,Z_sz+slab*.7-red);
			glo_P_house[1]=PT(0,size*.1,Z_sz-red);
			glo_P_house[2]=PT(0,size*.1,Z_sz-red);
			glo_P_house[3]=PT(-size*.18,size*.1,Z_sz-red);
			house0(1);
			
			
   
			
			COL(.73, .6, .3);//main color
			glo_P_house[0]=PT(-size*.07,-size*.06,Z_sz/2-red+slab*.7);
			glo_P_house[1]=PT(-size*.07,size*.1,Z_sz/2-red+slab*.7);
			glo_P_house[2]=PT(-size*.18,size*.1,Z_sz/2-red);
			glo_P_house[3]=PT(-size*.18,-size*.06,Z_sz/2-red);
			house0(1);

			



			
			glPushMatrix ();
			{
				glTranslatef(-size*.1,-size*.42,0);
				glRotatef(171,0,0,1);
				glScalef(.9,.9,1);
				
				tower_type4(0,0,0,60);
			}
			glPopMatrix ();

			





			//Solid_Cylinder2(X,Y,				Z,				R_out,	R_in,	out_Height,in_Height,	slice)
			COL(.73, .7, .4);//main color
			Solid_Cylinder2(+size*.18,-size*.06,	0,		size*.12,		0,	Z_sz*.4,	Z_sz*.4,	16);
			COL(.2, .1, .1);//brown
			Solid_Cylinder2(+size*.18,-size*.06,Z_sz*.4,	size*.13,		0,	2,	2,	16);
			COL(.2, .3, .3);//dark green
			Solid_Cylinder(+size*.18,-size*.06,Z_sz*.4+2,	0,		size*.13		,	10,	16);
			
				

		
			

			COL(.73, .6, .4);//main color
			glo_P_house[0]=PT(0,size*.93,Z_sz+slab*.7-red);
			glo_P_house[1]=PT(-size*.18,size*.93,Z_sz-red);
			glo_P_house[2]=PT(-size*.18,size*.93+slab*.9,Z_sz-red-slab*.7);
			glo_P_house[3]=PT(0,size*.93+slab*.9,Z_sz-red);
			house0(1);
			
				
			
			
			COL(.73, .6, .4);//main color
			glo_P_house[0]=PT(size*.30,size*.93+slab*.7,Z_sz-red+slab*.7);
			glo_P_house[1]=PT(size*.1,size*.93+slab*.7,Z_sz-red+slab*.7);
			glo_P_house[2]=PT(size*.1,size*.93+slab*2.8,Z_sz-red);
			glo_P_house[3]=PT(size*.30,size*.93+slab*2.8,Z_sz-red);
			house0(2);
			
				
			
			COL(.73, .6, .4);//main color
			glo_P_house[0]=PT(size*.1,size*.93+slab*.7,Z_sz-red+slab*.7);
			glo_P_house[1]=PT(size*.1,size*.93+slab*2.8,Z_sz-red);
			glo_P_house[2]=PT(-size*.05,size*.93+slab*2.8,Z_sz-red-slab*.7);
			glo_P_house[3]=PT(-size*.05,size*.93+slab*.7,Z_sz-red+slab*.7-slab*.7);
			house0(1);
			
		
			
			double shift=237;
			
			COL(.73, .6, .4);//main color
			glo_P_house[0]=PT(shift,size*.93,Z_sz+slab*.7-red);
			glo_P_house[1]=PT(shift+size*.18,size*.93,Z_sz-red);
			glo_P_house[2]=PT(shift+size*.18,size*.93+slab*.9,Z_sz-red-slab*.7);
			glo_P_house[3]=PT(shift,size*.93+slab*.9,Z_sz-red);
			house0(1);


			COL(.73, .6, .4);//main color
			glo_P_house[0]=PT(shift-size*.30,size*.93+slab*.7,Z_sz-red+slab*.7);
			glo_P_house[1]=PT(shift-size*.1,size*.93+slab*.7,Z_sz-red+slab*.7);
			glo_P_house[2]=PT(shift-size*.1,size*.93+slab*2.8,Z_sz-red);
			glo_P_house[3]=PT(shift-size*.30,size*.93+slab*2.8,Z_sz-red);
			house0(2);
			
				
			
			COL(.73, .6, .4);//main color
			glo_P_house[0]=PT(shift-size*.1,size*.93+slab*.7,Z_sz-red+slab*.7);
			glo_P_house[1]=PT(shift-size*.1,size*.93+slab*2.8,Z_sz-red);
			glo_P_house[2]=PT(shift+size*.05,size*.93+slab*2.8,Z_sz-red-slab*.7);
			glo_P_house[3]=PT(shift+size*.05,size*.93+slab*.7,Z_sz-red+slab*.7-slab*.7);
			house0(1);


		}
		glPopMatrix ();
		
		
		
	

		

		glPushMatrix ();
		{
			glTranslatef(1.38*cc,-cc,0);
			double red=.3*slab;
			
			COL(.73, .6, .4);//main color
			glo_P_house[0]=PT(0,size*.58,Z_sz-red);
			glo_P_house[1]=PT(0,size*.93,Z_sz-red);
			glo_P_house[2]=PT(-size*.18,size*.93,Z_sz-red+slab*.7);
			glo_P_house[3]=PT(-size*.18,size*.58,Z_sz-red+slab*.7);
			house0(2);
			


		}
		glPopMatrix ();
		
		
		
		double m1=1.8,m2=1.19;
		glPushMatrix ();
		{
			glTranslatef(-m1*cc,m2*cc,0);
			tower_type5(0,0,0,.35*Z_sz);//x,y,z,R
		}
		glPopMatrix ();
		

		glPushMatrix ();
		{
			glTranslatef(m1*cc,m2*cc,0);
			glRotatef(180,0,0,1);
			tower_type5(0,0,0,.35*Z_sz);//x,y,z,R
		}
		glPopMatrix ();

		

		glPushMatrix ();
		{
			glTranslatef(cc,-cc,0);

			
			COL(.73, .6, .4);//main color
			glo_P_house[0]=PT(-13,0,Z_sz-slab*.9);
			glo_P_house[1]=PT(10,-70,Z_sz-slab*1.8);
			glo_P_house[2]=PT(-98,-70,Z_sz-slab*1.8);
			glo_P_house[3]=PT(-98,0,Z_sz-slab*.9);
			house0(2);
			
			
			COL(.73, .6, .4);//main color
			glo_P_house[0]=PT(-13,0,Z_sz-slab*.9);
			glo_P_house[1]=PT(10,-70,Z_sz-slab*1.8);
			glo_P_house[2]=PT(10,-70,Z_sz-slab*1.8);
			glo_P_house[3]=PT(10,0,Z_sz-slab*1.8);
			house0(1);
			
		
			tower_type_kata(-35,-30,Z_sz-slab*3.3,size*.13);
			
		


			COL(.73, .6, .4);//main color
			glo_P_house[0]=PT(41,72,Z_sz*.97);
			glo_P_house[1]=PT(21.5,72-20,Z_sz*.85+slab);
			glo_P_house[2]=PT(21.5,-58+20,Z_sz*.85+slab);
			glo_P_house[3]=PT(41,-58,Z_sz*.97);
			house0(2);
	

			
			COL(.73, .6, .4);//main color
			glo_P_house[0]=PT(21.5,72-20,Z_sz*.85+slab);
			glo_P_house[1]=PT(2,72,Z_sz*.97);
			glo_P_house[2]=PT(2,-58,Z_sz*.97);
			glo_P_house[3]=PT(21.5,-58+20,Z_sz*.85+slab);
			house0(1);
			
			
			
			COL(.73, .6, .4);//main color
			glo_P_house[0]=PT(41,72,Z_sz*.97);
			glo_P_house[1]=PT(21.5,72-20,Z_sz*.85+slab);
			glo_P_house[2]=PT(2,72,Z_sz*.97);
			glo_P_house[3]=PT(2,72,Z_sz*.97);
			house0(2);
			
			COL(.73, .6, .4);//main color
			glo_P_house[0]=PT(21.5,-58+20,Z_sz*.85+slab);
			glo_P_house[1]=PT(2,-58,Z_sz*.97);
			glo_P_house[2]=PT(41,-58,Z_sz*.97);
			glo_P_house[3]=PT(41,-58,Z_sz*.97);
			house0(2);
	
			
			COL(.73, .6, .4);//main color
			glo_P_house[0]=PT(21.5,-58+20,Z_sz*.85+slab);
			glo_P_house[1]=PT(2,-58,Z_sz*.97);
			glo_P_house[2]=PT(41,-58,Z_sz*.97);
			glo_P_house[3]=PT(41,-58,Z_sz*.97);
			house0(2);
	
			

			double shift1=3;
			glo_P_house[0]=PT(shift1-120,-70,Z_sz*.48);
			glo_P_house[1]=PT(shift1-120,-80,Z_sz*.48);
			glo_P_house[2]=PT(shift1-103,-80,Z_sz*.4);
			glo_P_house[3]=PT(shift1-103,-70,Z_sz*.4);
			house(1,2);
			
			

			glo_P_house[0]=PT(shift1-120,-70,Z_sz*.48);
			glo_P_house[1]=PT(shift1-120,-80,Z_sz*.48);
			glo_P_house[2]=PT(shift1-137,-80,Z_sz*.4);
			glo_P_house[3]=PT(shift1-137,-70,Z_sz*.4);
			house(1,2);
			
			
		


			COL(.2, .3, .3);//dark green
			glo_P_house[0]=PT(shift1-103,-70,Z_sz*.2);
			glo_P_house[1]=PT(shift1-103,-80,Z_sz*.2);
			glo_P_house[2]=PT(shift1-103,-80,Z_sz*.4);
			glo_P_house[3]=PT(shift1-103,-70,Z_sz*.4);
			plane(glo_P_house[0],glo_P_house[1],glo_P_house[2],glo_P_house[3]);
			
			

			
			COL(.2, .3, .3);//dark green
			glo_P_house[0]=PT(shift1-137,-70,Z_sz*.2);
			glo_P_house[1]=PT(shift1-137,-80,Z_sz*.2);
			glo_P_house[2]=PT(shift1-137,-80,Z_sz*.4);
			glo_P_house[3]=PT(shift1-137,-70,Z_sz*.4);
			plane(glo_P_house[0],glo_P_house[1],glo_P_house[2],glo_P_house[3]);
			


					
			double equ[4];
			equ[0] = -1;	//0.x
			equ[1] = 0;	//0.y
			equ[2] = 0;//-1.z
			equ[3] = 14;//0
			
			
			glPushMatrix ();
			{
				glClipPlane(GL_CLIP_PLANE0,equ);//now we enable the clip plane
				glEnable(GL_CLIP_PLANE0);
				{
					glTranslatef(-30,-68,0);
					glRotatef(18,0,0,1);
					
					
					double equ1[4];
					equ1[0] = 1;	//0.x
					equ1[1] = 0;	//0.y
					equ1[2] = 0;//-1.z
					equ1[3] = 50;//0
					

					glPushMatrix ();
					{
						glClipPlane(GL_CLIP_PLANE1,equ1);//now we enable the clip plane
						glEnable(GL_CLIP_PLANE1);
						{
								
								
							
							COL(.73, .5, .3);//main color
							Solid_Cylinder(0,0,0,55,55,40,40);
							COL(.2, .1, .1);//brown
							Solid_Cylinder(0,0,40,57,57,2,40);
							COL(.2, .3, .3);//dark green
							Cylinder(0,0,42,57,0,10,40);
							if(debug)Cylinder_Cage(0,0,42,57,0,10,40);





						}	
						glDisable(GL_CLIP_PLANE1);
					}
					glPopMatrix ();
					
					
					
				
					shift1=40;
					//shift1+=30;
					double shift2=83;
					COL(.2, .1, .1);//brown
					glo_P_house[0]=PT(shift1-90,shift2-70,40);
					glo_P_house[1]=PT(shift1-90,shift2-110.3,40);
					glo_P_house[2]=PT(shift1-90,shift2-110.3,42);
					glo_P_house[3]=PT(shift1-90,shift2-70,43);
					plane(glo_P_house[0],glo_P_house[1],glo_P_house[2],glo_P_house[3]);
					


					COL(.73, .5, .3);//main color
					glo_P_house[0]=PT(shift1-90,shift2-70,0);
					glo_P_house[1]=PT(shift1-90,shift2-106,0);
					glo_P_house[2]=PT(shift1-90,shift2-106,40);
					glo_P_house[3]=PT(shift1-90,shift2-70,40);
					plane(glo_P_house[0],glo_P_house[1],glo_P_house[2],glo_P_house[3]);
					
					
				
					
					
					
				
				}
				glDisable(GL_CLIP_PLANE0);
			}
			glPopMatrix ();

		
			
			
			
			equ[0] = -1;	//0.x
			equ[1] = 0;	//0.y
			equ[2] = 0;//-1.z
			equ[3] = 41;//0
			glPushMatrix ();
			{
				glClipPlane(GL_CLIP_PLANE0,equ);//now we enable the clip plane
				glEnable(GL_CLIP_PLANE0);
				{
					glTranslatef(10,-60,0);
					




					double equ1[4];
					equ1[0] = 1;	//0.x
					equ1[1] = 0;	//0.y
					equ1[2] = 0;//-1.z
					equ1[3] = 0;//0
					

					glPushMatrix ();
					{
						glClipPlane(GL_CLIP_PLANE1,equ1);//now we enable the clip plane
						glEnable(GL_CLIP_PLANE1);
						{
								
								
							COL(.73, .6, .4);//main color
							Solid_Cylinder(0,0,0,41.5,41.5,40,40);
							COL(.2, .1, .1);//brown
							Solid_Cylinder(0,0,40,43.5,43.5,2,40);
							COL(.2, .3, .4);//dark green
							Solid_Cylinder(0,0,42,0,43.5,10,40);
							if(debug) Cylinder_Cage(0,0,42,43,0.5,10,40);
						}	
						glDisable(GL_CLIP_PLANE1);
					}
					glPopMatrix ();

				
				}
				glDisable(GL_CLIP_PLANE0);
			}
			glPopMatrix ();
			
			
			
			
			shift1=131;
			//shift1+=30;
			double shift2=12;
			
			COL(.2, .1, .1);//brown
			glo_P_house[0]=PT(shift1-90,shift2-70,40);
			glo_P_house[1]=PT(shift1-90,shift2-102.8,40);
			glo_P_house[2]=PT(shift1-90,shift2-102.8,42);
			glo_P_house[3]=PT(shift1-90,shift2-70,45);
			plane(glo_P_house[0],glo_P_house[1],glo_P_house[2],glo_P_house[3]);
			


			COL(.73, .6, .4);//main color
			glo_P_house[0]=PT(shift1-90,shift2-70,0);
			glo_P_house[1]=PT(shift1-90,shift2-99.7,0);
			glo_P_house[2]=PT(shift1-90,shift2-99.7,40);
			glo_P_house[3]=PT(shift1-90,shift2-70,40);
			plane(glo_P_house[0],glo_P_house[1],glo_P_house[2],glo_P_house[3]);
			


			
			shift1=100;
			shift2=0;
			COL(.2, .3, .4);//dark green
			glo_P_house[0]=PT(shift1-90,shift2-70,40);
			glo_P_house[1]=PT(shift1-90,shift2-103,40);
			glo_P_house[2]=PT(shift1-90,shift2-103,42);
			glo_P_house[3]=PT(shift1-90,shift2-70,50);
			plane(glo_P_house[0],glo_P_house[1],glo_P_house[2],glo_P_house[3]);
			
			
			
			COL(.73, .6, .4);//main color
			glo_P_house[0]=PT(shift1-90,shift2-70,0);
			glo_P_house[1]=PT(shift1-90,shift2-99.7,0);
			glo_P_house[2]=PT(shift1-90,shift2-99.7,40);
			glo_P_house[3]=PT(shift1-90,shift2-70,40);
			plane(glo_P_house[0],glo_P_house[1],glo_P_house[2],glo_P_house[3]);
			
			
			shift2=12;
			shift1=131;
			COL(.2, .3, .4);//dark green
			glo_P_house[0]=PT(shift1-90,shift2-71,0);
			glo_P_house[1]=PT(shift1-130,shift2-71,0);
			glo_P_house[2]=PT(shift1-130,shift2-71,52);
			glo_P_house[3]=PT(shift1-90,shift2-71,45);
			plane(glo_P_house[0],glo_P_house[1],glo_P_house[2],glo_P_house[3]);
			
			
			shift1=135;
			COL(.2, .3, .4);//dark green
			glo_P_house[0]=PT(shift1-120.2,shift2-82,0);
			glo_P_house[1]=PT(shift1-120.2,shift2-71,0);
			glo_P_house[2]=PT(shift1-120.2,shift2-71,49);
			glo_P_house[3]=PT(shift1-120.2,shift2-82,45);
			plane(glo_P_house[0],glo_P_house[1],glo_P_house[2],glo_P_house[3]);
			
			
			
			glPushMatrix ();
			{
				glTranslatef(15,-10,0);
				tower_type7(0,0,0,60);
				
				
				glScalef(1,1,.88);
				COL(.73, .6, .4);//main color
				////	X0,Y0,Z0,	R_out,	R_in,		Height,	Width,	ratate_ang)
				cave(56,0,36		,38,	33.5,			35,		9,		0);
			}
			glPopMatrix ();
			

			glPushMatrix ();
			{
				glTranslatef(43,-10,0);
				
				
				
				COL(.73, .6, .4);//main color
				cave(19,-36.5,36		,7,		6,			28,		9,		270);
				COL(.73, .6, .4);//main color
				cave(19-2*7,-36.5,36		,7,		6,			28,		9,		270);
				
				COL(.73, .6, .4);//main color
				cave(19,36.5,36		,7,		6,			28,		9,		90);
				COL(.73, .6, .4);//main color
				cave(19-2*7,36.5,36		,7,		6,			28,		9,		90);
				
			}
			glPopMatrix ();
			

			


		}
		glPopMatrix ();
		


	}
	glPopMatrix ();

	

	
	
	

	
}




vector <PT> P;


void house_base()
{
	int i;
	vector<PT> P2;
	PT p1,p2;
	

	for(i=0;i<P.size();i++)
	{
		P2.push_back(P[i]);
		

	
		P[i].z=-.2;
		P2[i].z=-50;
	}

	
	int j=P.size()-1;
	for(i=0;i<P.size();i++,j--)
	{
		COL(.58 ,.58 ,.66);
		plane(P[i],P[i+1],P[j-1],P[j]);
		plane(P2[i],P2[i+1],P2[j-1],P2[j]);
		
		if(i>j) break;
	}
	
	
	int n=P.size();
	for(i=0;i<P.size();i++)
	{
		COL(.58 ,.58 ,.66);
		plane(P[i],P2[i],P2[(i+1)%n],P[(i+1)%n]);
	}
	


	double R=45,R1,now=0;
	double ang;
	vector<PT> PP;
	int k;
	for(i=0;i<P.size();i++)
	{
		R1=sqrt( (P[i].x-P[(i+1)%n].x)*(P[i].x-P[(i+1)%n].x) + (P[i].y-P[(i+1)%n].y)*(P[i].y-P[(i+1)%n].y) );
		ang=atan2(-(P[i].y-P[(i+1)%n].y),-(P[i].x-P[(i+1)%n].x));
		p1=P[i];
		

		//
		

		//cout<<R1<<" "<<ang<<endl;
		
		//for(k=0;k<10;k++)

		now=0;
		while(now<R1)
		{
			PP.push_back(p1);
			p1.x+=R*cos(ang);
			p1.y+=R*sin(ang);
			now+=R;
		}

		
	}
	



	


	P=PP;
	n=P.size();


	for(i=0;i<P.size();i++)
	{
		if(P[i].x>=672) P[i].x-=3;
		else P[i].x+=3;

		if(P[i].y>=216) P[i].y-=3;
		else P[i].y+=3;	
	}
	
	for(i=0;i<P.size();i++)
	{
		wall_post(P[i].x,P[i].y,P[i].z,4);
		
		if(i%2) COL(.9, .8, .7);//cream
		else COL(.3, .1, .1);//brown

		
		p1=P[i];
		p2=P[(i+1)%n];
		
		P[i].z+=.5;
		P[(i+1)%n].z+=.5;
		p1.z+=11;
		p2.z+=11;
		
		
		plane(P[i],P[(i+1)%n],p2,p1);

		
		P[i].z-=.5;
		P[(i+1)%n].z-=.5;
	}


	
}






void base2()
{
	int i;
	P.clear();


	double R=120;

	
	P.push_back(PT(R*1.84,R*4.42,0));
	P.push_back(PT(R*2.1,R*4.76,0));
	P.push_back(PT(R*2.64,R*4.86,0));
	P.push_back(PT(R*10.28,R*3.92,0));
	P.push_back(PT(R*10.56,R*3.7,0));//E
	P.push_back(PT(R*10.72,R*2.7,0));//F
	P.push_back(PT(R*10.64,R*2.18,0));//G
	P.push_back(PT(R*10.06,R*1,0));//H
	P.push_back(PT(R*9.5, R*0.34, 0));// kk point1
	P.push_back(PT(R*7.2,R*-0.75,0));
	P.push_back(PT(R*6,R*-1.25,0));//I
	P.push_back(PT(R*4.63,R*-1.59,0));//N
	P.push_back(PT(R*3.01,R*-1.57,0));//M
	P.push_back(PT(R*1,R*-1.16,0));//L
	P.push_back(PT(R*.68,R*-.93,0));//K
	P.push_back(PT(R*.6,R*-.32,0));//J
	
	

	
	glPushMatrix();
	{
		glTranslatef(-700,-250,0);
		glScalef(.95,1,1);

		house_base();	
	}
	glPopMatrix();
}












void INI()
{
	int i;
	kata_rotate_mult[0]=0;
	for(i=1;i<7;i++)
	{
		kata_rotate_mult[i]=(45-kata_rotate_mult[i-1])*.2+kata_rotate_mult[i-1];
	}
}









int UU,II,OO;



void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(BLACK, 0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera is looking?
	//3. Which direction is the camera's UP direction?

	//instead of CONSTANT information, we will define a circular path.
	//gluLookAt(0,0,500,	0,0,0,	0,0,1);
	

	if(birdy)
	{
		gluLookAt(cameraX,cameraY,cameraZ+500,cameraX,cameraY,cameraZ,0,1,0);	
	}
	else
	{
		if(rotate_YES)
		{
			gluLookAt(cameraRadius*cos(cameraAngle), cameraRadius*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
		}
		else
		{
			gluLookAt(cameraX,cameraY,cameraZ,lookX,lookY,lookZ,0,0,1);
		}
	}
	
	
	

	
	
	
	//NOTE: the camera still CONSTANTLY looks at the center
	// cameraAngle is in RADIAN, since you are using inside COS and SIN
	
	
	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);





	/****************************
	/ Add your objects from here
	****************************/



	//floor
	glPushMatrix();
	{	//STORE the state
		//a simple rectangles
	
		///lets draw another one in the XY plane --- i.e. Z = 0


		COL(1,1,1);
		glBegin(GL_QUADS);
		{
			glVertex3f(-1000,-1000,-20);
			glVertex3f(-1000,1000,-20);
			glVertex3f(1000,1000,-20);
			glVertex3f(1000,-1000,-20);
		}glEnd();
	}glPopMatrix();		//the effect of rotation is not there now.
	


	

	
	
	
	


	//some gridlines along the field
	int i;

	//WILL draw grid IF the "canDrawGrid" is true:
	COL(0.3, 0.3, 0.3);
	if(canDrawGrid == true)
	{
		glBegin(GL_LINES);
		{
			for(i=-60;i<=60;i++)
			{

				if(i==0)
					continue;	//SKIP the MAIN axes
				
				//lines parallel to Y-axis
				glVertex3f(i*10, -1000, -20);
				glVertex3f(i*10,  1000, -20);

				//lines parallel to X-axis
				glVertex3f(-1000, i*10, -20);
				glVertex3f( 1000, i*10, -20);
			}
		}glEnd();
	}
	


	
	// draw the two AXES
	COL(1, 1, 1);	//100% white
	glBegin(GL_LINES);{
		//Y axis
		glVertex3f(0, -150, 0);	// intentionally extended to -150 to 150, no big deal
		glVertex3f(0,  150, 0);

		//X axis
		glVertex3f(-150, 0, 0);
		glVertex3f( 150, 0, 0);
	}glEnd();

	
	INI();
	
	COL(.2+i*.05, .3, .3);//dark green
	

	//gombuj(0,0,30,20);//x,y,z,R



	//tower_type1(0,60,0,60);//x,y,z,R
	//tower_type1(0,-40,0,20);//x,y,z,R
	//tower_type1(0,100,0,10);//x,y,z,R
	
	//tower_type2(0,100,0,10);//x,y,z,R
	
	
	
	

	//		(x,y,z)			radius,	 size_of_cube	scaled_up	rotate_about_z_axis
	//ribbon_type4(0,0,0,		 50,		 20,		2,			0);
	//ribbon_type4(10,0,20,		 50,		 20,		1,			30);
	
	

	
	//		(x,y,z)			radius,	 size_of_cube	scaled_up	rotate_about_z_axis
	//ribbon_type5(0,0,0,		 50,		 15,		2.5,			22.5);
	//ribbon_type5(0,0,0,		 50,		 15,		2.5,			0);
	

	
	//		(x,y,z)			radius,	 size_of_cube	scaled_up  rotate_about_z_axis
	//ribbon_type6(0,0,0,		 70,		 10,		2			,	0);
	//ribbon_type6(10,20,30,	 40,		 10,		1			,	0); 
	//ribbon_type6(0,0,0,		 40,		 1,		100			,	0);
	
	

	
	//						R,	R_out,		R_in, out_Height,	in_Height,ratate_ang,	quantity_of_cyl_per_side	,	interdist_of_cyls)
	//ribbon_type2(0,0,0,  80,   10,			8,		30,			20,        0,			3,								0);
	//ribbon_type2(0,0,0,  80,   10,			8,		30,			20,        10,			3,								1);
	//ribbon_type2(10,35,22,  50,   10,			8,		20,			18,        0,			2,								1);
	//ribbon_type2(0,0,0,  80,   10,			8,		30,			20,        10,			3,								1);
	//ribbon_type2(0,0,22,  50,   10,			8,		20,			18,        0,			2,								1);
	

	

	
	
				//X0 Y0 Z0,		R,		size,	scale_val	,ratate_ang,	discs_per_side,		inter_dist_of_disc)
	//ribbon_type3(10,2,10,			70,		10,        1,				0,			1,				3);
	
	
	
				//X0 Y0 Z0,		R,		size,	scale_val	,ratate_ang,	discs_per_side,		inter_dist_of_disc)
	//ribbon_type9(10,2,10,			60,		10,        1,				0,			3,				4);
	
	
	//kata(10,10,10,20,90);
	
	//				X0,Y0,Z0,		R,size,scale_val,ratate_ang,kata_angle)
	//ribbon_type_kata(0,0,10,		60, 5,		1	,	0,			30 );
	//ribbon_type_kata(60,1,10,		60, 5,		1	,	0,			0 );
	
	
	//tower_type_kata(0,0,0,50);//x,y,z,R
	//tower_type3(20,-50,10,50);//x,y,z,R
	




	

	

	//tower_type4(20,0,0,30);//x,y,z,R
	//tower_type4(60,60,30,20);//x,y,z,R
	//tower_type4(10,10,20,10);//x,y,z,R
	//tower_type4(10,10,0,50);//x,y,z,R
	
	
	
	//stand_type_5(10,20,0,20);//x,y,z,R
	//tower_type4(0,0,0,40);//x,y,z,R
	


	//tower_type6(0,0,0,50);

	
	
	glRotatef(10*UU,1,0,0);
	glRotatef(10*II,0,1,0);
	glRotatef(10*OO,0,0,1);

	
	base1(0,0,102.5,240);
	base2();
	
	

	//Cylinder(0,0,0,30,40,60,4);
	//Cylinder_Cage(0,0,0,30,40,60,4);

	

	
	//	X0,Y0,Z0,	R_out,R_in,		Height,	Width,	ratate_ang)
	//cave(10,10,30,		50,		10,		30,		10,			0);
	

	


	
	

	

	
	//////// ------ NOTE ---- ORDER matters. compare last two spheres!
	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}





void animate(){
	//codes for any changes in Camera
	


	if(rotate_YES)
	{
		cameraAngle += cameraAngleDelta;	// camera will rotate at 0.002 radians per frame.	// keep the camera steady NOW!!
		//glRotatef(90,1,0,0);
		//codes for any changes in Models
		
		rectAngle -= 1;
	}
	//MISSING SOMETHING? -- YES: add the following
	glutPostRedisplay();	//this will call the display AGAIN
}

void keyboardListener(unsigned char key, int x,int y){

	double R,ang;


	if(rotate_YES)
	{
		switch(key)
		{
			case 'd':	//reverse the rotation of camera
				debug^=1;
				break;
			
			case 'u':	//reverse the rotation of camera
				UU++;
				break;
			case 'i':	//reverse the rotation of camera
				II++;
				break;
			case 'o':	//reverse the rotation of camera
				OO++;
				break;
			
			case 'a':	//reverse the rotation of camera
				rotate_YES^=1;
				break;
			case '1':	//reverse the rotation of camera
				cameraAngleDelta = -cameraAngleDelta;
				break;

			case '2':	//increase rotation of camera by 10%
				cameraAngleDelta *= 1.1;
				break;

			case '3':	//decrease rotation
				cameraAngleDelta /= 1.1;
				break;

			case '8':	//toggle grids
				canDrawGrid =  !canDrawGrid;
				break;
			

			case 27:	//ESCAPE KEY -- simply exit
				exit(0);
				break;
			
			default:
				break;
		}


	}
	else
	{
		switch(key){

			case 'a':	//reverse the rotation of camera
				rotate_YES^=1;
				break;
		
			case 'b':	//reverse the rotation of camera
				birdy^=1;
				break;
		
					
			case '8':	//toggle grids
				canDrawGrid =  !canDrawGrid;
				break;
			

			case 27:	//ESCAPE KEY -- simply exit
				exit(0);
				break;
			
			case 'w':	//ESCAPE KEY -- simply exit
				
				R=sqrt( (lookX-cameraX)*(lookX-cameraX) + (lookY-cameraY)*(lookY-cameraY) );
				ang=atan2((lookY-cameraY),(lookX-cameraX));
				
				
				lookX+=10*cos(ang);
				lookY+=10*sin(ang);
				cameraX+=10*cos(ang);
				cameraY+=10*sin(ang);

				break;

			case 's':	//ESCAPE KEY -- simply exit
				R=sqrt( (lookX-cameraX)*(lookX-cameraX) + (lookY-cameraY)*(lookY-cameraY) );
				ang=atan2((lookY-cameraY),(lookX-cameraX));
				
				
				lookX-=10*cos(ang);
				lookY-=10*sin(ang);
				cameraX-=10*cos(ang);
				cameraY-=10*sin(ang);
				break;
			default:
				break;
		}
	}
}






void specialKeyListener(int key, int x,int y)
{
	double R,ang;
		
	if(rotate_YES)
	{
		switch(key)
		{
			case GLUT_KEY_DOWN:		//down arrow key
				cameraRadius += 10;
				break;
			case GLUT_KEY_UP:		// up arrow key
				if(cameraRadius > 10)
					cameraRadius -= 10;
				break;

			case GLUT_KEY_RIGHT:
				break;
			case GLUT_KEY_LEFT:
				break;

			case GLUT_KEY_PAGE_UP:
				cameraHeight += 10;
				break;
			case GLUT_KEY_PAGE_DOWN:
				cameraHeight -= 10;
				break;

			case GLUT_KEY_INSERT:
				break;

			case GLUT_KEY_HOME:
				break;
			case GLUT_KEY_END:
				break;

			default:
				break;
		}
	}
	else
	{
		switch(key)
		{
			case GLUT_KEY_DOWN:		//down arrow key
				lookZ -= 30;
				break;
			case GLUT_KEY_UP:		// up arrow key
				lookZ += 30;
				break;
				
				
		
			case GLUT_KEY_RIGHT:
				
				R=sqrt( (lookX-cameraX)*(lookX-cameraX) + (lookY-cameraY)*(lookY-cameraY) );
				ang=atan2((lookY-cameraY),(lookX-cameraX));
				
				ang-=(PI/18);
				
				lookX=R*cos(ang);
				lookY=R*sin(ang);

				break;
			case GLUT_KEY_LEFT:
				R=sqrt( (lookX-cameraX)*(lookX-cameraX) + (lookY-cameraY)*(lookY-cameraY) );
				ang=atan2((lookY-cameraY),(lookX-cameraX));
				
				ang+=(PI/18);
				
				lookX=R*cos(ang);
				lookY=R*sin(ang);
				break;

			case GLUT_KEY_PAGE_UP:
				cameraZ += 20;
				break;
			case GLUT_KEY_PAGE_DOWN:
				cameraZ -= 20;
				break;

			case GLUT_KEY_INSERT:
				break;

			case GLUT_KEY_HOME:
				break;
			case GLUT_KEY_END:
				break;

			default:
				break;	
		}
	
	}
}

void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN)
			{		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				cameraAngleDelta = -cameraAngleDelta;	
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}


void init(){
	//codes for initialization
	canDrawGrid = true;
	
	rotate_YES=1;
	cameraX = 150;
	cameraY= 150;
	cameraZ= 150;

	lookX = 0;
	lookY= 0;
	lookZ= 0;
	

	cameraAngle = 0;	//// init the cameraAngle
	cameraAngleDelta = 0.002;
	rectAngle = 0;
	cameraHeight = 150;
	cameraRadius = 150;

	


	//clear the screen
	glClearColor(BLACK, 0);
	

	quad= gluNewQuadric ();
	

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);
	
	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(70,	1,	0.1,	10000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();
	
	

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	//ADD keyboard listeners:
	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);

	//ADD mouse listeners:
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}



