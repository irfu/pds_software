#define _POSIX_C_SOURCE 200112L

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include "plnk.h"

void ProtectPlnkInit();                                         // Setup protection mutex

int InitP(prp_type *);						// Init pds linked list of property nodes.
int Append(prp_type *,char *,char *);				// Append at end of list
int DeleteNo(prp_type *,int);					// Delete by number in list
int DeleteP(prp_type *,char *,int);				// Delete by name and occurrence
int InsertTopK(prp_type *,char *,char *);			// Insert at top of list as keyword
int InsertTopQ(prp_type *,char *,char *);			// Insert at top of list as quoted string
int InsertTopV(prp_type *,char *,unsigned int);			// Insert at top as value
int InsertTopQV(prp_type *,char *,unsigned int);		// Insert at top as quoted value


// Delete between "from name" to "to name" 
int DeleteMidd(prp_type *p,char *frn,char *ton); 
int GetNo(prp_type *,property_type **,int);			// Get by number 
int FindP(prp_type *,property_type **,char *,int,char);		// Find by name, occurrence and if checked, or not, or don't care
// Find by name backwards starting with  *prop1 returning result in prop2, and if checked or not or don't care
int FindB(prp_type *p,property_type **prop1,property_type **prop2,char *name,char); 
int SetPT(prp_type *,property_type *prop,int);			// Set by property type and occurrence
int SetP(prp_type *,char *,char *,int);				// Set value by name and occurrence 
int InsertA(prp_type *,char *,char *,char *,int);		// Insert after occ occurrence of name
int InsertAV(prp_type *,char *,char *,unsigned int,int);	// Same as insert_a but value is integer 
int InsertB(prp_type *,char *,char *,char *,int);		// Insert before occ occurrence of name
int CopyPrp(property_type *,char *,char *);			// Copy property type to property type
int DumpPrp(prp_type *);					// Dump property structure to screen
int FDumpPrp(prp_type *,FILE *);				// Dump properties to file
int FreePrp(prp_type *);					// Free up memory in property structure
int ComparePrp(property_type *,property_type *);		// Compare property/value pairs
int UnCheckAll(prp_type *p);					// Uncheck all checked properties
int Check(property_type *prop);					// Check this property node
int UnCheck(property_type *prop);				// Uncheck this property node



static pthread_mutexattr_t    attr_plnk;
static pthread_mutex_t        protect_plnk;


// TEST OF LINKED PROPERTY LIST
/*
int main()
{
  property_type *prop=NULL;
  prp_type pds;
 
  InitP(&pds);
  Append(&pds,"Hejja","Surk�ping");
  Append(&pds,"Ni","e b�st");
  Append(&pds,"Ni","e dom");
  Append(&pds,"Ni","som");
  Append(&pds,"svettas","mest");
  Append(&pds,"kaka","raka");
  Append(&pds,"borja","kraka");
  DumpPrp(&pds);
  printf("-----------------------\n");
 
  //DeleteMidd(&pds,"Dom","svettas");
 
  FindP(&pds,&prop,"Ni",3,0);
  if(FindB(&pds,&prop,"Ni",0)>0)
    printf("Prop %s = %s\n",prop->name,prop->value);
  DumpPrp(&pds);
  FreePrp(&pds);
}
*/


// It appears (empirically) that one has to call this function once in the application to initialize the "mutex" "protect_plnk".
// Otherwise functions here might hang.
// Erik P G Johansson
void ProtectPlnkInit()
{
    pthread_mutexattr_settype(&attr_plnk,PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&protect_plnk,&attr_plnk);
    
}

// NOTE: Does NOT allocate any dynamic memory.
int InitP(prp_type *p)
{
    pthread_mutex_lock(&protect_plnk);
    
    if(p==NULL)
    {
        pthread_mutex_unlock(&protect_plnk);
        return -1; // Err null pointer
    }
    
    p->properties=NULL;
    p->head=NULL;
    p->no_prop=0;
    
    pthread_mutex_unlock(&protect_plnk);
    
    return 0; // Ok
}

// Append property to end of property list, regardless
// of whether or not "name" can be found in the initial list.
// The new property is UNCHECKED (through CopyPrp).
int Append(prp_type *p,char *name,char *value)
{
    property_type *tmp;
    
    pthread_mutex_lock(&protect_plnk);
    
    if(p->no_prop==0)
    {
        // CASE: The linked list is empty.
        if((p->properties=malloc(sizeof(property_type)))==NULL) 
        {
            pthread_mutex_unlock(&protect_plnk); 
            return -1;
        }
        
        if(CopyPrp(p->properties,name,value)<0) 
        {
            pthread_mutex_unlock(&protect_plnk); 
            return -3;
        }
        
        p->properties->next_p=NULL;
        p->properties->prev_p=NULL;
        p->head=p->properties;
        p->no_prop++; 
        pthread_mutex_unlock(&protect_plnk);
        return 0; //Ok
    }
    else {
        if(p->no_prop>0)
        {
            // CASE: The linked list is non-empty.
            if((tmp=malloc(sizeof(property_type)))==NULL)  
            {
                pthread_mutex_unlock(&protect_plnk);
                return -1;
            }
            
            if(CopyPrp(tmp,name,value)<0)  
            {
                pthread_mutex_unlock(&protect_plnk); 
                return -3;
            }
            
            p->head->next_p=tmp;
            tmp->prev_p=p->head;
            tmp->next_p=NULL;
            p->head=tmp;
            p->no_prop++;
            pthread_mutex_unlock(&protect_plnk);
            return 0; //Ok
        }  
        
        // CASE: p->no_prop < 0
        pthread_mutex_unlock(&protect_plnk);
        return -2; // Error
    }
}

int InsertTopQ(prp_type *p,char *name,char *value)
{
    char tstr[MAX_STR]; //Temporary..
    property_type *tmp;
    
    
    pthread_mutex_lock(&protect_plnk);	
    
    if(value!=NULL) {
        sprintf(tstr,"\"%s\"",value);//Add quotes
    }
        
        if(p->no_prop==0)
        {
            if((p->properties=malloc(sizeof(property_type)))==NULL) 
            { 
                pthread_mutex_unlock(&protect_plnk); 
                return -1;
            }
            
            if(CopyPrp(p->properties,name,tstr)<0) 
            { 
                pthread_mutex_unlock(&protect_plnk); 
                return -3;
            }
            
            p->properties->next_p=NULL;
            p->properties->prev_p=NULL;
            p->head=p->properties;
            p->no_prop++;
            pthread_mutex_unlock(&protect_plnk);
            return 0; //Ok
        }
        else
        {
            if(p->no_prop>0)
            {
                if((tmp=malloc(sizeof(property_type)))==NULL) 
                { 
                    pthread_mutex_unlock(&protect_plnk); 
                    return -1;
                }
                
                if(CopyPrp(tmp,name,tstr)<0) 
                { 
                    pthread_mutex_unlock(&protect_plnk); 
                    return -3;
                }
                
                tmp->prev_p=NULL;
                tmp->next_p=p->properties;
                p->properties->prev_p=tmp;
                p->properties=tmp;
                p->no_prop++;
                pthread_mutex_unlock(&protect_plnk);
                return 0; //Ok
            }
        }
            
        pthread_mutex_unlock(&protect_plnk);
        return -2; // Error
}

int InsertTopK(prp_type *p,char *name,char *value)
{
    property_type *tmp;
    
    pthread_mutex_lock(&protect_plnk);	
    if(p->no_prop==0)
    {
        if((p->properties=malloc(sizeof(property_type)))==NULL) 
        {
            pthread_mutex_unlock(&protect_plnk);  
            return -1;
        }
        
        if(CopyPrp(p->properties,name,value)<0) 
        {
            pthread_mutex_unlock(&protect_plnk);
            return -3;
        }
        
        p->properties->next_p=NULL;
        p->properties->prev_p=NULL;
        p->head=p->properties;
        p->no_prop++;
        pthread_mutex_unlock(&protect_plnk);
        return 0; //Ok
    }
    else
    {
        if(p->no_prop>0)
        {
            if((tmp=malloc(sizeof(property_type)))==NULL) 
            {
                pthread_mutex_unlock(&protect_plnk);
                return -1;
            }
            
            if(CopyPrp(tmp,name,value)<0) 
            {
                pthread_mutex_unlock(&protect_plnk);
                return -3;
            }
            
            tmp->prev_p=NULL;
            tmp->next_p=p->properties;
            p->properties->prev_p=tmp;
            p->properties=tmp;
            p->no_prop++;     
            pthread_mutex_unlock(&protect_plnk);
            return 0; //Ok
        }     
    }
    
    pthread_mutex_unlock(&protect_plnk);
    return -2; // Error
}

int InsertTopV(prp_type *p,char *name,unsigned int value)
{
    property_type *tmp;
    char ivalue[32];
    
    pthread_mutex_lock(&protect_plnk);	
    sprintf(ivalue,"0x%04x",value);
    if(p->no_prop==0)
    { 
        if((p->properties=malloc(sizeof(property_type)))==NULL) 
        { 
            pthread_mutex_unlock(&protect_plnk);
            return -1;
        }
        
        if(CopyPrp(p->properties,name,ivalue)<0) 
        { 
            pthread_mutex_unlock(&protect_plnk);
            return -3;
        }
        
        p->properties->next_p=NULL;
        p->properties->prev_p=NULL;
        p->head=p->properties;
        p->no_prop++;
        pthread_mutex_unlock(&protect_plnk);
        return 0; //Ok
    }
    else
    {
        if(p->no_prop>0)
        {
            if((tmp=malloc(sizeof(property_type)))==NULL) 
            { 
                pthread_mutex_unlock(&protect_plnk);
                return -1;
            }
            
            if(CopyPrp(tmp,name,ivalue)<0) 
            { 
                pthread_mutex_unlock(&protect_plnk);
                return -3;
            }
            
            tmp->prev_p=NULL;
            tmp->next_p=p->properties;
            p->properties->prev_p=tmp;
            p->properties=tmp;
            p->no_prop++;
            pthread_mutex_unlock(&protect_plnk);
            return 0; //Ok
        }
    }
    
    pthread_mutex_unlock(&protect_plnk);
    return -2; // Error
}

int InsertTopQV(prp_type *p,char *name,unsigned int value)
{
    property_type *tmp;
    char ivalue[32];
    
    pthread_mutex_lock(&protect_plnk);	
    
    sprintf(ivalue,"\"0x%04x\"",value);
    if(p->no_prop==0)
    { 
        if((p->properties=malloc(sizeof(property_type)))==NULL)
        { 
            pthread_mutex_unlock(&protect_plnk); 
            return -1;
        }
        
        if(CopyPrp(p->properties,name,ivalue)<0)
        { 
            pthread_mutex_unlock(&protect_plnk); 
            return -3;
        }
        
        p->properties->next_p=NULL;
        p->properties->prev_p=NULL;
        p->head=p->properties;
        p->no_prop++;     
        pthread_mutex_unlock(&protect_plnk);
        return 0; //Ok
    }
    else
    {
        if(p->no_prop>0)
        {
            if((tmp=malloc(sizeof(property_type)))==NULL) 
            { 
                pthread_mutex_unlock(&protect_plnk);
                return -1;
            }
            
            if(CopyPrp(tmp,name,ivalue)<0) 
            { 
                pthread_mutex_unlock(&protect_plnk);
                return -3;
            }
            
            tmp->prev_p=NULL;
            tmp->next_p=p->properties;
            p->properties->prev_p=tmp;
            p->properties=tmp;
            p->no_prop++;     
            pthread_mutex_unlock(&protect_plnk);
            return 0; //Ok
        }
    }
    
    pthread_mutex_unlock(&protect_plnk);
    return -2; // Error
}



// Sets "name", "value" and "checked" of a (preallocated) property.
// NOTE: Copies the values of "name" and "value" into their own newly allocated memory.
// NOTE: Sets checked := UNCHECKED.
int CopyPrp(property_type *dest, char *name, char *value)
{
    int len;
    
    pthread_mutex_lock(&protect_plnk);
    
    len=strlen(name);
    if(len==0 || len > MAX_STR) 
    {
        pthread_mutex_unlock(&protect_plnk);
        return -1;
    }
    
    if((dest->name=malloc(len+1))==NULL) 
    {
        pthread_mutex_unlock(&protect_plnk);
        return -2;
    }
    
    strncpy(dest->name,name,len+1);
    
    // if len is 0 we still need to make space for the null character!
    len=strlen(value); 
    
    if((dest->value=malloc(len+1))==NULL) 
    {
        pthread_mutex_unlock(&protect_plnk);
        return -5;
    }
    
    strncpy(dest->value,value,len+1);
    dest->checked=UNCHECKED; // Set checked to 0 as default
    
    pthread_mutex_unlock(&protect_plnk);
    return 0;
}



// Compare property/value pairs
int ComparePrp(property_type *p1,property_type *p2)
{  
    
    pthread_mutex_lock(&protect_plnk);	
    if(strcmp(p1->name,p2->name)) 
    {
        pthread_mutex_unlock(&protect_plnk);
        return -1;   // Property name doesn't match
    }
    
    if(strcmp(p1->value,p2->value)) 
    {
        pthread_mutex_unlock(&protect_plnk);
        return -2; // Property value doesn't match
    }
    
    pthread_mutex_unlock(&protect_plnk);
    
    return 0; // Property/Value pairs match!
}

int UnCheckAll(prp_type *p)
{
    int i;
    property_type *tmp;
    
    pthread_mutex_lock(&protect_plnk);	
    
    if(p->properties!=NULL && p->head!=NULL)
    {
        tmp=p->properties;
        for(i=0;i<p->no_prop;i++)
        {
            if(tmp!=NULL)
                tmp->checked=UNCHECKED;
            tmp=tmp->next_p;
        } 
        pthread_mutex_unlock(&protect_plnk);
        return 0; // Ok
    }
    pthread_mutex_unlock(&protect_plnk);
    return -1; // Error empty
}


// Check this property node
int Check(property_type *prop)   
{ 
    pthread_mutex_lock(&protect_plnk);	
    
    if(prop!=NULL)
    {
        prop->checked=CHECKED;
        pthread_mutex_unlock(&protect_plnk);
        return 0; // Ok!  
    }  
    pthread_mutex_unlock(&protect_plnk);
    return -1;
}

// Uncheck this property node
int UnCheck(property_type *prop) 
{ 
    
    pthread_mutex_lock(&protect_plnk);	
    if(prop!=NULL)
    {
        prop->checked=UNCHECKED;
        pthread_mutex_unlock(&protect_plnk);
        return 0; // Ok!
    }     
    pthread_mutex_unlock(&protect_plnk);
    return -1;
}



/**
 * Prints all the properties as key = value.
 * Presumably intended for debugging, maybe logging.
 *
 * NOTE: Prints nothing (empty string) as value for value pointer=NULL.
 * NOTE: Does not quote strings. Note that some values may, by themselves, contain quotes.
 *
 * Return value : 0=Ok, 1=Something wrong with linked list
 */
int DumpPrp(prp_type *p)
{
    int i;
    property_type *tmp;
    
    pthread_mutex_lock(&protect_plnk);
    
    if(p->properties!=NULL && p->head!=NULL)
    {
        tmp=p->properties;
        for(i=0;i<p->no_prop;i++)
        {
            if(tmp!=NULL && tmp->name!=NULL)
            {
                printf("%s",tmp->name);
                if(tmp->value!=NULL) {
                    printf("= %s",tmp->value);
                }
                printf("\n");
                //CPrintf("%s = %s\n", tmp->name, tmp->value);
            }
            tmp=tmp->next_p;
        }
        printf("No. properties: %d\n",p->no_prop);
        //CPrintf("No. properties: %d\n",p->no_prop);
        pthread_mutex_unlock(&protect_plnk);
        return 0; // Ok
    }
    pthread_mutex_unlock(&protect_plnk);
    return -1; // Error empty
}



// Write properties to file, as an ODL file.
int FDumpPrp(prp_type *p,FILE *fd)
{
    int i;
    property_type *tmp;
    
    pthread_mutex_lock(&protect_plnk);
    
    if(p->properties!=NULL && p->head!=NULL && fd!=NULL)
    {
        tmp=p->properties;
        for(i=0;i<p->no_prop;i++)
        {
            if(tmp!=NULL && tmp->name!=NULL)
            {
                fprintf(fd,"%s",tmp->name);
                if(tmp->value!=NULL) {
                    fprintf(fd," = %s",tmp->value);
                }
                fprintf(fd,"\r\n");
            }
            tmp=tmp->next_p;
            if(tmp==NULL)
            {
                pthread_mutex_unlock(&protect_plnk);
                return -1; // Error
            }
        }
        pthread_mutex_unlock(&protect_plnk);
        return 0; // Ok
    }
    pthread_mutex_unlock(&protect_plnk);
    return -1; // Error empty
}

// Clear the property list (clear the linked list and deallocate all its allocated memory, incl. name+value) and set it to length zero.
int FreePrp(prp_type *p)
{
    int i;
    property_type *tmp1;
    property_type *tmp2;
    
    pthread_mutex_lock(&protect_plnk);
    
    if(p->properties!=NULL && p->head!=NULL && p->no_prop!=0)
    {
        tmp1=p->properties;
        for(i=0;i<p->no_prop;i++)
        {
            if(tmp1!=NULL)
            {
                if(tmp1->name!=NULL) free(tmp1->name);
                if(tmp1->value!=NULL) free(tmp1->value);
                tmp2=tmp1;
                tmp1=tmp1->next_p;
                if(tmp2!=NULL) free(tmp2);
            }
        }
        p->properties=NULL;
        p->head=NULL;
        p->no_prop=0; 
        pthread_mutex_unlock(&protect_plnk);
        return 0; // Ok
    }
    
    pthread_mutex_unlock(&protect_plnk);
    return 1; // Warning nothing deleted! But it was empty anyway
}



//
// Return in prop the no'th property_type in p (starting from p->properties).
//
int GetNo(prp_type *p,property_type **prop,int no)
{
    int i;
    property_type *tmp;
    
    pthread_mutex_lock(&protect_plnk);	
    
    *prop=NULL;
    
    if(p->properties!=NULL && p->head!=NULL)
    {
        tmp=p->properties;
        for(i=0;i<p->no_prop && i!=no;i++)
            tmp=tmp->next_p;
        *prop=tmp; 
        
        pthread_mutex_unlock(&protect_plnk);
        return 0; // Ok
    } 
    
    pthread_mutex_unlock(&protect_plnk);
    return -1; // Error empty
}



/**
 * Return pointer ("*prop") to instance of property_type within *p where *prop->name matches *name.
 * Of those matches that have the specified "check" value matching [property_type]->check, the functions returns the "occ:th" one.
 *
 * check = DNTCARE, CHECKED, or UNCHECKED
 * Return value : 1=Found it, 0=Did not find it (but otherwise OK), -2=Error (Something is wrong with the linked list)
*/
int FindP(prp_type *p, property_type **prop, char *name, int occ, char check)
{
    int i;
    int oc=0;  // Occurrence
    property_type *tmp; 
    
    pthread_mutex_lock(&protect_plnk);
    
    
    *prop=NULL;
    if(p->properties!=NULL && p->head!=NULL && name!=NULL)
    {
        tmp=p->properties;
        for(i=0;i<p->no_prop;i++)
        {
            if(tmp==NULL)   
            {
                pthread_mutex_unlock(&protect_plnk);
                return -1;  // Error, not found
            }
            
            if(!strcmp(tmp->name,name))
            {
                if(tmp->checked==check || check==DNTCARE) // Find occ occurrence of checked or unchecked properties or don't care
                {
                    *prop=tmp;
                    oc++;
                    if(oc==occ)  // Found occ occurrence of name  
                    {
                        pthread_mutex_unlock(&protect_plnk);
                        return 1;  // Ok! Found it
                    }
                }
            } 
            tmp=tmp->next_p;
        }  
        
        pthread_mutex_unlock(&protect_plnk);
        return 0;  // Ok, but didn't find it.
    }  
    pthread_mutex_unlock(&protect_plnk);
    return -2;  // Error! Empty
}



//
// Find name backwards (->prev_p direction) starting with *prop1.
// "FindB" = "Find backwards"
// 
// *prop1 : Pointer to the property where the search begins. Must be in the linked property list.
// *prop2 : Pointer to the property that was found (if any).
// *name : Property name to search for.
// 
// Return result:
// -1 = Error
//  0 = Found nothing
//  1 = Found it
//
int FindB(prp_type *p, property_type **prop1, property_type **prop2, char *name, char check)
{
    property_type *tmp;
    
    pthread_mutex_lock(&protect_plnk);	
    
    tmp=*prop1; // Start at property
    if(p->properties!=NULL && p->head!=NULL && name!=NULL && tmp!=NULL)
    { 
        tmp=tmp->prev_p; // Go backwards
        while(tmp!=NULL) // Search backwards
        { 
            if(!strcmp(tmp->name,name)) // Found it ?
            {
                if(tmp->checked==check || check==DNTCARE) // Find checked or unchecked
                {
                    *prop2=tmp; // Found it return new pointer to it 
                    
                    pthread_mutex_unlock(&protect_plnk);
                    return 1; // Ok! found it
                }
            } 
            tmp=tmp->prev_p; // Go backwards
        }
        
        pthread_mutex_unlock(&protect_plnk);
        return 0; // Ok! but didn't find it
    } 
    
    pthread_mutex_unlock(&protect_plnk);
    return -1; // Error! empty
}


int DeleteP(prp_type *p,char *name,int occ)
{
    int i;
    int oc=0; // Occurrence
    property_type *tmp;
    
    pthread_mutex_lock(&protect_plnk);	
    
    if(p->properties!=NULL && p->head!=NULL && name!=NULL)
    {
        tmp=p->properties;
        for(i=0;i<p->no_prop;i++)
        {
            if(tmp==NULL) 
            {
                pthread_mutex_unlock(&protect_plnk);
                return -1; //Err not found
            }
            
            if(!strcmp(tmp->name,name))
            {
                oc++;
                if(oc==occ) // Found occ occurrence of name
                {
                    if(DeleteNo(p,i)<0) //Delete by number      
                    {
                        pthread_mutex_unlock(&protect_plnk);
                        return -2; //Err mangled or out of range
                    }
                    else
                    {
                        pthread_mutex_unlock(&protect_plnk);
                        return 1; // Ok! found it and deleted it
                    }
                }
            } 
            tmp=tmp->next_p;
        }
        pthread_mutex_unlock(&protect_plnk);
        return 0; // Ok! but didn't find it
    }
    pthread_mutex_unlock(&protect_plnk);
    return -3; // Error! empty
}

int DeleteMidd(prp_type *p,char *frn,char *ton)
{
    property_type *fr=NULL;
    property_type *to=NULL;
    
    pthread_mutex_lock(&protect_plnk);	
    
    if(p->properties!=NULL && p->head!=NULL && frn!=NULL && ton!=NULL)
    {
        if(FindP(p,&fr,frn,1,DNTCARE)<=0)
        {
            if(FindP(p,&fr,frn,1,CHECKED)<=0)  
            {
                pthread_mutex_unlock(&protect_plnk);
                return -1; // Couldn't find start
            }
        }
        
        if(FindP(p,&to,ton,1,DNTCARE)<=0) 
        {
            if(FindP(p,&to,ton,1,CHECKED)<=0)   
            {
                pthread_mutex_unlock(&protect_plnk);
                return -2; // Couldn't find end
            }
        }
        
        if(fr->next_p!=NULL && fr!=to) // Jump forward one step
            fr=fr->next_p;
        else
        {
            pthread_mutex_unlock(&protect_plnk);
            return -3; // Just one entry in list or nothing to delete
        }
        
        while(fr!=to)
        {
            if(fr==NULL)   
            {
                pthread_mutex_unlock(&protect_plnk);
                return -5; //Err missed end
            }
            
            if(fr->prev_p!=NULL)
                fr->prev_p->next_p=fr->next_p;
            
            if(fr->next_p!=NULL)
                fr->next_p->prev_p=fr->prev_p;
            
            if(fr->name!=NULL) free(fr->name);
            if(fr->value!=NULL) free(fr->value);
            
            if(fr==p->properties) p->properties=fr->next_p;
            if(fr==p->head)       p->head=fr->prev_p;
            
            free(fr);
            p->no_prop--;
            fr=fr->next_p;
        }  
        
        pthread_mutex_unlock(&protect_plnk);
        return 0; // Ok! deleted all there was
    }
    
    pthread_mutex_unlock(&protect_plnk);
    return -6; // Error! empty
}




int DeleteNo(prp_type *p,int no)
{
    int i;
    property_type *tmp;
    
    pthread_mutex_lock(&protect_plnk);	
    
    if(p->properties!=NULL && p->head!=NULL && no>=0 && no<p->no_prop)
    {
        tmp=p->properties;
        for(i=0;i<p->no_prop && i!=no && tmp!=NULL;i++)
            tmp=tmp->next_p;
        
        if(tmp==NULL) 
        {
            pthread_mutex_unlock(&protect_plnk); 
            return -1;// Error in range but is pointer mangled
        }
        
        if(tmp->prev_p!=NULL)
            tmp->prev_p->next_p=tmp->next_p;
        
        if(tmp->next_p!=NULL)
            tmp->next_p->prev_p=tmp->prev_p;
        
        if(tmp->name!=NULL) free(tmp->name);
        if(tmp->value!=NULL) free(tmp->value);
        
        if(no ==0) p->properties=tmp->next_p;
        if(tmp==p->head) p->head=tmp->prev_p;
        
        free(tmp);
        p->no_prop--;
        
        pthread_mutex_unlock(&protect_plnk);
        return 0; // Ok
    }
    
    pthread_mutex_unlock(&protect_plnk);
    return -2; // Error empty or out of range
}

int SetPT(prp_type *p,property_type *prop,int occ)
{
    
    int i,len;
    int oc=0; // Occurrence
    property_type *tmp;
    
    pthread_mutex_lock(&protect_plnk);	
    
    if(p->properties!=NULL && p->head!=NULL && prop->name!=NULL && prop->value!=NULL)
    {
        tmp=p->properties;
        for(i=0;i<p->no_prop;i++)
        {
            if(tmp==NULL)     
            {
                pthread_mutex_unlock(&protect_plnk); 
                return -1; //Err not found
            }
            
            if(!strcmp(tmp->name,prop->name))
            {
                oc++;
                if(oc==occ) // Found occ occurrence of name
                {
                    len=strlen(prop->value); // Get length of string
                    if(len>0 && len<MAX_STR) // Accepted length?
                    {
                        //Reallocate string size
                        if((tmp->value=realloc(tmp->value,strlen(prop->value)+1))==NULL)
                        {
                            pthread_mutex_unlock(&protect_plnk);
                            return -2; // Realloc error
                        } 
                        
                        strcpy(tmp->value,prop->value); // Set new value
                        tmp->checked=prop->checked; // Set checked/unchecked
                        
                        pthread_mutex_unlock(&protect_plnk);
                        return 1; // Ok! found it
                    }
                    else
                    {
                        pthread_mutex_unlock(&protect_plnk);
                        return -3; // Err string length error
                    }
                }  
            }
            tmp=tmp->next_p;
        }
        pthread_mutex_unlock(&protect_plnk);
        return 0; // Ok! But didn't find it
    }
    pthread_mutex_unlock(&protect_plnk);
    return -4; // Error! empty
}

// Set the "occ'th" entry with the name "name" in the property list to (1) be UNCHECKED, and (2) point to a newly allocated copy of "value".
// NOTE: If there is no "occ'th" entry named "name", then error.
int SetP(prp_type *p,char *name,char *value,int occ)
{
    int i,len;
    int oc=0;   // Occurrence
    property_type *tmp;
    
    pthread_mutex_lock(&protect_plnk);
    
    if(p->properties!=NULL && p->head!=NULL && name!=NULL && value!=NULL)
    {
        tmp=p->properties;
        for(i=0;i<p->no_prop;i++)
        {
            if(tmp==NULL) 
            {
                pthread_mutex_unlock(&protect_plnk); 
                return -1; // Err not found
            }
            
            if(!strcmp(tmp->name,name))
            {
                oc++;
                if(oc==occ) // Found occ occurrence of name
                {
                    len=strlen(value); // Get length of string
                    if(len>0 && len<MAX_STR) // Accepted length?
                    {
                        // Reallocate string size
                        if((tmp->value=realloc(tmp->value,strlen(value)+1))==NULL)
                        {
                            pthread_mutex_unlock(&protect_plnk); 
                            return -2; // Realloc error
                        }
                        
                        strcpy(tmp->value,value); // Set new value
                        tmp->checked=UNCHECKED;   // Set unchecked as default
                        pthread_mutex_unlock(&protect_plnk); 
                        return 1; // Ok! found it
                    }
                    else
                    {
                        pthread_mutex_unlock(&protect_plnk); 
                        return -3; // Err string length error
                    }
                }
            }
            tmp=tmp->next_p;
        }
        pthread_mutex_unlock(&protect_plnk); 
        return 0; // Ok! But didn't find it
    }
    pthread_mutex_unlock(&protect_plnk); 
    return -4; // Error! Empty argument.
}

// Insert (iname,value) after occ occurrence of name
int InsertA(prp_type *p,char *name,char *iname,char *ivalue,int occ)
{
    int i;
    int oc=0; // Occurrence
    property_type *tmp1;
    property_type *tmp2;
    
    pthread_mutex_lock(&protect_plnk);	
    
    if(p->properties!=NULL && p->head!=NULL && name!=NULL && iname!=NULL && ivalue!=NULL)
    {
        tmp1=p->properties;
        for(i=0;i<p->no_prop;i++)
        {
            if(tmp1==NULL) 
            {
                pthread_mutex_unlock(&protect_plnk); 
                return -1; //Err not found
            }
            
            if(!strcmp(tmp1->name,name))
            {
                oc++;
                if(oc==occ) // Found occ occurrence of name
                {
                    //Create and insert new property node
                    if((tmp2=malloc(sizeof(property_type)))==NULL)
                    {
                        pthread_mutex_unlock(&protect_plnk); 
                        return -1;
                    }
                    
                    if(CopyPrp(tmp2,iname,ivalue)<0) 
                    {
                        pthread_mutex_unlock(&protect_plnk);
                        return -3;
                    }
                    
                    if(tmp1->next_p!=NULL) // If no next don't try to dereference it!
                        tmp1->next_p->prev_p=tmp2;
                    tmp2->next_p=tmp1->next_p;
                    tmp2->prev_p=tmp1;
                    tmp1->next_p=tmp2;
                    
                    p->no_prop++;
                    pthread_mutex_unlock(&protect_plnk);
                    return 0; //Ok
                }
            } 
            tmp1=tmp1->next_p;
        }
        pthread_mutex_unlock(&protect_plnk);
        return 1; // Ok! but didn't find it
    }
    pthread_mutex_unlock(&protect_plnk);
    return -3; // Error! empty
}

// Insert (iname,ivalue) after occ occurrence of name
// value is not a string! Default converts to 0x0000
int InsertAV(prp_type *p,char *name,char *iname,unsigned int value,int occ)
{
    int i;
    int oc=0; // Occurrence
    property_type *tmp1;
    property_type *tmp2;
    char ivalue[32];
    
    pthread_mutex_lock(&protect_plnk);	
    if(p->properties!=NULL && p->head!=NULL && name!=NULL && iname!=NULL && ivalue!=NULL)
    {
        sprintf(ivalue,"0x%04x",value);
        tmp1=p->properties;
        for(i=0;i<p->no_prop;i++)
        {
            if(tmp1==NULL) 
            {
                pthread_mutex_unlock(&protect_plnk); 
                return -1; //Err not found
            }
            
            if(!strcmp(tmp1->name,name))
            {
                oc++;
                if(oc==occ) // Found occ occurrence of name
                {
                    //Create and insert new property node
                    if((tmp2=malloc(sizeof(property_type)))==NULL)
                    {
                        pthread_mutex_unlock(&protect_plnk);  
                        return -1;
                    }
                    
                    if(CopyPrp(tmp2,iname,ivalue)<0) 
                    {
                        pthread_mutex_unlock(&protect_plnk); 
                        return -3;
                    }
                    
                    if(tmp1->next_p!=NULL) // If no next don't try to dereference it!
                        tmp1->next_p->prev_p=tmp2;
                    tmp2->next_p=tmp1->next_p;
                    tmp2->prev_p=tmp1;
                    tmp1->next_p=tmp2;
                    
                    p->no_prop++;
                    pthread_mutex_unlock(&protect_plnk);
                    return 0; //Ok
                }
            } 
            tmp1=tmp1->next_p;
        }
        pthread_mutex_unlock(&protect_plnk); 
        return 1; // Ok! but didn't find it
    }
    pthread_mutex_unlock(&protect_plnk);
    return -3; // Error! empty
}

int InsertB(prp_type *p,char *name,char *iname,char *ivalue,int occ)
{
    int i;
    int oc=0; // Occurrence
    property_type *tmp1;
    property_type *tmp2;
    
    pthread_mutex_lock(&protect_plnk);	
    
    if(p->properties!=NULL && p->head!=NULL && name!=NULL && iname!=NULL && ivalue!=NULL)
    {
        tmp1=p->properties;
        for(i=0;i<p->no_prop;i++)
        {
            if(tmp1==NULL)
            {  
                pthread_mutex_unlock(&protect_plnk); 
                return -1; //Err not found
            }
            
            if(!strcmp(tmp1->name,name))
            {
                oc++;
                if(oc==occ) // Found occ occurrence of name
                {
                    //Create and insert new property node
                    if((tmp2=malloc(sizeof(property_type)))==NULL) 
                    { 
                        pthread_mutex_unlock(&protect_plnk); 
                        return -1;
                    }
                    
                    if(CopyPrp(tmp2,iname,ivalue)<0) 
                    { 
                        pthread_mutex_unlock(&protect_plnk); 
                        return -3;
                    }
                    
                    if(tmp1->prev_p!=NULL)
                    {
                        tmp1->prev_p->next_p=tmp2; // Set previous next to new node
                    }
                    else
                    {
                        p->properties=tmp2; // New start of list
                    }
                    
                    tmp2->prev_p=tmp1->prev_p; // Set new previous to old previous
                    tmp2->next_p=tmp1; // Set new next to current
                    tmp1->prev_p=tmp2; // Set current previous to new
                    p->no_prop++; // Increase number of property nodes
                    pthread_mutex_unlock(&protect_plnk); 
                    
                    return 0; //Ok
                }
            } 
            tmp1=tmp1->next_p;
        }
        pthread_mutex_unlock(&protect_plnk); 
        return 1; // Ok! but didn't find it
    }
    pthread_mutex_unlock(&protect_plnk); 
    return -3; // Error! empty
}


