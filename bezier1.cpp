//#include <OpenGL/glu.h>
//#include <OpenGL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include<fstream>
#define STEPS 1000 ///The number of steps to draw the Bezier curve so that it appears smooth
#define PI 3.14159265358979323846 
#define SIDES 32 ///Number of sides to control the increment of surface of revolution over 2 pi radians
#define INC 100 /// The number of steps for sampling the Bezier curve for creating surface of revolution
using namespace std;
 
struct sample{/**Structure to hold x,y and z coordinates of a sample point for surface of revolution*/
	int x, y, z;
};


struct sample s[INC+1]; ///Instantiating a sample structure to hold INC number of points.
/** Define a global array of x and y, the coords of control points */
int arrx[20]; //Let's assume there are 20 points max, for now.
int arry[20];
int i = 0; ///keeping count of the number of ctrl pts chosen so far
int num; ///total no of control points
char key; ///Global variable to keep track of the current key chosen
int remx, remy; ///Global variables to keep track of the point chosen to be deleted, so that the global arrays can be adjusted 
int dragx, dragy; ///Global variables to eep track of the point chosen to be dragged around	
int dragindex;	///Global variable to hold the index of the point in the globals array chosen to be dragged.
void Mouse(int button, int state, int x, int y); ///Mouse click function
float angle_increment = 2*PI/SIDES; ///Global variable to hold the angle between two consecutive faces about the axis of revolution
void drawCurve();



//Camera cam1;



float Bezier(int *arr, int i, int j, float t){/**Recursive de Casteljau's algorithm to find the points on the Bezier curve*/ 
    if(j == 0)
        return arr[i];
    return Bezier(arr, i, j-1, t)*(1-t)+ t*Bezier(arr, i+1,j-1, t);
}


void createOff(){ /**Function to create .off file to create a surface of revolution from the Bezier curve:
The file needs to have 'OFF' in the first line, followed by the number of vertices, faces and edges in the next line, after each lvertex is enlisted in a separate line, followed by enumaration of surfaces each in a separate line as the number of edges forming the surface along with the vertices listed in order */
	drawCurve(); // This is called to reset the sample array s to the initial values
	int vertices = (INC+1)*SIDES; /// Number of vertices
	int faces = INC*SIDES+2; /// Number of faces
	int edges= SIDES*(INC+2); ///Number of edges
	ofstream outfile; 
	ifstream infile;
	outfile.open("SOR.off", ios::out);
	outfile<<"OFF"<<endl;
	outfile<<vertices<<" "<<faces<<" "<<edges<<endl;
	int j;
	for(j=0;j<(INC+1);j++){
		//s[j].x -= s[0].x;
		outfile<<s[j].x<<" "<<s[j].y<<" "<<s[j].z<<endl;
	}
	int t=1;
	float ang=angle_increment;
	while(t<SIDES){
		for(j=0;j<INC+1;j++){
			outfile<<(int)round(((s[j].x)*cos(ang))-((s[j].z)*sin(ang)))<<" "<<s[j].y<<" "<<(int)round(((s[j].x)*sin(ang))+(s[j].z)*cos(ang))<<endl;
		}
		ang+=angle_increment;
		t++;
	}


	//print the faces 
	
	int f = 4;
	//int first=1;
	for(t=0;t<SIDES;t++){
		for(j=0;j<INC;j++){
			if(t<(SIDES-1))
				outfile<<f<<" "<<((INC+1)*t)+j<<" "<<((INC+1)*t)+j+1<<" "<<((INC+1)*t)+j+(INC+2)<<" "<<((INC+1)*t)+j+(INC+1)<<endl;
			if(t==(SIDES-1)) 
				outfile<<f<<" "<<((INC+1)*t)+j<<" "<<((INC+1)*t)+j+1<<" "<<(j+(INC+2))%(INC+1)<<" "<<(j+(INC+1))%(INC+1)<<endl;				
		}	
	}

	
	
	f = SIDES;
	int index = 0;
	outfile<<f;
	while(index<vertices){
		outfile<<" "<<index;
		index+=INC+1;
	}
	outfile<<endl;
	outfile<<f;
	index = INC;
	while(index<vertices){
		outfile<<" "<<index;
		index+=INC+1;
	}
	outfile<<endl;

	outfile.close();

}


void searchndelete(){ /**Function to search an element in the arrays having the values held in remx,remy and deleting them by adjusting the array indices*/
	int c,j;
	for(c=0;c<num;c++){
			if(remx < (arrx[c]+5) && remx > (arrx[c]-5)&& remy < (arry[c]+5) && remy>(arry[c]-5)){//shift
				for(j = c;j<num-1;j++){
					arrx[j] = arrx[j+1];
					arry[j] = arry[j+1];
					cout<<"Deleted one point"<<endl;
				}
				num--;
				i--;
					}
	}

}

int search(int x, int y){/**This function is used to search a point passed through the arguments and record the index of the point in the array in dragindex variable, so that it could be modified as the user drags the point along. Returns 0 if point is not found.*/
	int ret = 0;
	int c,j;
	for(c=0;c<num;c++){
		if(x < (arrx[c]+5) && x >(arrx[c]-5) && y<(arry[c]+5) &&y> arry[c]-5){
			dragindex = c;
			return 1;
		}
	}
	return 0;
}

void drawCurve(){/**This function takes x and y values of control points from the global arrays, finds the coordinates to plot Bezier curve,samples every tenth point and adds it to sample array s*/
	int k, X, Y;
	float t;
	glClear(GL_COLOR_BUFFER_BIT);
	for(k=0;k<num;k++){
		glPointSize(8);
		glColor3f(1,0,0);
		glBegin(GL_POINTS);
		glVertex2i(arrx[k], arry[k]);
		glEnd();
		glPointSize(5);	
	}
	for(k=0;k<=STEPS;k++){
		t = float(k)/(float)STEPS;
		X = Bezier(arrx, 0, num-1, t);			
		Y = Bezier(arry, 0, num-1, t);
		if(k%(1000/INC)==0){
			s[k/(1000/INC)].x = X;
			s[k/(1000/INC)].y = Y;
			s[k/(1000/INC)].z = 0;
		}
		s[INC].x=X;
		s[INC].y = Y;
		s[INC].z = 0;
		//cout<<X<<" "<<Y<<endl; //Seems to be working.
		//glPointSize(3); 				//Setting the point size to 3
		glColor3f(0, 0, 1); 
		glBegin(GL_POINTS);
        	glVertex2i(X, Y);
       		glEnd();		
		}
	
}


void Display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();
}

void drag(int x, int y){/**Function to implement dragging about of a chosen point, called by glutMotionFunc*/
	int yy = glutGet(GLUT_WINDOW_HEIGHT);
	arrx[dragindex] = x;
	arry[dragindex] = yy-y;
	drawCurve();
	glutSwapBuffers();

}

void Keyboard(unsigned char k,int x, int y){/**Function to implement keyboard function.
d- draw the curve
a- add a new point.
r- remove a point
m- dragging a point around.
*/
	key = k;
	if(k == 'd' && i == num){ 
		drawCurve();
		glutSwapBuffers();		
		}
	if(k == 'a' && i == num){
		num++;
		glutMouseFunc(Mouse);
			
	}
	if(k == 'r' && i == num){
		glutMouseFunc(Mouse);
			
	}
	if(k == 'm' && i==num){
		glutMouseFunc(Mouse);
	}
	
}


void Mouse(int button, int state, int x, int y){/**Function to implement mouse function. Initially used for choosing the control points of bezier curve. After adding all the points, mouse function is used for choosing a new point to add, or choosing a point to remove, or choosing a point to drag around and dragging it around*/
    int yy;
    yy = glutGet(GLUT_WINDOW_HEIGHT);
    int xx = glutGet(GLUT_WINDOW_WIDTH);
    y = yy - y; /* In Glut, Y coordinate increases from top to bottom */
    glColor3f(1.0, 0, 0);
	if((i<num)&&(button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)){
   		cout<<x<<" "<<y<<endl;
    	
        	glBegin(GL_POINTS);
        	glVertex2i(x, y);
        	glEnd();
    	
		arrx[i] = x;
		arry[i] = y;
		i++;
    		glutSwapBuffers();
	}
	if(i == num && key == 'r') {
		cout<<"Point to be deleted:"<<x<<" "<<y<<endl;
		remx = x;
		remy = y;
		searchndelete();
	}
	if(i==num && key == 'm'){
		if(search(x,y))
			glutMotionFunc(drag);;
	}

	if(i==num && key =='s'){ //Surface of revolution
		createOff();
		
	}	
	
}

void Init(void){/**Initializes the window and sets to 2D projection*/
    glClearColor(1.0f, 1.0f, 1.0f, 0.0);
    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(5.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 1309.0, 0.0, 716.0);
}

int main(int argc, char** argv){
	cout<<"Enter number of points:"<<endl;
	cin>>num;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1309, 716);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Bezier Curve");
    glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);    
	glutMouseFunc(Mouse);
	Init();
    glutMainLoop();
}
