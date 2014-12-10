#include "catalog.h"
#include <unordered_set>
/* as reference
typedef struct {
  char relName[MAXNAME];                // relation name
  char attrName[MAXNAME];               // attribute name
  int attrOffset;                       // attribute offset
  int attrType;                         // attribute type
  int attrLen;                          // attribute length
} AttrDesc;
*/

/* as reference
typedef struct {
  char relName[MAXNAME];                // relation name
  char attrName[MAXNAME];               // attribute name
  int  attrType;                        // INTEGER, FLOAT, or STRING
  int  attrLen;                         // length of attribute in bytes
  void *attrValue;                      // ptr to binary value
} attrInfo; 
 */

const Status RelCatalog::createRel(const string & relation, 
				   const int attrCnt,
				   const attrInfo attrList[])
{
  Status s;
  RelDesc rd;
  AttrDesc ad;

  if (relation.empty() || attrCnt < 1)
    return BADCATPARM;

  if (relation.length() >= sizeof rd.relName)
    return NAMETOOLONG;

  int totalLen = 0;
  /** my code starts here **/
  // check attrList dups and attrLen
  std::unordered_set<string> attrSet;
  for(int i = 0; i < attrCnt; i++){
    // dups 
    string newStr(attrList[i].attrName);
    if(attrSet.find(newStr) != attrSet.end()){
      return DUPLATTR;
    }
    attrSet.insert(newStr);
    // attrLen
    if(attrList[i].attrName[MAXSTRINGLEN-1] != '\0'){
      return NAMETOOLONG;
    }
    // relname
    if(attrList[i].relName[MAXNAME-1] != '\0'){
      return NAMETOOLONG;
    }
    totalLen += attrList[i].attrLen;
  }
  if(totalLen > PAGESIZE){
    return ATTRTOOLONG;
  }

  s = getInfo(relation, rd);
  if(s == OK) return RELEXISTS;
  strcpy(rd.relName, relation.c_str());
  rd.attrCnt = attrCnt;
  s = addInfo(rd);
  CHKSTAT(s);
  for(int i = 0; i < attrCnt; i++){
    strcpy(ad.relName, attrList[i].relName);
    strcpy(ad.attrName, attrList[i].attrName);
    ad.attrOffset = i;
    ad.attrType = attrList[i].attrType;
    ad.attrLen = attrList[i].attrLen;
    s = attrCat->addInfo(ad);
    CHKSTAT(s);
  }
  s = createHeapFile(relation);
  CHKSTAT(s);
  return OK;
}

