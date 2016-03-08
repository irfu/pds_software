#define MAX_STR 1024 
#define DNTCARE      0
#define CHECKED      1
#define UNCHECKED    2



// WARNING: Confusing names of type definitions:
//    property_type : Property
//    prp_type      : Linked list of properties



// A "property" = (name,value,checked).
// Instances are LINKS, in linked lists.
// Is used for PDS LBL keywords (names) and their values.
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
                          // fit the three first chars into a 4 byte aligned word space.
                          // It is always working with 4-byte boundaries.)
                          // 
                          // Values: CHECKED, UNCHECKED
                          // (DNTCARE is used for function calls, but it is uncertain/doubtful if it is ever used as a value).
} property_type;



// Linked list of property_type.
typedef struct prp_str_type_def
{
  int no_prop;               // Number of properties
  property_type *head;
  property_type *properties; // Linked list of properties
} prp_type;


