extern void ProtectPlnkInit(); // Setup protection mutex

extern int InitP(prp_type *); // Init pds linked list of property nodes
extern int Append(prp_type *,char *,char *); // Append at end of list
extern int DeleteNo(prp_type *,int);        // Delete by number in list
extern int DeleteP(prp_type *,char *,int);  // Delete by name and occurance
extern int InsertTopQ(prp_type *p,char *,char *); // Insert quoted string value at top of list
extern int InsertTopK(prp_type *p,char *,char *); // Insert string keywor value at top of list
extern int InsertTopQV(prp_type *,char *,unsigned int); // Insert at top as quoted value

extern int InsertTopV(prp_type *p,char *,unsigned int); // Insert value at top of list
// Delete between "from name" to "to name" 
extern int DeleteMidd(prp_type *p,char *frn,char *ton); 
extern int GetNo(prp_type *,property_type **,int); // Get by number 
extern int FindP(prp_type *,property_type **,char *,int,char);  // Find by name and occurance
 // Find by name backwards starting with  *prop1 returning result in prop2
extern int FindB(prp_type *p,property_type **prop1,property_type **prop2,char *name,char);
extern int SetPT(prp_type *,property_type *prop,int); // Set by property type and occurance
extern int SetP(prp_type *,char *,char *,int); // Set value by name and occurance 
extern int InsertA(prp_type *,char *,char *,char *,int); // Insert after occ occurance of name
extern int InsertAV(prp_type *,char *,char *,unsigned int,int); // Same as insert_a but value is integer 
extern int InsertB(prp_type *,char *,char *,char *,int); // Insert before occ occurance of name
extern int CopyPrp(property_type *,char *,char *); // Copy property type to property type
extern int DumpPrp(prp_type *); // Dump property structure to screen
extern int FDumpPrp(prp_type *,FILE *); // Dump property structure to file
extern int FreePrp(prp_type *); // Free up memory in property structure
extern int ComparePrp(property_type *,property_type *); // Compare property/value pairs
extern int UnCheckAll(prp_type *p); // Uncheck all checked properties
extern int Check(property_type *prop);   // Check this property node
extern int UnCheck(property_type *prop); // Uncheck this property node
