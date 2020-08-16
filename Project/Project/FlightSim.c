#define _CRT_SECURE_NO_DEPRECATE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glut.h>
#include <freeglut.h>
#include <math.h>

// Resolution of the islands
#define islandRes 33

float mountain1z[islandRes][islandRes];
float mountain2z[islandRes][islandRes];
float mountain3z[islandRes][islandRes];
float mountain1Size;
float mountain2Size;
float mountain3Size;
float mountain1offset[2];
float mountain2offset[2];
float mountain3offset[2];

mountain1FaceNormals[islandRes - 1][islandRes - 1][3];
mountain1VertexNormals[islandRes][islandRes][3];

mountain2FaceNormals[islandRes - 1][islandRes - 1][3];
mountain2VertexNormals[islandRes][islandRes][3];

mountain3FaceNormals[islandRes - 1][islandRes - 1][3];
mountain3VertexNormals[islandRes][islandRes][3];

int windowHeight = 500;
int windowWidth = 500;

int c = 1;

int upToggle = 0;
int downToggle = 0;

int fogToggle = 1;
int textureToggle = 1;
int wireframeToggle = 1;
int fullscreenToggle = 0;
int mountainToggle = 1;
int rainToggle = 0;

// variable that keeps track of the scene (textured disks vs grid)
int scene = 0;

float fov = 60;

float speed = 10;
float cameraPos[3] = { 0, 5, 20 };
// vector keeping track of where the plane is looking
float planeDirAngle = 0;
float planeDirection[3] = { 0, 0, 0 };
float planeSpeed = 10;
float mousePos[2];

// Light settings
GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat ambient[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat position[] = { 0.0, 100.0, 0.0, 0.0 };
GLfloat direction[] = { 1.0, 2.0, 2.0 };

GLfloat globalAmbient[] = { 0.01f, 0.01f, 0.01f, 1.0 };

GLfloat zeroMaterial[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat redDiffuse[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat greenDiffuse[] = { 0.0, 1.0, 0.0, 1.0 };
GLfloat blueDiffuse[] = { 0.0, 0.0, 1.0, 1.0 };
GLfloat whiteSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat seaWireframeDiffuse[] = { 0.4, 0.8, 0.9, 1.0};
GLfloat skyWireframeDiffuse[4] = { 0.9, 0.25, 0.5, 1.0 };
GLfloat shine = 100;

GLfloat planePurple[] = { 1.0, 0.0, 1.0, 1.0 };
GLfloat planeYellow[] = { 0.6, 0.6, 0.0, 1.0 };
GLfloat planeBlue[] = { 0.0, 0.0, 1.0, 1.0 };
GLfloat planeBlack[] = { 0.1, 0.1, 0.1, 1.0 };

GLfloat noShininess = 0.0;
GLfloat mountainShininess = 0.0;
GLfloat highShininess = 100.0;

GLUquadricObj* cylinderPtr;
GLUquadricObj* diskPtr;

GLubyte *mountainTex;
GLint mountainTexDimensions[2];
GLubyte *seaTex;
GLint seaTexDimensions[2];
GLubyte *skyTex;
GLint skyTexDimensions[2];

float *verts;
float *normals;
int *faces[32];
int numFaces[32];

float shipOffsetX = 20;
float shipOffsetY = -5;
float shipOffsetZ = 0;


float makeRand(void) {
	return  (rand() / (float)RAND_MAX);
}


void cleanup(void) {
	gluDeleteQuadric(cylinderPtr);
	gluDeleteQuadric(diskPtr);
	free(mountainTex);
	free(seaTex);
	free(skyTex);
}

void keyboard(unsigned char key, int x, int y) {
	if (key == 'b') {
		fogToggle = 1 - fogToggle;
		glutPostRedisplay();
	}
	if (key == 't') {
		textureToggle = 1 - textureToggle;
		glutPostRedisplay();
	}
	if (key == 'w') {
		wireframeToggle = 1 - wireframeToggle;
		if (wireframeToggle == 1) {
			glDisable(GL_TEXTURE_2D);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			gluQuadricTexture(diskPtr, GLU_FALSE);
			gluQuadricDrawStyle(diskPtr, GLU_LINE);
			gluQuadricTexture(cylinderPtr, GLU_FALSE);
			gluQuadricDrawStyle(cylinderPtr, GLU_LINE);
		}
			
		else {
			glEnable(GL_TEXTURE_2D);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			gluQuadricTexture(diskPtr, GLU_TRUE);
			gluQuadricDrawStyle(diskPtr, GLU_FILL);
			gluQuadricTexture(cylinderPtr, GLU_TRUE);
			gluQuadricDrawStyle(cylinderPtr, GLU_FILL);
		}
			
		glutPostRedisplay();
	}
	if (key == 'f') {
		fullscreenToggle = 1 - fullscreenToggle;
		if (fullscreenToggle == 1) {
			glutFullScreen();
		}
		else {
			glutPositionWindow(0, 0);
			glutReshapeWindow(500, 500);
		}
		
		glutPostRedisplay();
	}
	if (key == 's') {
		scene = 1 - scene;
		glutPostRedisplay();
	}
	if (key == 'm') {
		mountainToggle = 1 - mountainToggle;
		glutPostRedisplay();
	}
	if (key == 'q') {
		cleanup();
		exit(0);
	}
	if (key == 'r') {
		rainToggle = 1 - rainToggle;
		if (rainToggle == 0) {
			glColor4f(1.0, 1.0, 1.0, 1.0);
		}
		glutPostRedisplay();
	}
}

void specialKeyboard(int key, int x, int y) {
	switch (key) {
	case (GLUT_KEY_UP):
		upToggle = 1;
		glutPostRedisplay();
		break;
	case (GLUT_KEY_DOWN):
		downToggle = 1;
		glutPostRedisplay();
		break;

	case (GLUT_KEY_PAGE_DOWN):
		speed -= 2;
		glutPostRedisplay();
		break;
	case (GLUT_KEY_PAGE_UP):
		speed += 2;
		glutPostRedisplay();
		break;
	}

}
void specialKeyboardUp(int key, int x, int y) {
	switch (key) {
	case (GLUT_KEY_UP):
		upToggle = 0;
		glutPostRedisplay();
		break;
	case (GLUT_KEY_DOWN):
		downToggle = 0;
		glutPostRedisplay();
		break;
	}
}

void initPlane(void) {

	FILE *fptr;
	fptr = fopen("cessna.txt", "r");

	if (fptr == NULL) {
		printf("Error!");
		exit(1);
	}

	// first read in all vertexes
	int vertSize = 1024;
	verts = (float *)malloc(sizeof(float) * vertSize * 3);

	char mode;
	float x, y, z;
	int i = 0;
	while (fscanf(fptr, "%c ", &mode) != EOF && mode == 'v') {

		if (i >= vertSize) {
			vertSize *= 2;
			verts = (float *)realloc(verts, sizeof(float) * vertSize * 3);
		}
		fscanf(fptr, " %f %f %f \n", &x, &y, &z);
		*(verts + i * 3 + 0) = x;
		*(verts + i * 3 + 1) = y;
		*(verts + i * 3 + 2) = z;
		printf("Vert %d: %f %f %f\n", i, *(verts + i * 3), *(verts + i * 3 + 1), *(verts + i * 3 + 2));
		i++;
	}
	// return last mode fetched
	ungetc(mode, fptr);

	// first read in all normals
	i = 0;
	int normalSize = vertSize;
	normals = (float *)malloc(sizeof(float) * normalSize * 3);
	while (fscanf(fptr, "%c ", &mode) != EOF && mode == 'n') {

		if (i >= normalSize) {
			normalSize *= 2;
			normals = (float *)realloc(verts, sizeof(float) * normalSize * 3);
		}
		fscanf(fptr, " %f %f %f \n", &x, &y, &z);
		*(normals + i * 3 + 0) = x;
		*(normals + i * 3 + 1) = y;
		*(normals + i * 3 + 2) = z;
		//printf("n %f %f %f\n", *(normals + i * 3), *(normals + i * 3 + 1), *(normals + i * 3 + 2));
		i++;
	}

	ungetc(mode, fptr);

	// Faces
	// buffer for fgets
	char buff[128];
	int j = 0;
	int x2;
	// Number of triangles to initially allocate
	char objName[80];
	while (fscanf(fptr, " %c %s ", &mode, &objName) != EOF && mode == 'g') {
		printf("Loading %s\n", objName);
	
		int faceSize = 1024;
		// allocate memory for 20d int array
		faces[j] = (int *)malloc(sizeof(int) * faceSize * 20);

		i = 0;
		int k;
		// loop through lines until reaching end of file or non f char
		while (fscanf(fptr, "%c ", &mode) != EOF && mode == 'f') {
			k = 0;
			// If we've run out of memory, double it
			if (i >= faceSize) {
				faceSize *= 2;
				faces[j] = (int *)realloc(faces[j], sizeof(int) * faceSize * 20);
			}
			// scan and store integer values
			fgets(buff, 127, fptr);

			char *tempChar;
			char delim[] = " ";
			tempChar = strtok(buff, delim);
			int vert;
			while (tempChar != NULL) {
				// changing string into int
				x2 = atoi(tempChar);
				*(faces[j] + i * 20 + k) = x2;
				// getting next value
				tempChar = strtok(NULL, delim);
				//printf("%d ", *(faces[j] + i * 20 + k));
				k++;
			}
			while (k < 20) {
				*(faces[j] + i * 20 + k) = NULL;
				k++;
			}
			i++;
		}
		//printf("%d %d %d %d\n", *faces, *(faces+1), *(faces+2), *(faces + 3));
		numFaces[j] = i;
		j++;
	}
	//fclose(err);
	//printf("Last loaded - Mode: %s Name: %c \n", objName, mode);

	fclose(fptr);
}

// draws the space ship

void drawPlane(void) {
	glDisable(GL_TEXTURE_2D);

	int i;
	float shipSize = 1;
	float offsetX = 0;
	float offsetY = 0;
	float offsetZ = 0;

	glPushMatrix();
	glLoadIdentity();
;
	glTranslatef(0, -1.5, -5);
	glRotatef(-50 * (mousePos[0] - 0.5), 0, 0, 1);
	glRotatef(270, 0, 1, 0);
	
	int j;
	int k = 0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, globalAmbient);

	for (j = 0; j < 32; j++) {
		// colours based on what sub part we're dealing with
		if (j < 4 || (j > 7 && j < 14) || j > 25) {
			glMaterialfv(GL_FRONT, GL_DIFFUSE, planeYellow);
		}
		else if (j == 4 || j == 5) {
			glMaterialfv(GL_FRONT, GL_DIFFUSE, planeBlack);
		}
		else if (j == 6) {
			glMaterialfv(GL_FRONT, GL_DIFFUSE, planePurple);
		}
		else {
			glMaterialfv(GL_FRONT, GL_DIFFUSE, planeBlue);
		}

		for (i = 0; i < numFaces[j]; i++) {
			glBegin(GL_POLYGON);
			for (k = 0; k < 20; k++) {
				// Getting the vertex number 
				int vert = *(faces[j] + (i * 20) + k);
				// Reducing value gotten from face array by 1 (verts listed in the file start at 1 instead of 0)
				vert -= 1;
				// If there are no more vertecies in this face
				if (vert == NULL || vert <= -1) {
					break;
				}
				glNormal3f(*(normals + vert * 3 + 0), *(normals + vert * 3 + 1), *(normals + vert * 3 + 2));
				//printf("vert %d: %f %f %f\n", vert, *(verts + vert * 3 + 0), *(verts + vert * 3 + 1), *(verts + vert * 3 + 2));
				glVertex3f(*(verts + vert * 3 + 0) * shipSize + offsetX, *(verts + vert * 3 + 1) * shipSize + offsetY, *(verts + vert * 3 + 2) * shipSize + offsetZ);
			}
			glEnd();
			//printf("\n");
		}
	}
	glPopMatrix();
	glutPostRedisplay();
}

void drawGrid(void) {
	glMaterialfv(GL_FRONT, GL_AMBIENT, globalAmbient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, zeroMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, noShininess);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, blueDiffuse);

	glNormal3d(0, 1, 0);

	float size = 100;
	float width = c;
	float zOffset = 0;
	float i;
	float j;

	glBegin(GL_QUADS);
	for (i = -(size / 2); i < (size / 2) + width; i = i + width) {
		for (j = -(size / 2); j < (size / 2) + width; j = j + width) {
			glVertex3d(i * width, zOffset, j * width);
			glVertex3d((i+1) * width, zOffset, j * width);
			glVertex3d((i+1) * width, zOffset, (j+1) * width);
			glVertex3d(i * width, zOffset, (j+1) * width);
		}
	}
	glEnd();

	glutPostRedisplay();
}

void drawAxis(void) {
	glDisable(GL_TEXTURE_2D);
	glMaterialfv(GL_FRONT, GL_AMBIENT, globalAmbient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, zeroMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, noShininess);

	glNormal3d(0, 1, 0);

	glBegin(GL_LINES);
	glLineWidth(5);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, blueDiffuse);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, 1.0);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, greenDiffuse);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 1.0, 0);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, redDiffuse);
	glVertex3d(0, 0, 0);
	glVertex3d(1.0, 0, 0);
	glEnd();

	glMaterialfv(GL_FRONT, GL_DIFFUSE, whiteSpecular);
	glutSolidSphere(0.1, 12, 8);


	glutPostRedisplay();
}

// Material colours based on vertex height - snowy mountain tops!
void mountainMaterialCalc(float z) {
	GLfloat mountainGreenDiffuse[4] = { 0.1, 0.5, 0.1, 1.0 };
	GLfloat mountainBrownDiffuse[4] = { 0.5, 0.35, 0.2, 1.0 };
	GLfloat mountainWhiteDiffuse[4] = { 0.9, 0.9, 0.9, 1.0 };

	if (textureToggle == 0) {
		if (z >= 5)
			glMaterialfv(GL_FRONT, GL_DIFFUSE, mountainWhiteDiffuse);
		else if (z >= 3.5)
			glMaterialfv(GL_FRONT, GL_DIFFUSE, mountainBrownDiffuse);
		else
			glMaterialfv(GL_FRONT, GL_DIFFUSE, mountainGreenDiffuse);
	}
	else {
		glMaterialfv(GL_FRONT, GL_DIFFUSE, whiteSpecular);
	}
}

drawSingleMountain(float mountain[islandRes][islandRes], float mountainVertexNorms[islandRes][islandRes][3], float mountain1Size, float x, float z) {
	float mountainHeight = -2;
	glBegin(GL_QUADS);
	int i, j;
	for (i = 0; i < islandRes - 1; i++) {
		for (j = 0; j < islandRes - 1; j++) {

			mountainMaterialCalc(mountain[j][i]);

			glNormal3f(mountainVertexNorms[j][i][0], mountainVertexNorms[j][i][1], mountainVertexNorms[j][i][2]);
			glTexCoord2f(i / (float)islandRes, j / (float)islandRes);
			glVertex3f(i * mountain1Size + x, mountainHeight + mountain1z[i][j], j * mountain1Size + z);

			mountainMaterialCalc(mountain[j+1][i]);

			glNormal3f(mountainVertexNorms[j+1][i][0], mountainVertexNorms[j+1][i][1], mountainVertexNorms[j+1][i][2]);
			glTexCoord2f(i / (float)islandRes, (j + 1) / (float)islandRes);
			glVertex3f(i * mountain1Size + x, mountainHeight + mountain1z[i][j + 1], (j + 1) * mountain1Size + z);

			mountainMaterialCalc(mountain[j+1][i+1]);

			glNormal3f(mountainVertexNorms[j+1][i+1][0], mountainVertexNorms[j+1][i+1][1], mountainVertexNorms[j+1][i+1][2]);
			glTexCoord2f((i + 1) / (float)islandRes, (j + 1) / (float)islandRes);
			glVertex3f((i + 1) * mountain1Size + x, mountainHeight + mountain1z[i + 1][j + 1], (j + 1) * mountain1Size + z);

			mountainMaterialCalc(mountain[j][i+1]);

			glNormal3f(mountainVertexNorms[j][i+1][0], mountainVertexNorms[j][i+1][1], mountainVertexNorms[j][i+1][2]);
			glTexCoord2f((i + 1) / (float)islandRes, j / (float)islandRes);
			glVertex3f((i + 1) * mountain1Size + x, mountainHeight + mountain1z[i + 1][j], j * mountain1Size + z);
		}
	}
	glEnd();
}

void drawMountains(void) {
	if (wireframeToggle == 0 && textureToggle == 1) {
		glEnable(GL_TEXTURE_2D);
		// tell openGL how to scale the texture image up if needed
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		// tell openGL how to scale the texture image down if needed
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else {
		glDisable(GL_TEXTURE_2D);
	}
	
	glMaterialfv(GL_FRONT, GL_SPECULAR, zeroMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, noShininess);

	glMaterialfv(GL_FRONT, GL_AMBIENT, globalAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, whiteSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, shine);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, mountainTexDimensions[0], mountainTexDimensions[1], 0, GL_RGB, GL_UNSIGNED_BYTE, mountainTex);

	drawSingleMountain(mountain1z, mountain1VertexNormals, mountain1Size, mountain1offset[0], mountain1offset[1]);
	drawSingleMountain(mountain2z, mountain2VertexNormals, mountain2Size, mountain2offset[0], mountain2offset[1]);
	drawSingleMountain(mountain3z, mountain3VertexNormals, mountain3Size, mountain3offset[0], mountain3offset[1]);

	glutPostRedisplay();
}

// calculates the face and then the vertex normals for the mountains
void calcMountainNormals(float mountain[islandRes][islandRes], float mountainFaceNormals[islandRes - 1][islandRes - 1][3], float mountainVertexNormals[islandRes][islandRes][3]) {
	int i, j;

	for (i = 0; i < islandRes - 1; i++) {
		for (j = 0; j < islandRes - 1; j++) {
			float vector1[3] = { 0, mountain[j][i + 1] - mountain[j][i], 1 };
			float vector2[3] = { 1, mountain[j + 1][i] - mountain[j][i], 0 };
			// Cross product
			float normalVector[3] = {	vector1[1]*vector2[2] - vector1[2]*vector2[1], 
										vector1[2]*vector2[0] - vector1[0]*vector2[2],
										vector1[0]*vector2[1] - vector1[1]*vector2[0]	};
			mountainFaceNormals[j][i][0] = normalVector[0];
			mountainFaceNormals[j][i][1] = normalVector[1];
			mountainFaceNormals[j][i][2] = normalVector[2];
		}
	}

	// initializing vertex normals
	for (i = 0; i < islandRes; i++) {
		for (j = 1; j < islandRes; j++) {
			mountainVertexNormals[j][i][0] = 0;
			mountainVertexNormals[j][i][1] = 1;
			mountainVertexNormals[j][i][2] = 0;
		}
	}
	// now calculating vertex norms
	for (i = 1; i < islandRes - 1; i++) {
		for (j = 1; j < islandRes - 1; j++) {
			//Averaging the normals of nearby faces
			float f1[3] = { mountainFaceNormals[j-1][i-1][0], mountainFaceNormals[j-1][i-1][1], mountainFaceNormals[j-1][i-1][2] };
			float f2[3] = { mountainFaceNormals[j][i - 1][0], mountainFaceNormals[j][i - 1][1], mountainFaceNormals[j][i - 1][2] };
			float f3[3] = { mountainFaceNormals[j - 1][i][0], mountainFaceNormals[j - 1][i][1], mountainFaceNormals[j - 1][i][2] };;
			float f4[3] = { mountainFaceNormals[j][i][0], mountainFaceNormals[j][i][1], mountainFaceNormals[j][i][2] };;
			float vnormal[3] = {(f1[0]+f2[0]+f3[0]+f4[0])/4,(f1[1] + f2[1] + f3[1] + f4[1]) / 4,(f1[2] + f2[2] + f3[2] + f4[2]) / 4 };
			mountainVertexNormals[j][i][0] = vnormal[0];
			mountainVertexNormals[j][i][1] = vnormal[1];
			mountainVertexNormals[j][i][2] = vnormal[2];
		}
	}
}

float mountainRand(float diff) {
	float rand = (fabs(diff)/3) * (0.6*makeRand() - 0.3) + (0.5*makeRand() - 0.25);
	//rand = 0;
	return rand;
}

void recursiveMountain(int tl[2], int tr[2], int bl[2], int br[2], int res, float mountain[islandRes][islandRes]) {
	int isFirstPass = 0;
	if (res == islandRes) {
		isFirstPass = 1;
	}
	if (res <= 1) {
		// base case
	}
	else {
		//printf("~~~~~\nResolution: %d\n", res);
		//printf("TL|%d, %d\nTR|%d, %d\nBL|%d, %d\nBR|%d, %d\n", tl[0], tl[1], tr[0], tr[1], bl[0], bl[1], br[0], br[1]);

		res /= 2;

		// midpoint calculation
		float midz; // middle z value (true middle, not midpoint)
		float diff; // diff in z values
		int midx, midy;

		// Middle
		if (isFirstPass == 0) {
			diff = (mountain[bl[0]][bl[1]] + mountain[br[0]][br[1]] + mountain[tl[0]][tl[1]] + mountain[tr[0]][tr[1]]) / 4;
			midx = tl[0] + res;
			midy = tl[1] + res;
			midz = diff + mountainRand(diff);
			mountain[midx][midy] = midz;
		}
		else {
			midz = mountain[res][res];
		}

		// top
		diff = (mountain[tl[0]][tl[1]] + mountain[tr[0]][tr[1]] + midz) / 3;
		midx = tl[0] + res;
		midy = tl[0]; // same height for both
		
		if (mountain[midx][midy] == -1) { // if the midpoint has not already been set
			mountain[midx][midy] = diff + mountainRand(diff);
		}
		//printf("%f %f\n", mountain[tl[0]][tl[1]], mountain[tr[0]][tr[1]]);
		//printf("TRx: %d TRy: %d TRz: %f \nTLx: %d TLy: %d TLz: \n%f Midx: %d Midy: %d Midz: %f\n\n", tr[0], tr[1], mountain[tr[0]][tr[1]], tl[0], tl[1], mountain[tl[0]][tl[1]], midx, midy, mountain[midx][midy]);
		
		
		// left
		diff = (mountain[tl[0]][tl[1]] + mountain[bl[0]][bl[1]] + midz) / 3;
		midx = tl[0];
		midy = tl[1] + res;
		mountain[midx][midy] = diff + mountainRand(diff);
		// right
		diff = (mountain[br[0]][br[1]] + mountain[tr[0]][tr[1]] + midz) / 3;
		midx = tr[0];
		midy = tr[1] + res;
		mountain[midx][midy] = diff + mountainRand(diff);
		// bottom
		diff = (mountain[bl[0]][bl[1]] + mountain[br[0]][br[1]] + midz) / 3;
		midx = bl[0] + res;
		midy = bl[1];
		mountain[midx][midy] = diff + mountainRand(diff);


		// setting up recursion quadrants
		int tltl[2], tltr[2], tlbl[2], tlbr[2];

		// top left quadrant recursion
		tltl[0] = tl[0];
		tltl[1] = tl[1];

		tltr[0] = tl[0] + res;
		tltr[1] = tl[1];

		tlbl[0] = tl[0];
		tlbl[1] = tl[1] + res;

		tlbr[0] = tl[0] + res;
		tlbr[1] = tl[1] + res;
		recursiveMountain(tltl, tltr, tlbl, tlbr, res, mountain);

		// top right recursion
		tltl[0] = tl[0] + res;
		tltl[1] = tl[1];

		tltr[0] = tr[0];
		tltr[1] = tr[1];

		tlbl[0] = tl[0] + res;
		tlbl[1] = tl[1] + res;

		tlbr[0] = tr[0];
		tlbr[1] = tr[1] + res;
		recursiveMountain(tltl, tltr, tlbl, tlbr, res, mountain);


		// bottom left recursion
		tltl[0] = tl[0];
		tltl[1] = tl[1] + res;

		tltr[0] = tl[0] + res;
		tltr[1] = tl[1] + res;

		tlbl[0] = bl[0];
		tlbl[1] = bl[1];

		tlbr[0] = bl[0] + res;
		tlbr[1] = bl[1];
		recursiveMountain(tltl, tltr, tlbl, tlbr, res, mountain);


		// bottom right recursion
		tltl[0] = tl[0] + res;
		tltl[1] = tl[1] + res;

		tltr[0] = tr[0];
		tltr[1] = tr[1] + res;

		tlbl[0] = bl[0] + res;
		tlbl[1] = bl[1];

		tlbr[0] = br[0];
		tlbr[1] = br[1];
		recursiveMountain(tltl, tltr, tlbl, tlbr, res, mountain);
	}


}

void initMountains(void) {

	int i, j;
	for (i = 0; i < islandRes; i++) {
		for (j = 0; j < islandRes; j++) {
			mountain1z[i][j] = -1;
			mountain2z[i][j] = -1;
			mountain3z[i][j] = -1;
		}
	}
	mountain1Size = 0.3 + makeRand();
	mountain2Size = 0.3 + makeRand();
	mountain3Size = 0.3 + makeRand();
	mountain1offset[0] = makeRand() * 120 - 60;
	mountain1offset[1] = makeRand() * 120 - 60;
	mountain2offset[0] = makeRand() * 120 - 60;
	mountain2offset[1] = makeRand() * 120 - 60;
	mountain3offset[0] = makeRand() * 120 - 60;
	mountain3offset[1] = makeRand() * 120 - 60;
	mountain1z[0][0] = 0;
	mountain1z[islandRes-1][0] = 0;
	mountain1z[0][islandRes-1] = 0;
	mountain1z[islandRes-1][islandRes-1] = 0;
	mountain1z[islandRes / 2][islandRes / 2] = 3 + makeRand() * 5;

	mountain2z[0][0] = 0;
	mountain2z[islandRes-1][0] = 0;
	mountain2z[0][islandRes-1] = 0;
	mountain2z[islandRes-1][islandRes-1] = 0;
	mountain2z[islandRes / 2][islandRes / 2] = 3 + makeRand() * 5;

	mountain3z[0][0] = 0;
	mountain3z[islandRes-1][0] = 0;
	mountain3z[0][islandRes-1] = 0;
	mountain3z[islandRes-1][islandRes-1] = 0;
	mountain3z[islandRes / 2][islandRes / 2] = 3 + makeRand() * 5;
	int tl[2], tr[2], bl[2], br[2];
	tl[0] = 0;
	tl[1] = 0;
	tr[0] = islandRes-1;
	tr[1] = 0;
	bl[0] = 0;
	bl[1] = islandRes-1;
	br[0] = islandRes-1;
	br[1] = islandRes-1;
	recursiveMountain(tl, tr, bl, br, islandRes, mountain1z);
	/*
	FILE *fp;
	int k;
	fp = fopen("Output.txt", "w");

	for (i = 0; i < islandRes; i++) {
		for (j = 0; j < islandRes; j++) {
			fprintf(fp, "% 1.2f ", mountain1z[j][i]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	*/
	recursiveMountain(tl, tr, bl, br, islandRes, mountain2z);
	recursiveMountain(tl, tr, bl, br, islandRes, mountain3z);

	calcMountainNormals(mountain1z, mountain1FaceNormals, mountain1VertexNormals);
	calcMountainNormals(mountain2z, mountain2FaceNormals, mountain2VertexNormals);
	calcMountainNormals(mountain3z, mountain3FaceNormals, mountain3VertexNormals);


}

// drawing the disk + walls
void drawCylinder(void) {

	// tell openGL how to scale the texture image up if needed
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// tell openGL how to scale the texture image down if needed
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, skyTexDimensions[0], skyTexDimensions[1], 0, GL_RGB, GL_UNSIGNED_BYTE, skyTex);

	glMaterialfv(GL_FRONT, GL_SPECULAR, zeroMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, noShininess);

	// Only use ambient here because the light source does not hit the walls properly
	if (wireframeToggle == 1) {
		glMaterialfv(GL_FRONT, GL_AMBIENT, skyWireframeDiffuse);
	}
	else {
		glMaterialfv(GL_FRONT, GL_AMBIENT, whiteSpecular);
		glEnable(GL_TEXTURE_2D);
	}
	
	glPushMatrix();
	glRotatef(270, 1, 0, 0);
	glTranslatef(0, 0, 0);
	gluCylinder(cylinderPtr, 100, 100, 100, 128, 32);
	glPopMatrix();


	glTexImage2D(GL_TEXTURE_2D, 0, 3, seaTexDimensions[0], seaTexDimensions[1], 0, GL_RGB, GL_UNSIGNED_BYTE, seaTex);
	glMaterialfv(GL_FRONT, GL_AMBIENT, globalAmbient);
	if (wireframeToggle == 1) {
		glMaterialfv(GL_FRONT, GL_DIFFUSE, seaWireframeDiffuse);
	}
	else {
		glMaterialfv(GL_FRONT, GL_DIFFUSE, whiteSpecular);
		if (fogToggle == 1) {
			glEnable(GL_FOG);
		}
	}
	
	glPushMatrix();
	glRotatef(270, 1, 0, 0);
	gluDisk(diskPtr, 0, 101, 128, 16);
	glPopMatrix();

	glDisable(GL_FOG);

	glutPostRedisplay();
}

void drawRain(void) {
	int i;
	float randAlpha;
	float startx;
	float starty;
	float startz;

	glBegin(GL_LINES);
	glLineWidth(3);
	for (i = 0; i < 500; i++) {
		randAlpha = makeRand() * 0.6;
		startx = 100 * cos(360 * makeRand());
		starty = 40 * makeRand() + 5;
		startz = 100 * sin(360 * makeRand());
		glColor4f(0.0, 0.0, 0.6, randAlpha);
		glVertex3f(startx, starty, startz);
		glVertex3f(startx -1, starty - 5, startz + 1);
	}
	glEnd();
	glColor4f(0.5, 0.5, 0.5, 0.5);


}

void display(void) {
	// Clearing screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	// Loading identity matrix
	glLoadIdentity();
	

	// calculating camera position
	planeDirAngle += (mousePos[0] - 0.5) / 50;
	if (planeDirAngle >= 360 || planeDirAngle <= -360) {
		planeDirAngle = 0;
	}
	planeDirection[0] = sin(planeDirAngle);
	planeDirection[1] = 0;
	planeDirection[2] = -cos(planeDirAngle);

	cameraPos[0] += (speed / 10.0) * planeDirection[0] / 40;
	cameraPos[2] += (speed / 10.0) * planeDirection[2] / 40;
	gluLookAt(cameraPos[0], cameraPos[1], cameraPos[2], cameraPos[0] + planeDirection[0], cameraPos[1] + planeDirection[1], cameraPos[2] + planeDirection[2], 0, 1, 0);

	drawPlane();

	if (scene == 0) {
		drawAxis();
		drawGrid();
	}
	if (scene == 1) {
		if (mountainToggle == 1) {
			drawMountains();
		}
		drawCylinder();
	}
	if (rainToggle == 1) {
		drawRain();
	}
	glutSwapBuffers();
}

void idle(void) {
	if (upToggle == 1) {
		cameraPos[1] += 0.02f;
	}
	if (downToggle == 1) {
		cameraPos[1] += -0.02f;
	}

}
void windowResize(int width, int height) {
	windowWidth = width;
	windowHeight = height;

	glViewport(0, 0, windowWidth, windowHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, (float)windowWidth / (float)windowHeight, 0.1, 10000);
	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}
void mouseMovement(int x, int y) {
	mousePos[0] = x / (float)windowWidth;
	mousePos[1] = y / (float)windowHeight;
	//planeDirAngle = (mousePos[0] - 0.5);
	//printf("%.3f\n", (mousePos[0] - 0.5) / 360);
}
void loadTexture(char *fileName, GLubyte **filePtr, GLint *dimensions) {
	printf("Loading %s\n", fileName);

	// the ID of the image file
	FILE *fileID;

	int  maxValue;
	// total number of pixels in the image
	int  totalPixels;
	// temporary character
	char tempChar;
	// counter variable for the current pixel in the image
	int i;
	// array for reading in header information
	char headerLine[100];
	// if the original values are larger than 255
	float RGBScaling;

	// temporary variables for reading in the red, green and blue data of each pixel
	int red, green, blue;

	// open the image file for reading
	fileID = fopen(fileName, "r");

	// read in the first header line
	//    - "%[^\n]"  matches a string of all characters not equal to the new line character ('\n')
	//    - so we are just reading everything up to the first line break
	fscanf(fileID, "%[^\n] ", headerLine);

	// make sure that the image begins with 'P3', which signifies a PPM file
	if ((headerLine[0] != 'P') || (headerLine[1] != '3')) {
		printf("This is not a PPM file!\n");
		exit(0);
	}

	// we have a PPM file
	printf("This is a PPM file\n");

	// read in the first character of the next line
	fscanf(fileID, "%c", &tempChar);
	// while we still have comment lines (which begin with #)
	while (tempChar == '#') {
		fscanf(fileID, "%[^\n] ", headerLine);
		printf("%s\n", headerLine);
		fscanf(fileID, "%c", &tempChar);
	}

	// the last one was not a comment character '#', so we need to put it back into the file stream (undo)
	ungetc(tempChar, fileID);
	
	// read in the image hieght, width and the maximum value
	fscanf(fileID, "%d %d %d", &dimensions[0], &dimensions[1], &maxValue);
	// print out the information about the image file
	printf("%d rows  %d columns  max value= %d\n", dimensions[1], dimensions[0], maxValue);
	// compute the total number of pixels in the image
	totalPixels = dimensions[0] * dimensions[1];
	// allocate enough memory for the image  (3*) because of the RGB data
	*filePtr = malloc(3 * sizeof(GLuint) * totalPixels);
	// determine the scaling for RGB values
	RGBScaling = 255.0 / maxValue;

	// if the maxValue is 255 then we do not need to scale the 
	//    image data values to be in the range or 0 to 255
	if (maxValue == 255) {
		for (i = 0; i < totalPixels; i++) {
			// read in the current pixel from the file
			fscanf(fileID, "%d %d %d", &red, &green, &blue);

			// store the red, green and blue data of the current pixel in the data array
			*(*filePtr + (3 * totalPixels - 3 * i - 3)) = red;
			*(*filePtr + (3 * totalPixels - 3 * i - 2)) = green;
			*(*filePtr + (3 * totalPixels - 3 * i - 1)) = blue;
		}
	}
	else {
		for (i = 0; i < totalPixels; i++) {
			// read in the current pixel from the file
			fscanf(fileID, "%d %d %d", &red, &green, &blue);

			// store the red, green and blue data of the current pixel in the data array
			*(*filePtr + (3 * totalPixels - 3 * i - 3)) = red * RGBScaling;
			*(*filePtr + (3 * totalPixels - 3 * i - 2)) = green * RGBScaling;
			*(*filePtr + (3 * totalPixels - 3 * i - 1)) = blue * RGBScaling;
		}
	}
	// close the image file
	fclose(fileID);
}

// Initialize textures (call functions to load them in)
void initTextures(void) {
	char str1[] = "texturesPPM/mount03.ppm";
	char str2[] = "texturesPPM/sea02.ppm";
	char str3[] = "texturesPPM/sky08.ppm";
	loadTexture(&str1, &mountainTex, &mountainTexDimensions);
	loadTexture(&str2, &seaTex, &seaTexDimensions);
	loadTexture(&str3, &skyTex, &skyTexDimensions);
}

void init(void) {
	// enables normals truncating (to unit length) so that lighting calculations are correct
	glEnable(GL_NORMALIZE);


	cylinderPtr = gluNewQuadric();
	// flipping cylinder inside out so normals face inwards
	gluQuadricOrientation(cylinderPtr, GLU_OUTSIDE);
	gluQuadricNormals(cylinderPtr, GLU_SMOOTH);

	diskPtr = gluNewQuadric();
	gluQuadricNormals(diskPtr, GLU_SMOOTH);


	if (wireframeToggle == 1) {
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		gluQuadricTexture(diskPtr, GLU_FALSE);
		gluQuadricDrawStyle(diskPtr, GLU_LINE);
		gluQuadricTexture(cylinderPtr, GLU_FALSE);
		gluQuadricDrawStyle(cylinderPtr, GLU_LINE);
	}

	else {
		glEnable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		gluQuadricTexture(diskPtr, GLU_TRUE);
		gluQuadricDrawStyle(diskPtr, GLU_FILL);
		gluQuadricTexture(cylinderPtr, GLU_TRUE);
		gluQuadricDrawStyle(cylinderPtr, GLU_FILL);
	}
	initMountains();
	initPlane();
	//printf("\n");
	initTextures();
}
void main(int argc, char **argv)
{
	glutInit(&argc, argv);

	srand(time(NULL));

	/* need both double buffering and z buffer */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);

	init();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, (float)windowWidth / (float)windowHeight, 0.1, 10000);
	glMatrixMode(GL_MODELVIEW);

	glutCreateWindow("Flight Sim");

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyboard);
	glutSpecialUpFunc(specialKeyboardUp);
	glutPassiveMotionFunc(mouseMovement);
	glutReshapeFunc(windowResize);

	// Enable Transparancy

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glShadeModel(GL_SMOOTH);


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

	glEnable(GL_DEPTH_TEST); /* Enable hidden--surface--removal */
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	// GL LINE for wireframe, GL_Fill for solid
	glPolygonMode(GL_FRONT, GL_LINE);

	glFogfv(GL_FOG_COLOR, skyWireframeDiffuse);
	glFogf(GL_FOG_MODE, GL_EXP);
	glFogf(GL_FOG_DENSITY, 0.01);

	glutMainLoop();

	cleanup();
}