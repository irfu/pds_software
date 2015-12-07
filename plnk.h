#define MAX_STR 1024 
#define DNTCARE      0
#define CHECKED      1
#define UNCHECKED    2

typedef struct prp_property_type_def
{
  struct prp_property_type_def *next_p;
  struct prp_property_type_def *prev_p;
  char *name;
  char *value;
  char  checked;          // This adds the ability to check/uncheck a property
                          // We can then search among for instance the checked ones 
                          // or the unchecked ones.
                          // (It's put in as a char to save space, the compiler will
                          // fit the three first chars into a 4 byte aligned word space
                          // , It's always working with 4 byte boundaries.)
} property_type;

typedef struct prp_str_type_def
{
  int no_prop; // Number of properties
  property_type *head;
  property_type *properties; // Linked list of properties
} prp_type;


