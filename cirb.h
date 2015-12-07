#ifndef _CIRB_H_
#define _CIRB_H_
typedef struct buffer_struct
{
  unsigned int max;
  unsigned int fill;
  unsigned int mask;
  unsigned int inp;
  unsigned char *data;
} buffer_struct_type;
#endif // _CIRB_H_


