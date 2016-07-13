/*
Code that reads one Rosetta command log file and PRESUMABLY extracts RPCLAP bias commands.
mpb = make pds bias

Arguments: <Input file> <Output file>
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define 	LINESZ 2048  		

char *time2pdsdate(unsigned char *dt)
{

  static unsigned char stime[30];
  int year;
  int doy;
  int h;
  int m;
  int s;
  int ms;
  
  time_t time;
  struct tm tm;
  struct tm *to;

  sscanf(dt,"%02d.%03d.%02d.%02d.%02d.%03d",&year,&doy,&h,&m,&s,&ms);

  tm.tm_sec=s;
  tm.tm_min=m;
  tm.tm_hour=h;
  tm.tm_mday=1;
  tm.tm_mon=0;
  tm.tm_year=(2000-1900+year);
  tm.tm_wday=0;  
  tm.tm_yday=0; // This field is ignored by mktime there fore we need to do things akwardly
  tm.tm_isdst=0;
  time=mktime(&tm); // Here we have the time since 1970 up to "year" 1 Jan 00:00:00

  // Compute time in seconds, compensate for mday 1 above

  time=time+(doy-1)*86400-timezone; // +3600;

  to=gmtime(&time); // Break time down again, to get month and day in month
// FKJN 8Jan2015 added milliseconds. I don't see why they weren't added before, probably forgotten because mktime doesnt use it. Eitherway, I anticipate no rounding errors because of this
  sprintf(stime,"%4d-%02d-%02dT%02d:%02d:%06.3f",to->tm_year+1900,to->tm_mon+1,to->tm_mday,to->tm_hour,to->tm_min,((double)to->tm_sec)+(double)ms/1000);
  
  return(stime);

}

int main (int argc, char **argv) 
{
  
  FILE *ifd,*ofd;
  int i;
  int ch;
  char line[LINESZ];
  char *match;
  char *token;

  char date[20];
  unsigned int dbias_12;
  unsigned int dbias_1;
  unsigned int ebias_12;

  unsigned int mode;

  int skip=0;

  int state=0;
  int ostate=0;

  if (argc!=3) {
    printf("%s <infile> <outfile>\n",argv[0]);
    exit(1);
  }

  if (!(ifd=fopen(argv[1],"r"))) 
    {
      perror("input file");
      exit(1);
    }
  
  if (!(ofd=fopen(argv[2],"w"))) 
    {
      perror("output file");
      fclose(ifd);
      exit(1);
    }

  while(fgets(line,LINESZ-1,ifd)!=NULL)
    {
      line[LINESZ-1]='\0';

      // If anything is aborted..do abort skipping of lines after this
      if(strstr(line,"ABORTED")!=NULL)
	{
	  ostate=state;
	  state=1;
	  continue;
	}

      switch(state)
	{
	  // Match start of bias settings
	case 0:

	  if(strstr(line,"ZRP23013")!=NULL)
	    {
	      if((match=strstr(line,"ARPF390A"))!=NULL)
		{
		  strtok(match," ");
		  strtok(NULL," ");
		  strcpy(date,strtok(NULL," "));

		  fprintf(ofd,"%s",time2pdsdate(date));
		  state=2;
		  continue;
		}
	    }
	 
//edit FKJN 8Jan2015  extra if statement	 
	  if((match=strstr(line,"INSERTTT"))==NULL)
                {
 
		  if((match=strstr(line,"ARPS804A"))!=NULL)
			{
			  strtok(match," ");
			  strtok(NULL," ");
			  strcpy(date,strtok(NULL," "));
	
			  fprintf(ofd,"%s",time2pdsdate(date));
			  state=5;
			  continue;
			}

//edited one line, since we have new OBCPs but all are named ARPS809A / ARPS809C OR ARPS809D
//FJ & AIE 2014 05 13
// original code:   if((match=strstr(line,"ARPS809A"))!=NULL)

		  if((match=strstr(line,"ARPS809"))!=NULL)
			{
			  strtok(match," ");
			  strtok(NULL," ");
			  strcpy(date,strtok(NULL," "));
	
			  fprintf(ofd,"%s",time2pdsdate(date));
			  state=5;
			  continue;
			}
		}

	  break;

	  // Skip 4 next lines starting with PRPD..revert to old state if none
  	case 1:
	  if(strstr(line,"PRPD30")!=NULL)
	    skip++;
	  else
	    state=ostate;

	  if(skip==4)
	    {
	      skip=0;
	      state=ostate;
	    }
	  continue;
	  break;


	case 2:
	  if((match=strstr(line,"PRPD3046"))!=NULL)
	    { 
	      token=strtok(match," ");
	      while(strncmp(token,"Dec",3)!=0)
		token=strtok(NULL," ");

	      token=strtok(NULL," ");
	      
	      sscanf(token," %d",&dbias_12);
	       
	      state=3;
	      continue;
	    }
	  break;

	   // The way procedure ARPF390A is made, is that it supports old software
	   // with a fix. Thus Density P1 is not found in state 1 but here
	   // ARPF390A could remove this step.
	case 3:
	   if((match=strstr(line,"PRPD3055"))!=NULL)
	    {
	      
	      token=strtok(match," ");

	      while(strncmp(token,"Dec",3)!=0)
		token=strtok(NULL," ");

	      token=strtok(NULL," ");
	      
	      sscanf(token," %d",&dbias_1);
	       
	      fprintf(ofd,"\t0x%02x%02x",dbias_1,(dbias_12 & 0xff));
	      state=4;
	      continue;
	    }
	   break;

	case 4:  
	   if((match=strstr(line,"PRPD3050"))!=NULL)
	    {
	      token=strtok(match," ");

	      while(strncmp(token,"Dec",3)!=0)
		token=strtok(NULL," ");

	      token=strtok(NULL," ");
	      
	      sscanf(token," %d",&ebias_12);
	       
	      fprintf(ofd,"\t0x%04x\n",ebias_12);
	      state=0;
	      continue;
	    }
	  break;

	case 5:
	  if((match=strstr(line,"FSK01267LAPParam"))!=NULL)
	    { 
	      token=strtok(match," ");

	      while(strncmp(token,"Dec",3)!=0 && strncmp(token,"Hex",3)!=0)
		token=strtok(NULL," ");

	      token=strtok(NULL," ");
	      
	      sscanf(token," %d",&mode);
	      fprintf(ofd,"\t*Mode*\t0x%04x\n",mode);
	      state=0;
	      continue;
	    }
	  break;
	}
    }
  fclose(ofd);
  fclose(ifd);

}
