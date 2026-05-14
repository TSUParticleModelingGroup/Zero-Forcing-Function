//void drawPicture();
//void Display(void);
//void reshape(int w, int h);
//void orthoganialView();
//void fulstrumView();
//void KeyPressed(unsigned char key, int x, int y);
//void mymouse(int button, int state, int x, int y);

void drawPicture()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	// Drawing nodes
	for(int i = 0; i < NumberOfNodes; i++)
	{
		glColor3d(Node[i].color.x, Node[i].color.y, Node[i].color.z);
		glPushMatrix();
		glTranslatef(Node[i].position.x, Node[i].position.y, Node[i].position.z);
		glutSolidSphere(NodeRadius,5,5);
		glPopMatrix();
	}
	/*
	glPointSize(10.0);
	glBegin(GL_POINTS);
	  for(int i = 0; i < NumberOfNodes; i++)
	  {
		  glColor3d(Node[i].color.x, Node[i].color.y, Node[i].color.z);
		  glVertex3f(Node[i].position.x, Node[i].position.y, Node[i].position.z);
	  }
	glEnd();
	*/
	
	// Drawing Edges
	glLineWidth(LineWidth);
	glColor3d(1.0, 1.0, 0.0);
	for(int i = 0; i < NumberOfEdges; i++)
	{
	      glBegin(GL_LINES);
	      glVertex3f(Node[Edge[i].nodeA].position.x, Node[Edge[i].nodeA].position.y, Node[Edge[i].nodeA].position.z);
	      glVertex3f(Node[Edge[i].nodeB].position.x, Node[Edge[i].nodeB].position.y, Node[Edge[i].nodeB].position.z);
              glEnd();
        }
        
        // Drawing selection sphere.
        if(SelectionFlag == 1)
        {
              glColor3d(1.0, 0.0, 0.0);
	      glPushMatrix();
	      glTranslatef(MouseX, MouseY, MouseZ);
	      glutSolidSphere(SelectionSize,10,10);
	      glPopMatrix();
        }
        
	glutSwapBuffers();
	
	if(MovieFlag == 1)
	{
		glReadPixels(5, 5, XWindowSize, YWindowSize, GL_RGBA, GL_UNSIGNED_BYTE, Buffer);
		fwrite(Buffer, sizeof(int)*XWindowSize*YWindowSize, 1, MovieFile);
	}
}

string getTimeStamp()
{
	// Want to get a time stamp string representing current date/time, so we have a
	// unique name for each video/screenshot taken.
	time_t t = time(0); 
	struct tm * now = localtime( & t );
	int month = now->tm_mon + 1, day = now->tm_mday, year = now->tm_year, 
				curTimeHour = now->tm_hour, curTimeMin = now->tm_min, curTimeSec = now->tm_sec;
	stringstream smonth, sday, syear, stimeHour, stimeMin, stimeSec;
	smonth << month;
	sday << day;
	syear << (year + 1900); // The computer starts counting from the year 1900, so 1900 is year 0. So we fix that.
	stimeHour << curTimeHour;
	stimeMin << curTimeMin;
	stimeSec << curTimeSec;
	string timeStamp;

	if (curTimeMin <= 9)	
		timeStamp = smonth.str() + "-" + sday.str() + "-" + syear.str() + '_' + stimeHour.str() + ".0" + stimeMin.str() + 
					"." + stimeSec.str();
	else			
		timeStamp = smonth.str() + "-" + sday.str() + '-' + syear.str() + "_" + stimeHour.str() + "." + stimeMin.str() +
					"." + stimeSec.str();
	return timeStamp;
}

void Display(void)
{
	drawPicture();
	zeroForcing();
}

void idle()
{
	zeroForcing();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

void orthoganialView()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(-RadiusOfObject, RadiusOfObject, -RadiusOfObject, RadiusOfObject, Near, Far);
	glOrtho(-1.0, 1.0, -1.0, 1.0, Near, Far);
	glMatrixMode(GL_MODELVIEW);
	ViewFlag = 0;
	drawPicture();
}

void fulstrumView()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-0.2, 0.2, -0.2, 0.2, Near, Far);
	glMatrixMode(GL_MODELVIEW);
	ViewFlag = 1;
	drawPicture();
}

void movieOn()
{
      string ts = getTimeStamp();
      ts.append(".mp4");

      // Build resolution string like "1000x1000"
      string resolution =
          std::to_string(X_WINDOW_SIZE) + "x" +
          std::to_string(Y_WINDOW_SIZE);

      // Setting up the movie buffer.
      string baseCommand =
          "ffmpeg -loglevel quiet -r 60 -f rawvideo "
          "-pix_fmt rgba -s " + resolution + " -i - "
          "-c:v libx264rgb -threads 0 -preset fast -y "
          "-pix_fmt yuv420p -crf 0 -vf vflip ";

      string z = baseCommand + ts;

      const char *ccx = z.c_str();
      MovieFile = popen(ccx, "w");

      Buffer = (int*)malloc(X_WINDOW_SIZE * Y_WINDOW_SIZE * sizeof(int));

      MovieFlag = 1;
}

void movieOff()
{
	if(MovieFlag == 1) 
	{
		pclose(MovieFile);
	}
	free(Buffer);
	MovieFlag = 0;
}

void screenShot()
{	
        FILE* ScreenShotFile;
        int* buffer;

        // Build resolution string like "1920x1080"
        string resolution =
            std::to_string(X_WINDOW_SIZE) + "x" +
            std::to_string(Y_WINDOW_SIZE);

        // Build ffmpeg command dynamically
        string cmdStr =
            "ffmpeg -loglevel quiet -framerate 60 "
            "-f rawvideo -pix_fmt rgba -s " + resolution + " -i - "
            "-c:v libx264rgb -threads 0 -preset fast "
            "-y -crf 0 -vf vflip output1.mp4";

        // Convert to C-string for popen
        const char* cmd = cmdStr.c_str();

        ScreenShotFile = popen(cmd, "w");

        buffer = (int*)malloc(XWindowSize * YWindowSize * sizeof(int));

        for (int i = 0; i < 1; i++)
        {
            drawPicture();

            glReadPixels(
                5, 5,
                XWindowSize,
                YWindowSize,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                buffer
            );

            fwrite(
                buffer,
                sizeof(int) * XWindowSize * YWindowSize,
                1,
                ScreenShotFile
            );
        }

        pclose(ScreenShotFile);
        free(buffer);

        string ts = getTimeStamp();

        // Build JPEG conversion command
        string s =
            "ffmpeg -loglevel quiet -i output1.mp4 "
            "-qscale:v 1 -qmin 1 -qmax 1 " +
            ts + ".jpeg";

        // Convert back to C-style string
        const char* ccx = s.c_str();

        system(ccx);
        system("rm output1.mp4");

        printf("\nScreenshot Captured: \n");
        cout << "Saved as " << ts << ".jpeg" << endl;
}

void KeyPressed(unsigned char key, int x, int y)
{
	float temp;
	
	if(key == 'q') // quit
	{
		glutDestroyWindow(Window);
		printf("\n Good Bye\n");
		exit(0);
	}
	
	if(key == '1')
	{
		IterateFlag = 1;
		terminalPrint();
	}
	
	if(key == 'f')
	{	
		if(FinishFlag == 0) FinishFlag = 1;
		else FinishFlag = 0;
		drawPicture();
		terminalPrint();
	}
	
	if(key == 'v') // Fulstrum/Orthoganal view toggle
	{
		if(ViewFlag == 0) 
		{
			ViewFlag = 1;
			fulstrumView();
		}
		else 
		{
			ViewFlag = 0;
			orthoganialView();
		}
		drawPicture();
		terminalPrint();
	}
	
	if(key == 'm')  // Movie on/off
	{
		if(MovieFlag == 0) 
		{
			MovieFlag = 1;
			movieOn();
		}
		else 
		{
			MovieFlag = 0;
			movieOff();
		}
		terminalPrint();
	}
	
	if(key == 'S')  // Screenshot
	{	
		screenShot();
		terminalPrint();
	}
	
	if(key == 's')
	{
	      if(SelectionFlag == 1) SelectionFlag = 0;
	      else
	      {
	          SelectionFlag = 1;
	          MouseX = 0.0;
	          MouseY = 0.0;
	          MouseZ = 0.0;
	      }
	      terminalPrint();
	      drawPicture();
	}
	if(key == '+')
	{
	      SelectionSize *= 1.1;
	      drawPicture();
	}
	if(key == '-')
	{
	      SelectionSize *= .9;
	      drawPicture();
	}
	
	if(key == 't')
	{
	      if(RotateTranslateFlag == 1) RotateTranslateFlag = 0;
	      else RotateTranslateFlag = 1;
	      terminalPrint();
	}
	if(key == 'x' && RotateTranslateFlag == 1)  // Rotate counterclockwise on the x-axis
	{
		for(int i = 0; i < NumberOfNodes; i++)
		{
			Node[i].position.x -= CenterOfSimulation.x;
			Node[i].position.y -= CenterOfSimulation.y;
			Node[i].position.z -= CenterOfSimulation.z;
			float temp = cos(RotateSpeed)*Node[i].position.y - sin(RotateSpeed)*Node[i].position.z;
			Node[i].position.z  = sin(RotateSpeed)*Node[i].position.y + cos(RotateSpeed)*Node[i].position.z;
			Node[i].position.y  = temp;
			Node[i].position.x += CenterOfSimulation.x;
			Node[i].position.y += CenterOfSimulation.y;
			Node[i].position.z += CenterOfSimulation.z;
		}
		drawPicture();
		AngleOfSimulation.x += RotateSpeed;
	}
	if(key == 'X' && RotateTranslateFlag == 1)  // Rotate clockwise on the x-axis
	{
		for(int i = 0; i < NumberOfNodes; i++)
		{
			Node[i].position.x -= CenterOfSimulation.x;
			Node[i].position.y -= CenterOfSimulation.y;
			Node[i].position.z -= CenterOfSimulation.z;
			temp = cos(-RotateSpeed)*Node[i].position.y - sin(-RotateSpeed)*Node[i].position.z;
			Node[i].position.z  = sin(-RotateSpeed)*Node[i].position.y + cos(-RotateSpeed)*Node[i].position.z;
			Node[i].position.y  = temp; 
			Node[i].position.x += CenterOfSimulation.x;
			Node[i].position.y += CenterOfSimulation.y;
			Node[i].position.z += CenterOfSimulation.z;
		}
		drawPicture();
		AngleOfSimulation.x -= RotateSpeed;
	}
	if(key == 'y' && RotateTranslateFlag == 1)  // Rotate counterclockwise on the y-axis
	{
		for(int i = 0; i < NumberOfNodes; i++)
		{
			Node[i].position.x -= CenterOfSimulation.x;
			Node[i].position.y -= CenterOfSimulation.y;
			Node[i].position.z -= CenterOfSimulation.z;
			temp =  cos(-RotateSpeed)*Node[i].position.x + sin(-RotateSpeed)*Node[i].position.z;
			Node[i].position.z  = -sin(-RotateSpeed)*Node[i].position.x + cos(-RotateSpeed)*Node[i].position.z;
			Node[i].position.x  = temp;
			Node[i].position.x += CenterOfSimulation.x;
			Node[i].position.y += CenterOfSimulation.y;
			Node[i].position.z += CenterOfSimulation.z;
		}
		drawPicture();
		AngleOfSimulation.y -= RotateSpeed;
	}
	if(key == 'Y' && RotateTranslateFlag == 1)  // Rotate clockwise on the y-axis
	{
		for(int i = 0; i < NumberOfNodes; i++)
		{
			Node[i].position.x -= CenterOfSimulation.x;
			Node[i].position.y -= CenterOfSimulation.y;
			Node[i].position.z -= CenterOfSimulation.z;
			temp = cos(RotateSpeed)*Node[i].position.x + sin(RotateSpeed)*Node[i].position.z;
			Node[i].position.z  = -sin(RotateSpeed)*Node[i].position.x + cos(RotateSpeed)*Node[i].position.z;
			Node[i].position.x  = temp;
			Node[i].position.x += CenterOfSimulation.x;
			Node[i].position.y += CenterOfSimulation.y;
			Node[i].position.z += CenterOfSimulation.z;
		}
		drawPicture();
		AngleOfSimulation.y += RotateSpeed;
	}
	if(key == 'z' && RotateTranslateFlag == 1)  // Rotate counterclockwise on the z-axis
	{
		for(int i = 0; i < NumberOfNodes; i++)
		{
			Node[i].position.x -= CenterOfSimulation.x;
			Node[i].position.y -= CenterOfSimulation.y;
			Node[i].position.z -= CenterOfSimulation.z;
			temp = cos(RotateSpeed)*Node[i].position.x - sin(RotateSpeed)*Node[i].position.y;
			Node[i].position.y  = sin(RotateSpeed)*Node[i].position.x + cos(RotateSpeed)*Node[i].position.y;
			Node[i].position.x  = temp;
			Node[i].position.x += CenterOfSimulation.x;
			Node[i].position.y += CenterOfSimulation.y;
			Node[i].position.z += CenterOfSimulation.z;
		}
		drawPicture();
		AngleOfSimulation.z += RotateSpeed;
	}
	if(key == 'Z' && RotateTranslateFlag == 1)  // Rotate clockwise on the z-axis
	{
		for(int i = 0; i < NumberOfNodes; i++)
		{
			Node[i].position.x -= CenterOfSimulation.x;
			Node[i].position.y -= CenterOfSimulation.y;
			Node[i].position.z -= CenterOfSimulation.z;
			temp = cos(-RotateSpeed)*Node[i].position.x - sin(-RotateSpeed)*Node[i].position.y;
			Node[i].position.y  = sin(-RotateSpeed)*Node[i].position.x + cos(-RotateSpeed)*Node[i].position.y;
			Node[i].position.x  = temp;
			Node[i].position.x += CenterOfSimulation.x;
			Node[i].position.y += CenterOfSimulation.y;
			Node[i].position.z += CenterOfSimulation.z;
		}
		drawPicture();
		AngleOfSimulation.z -= RotateSpeed;
	}
	
	if(key == 'x'  && RotateTranslateFlag == 0)  // Zoom in on x.
	{
		for(int i = 0; i < NumberOfNodes; i++)
		{
			Node[i].position.x += ZoomSpeed;
		}
		CenterOfSimulation.x += ZoomSpeed;
		drawPicture();
	}
	if(key == 'X'  && RotateTranslateFlag == 0)  // Zoom out on x.
	{
		for(int i = 0; i < NumberOfNodes; i++)
		{
			Node[i].position.x -= ZoomSpeed;
		}
		CenterOfSimulation.x -= ZoomSpeed;
		drawPicture();
	}
	if(key == 'y'  && RotateTranslateFlag == 0)  // Zoom in on x.
	{
		for(int i = 0; i < NumberOfNodes; i++)
		{
			Node[i].position.y += ZoomSpeed;
		}
		CenterOfSimulation.y += ZoomSpeed;
		drawPicture();
	}
	if(key == 'Y'  && RotateTranslateFlag == 0)  // Zoom out on x.
	{
		for(int i = 0; i < NumberOfNodes; i++)
		{
			Node[i].position.y -= ZoomSpeed;
		}
		CenterOfSimulation.y -= ZoomSpeed;
		drawPicture();
	}
	if(key == 'z'  && RotateTranslateFlag == 0)  // Zoom in on x.
	{
		for(int i = 0; i < NumberOfNodes; i++)
		{
			Node[i].position.z += ZoomSpeed;
		}
		CenterOfSimulation.z += ZoomSpeed;
		drawPicture();
	}
	if(key == 'Z'  && RotateTranslateFlag == 0)  // Zoom out on x.
	{
		for(int i = 0; i < NumberOfNodes; i++)
		{
			Node[i].position.z -= ZoomSpeed;
		}
		CenterOfSimulation.z -= ZoomSpeed;
		drawPicture();
	}
}

void mousePassiveMotionCallback(int x, int y) 
{
	// This function is called when the mouse moves without any button pressed
	// x and y are the current mouse coordinates
	// Use these coordinates as needed
	// x and y come in as 0 to XWindowSize and 0 to YWindowSize. This traveslates them to -1 to 1 and -1 to 1.

	MouseX = ( 2.0*x/XWindowSize - 1.0);
	MouseY = (-2.0*y/YWindowSize + 1.0);
	drawPicture();
}

void mymouse(int button, int state, int x, int y)
{	
	float dx, dy, dz;
	
	if(state == GLUT_DOWN && SelectionFlag == 1)
	{	
		if(button == GLUT_LEFT_BUTTON)
		{	
			for(int i = 0; i < NumberOfNodes; i++)
			{
				dx = MouseX - Node[i].position.x;
				dy = MouseY - Node[i].position.y;
				dz = MouseZ - Node[i].position.z;
				
				if(sqrt(dx*dx + dy*dy + dz*dz) < SelectionSize)
				{
					Node[i].on = 1;
					Node[i].color.x = 0.0;
					Node[i].color.y = 0.0;
					Node[i].color.z = 1.0;
				}
			}
		}
		else if(button == GLUT_RIGHT_BUTTON) // Right Mouse button down
		{
			for(int i = 0; i < NumberOfNodes; i++)
			{
				dx = MouseX - Node[i].position.x;
				dy = MouseY - Node[i].position.y;
				dz = MouseZ - Node[i].position.z;
				if(sqrt(dx*dx + dy*dy + dz*dz) < SelectionSize)
				{
					Node[i].on = 0;
					Node[i].color.x = 1.0;
					Node[i].color.y = 1.0;
					Node[i].color.z = 1.0;
				}
			}
		}
		drawPicture();
	}
	if(state == 0)
	{
		if(button == 3) //Scroll up
		{
			MouseZ -= ScrollSpeed;
		}
		else if(button == 4) //Scroll down
		{
			MouseZ += ScrollSpeed;
		}
		drawPicture();
	}
}

