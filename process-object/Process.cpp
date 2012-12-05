#include "Process.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <string.h>
#include <algorithm>
#include <stdexcept>
//

using namespace std;

 /* Initialize the process, create input/output pipes */
    Process::Process(const std::vector<std::string> &args)
    {	
		try
		{
				//char *newargv[] = { NULL };
	   		char *newenviron[] = { NULL };
	   		
	   		pipe(readpipe);
	   		pipe(writepipe);
	   		m_pread = fdopen(readpipe[0], "r");\
	   		
	   		//std::vector<std::string> = {"first arg", "second arg", "third"}
			std::vector<const char *> cargs;
			std::transform(args.begin(), args.end(), std::back_inserter(cargs),
					 []( const std::string s) { return s.c_str(); /*what would you return here?*/ } );
				cargs.push_back(NULL); // exec expects a NULL terminated array
	   		
	   		m_name = cargs[0];
	   		
	   		m_pid = fork();
	   		
	   		if (m_pid == 0) // this is child
	   		{
	   		
	   			try
	   			{
	   				dup2(writepipe[0], 0);
	   				close(writepipe[1]);
	   			}
	   			
	   			catch (const char* e)
	   			{
	   				std::cerr << strerror(errno);
	   			}
		 		
		 		try
	   			{
	   				dup2(readpipe[1], 1);
		 			close(readpipe[0]);
	   			}
	   			
	   			catch (const char* e)
	   			{
	   				std::cerr << strerror(errno);
	   			}
		 		
	   			//m_pread = cargs[0];  s.c_str()
	   			
	   			try
	   			{
	   				execve(m_name.c_str(), const_cast<char**>(&cargs[0]), newenviron);
	   			}
	   			
	   			catch (const char* e)
	   			{
	   				std::cerr << strerror(errno);
	   			}
	   			
	   			//execve(m_name.c_str(), const_cast<char**>(&cargs[0]), newenviron); // ./dispatcher.cpp ./generator ./consume
	   			
				//close(writepipe[0]);
				//close(writepipe[1]);
				//close(readpipe[0]);
				//close(readpipe[1]);
			
				exit(0);
	   		}
	   		
	   		else if (m_pid > 0) // parent process
	   		{	
	   			close(writepipe[0]);
				close(readpipe[1]);
			
				std::cout << "Parent[" <<  getpid() << "] Process constructor" << std::endl;
	   		}
	   		
	   		else
	   		{
	   			throw std::runtime_error(strerror(errno));	
			}
	   		
		}
		
		catch (const char* e)
		{
			std::cerr << e;
		}
    }
    
    /* Close any open file streams or file descriptors,
       insure that the child has terminated */
     Process::~Process()
    {
    	//close readpipe[1] and writepipe[0]
    	//close(readpipe[0]);          /* Close unused read end */
    	close(readpipe[1]);
    	close(writepipe[0]);
        //close(writepipefd[1]);
        fclose(m_pread);
        kill(m_pid, SIGTERM);
        int status;
        waitpid(m_pid, &status, 0);
    }
    
    /* write a string to the child process */
     void Process::writeline(const std::string& value)
    {
    
    		//close(readpipe[1]);          /* Close unused read end */
            write(writepipe[1], value.c_str(), strlen(value.c_str()));
            //close(writepipe[1]);          /* Reader will see EOF */
            //wait(NULL); 
            
           // dup2(writepipe[1], 1);
		   //close(writepipe[0]);
    }

    /* read a full line from child process, 
       if no line is available, block until one becomes available */
     std::string Process::readline()
    {
    	//string buf;
    	//read(readpipe[1], &buf, 1)
    	//return buf;
    	
    	char *line = NULL;
    	size_t len = 0;
    	
		getline(&line, &len, m_pread);
    	
    	return line;
    }
    
    //pid_t Process:: pid() const { return m_pid; };
