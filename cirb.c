// ====================================
// 2^n Circular Buffer Functions
// -----------------------------
//
// Author: Reine Gill
// Date: 2002-10-21
// ====================================

#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>      // Linux implementation of POSIX threads
#include <time.h>         // Standard date and time
#include "cirb.h"
#include "nice.h"

void         InB(buffer_struct_type *buff,unsigned char *data,int n);
void         In(buffer_struct_type *buff,unsigned char data);
int          InitBuffer(buffer_struct_type *buff,int size);
void         FreeBuffer(buffer_struct_type *buff);
int          Look(buffer_struct_type *buff,unsigned char *data,unsigned int offset);
int          LookB(buffer_struct_type *buff,unsigned char *data,unsigned int n);
int          Get(buffer_struct_type *buff,unsigned char *data);
int          GetB(buffer_struct_type *buff,unsigned char *data,unsigned int n);
int          Forward(buffer_struct_type *buff,unsigned int n);
int          FullBuffer(buffer_struct_type *buff,double fratio);
unsigned int GetBufferFill(buffer_struct_type *buff);

static pthread_mutex_t protect_cbuff=PTHREAD_MUTEX_INITIALIZER; 
 
//Add 1 byte of data to circular buffer 
void In(buffer_struct_type *buff,unsigned char data)
{

  pthread_mutex_lock(&protect_cbuff);	  

    buff->data[buff->inp++]=data;
    buff->inp&=buff->mask;
    if(buff->fill<buff->max) buff->fill++;

  pthread_mutex_unlock(&protect_cbuff);
}

//Add n bytes of data to circular buffer
void InB(buffer_struct_type *buff,unsigned char *data,int n)
{
  int i;
  pthread_mutex_lock(&protect_cbuff);	
    for(i=0;i<n;i++)
      {
        buff->data[buff->inp++]=data[i];
        buff->inp&=buff->mask;
        if(buff->fill<buff->max) 
	  buff->fill++;
      }
  pthread_mutex_unlock(&protect_cbuff);

}

// Get 1 byte of data from cirb, oldest data first
int Get(buffer_struct_type *buff,unsigned char *data)
{
  unsigned int outp;
  int status=0;
  pthread_mutex_lock(&protect_cbuff);	
  if(buff->fill>0)
    {
      outp=(buff->inp+buff->max-buff->fill) & buff->mask;
      *data=buff->data[outp];
      buff->fill--;
    }
  else
    status=-1; // Empty

  pthread_mutex_unlock(&protect_cbuff);
  return status;
}

// Get n bytes of data from circular buffer, oldest data first
// Assumes data buffer is large enough!
int GetB(buffer_struct_type *buff,unsigned char *data,unsigned int n)
{
  unsigned int outp;
  int i;  
  int status=0;
  
  pthread_mutex_lock(&protect_cbuff);	

    if(buff->fill>n)
      {
         for(i=0;i<n;i++)
	  {
	    outp=(buff->inp+buff->max-buff->fill) & buff->mask;
	    data[i]=buff->data[outp];
	    buff->fill--;
	  }
	  data[n]='\0';
      }
    else 
      status=-2;

  pthread_mutex_unlock(&protect_cbuff);
  return status;
}

// Look at n bytes of data from circular buffer, oldest data first
// Assumes data buffer is large enough!
// Note that the data is still considered to be in the circular buffer
int LookB(buffer_struct_type *buff,unsigned char *data,unsigned int n)
{
  unsigned int outp;
  unsigned int fill;
  int i;
  int status=0;
 
  pthread_mutex_lock(&protect_cbuff);	

  fill=buff->fill;
  if(buff->fill>n)
    {
        for(i=0;i<n;i++)
	{
	  outp=(buff->inp+buff->max-fill) & buff->mask;
	  data[i]=buff->data[outp];
	  fill--;
	}
	data[n]='\0';
    }
  else 
    status= -3;
  pthread_mutex_unlock(&protect_cbuff);
  return status;
}

// Look in buffer from oldest data and forward offset steps
int Look(buffer_struct_type *buff,unsigned char *data,unsigned int offset)
{
  unsigned int outp;
  int status=0;

   pthread_mutex_lock(&protect_cbuff);	

  if(buff->fill>offset)
    {
      outp=(buff->inp+buff->max-buff->fill+offset) & buff->mask;
      *data=buff->data[outp];
    }
  else
    status = -4;  //Out of range!
  pthread_mutex_unlock(&protect_cbuff);
  return status;
}

// Move Forward to newer data in buffer
// Returns steps left to move
int Forward(buffer_struct_type *buff,unsigned int n)
{
  int diff;

  pthread_mutex_lock(&protect_cbuff);	

  do
    {
      diff=buff->fill-n;
      if(diff<0)
	{
	  pthread_mutex_unlock(&protect_cbuff);	
	  sched_yield();
	  pthread_mutex_lock(&protect_cbuff);	
	}
      else
	{
	  buff->fill=diff;
	  pthread_mutex_unlock(&protect_cbuff);
	  return 0;
	}
    } while(diff<0);

  pthread_mutex_unlock(&protect_cbuff);
  return 0;
}

// Make buffer structure
int InitBuffer(buffer_struct_type *buff,int size)
{
  pthread_mutex_lock(&protect_cbuff);	

  buff->max  = size;
  buff->mask = size-1;
  buff->fill=0;
  buff->inp=0;
  if((buff->data =(unsigned char *) malloc(size))==NULL) return -5; 

  pthread_mutex_unlock(&protect_cbuff);
  return 0; //OK
}

// Free buffer
void FreeBuffer(buffer_struct_type *buff)
{ 

  pthread_mutex_lock(&protect_cbuff);	

  if(buff->data!=NULL)
    free(buff->data);

  buff->data=NULL;

  pthread_mutex_unlock(&protect_cbuff);
}


int FullBuffer(buffer_struct_type *buff,double fratio)
{
  int status;
  double ratio;
  pthread_mutex_lock(&protect_cbuff);	

  ratio=(((double)buff->fill)/((double)buff->max));

  if(ratio<fratio)
    status=0;
  else
    status=1;

  pthread_mutex_unlock(&protect_cbuff);


  return status;
}



// Erik P G Johansson 2015-03-31: Added function
// Read the circular buffer fill value, i.e. how much data is in the buffer.
unsigned int GetBufferFill(buffer_struct_type *buff)
{
  unsigned int fill_value;
  
  pthread_mutex_lock(&protect_cbuff);  // Should not use.
    
  fill_value = buff->fill;

  pthread_mutex_unlock(&protect_cbuff);

  return fill_value;
}
