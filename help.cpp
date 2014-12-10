#include <sys/types.h>
#include <functional>
#include <string.h>
#include <stdio.h>
using namespace std;

#include "error.h"
#include "utility.h"
#include "catalog.h"

// define if debug output wanted


//
// Retrieves and prints information from the catalogs about the for
// the user. If no relation is given (relation.empty() is true), then
// it lists all the relations in the database, along with the width in
// bytes of the relation, the number of attributes in the relation,
// and the number of attributes that are indexed.  If a relation is
// given, then it lists all of the attributes of the relation, as well
// as its type, length, and offset, whether it's indexed or not, and
// its index number.
//
// Returns:
// 	OK on success
// 	error code otherwise
//

const Status RelCatalog::help(const string & relation)
{
  Status s;
  RelDesc rd;
  AttrDesc *attrs;
  int attrCnt;
  RID rid;
  Record rec;
  if (relation.empty()) return UT_Print(RELCATNAME);

  /** my code starts here **/
  HeapFileScan* hfs = new HeapFileScan(RELCATNAME, s);
  CHKSTAT(s);
  s = hfs->startScan(0, MAXNAME, STRING, relation.c_str(), EQ);
  s = hfs->scanNext(rid);
  CHKSTAT(s);
  s = hfs->getRecord(rec);
  CHKSTAT(s);
  memcpy((void*)&rd, rec.data, rec.length);
  printf("Relation name: %s, (%d Attributes)\n", rd.relName, rd.attrCnt);
  printf("==========================================\n");
  printf("Attribute Name\t\tOffset\tType\tLength\n");
  delete hfs;
  hfs = new HeapFileScan(ATTRCATNAME, s);
  CHKSTAT(s);
  s = hfs->startScan(0, MAXNAME, STRING, relation.c_str(), EQ);
  CHKSTAT(s);
  attrs = new AttrDesc[rd.attrCnt];
  for(int i = 0; i < rd.attrCnt; i++){
    s = hfs->scanNext(rid);
    CHKSTAT(s);
    s = hfs->getRecord(rec);
    CHKSTAT(s);
    memcpy(attrs+i, rec.data, rec.length);
  }
  for(int i = 0; i < rd.attrCnt; i++){
    printf("%s\t\t%d\t%d\t%d\n", attrs[i].attrName, attrs[i].attrOffset, attrs[i].attrType, attrs[i].attrLen);
  }
  delete[] attrs;
  attrs = NULL;
  delete hfs;
  hfs = NULL;
  return OK;
}
