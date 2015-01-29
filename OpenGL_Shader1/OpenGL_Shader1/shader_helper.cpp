#include "shader_helper.h"

GLuint VertexShaderObject = 0;
GLuint FragmentShaderObject = 0;

GLchar *vertexShaderSource;
GLchar *fragmentShaderSource;

//typedef struct 
//{
//  float location[4]; 
//  float color[4]; 
//} Vertex; 

typedef enum {
    EVertexShader,
    EFragmentShader,
} EShaderType;

void check_graphics() 
{
  const GLubyte *renderer = glGetString(GL_RENDERER); 
  const GLubyte *vendor = glGetString(GL_VENDOR); 
  const GLubyte *version = glGetString(GL_VERSION); 
  const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION); 
  GLint major, minor; 
  glGetIntegerv(GL_MAJOR_VERSION, &major); 
  glGetIntegerv(GL_MINOR_VERSION, &minor); 
  
  printf(" ----------------- Checking graphics capability ...\n"); 
  printf(" GL Vendor: %s \n", vendor); 
  printf(" GL Renderer: %s \n", renderer); 
  printf(" GL version: %s\n", version); 
  printf(" GL version: %d.%d\n", major, minor); 
  printf(" GLSL version: %s\n", glslVersion); 

  // now check the availability of shaders 

  if (! GLEW_ARB_vertex_program) printf(" ARB vertex program is not supported!!\n");  
  else printf(" ARB vertex program is supported!!\n"); 

  if (! GLEW_ARB_vertex_shader) printf(" ARB vertex shader is not supported!!\n");  
  else printf(" ARB vertex shader is supported!!\n"); 

  if (! GLEW_ARB_fragment_program) printf(" ARB fragment program is not supported!!\n");  
  else printf(" ARB fragment program is supported!!\n"); 

  if (! GLEW_ARB_fragment_shader) printf(" ARB fragment shader is not supported!!\n");  
  else printf(" ARB fragment shader is supported!!\n"); 

  // another way to query the shaders support 

  if (glewGetExtension("GL_ARB_fragment_shader")      != GL_TRUE ||
      glewGetExtension("GL_ARB_vertex_shader")        != GL_TRUE ||
      glewGetExtension("GL_ARB_shader_objects")       != GL_TRUE ||
      glewGetExtension("GL_ARB_shading_language_100") != GL_TRUE)
    {
      fprintf(stderr, "Driver does not support OpenGL Shading Language\n");
      exit(1);
    }
  else fprintf(stderr, "GLSL supported and ready to go\n"); 

  printf(" -----------------  checking graphics capability done. \n"); 

} 

int shaderSize(char *fileName, EShaderType shaderType)
{
    //
    // Returns the size in bytes of the shader fileName.
    // If an error occurred, it returns -1.
    //
    // File name convention:
    //
    // <fileName>.vert
    // <fileName>.frag
    //

    char name[100];
    int count = -1;

    strcpy_s(name, fileName);

    switch (shaderType)
    {
        case EVertexShader:
            strcat_s(name, ".vert");
            break;
        case EFragmentShader:
            strcat_s(name, ".frag");
            break;
        default:
            printf("ERROR: unknown shader file type\n");
            exit(1);
            break;
    }

    //
    // Open the file, seek to the end to find its length
    //

	std::ifstream t;
	int length;
	t.open(name);      // open input file
	t.seekg(0, std::ios::end);    // go to the end
	length = (int)t.tellg();           // report location (this is the length)
	t.seekg(0, std::ios::beg);    // go back to the beginning
	char * buffer = new char[length];    // allocate memory for a buffer of appropriate dimension
	t.read(buffer, length);       // read the whole file into the buffer
	t.close();                    // close file handle

    return length;
}

int readShader(char *fileName, EShaderType shaderType, char *shaderText, int size)
{
    //
    // Reads a shader from the supplied file and returns the shader in the
    // arrays passed in. Returns 1 if successful, 0 if an error occurred.
    // The parameter size is an upper limit of the amount of bytes to read.
    // It is ok for it to be too big.
    //
    FILE *fh;
    char name[100];
    int count;

    strcpy_s(name, fileName);

    switch (shaderType) 
    {
        case EVertexShader:
            strcat_s(name, ".vert");
            break;
        case EFragmentShader:
            strcat_s(name, ".frag");
            break;
        default:
            printf("ERROR: unknown shader file type\n");
            exit(1);
            break;
    }

    //
    // Open the file
    //

	errno_t err;
	err = fopen_s(&fh, name, "r");
	if (err != 0) {
		printf("error with opening file in readShader\n");
		return -1;
	}

    //
    // Get the shader from a file.
    //
    fseek(fh, 0, SEEK_SET);
    count = fread(shaderText, 1, size, fh);
    shaderText[count] = '\0';

    if (ferror(fh))
        count = 0;

    fclose(fh);
    return count;
}

int readShaderSource(char *fileName, GLchar **vertexShader, GLchar **fragmentShader)
{
    int vSize, fSize;

    //
    // Allocate memory to hold the source of our shaders.
    //
    vSize = shaderSize(fileName, EVertexShader);
    fSize = shaderSize(fileName, EFragmentShader);

    if ((vSize == -1) || (fSize == -1))
    {
        printf("Cannot determine size of the shader %s\n", fileName);
        return 0;
    }

    *vertexShader = (GLchar *) malloc(vSize);
    *fragmentShader = (GLchar *) malloc(fSize);

    //
    // Read the source code
    //
    if (!readShader(fileName, EVertexShader, *vertexShader, vSize))
    {
        printf("Cannot read the file %s.vert\n", fileName);
        return 0;
    }

    if (!readShader(fileName, EFragmentShader, *fragmentShader, fSize))
    {
        printf("Cannot read the file %s.frag\n", fileName);
        return 0;
    }

    return 1;
}

GLint getUniLoc(GLuint program, const GLchar *name)
{
    GLint loc;

    loc = glGetUniformLocation(program, name);

    if (loc == -1)
        printf("No such uniform named \"%s\"\n", name);
    return loc;
}

GLuint SetupGLSL(char *fileName){

    GLuint programObject;
	GLuint vertexShaderObject;
	GLuint fragmentShaderObject;

	check_graphics();  // check the capability of the graphics card in use
	
	programObject = glCreateProgram();  // create an overall shader program 
	if (programObject == 0) {  // error checking 
	  printf(" Error creating shader program object.\n"); 
	  exit(1); 
	} 
	else printf(" Succeeded creating shader program object.\n"); 

	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	if (vertexShaderObject == 0) {  // error checking 
	  printf(" Error creating vertex shader object.\n"); 
	  exit(1); 
	} 
	else printf(" Succeeded creating vertex shader object.\n"); 

	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	if (fragmentShaderObject == 0) {  // error checking 
	  printf(" Error creating fragment shader object.\n"); 
	  exit(1); 
	} 
	else printf(" Succeeded creating fragment shader object.\n"); 

	// now input the vertex and fragment programs as ascii 
	readShaderSource(fileName, &vertexShaderSource, &fragmentShaderSource); 
	glShaderSource(vertexShaderObject,1,(const GLchar**)&vertexShaderSource,NULL);
	glShaderSource(fragmentShaderObject,1,(const GLchar**)&fragmentShaderSource,NULL);

	// now compile the shader code; vertex shader first, followed by fragment shader 
	glCompileShader(vertexShaderObject);

	// error checking and printing out log if error 
	GLint result; 
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &result); 
	if (result == GL_FALSE) {
	  printf(" vertex shader compilation failed!\n"); 
	  GLint logLen; 
	  glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &logLen); 
	  if (logLen > 0) {
	    char *log = (char*) malloc(logLen); 
	    GLsizei written; 
	    glGetShaderInfoLog(vertexShaderObject, logLen, &written, log); 
	    printf("Sahder log: \n %s", log); 
	    free(log); 
	  }
	}

	glCompileShader(fragmentShaderObject);
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &result); 
	if (result == GL_FALSE) {
	  printf(" fragment shader compilation failed!\n"); 
	  GLint logLen; 
	  glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &logLen); 
	  if (logLen > 0) {
	    char *log = (char*) malloc(logLen); 
	    GLsizei written; 
	    glGetShaderInfoLog(fragmentShaderObject, logLen, &written, log); 
	    printf("Sahder log: \n %s", log); 
	    free(log); 
	  }
	}
	// insert compiler error checking code later 

	glAttachShader(programObject, vertexShaderObject);
	glAttachShader(programObject, fragmentShaderObject);

	glLinkProgram(programObject);
	// insert linking error checking code later 

	return(programObject); 
}